#include <iostream>
#include <list>
#include <vector>
#include <numeric>
#include <tuple>
#include <random>
#include <ctime>
#include <algorithm>
#include <time.h>

#define N_common 4
#define N_relations 4


struct TABLE{
    char name;
    int cost_join_number;
    int joined_variable[N_common];
};

std::vector<TABLE> table_statistic; //Statistics on the input relations


void initialize_(int N_rel){
    table_statistic.push_back({'R', 1000, {100,200,-1,-1}});
    table_statistic.push_back({'S', 1000, {-1,100,500,-1}});
    table_statistic.push_back({'T', 1000, {-1,-1,20,50}});
    table_statistic.push_back({'U', 1000, {50,-1,-1,1000}});
}


int max_V(int rel_a, int rel_b){
    int maxV = 1, idx = 0;
    for (int i = 0 ; i < sizeof(table_statistic[0].joined_variable)/sizeof(table_statistic[0].joined_variable[0]); i++){
        if(table_statistic[rel_a].joined_variable[i] != -1 && table_statistic[rel_b].joined_variable[i] != -1){
           maxV = std::max(table_statistic[rel_a].joined_variable[i], table_statistic[rel_b].joined_variable[i]);
           idx = i;
        }
    }
    return maxV; //maximum variable, related coloumn
}

int size_join(std::vector <int> rel_a, int rel_b, int r, int right_product){

    int join = 0, maxV = 1;
    // std::cout <<std::endl<< "joining the table "<< rel_a <<"and " << rel_b;
    if(r == 2){
        auto maxV_ = max_V(rel_a[0], rel_b);
        maxV = maxV_;
        join = ((table_statistic[rel_a[0]].cost_join_number * table_statistic[rel_b].cost_join_number) /  maxV);

    }else if (r > 2){
        //find the joined coloumn
        int common_coloumn = rel_a[0];
        for(int i = 0 ;  i < rel_a.size(); i++){
            auto maxV_ = max_V(rel_a[i], rel_b);
            if(maxV_ > 1 ){
                common_coloumn = rel_a[i];
                maxV = maxV_;
            }
        }
        join = ((right_product * table_statistic[rel_b].cost_join_number) /  maxV);
        // std::cout <<"right product "<< r << "-> " << common_coloumn << " ; " << rel_b << " COST " << (right_product * table_statistic[rel_b].cost_join_number) << "/" << maxV;
    }
    // std::cout << " r " << r << "rel : " << rel_a <<  " & "<<  rel_b <<" max v -> " << maxV;
    return join;
}

int join_cost_function(std::vector<int> trialSolution, int n){
    int size_join_res = 100000000, join_cost_res = 100000000, maxV=1, max_IDX = 0;
    std::vector<int> right; int left=0, total_size = 0, total_join = 0;
    int global_maxV;

    for (int r = 1 ; r <= n ; r++){
        left = trialSolution[r-1];
        if(r == 1){
            // right.push_back(trialSolution[0]);
                size_join_res = table_statistic[trialSolution[r-1]].cost_join_number;
                join_cost_res = 0;
            }else if(r == 2){
                right.push_back((trialSolution[r-2] -1 ));
                auto temp_cost = size_join(right,left - 1, r, 0);
                size_join_res = temp_cost;
                join_cost_res = 0;
                global_maxV = maxV;

            }else{
                right.push_back((trialSolution[r-2] -1));
                join_cost_res = total_size;
                auto size_join__ = size_join(right,left - 1,r, (total_size+total_join));
                size_join_res = size_join__;
            }
            total_size = size_join_res;
            total_join += join_cost_res;
            // std::cout << "total size "<< total_size << " total join "<< total_join << std::endl;

        }
    return total_join;
}

//simulated annealing related func

std::vector<int> new_solution (std::vector<int> arr, int NRel){
    auto rng = std::default_random_engine {};
    std::shuffle(std::begin(arr), std::end(arr), rng);
   return arr;
}

double initiate_Temperature(std::vector<int> arr, int Nrel){
    double sumRel = 0;
    for(int i = 0 ; i < Nrel ; i++){
        std::vector<int> tmp = new_solution(arr,Nrel);
	    int join_size = join_cost_function(tmp, Nrel);
	    if(join_size > sumRel){
	        sumRel = join_size;
	    }
    }
    return 2*sumRel;
    // return 200;
}

void simulatedAnnealing(std::vector<int> arr, double temp, int Nrel){


    double decT = 0.99;
    double T = temp;
    int minimum_join_cost, new_join_size;
	std::vector<int> new_rel_path, best_path;
	int smallest_join_cost = join_cost_function(arr, Nrel);

    for(int i = 0; i < 10 ; i++){
	    minimum_join_cost = join_cost_function(arr, Nrel);
	    new_rel_path = new_solution(arr, Nrel);
	    std::cout << "the old -> ";
        for(int x = 0 ; x < Nrel; x++ ){
	        std::cout <<  arr[x] << " ";
	    }std::cout << " ; ";
        std::cout << "the new swap -> ";
        for(int x = 0 ; x < Nrel; x++ ){
	        std::cout <<  new_rel_path[x] << " ";
	    }
	    new_join_size = join_cost_function(new_rel_path, Nrel);

	    std::cout << "old -> " << minimum_join_cost << "; new value -> " << new_join_size;
	    if(new_join_size < minimum_join_cost){
	        minimum_join_cost = new_join_size;
	        arr = new_rel_path;
	        if(minimum_join_cost < smallest_join_cost){
	            smallest_join_cost = minimum_join_cost;
	            best_path = arr;
	        }
	    }else{
	        double rndNumber = ((double) rand() / (RAND_MAX));
	        float temp_cooling = exp(-1 * (new_join_size - smallest_join_cost) / T);
	       // std::cout << "random number and its exponential border : " << rndNumber << " & " << temp_cooling;
	        if(rndNumber < temp_cooling){
	            std::cout << " ** "; //it is not smaller join cost but exploring the area for possible global minima
	            arr = new_rel_path;
	        }//else ; maintain arr;
	    }
	    T = decT * T;
	    std::cout << std::endl;
    }

    std::cout << "SOLUTION -> ";

    for(int x = 0 ; x < Nrel; x++ ){
	        std::cout  << " " <<  arr[x];
        }
}

int main(){
    int N_rel = 4;
    initialize_(N_rel);
    std::vector<int> arr(N_rel) ; // vector with 100 ints.
    std::iota (std::begin(arr), std::end(arr), 1); // Fill with 0, 1, ..., N_rel
    // std::vector<int> arr {3,4,2,1};
// 	join_cost_function(arr, N_rel);
    srand (time(NULL));
    double initiate_T = initiate_Temperature(arr, N_rel);
    simulatedAnnealing(arr, initiate_T, N_rel);



}
