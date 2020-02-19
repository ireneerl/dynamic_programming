#include <iostream>
#include <list>
#include <vector>
#include <numeric>
#include <tuple>

#define N_common 4
#define N_relations 4
struct TABLE{
    char name;
    int cost_join_number;
    int joined_variable[N_common];
};

struct TABLE_MEMO{
    std::vector<int> rel_comb;
    int size;
    int cost_join;
    int max_V;
    int max_IDX;
    std::vector<int> best_rel;
	TABLE_MEMO (std::vector<int> rel_comb_ = std::vector<int>(), int a_=0, int a1=0, int a2=0, int a3=0, std::vector<int> best_rel_ = std::vector<int>()): rel_comb(rel_comb_), size(a_), cost_join(a1), max_V(a2), max_IDX(a3), best_rel(best_rel_) {}
};

std::vector<TABLE> table_statistic; //Statistics on the input relations

void initialize_(int N_rel){
    table_statistic.push_back({'R', 1000, {100,200,-1,-1}});
    table_statistic.push_back({'S', 1000, {-1,100,500,-1}});
    table_statistic.push_back({'T', 1000, {-1,-1,20,50}});
    table_statistic.push_back({'U', 1000, {50,-1,-1,1000}});
}

std::vector<std::list<TABLE_MEMO>> vect;
std::list <TABLE_MEMO> list_;
int combinatorial[1000] = {0};


std::tuple<int, int> max_V(int rel_a, int rel_b){
    int maxV = 1, idx = 0;
    for (int i = 0 ; i < sizeof(table_statistic[0].joined_variable)/sizeof(table_statistic[0].joined_variable[0]); i++){
        if(table_statistic[rel_a].joined_variable[i] != -1 && table_statistic[rel_b].joined_variable[i] != -1){
           maxV = std::max(table_statistic[rel_a].joined_variable[i], table_statistic[rel_b].joined_variable[i]);
           idx = i;
        }
    }
    return {maxV, idx}; //maximum variable, related coloumn
}


std::tuple<int, int, int> size_join(int rel_a, int rel_b, int r, int right_product, int maxV_right_product, int maxIDX){

    int join = 0, maxV = 1;
    if(r == 2){
        auto [maxV,maxIDX] = max_V(rel_a, rel_b);
        join = ((table_statistic[rel_a].cost_join_number * table_statistic[rel_b].cost_join_number) /  maxV);
    }else if (r > 2){
        // std::cout << "rel -> " << rel_a << "," << rel_b;
        auto [maxV_,maxIDX] = max_V(rel_a, rel_b);
        maxV = maxV_;
        // std::cout <<"right product "<< r << "-> " << right_product << ";"<<(right_product * table_statistic[rel_b].cost_join_number) << " / " << maxV <<  " COST " << join;
    }
    return {join, maxV, maxIDX};
}

std::vector<int> findRestoftheElement(std::vector<int> relations, std::vector<int> greddy_curr_solution, int n){
    std::vector<int> rest;
    for(int x=0; x<3; x++){
        int j ;
        for(j = 0 ; j < greddy_curr_solution.size() ; j++){
            if(relations[x] == greddy_curr_solution[j])
            break;
        }
        if(j == greddy_curr_solution.size()){
            rest.push_back (relations[x]);
        }
    }
    return rest;
}

void greedy_move(std::vector<int> &greedy_solution, std::vector<int> rest, int n, int r, int size_join_res, int join_cost_res){
    int left, right = greedy_solution[greedy_solution.size()-1];

    // std::cout << std::endl<< " join "<< join_cost_res << " size " << size_join_res;
    int temp_total_size = size_join_res,
    temp_total_join = size_join_res,
    total_size = temp_total_size,
    total_join = 100000000;
    for(int x = 0; x < rest.size();x++){
        left = rest[x];
        auto [size_join__, maxV_, max_IDX] = size_join(right - 1,left - 1,r, size_join_res, 0,0);
        temp_total_size = size_join_res;
        temp_total_join += join_cost_res;

        // std::cout << std::endl<< " join "<< join_cost_res << " size " << size_join_res << " with join "<<left  << " with temp " << temp_total_join<< ";" << size_join__ <<std::endl;
        if (temp_total_join <= total_join){
            total_size = temp_total_size;
            total_join = temp_total_join;
            // std::cout << " temp_total join " << temp_total_join << " total join " << total_join << "size " << size_join_res ;
            if(greedy_solution.size() == r){
                greedy_solution.pop_back();
            }
            greedy_solution.push_back(rest[x]);
        }
    }
}

std::tuple<int, int> combinationUtil(std::vector<int> arr, std::vector<int> data,  int start, int end,  int index, int r, int *total_size, int *total_join, std::vector<int> &greedy_solution) {
    if (index == r)  {
        int i = r-1; //r starts from the sequence number, translates the r to array indices
        int size_join_res = 100000000, join_cost_res = 100000000, maxV=1, max_IDX = 0;
        for (int j = 0; j < r; j++)  {
            std::cout << data[j] << " ";
        }std::cout << std::endl;
        int temp_total_size = 0, temp_total_join = 0;
        std::vector<int> right; int left=0;
        right = data;
        left = right[i];
        right.erase (right.begin()+(i));

        if(r == 1){
            size_join_res = table_statistic[data[i]].cost_join_number;
            join_cost_res = 0;
            return {size_join_res, join_cost_res};
        }
        else if(r == 2){
            auto [temp_cost, maxV, max_IDX] = size_join(right[i-1] - 1,left - 1, r, 0, 0, 0);
            size_join_res = temp_cost;
            join_cost_res = 0;
            if (size_join_res <= *total_size){
                *total_size = size_join_res;
                *total_join = join_cost_res;
                if(!greedy_solution.empty()){
                    greedy_solution.pop_back();
                    greedy_solution.pop_back();
                }
                greedy_solution.push_back(right[i-1]);
                greedy_solution.push_back(left);
            }
            return {*total_size, *total_join};
        }
    }
    for (int i = start; i <= end &&  end - i + 1 >= r - index; i++)  {
        data[index] = arr[i];
        auto [total_size_, total_join_] = combinationUtil(arr, data, i+1,  end, index+1, r, total_size, total_join, greedy_solution);
        *total_size = total_size_;
        *total_join = total_join_;
    }
    return {*total_size, *total_join};
}

void printCombination(std::vector<int> arr, int n, int r, std::vector<int> &greedy_solution, int *size_join_res, int *join_cost_res) {
    std::vector<int> data (r);
    if(r <= 2){
        *size_join_res = 100000000, *join_cost_res = 100000000;
        auto [total_size_, total_join_] = combinationUtil(arr, data, 0, n-1, 0, r, size_join_res, join_cost_res, greedy_solution);
        *size_join_res = total_size_;
        *join_cost_res = total_join_;
    }else{
        std::vector<int> rest = findRestoftheElement(arr, greedy_solution, n);
        greedy_move(greedy_solution, rest, n, r,*size_join_res, *join_cost_res);
    }
}

void printSolution (std::vector<int> solution){
    for(int i = 0 ; i < solution.size(); i++){
        std::cout << solution[i] << " ";
    }
}

int main()
{
    int N_rel = 4;
    initialize_(N_rel);
    std::vector<int> arr(N_rel) ; // vector with 100 ints.
    std::iota (std::begin(arr), std::end(arr), 1); // Fill with 0, 1, ..., N_rel
    std::vector<int> greedy_solution;

    int size, cost;

    for (int r = 1; r <= N_rel ; r++){
        printCombination(arr, N_rel, r, greedy_solution, &size, &cost);
        std::cout << std::endl;
        printSolution(greedy_solution);


    }
    return 0;
}
