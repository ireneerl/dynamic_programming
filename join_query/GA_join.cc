#include <iostream>
#include <list>
#include <vector>
#include <numeric>
#include <tuple>
#include <random>
#include <ctime>
#include <algorithm>

#define N_common 4
#define N_relations 4

struct INDVI{
    std::vector <int> gens ;
    double fitness_val;
    double probability;
    //only for memory ->
    int size;
    int cost;
};

struct TABLE{
    char name;
    int cost_join_number;
    int joined_variable[N_common];
};

std::vector<TABLE> table_statistic; //Statistics on the input relations
std::vector<INDVI> population;
std::vector<INDVI> parents;
std::vector<INDVI> children;

void initialize_(int N_rel){
    table_statistic.push_back({'R', 1000, {100,200,-1,-1}});
    table_statistic.push_back({'S', 1000, {-1,100,500,-1}});
    table_statistic.push_back({'T', 1000, {-1,-1,20,50}});
    table_statistic.push_back({'U', 1000, {50,-1,-1,1000}});
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

void printPopulation(std::vector<INDVI> comm){
    std::cout << std::endl;
   for(int indv = 0; indv < comm.size() ; indv++){
       std::cout << "#" << indv;
       for(int gen = 0 ; gen < comm[indv].gens.size() ; gen++){
           std::cout << " " << comm[indv].gens[gen];
       }
       std::cout << " ; cost = " << comm[indv].cost << " ; fitness value = " << comm[indv].fitness_val << std::endl;
   }
}

void generate_population(std::vector<int> arr, int NRel, int NIndiv){
    for(int indv = 0; indv < NIndiv ; indv++){
        std::vector<int> temp;
        for (int i = 0; i < NRel; i++){
    		int j = i + rand() % (NRel - i);
    		std::swap(arr[i], arr[j]);
            temp.push_back(arr[i]);
	    }
	    population.push_back({temp, 0, -1, -1});
    }
}

void fitness_function(int N_indv, int n){
    for(int x_indv = 0 ; x_indv < N_indv ; x_indv++){
    int size_join_res = 100000000, join_cost_res = 100000000, maxV=1, max_IDX = 0;
        for (int j = 0; j < n; j++)  {
          std::cout << population[x_indv].gens[j] << " ";
        }std::cout << std::endl;
        std::vector<int> right; int left=0, temp_total_size = 0, temp_total_join = 0;
        for (int r = 1 ; r <= n ; r++){
            left = population[x_indv].gens[r-1];
            if(r == 1){
                right.push_back(population[x_indv].gens[0]);
                size_join_res = table_statistic[population[x_indv].gens[r-1]].cost_join_number;
                join_cost_res = 0;
            }else if(r == 2){
                right.push_back(population[x_indv].gens[r-2]);
                auto [temp_cost, maxV, max_IDX] = size_join(right[r-1] - 1,left - 1, r, 0, 0, 0);
                size_join_res = temp_cost;
                join_cost_res = 0;
            }else{
                right.push_back(population[x_indv].gens[r-2]);
                auto [size_join__, maxV_, max_IDX] = size_join(right[right.size()-1] - 1,left - 1,r, size_join_res, 0,0);
                join_cost_res = size_join_res;
                size_join_res = size_join__;
                // std::cout << std::endl<< " -> " << join_cost_res<< " ;"<< std::endl;
            }

            temp_total_size += size_join_res;
            temp_total_join += join_cost_res;
            // std::cout <<" -> "<< join_cost_res << " "<<total_join;
        }
         population[x_indv].cost = temp_total_join;
         population[x_indv].size = temp_total_size;
         population[x_indv].fitness_val = 1.0/temp_total_join;
    }
    std::cout << std::endl;
    // return {total_size, total_join};
}

void select_parents(int NIndiv, int NRel){
    std::vector <std::vector <int> > new_parents;
    double totalFitness = 0;int i;
    for(i=0;i<NIndiv;i++){
        totalFitness += population[i].fitness_val;
    }
    for(i=0;i<NIndiv;i++){
        population[i].probability = (population[i].fitness_val)/totalFitness;
    }

    for (int new_P = 0; new_P < NIndiv; new_P++) {
        double rndNumber = rand() / (double) RAND_MAX, offset = 0.0;
        INDVI parent_choosen;
        for (i = 0; i < NIndiv; i++) {
            offset += population[i].probability;
            if (rndNumber < offset) {
                parent_choosen = population[i];
                break;
            }
        }
        parents.push_back(parent_choosen);
    }
}

void crossOver(double prob, int n){
    children = parents;
    for(int pair_idx = 0 ; pair_idx < n/2 ; pair_idx++){
        srand (time(NULL));
        double rndNumber = ((double) rand() / (RAND_MAX)) + 1;
        int id_parent1 = pair_idx * 2, id_parent2 = pair_idx * 2 + 1;
        if(rndNumber < prob){
            int left = rand() % 5 + 1;
            int right = rand() % 10 + left;
            for(int COx = left ; COx < right ; COx++){
                std::vector<int>::iterator it_parent1 = std::find(parents[id_parent1].gens.begin(), parents[id_parent1].gens.end(), parents[id_parent2].gens[COx]);
                int idx_origin_p1 = std::distance(parents[id_parent1].gens.begin(), it_parent1);
                std::vector<int>::iterator it_parent2 = std::find(parents[id_parent2].gens.begin(), parents[id_parent2].gens.end(), parents[id_parent1].gens[COx]);
                int idx_origin_p2 = std::distance(parents[id_parent2].gens.begin(), it_parent2);

                //crossing over
                children[id_parent1].gens[idx_origin_p1] = children[id_parent1].gens[COx];
                children[id_parent2].gens[idx_origin_p2] = children[id_parent2].gens[COx];
                int tmp = parents[id_parent1].gens[COx];
                children[id_parent1].gens[COx] = children[id_parent2].gens[COx];
                children[id_parent2].gens[COx] = tmp;
            }
        }
    }
}

void mutation(double prob, int nrel){
    for(int idv = 0 ; idv < nrel ; idv++){
        srand (time(NULL));
        double rndNumber = ((double) rand() / (RAND_MAX)) + 1;
        if(rndNumber < prob){
            int idx_mutated = rand() % nrel + 1;
            int tmp = children[idv].gens[idx_mutated];
            if(children[idv].gens[idx_mutated] < nrel && children[idv].gens[idx_mutated] > 1)
            {
                int mutation = children[idv].gens[idx_mutated]+1;
                std::vector<int>::iterator t_idv = std::find(children[idv].gens.begin(), children[idv].gens.end(), mutation);
                int idx_origin_ = std::distance(children[idv].gens.begin(), t_idv);
                children[idv].gens[idx_origin_] = tmp;
                children[idv].gens[idx_mutated] = mutation;
            }
        }

    }
}

bool compareByFitnessValue(const INDVI &a, const INDVI &b)
{
    return a.fitness_val > b.fitness_val;
}

int main(){
    int N_rel = 4;
    initialize_(N_rel);
    std::vector<int> arr(N_rel) ; // vector with 100 ints.
    std::iota (std::begin(arr), std::end(arr), 1); // Fill with 0, 1, ..., N_rel

    //GA parameters
    int NIndiv = 10;
    srand ( time(NULL) );

    generate_population(arr, N_rel, NIndiv);
    for(int i = 0 ; i < 10 ; i ++ ){
        std::cout << "HEEEY -> " << i;
        fitness_function(NIndiv, N_rel);
        printPopulation(population);
        select_parents(NIndiv, N_rel);
        printPopulation(parents);
        crossOver(0.8, NIndiv);
        printPopulation(children);
        mutation(1, N_rel);
        printPopulation(children);
        population.clear();
        parents.clear();
        population = children;
    }

    std::sort(population.begin(), population.end(), compareByFitnessValue);
    printPopulation(population);


}
