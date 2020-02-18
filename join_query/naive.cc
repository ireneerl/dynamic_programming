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
    // std::cout <<std::endl<< "joining the table "<< rel_a <<"and " << rel_b;
    if(r == 2){
        auto [maxV,maxIDX] = max_V(rel_a, rel_b);
        join = ((table_statistic[rel_a].cost_join_number * table_statistic[rel_b].cost_join_number) /  maxV);
    }else if (r > 2){
        auto [maxV_,maxIDX] = max_V(rel_a, rel_b);
        maxV = maxV_;
        join = ((right_product * table_statistic[rel_b].cost_join_number) /  maxV);
        // std::cout <<"right product "<< r << "-> " << right_product << ";"<<(right_product * table_statistic[rel_b].cost_join_number) << " / " << maxV <<  " COST " << join;
    }
    return {join, maxV, maxIDX};
}


std::tuple<int, int> combinationUtil(std::vector<int> arr, int index, int n, int total_size, int total_join) {
    if (index == n){
        int size_join_res = 100000000, join_cost_res = 100000000, maxV=1, max_IDX = 0;
        for (int j = 0; j < n; j++)  {
          std::cout << arr[j] << " ";
        }
        std::vector<int> right; int left=0, temp_total_size = 0, temp_total_join = 0;
        for (int r = 1 ; r <= n ; r++){
            left = arr[r-1];
            if(r == 1){
                right.push_back(arr[0]);
                size_join_res = table_statistic[arr[r-1]].cost_join_number;
                join_cost_res = 0;
            }else if(r == 2){
                right.push_back(arr[r-2]);
                auto [temp_cost, maxV, max_IDX] = size_join(right[r-1] - 1,left - 1, r, 0, 0, 0);
                size_join_res = temp_cost;
                join_cost_res = 0;
            }else{
                right.push_back(arr[r-2]);
                auto [size_join__, maxV_, max_IDX] = size_join(right[right.size()-1] - 1,left - 1,r, size_join_res, 0,0);
                join_cost_res = size_join_res;
                size_join_res = size_join__;
                // std::cout << std::endl<< " -> " << join_cost_res<< " ;"<< std::endl;
            }

            temp_total_size += size_join_res;
            temp_total_join += join_cost_res;
            // std::cout <<" -> "<< join_cost_res << " "<<total_join;
        }
        if (temp_total_join <= total_join){
            total_size = temp_total_size;
            total_join = temp_total_join;
            std::cout << "here is the smaller result -> " << total_join ;
        }
        std::cout << std::endl;
        return {total_size, total_join};
    }

    for (unsigned int i=index; i<n; i++){
        std::swap(arr[i], arr[index]);
        auto [total_size_, total_join_] = combinationUtil(arr, index+1, n,total_size, total_join);
        total_size = total_size_;
        total_join = total_join_;
        std::swap(arr[i], arr[index]);
    }
    return {total_size, total_join};
}

std::tuple<int, int> printCombination(std::vector<int> arr, int n) {
    int size_join_res = 100000000, join_cost_res = 100000000;
    auto [total_size_, total_join_] = combinationUtil(arr, 0, n, size_join_res, join_cost_res);
    return {total_size_, total_join_};
}

int main()
{
    int N_rel = 4;
    initialize_(N_rel);
    std::vector<int> arr(N_rel) ; // vector with 100 ints.
    std::iota (std::begin(arr), std::end(arr), 1); // Fill with 0, 1, ..., N_rel

    auto [total_size, total_join] = printCombination(arr, N_rel);

    //traceback
    std::cout << "total = " << total_join;

    return 0;
}
