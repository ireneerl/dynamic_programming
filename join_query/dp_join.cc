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

int fact(int n) { //also DP
   if (n >= 0) {
      combinatorial[0] = 1;
      for (int i = 1; i <= n; ++i) {
         combinatorial[i] = i * combinatorial[i - 1];
      }
   }
   return combinatorial[n];
}

int nCr(int n, int r){
    return fact(n) / (fact(r) * fact(n-r));
}

int GetCombID(std::vector<int> comb, int n_sub){
    int k = comb.size(), id = nCr(n_sub, k);
    for (int i = 0; i < k; i++){
        int new_n = n_sub - comb[i];
        int new_k = k-i;
        if(new_k <= new_n){
            id -= nCr(new_n, new_k);
        }
     }
    return (int)id;
}

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
        auto [maxV_,maxIDX] = max_V(rel_a, rel_b);
        maxV = maxV_;
        join = ((right_product * table_statistic[rel_b].cost_join_number) /  maxV);
        // std::cout <<"right product -> " << right_product << ";"<<(right_product * table_statistic[rel_b].cost_join_number) << " / " << maxV <<  " COST " << join;
    }
    return {join, maxV, maxIDX};
}


std::tuple<int, int, int> calcAndMemoized(std::vector<int> right, int left) {
    for (int i = 0 ; i < right.size() ; i++){
        std::cout << " "<< right[i] ;
    }std::cout << " //";
    int mem_idx = (GetCombID(right,4)-1);
    int right_size = right.size();
    int mem_cost, mem_maxV;
    if(right_size > 1){
        std::list<TABLE_MEMO>::iterator x = std::next(vect[(right.size()-1)].begin(), mem_idx); //prev hash bucket, with found memoized id
    	mem_cost = x->size;
    	mem_maxV = x->max_V;
    }
    return {mem_cost, mem_maxV, mem_maxV};
}

int construct_memoTable(std::vector<int> arr, int n, int r, int idx_hash){
    std::vector<int> right;
    int left;
    //first memoization
    if(r == 1){
        std::vector<int> joined_rel(r) ;
        joined_rel.assign(&arr[0], &arr[r]);
        TABLE_MEMO head(joined_rel, table_statistic[idx_hash-1].cost_join_number, 0, 0, 0, joined_rel);
        list_.push_back(head);
    }else {
        int size_join_res = 100000000, join_cost_res = 100000000, maxV=1, max_IDX = 0;
        std::vector<int> joined_rel(right.size()) ;
        std::vector<int> origin(r) ;
        origin.assign(&arr[0], &arr[r]);
        for (int i = 0 ; i < r ; i++){
            right = arr;
            right.erase (right.begin()+i);
            left = arr[i];
            int temp_cost = 100000000;
            if(r == 2){
                auto [temp_cost, maxV, max_IDX] = size_join(right[0] - 1,left - 1, r, 0, 0, 0); //pass the index
                if(temp_cost < size_join_res){
                    size_join_res = temp_cost;
                    joined_rel.assign({right[0],left});
                    join_cost_res = 0;
                }
            }
            else{
                int coloumn_idx = 0;
                auto [cost_mem, maxV_mem, max_IDX_mem] = calcAndMemoized(right, left); //memoization size
                if(cost_mem < join_cost_res){
                    join_cost_res = cost_mem;
                    joined_rel.assign(right.begin(),right.begin()+(r-1));
                    while(maxV == 1 && coloumn_idx <right.size()){
                        auto [size_join__, maxV_, max_IDX] = size_join(right[coloumn_idx] - 1,left - 1,r, join_cost_res, maxV_mem, max_IDX_mem);
                        size_join_res = size_join__; coloumn_idx++;maxV = maxV_;
                    }
                }
            }
        }
        TABLE_MEMO head(origin,size_join_res, join_cost_res, maxV, max_IDX, joined_rel); //0-> calculate function
        list_.push_back(head);
    }
    return ++idx_hash;
}

void combinationUtil(std::vector<int> arr, std::vector<int> data,  int start, int end,  int index, int r, int idx_hash) {
    if (index == r)  {
        int j=0;
        for (j = 0; j < r; j++)  {
            std::cout << data[j] << " ";
        }std::cout << ";";
        if(end>=r || r == end+1){
            int idx_hash = construct_memoTable(data,end,r, idx_hash); // if the last combination doesnt need this, separate if without this line
            std::cout << std::endl;
        }
        return;
    }
    for (int i = start; i <= end &&  end - i + 1 >= r - index; i++)  {
        data[index] = arr[i];
        combinationUtil(arr, data, i+1,  end, index+1, r, idx_hash);
    }
}

void printCombination(std::vector<int> arr, int n, int r) {
    std::vector<int> data (r);
    combinationUtil(arr, data, 0, n-1, 0, r, 0);
}

int binomialCoeffSum(int n){
    return (1 << n)-1;
}

void print_memo_table(int N_rel){
    for(int i=0;i<N_rel;i++){
    	for(auto x: vect[i]) {
    	    for (int i = 0; i < x.rel_comb.size(); i++) {
        		std::cout << x.rel_comb.at(i) << ';';
        	}
    		std::cout << " " << x.size << " " << x.cost_join << " ";
    		for (int i = 0; i < x.best_rel.size(); i++) {
        		std::cout << x.best_rel.at(i) << ';';
        	}
        	std::cout << std::endl;
    	}
    std::cout << std::endl;
	}
}

int main()
{
    int N_rel = 4;
    initialize_(N_rel);
    std::vector<int> arr(N_rel) ; // vector with 100 ints.
    std::iota (std::begin(arr), std::end(arr), 1); // Fill with 0, 1, ..., N_rel

    for (int r = 1; r <= N_rel ; r++){
        printCombination(arr, N_rel, r);
        vect.push_back(list_);
        std::cout << "END"<<std::endl;
        print_memo_table(r);
        std::cout << "END"<<std::endl;
        list_.clear();
    }

    //traceback
    int memid=0,total=0;
    for(int i = N_rel-1 ; i > 0 ; i--){
        std::list<TABLE_MEMO>::iterator x = std::next(vect[i].begin(), memid);
        memid = (GetCombID(x->best_rel,N_rel)-1);
        std::cout <<i<<"-"<< memid << ";";
        total += x->cost_join;
    }
    std::cout << "total = " << total;

    return 0;
}
