#include <iostream>
#include <string>
#include <vector>
#include <omp.h>
using std::string;
using std::to_string;
using std::cout;
using std::endl;
using std::cin;
using std::vector;

class Person{
    private:
        //int status = 0;
    public:
        int status = 0;
        string status_string(){
            string str;
            if(status == 0){
                //#pragma omp single
                str = "susceptible";
                return str;
            }
            else if(status == -1){
                //#pragma omp single
                str = "recovered";
                return str;
            }
            else if(status == -2){
                //#pragma omp single
                str = "vaccinated";
                return str;
            }
            else{
                //#pragma omp single
                //{
                str = "sick (";
                str += to_string(status);
                str += " to go)";
                //}
                return str;
            }
        }
        void update(){
            if(status > 1){
                //#pragma omp single
                status -= 1;
            }
            else if(status == 1){
                //#pragma omp single
                status = -1;
                }
        }
        void infect(int n){
            if(status == 0){
                //#pragma omp single
                status = n;
            }
        }
        bool is_stable(){
            if(status == -1 || status == -2)
                return true;
            return false;
        }
        int get_status(){
            return status;
        }
        void vaccinate(){
            //#pragma omp single
            status = -2;
        }
};

class Population{
    private:
        int num;
        float prob;
        vector<Person> people;
        void infect_neighbors(int index){
            if(index-1 >= 0){
                if(people[index-1].get_status() == 0){
                    float bad_luck = (float) rand()/(float)RAND_MAX;
                    if (bad_luck<prob)
                        people[index-1].infect(5);
                }
            }
            if(index+1 < num){
                if(people[index+1].get_status() == 0){
                    float bad_luck = (float) rand()/(float)RAND_MAX;
                    if (bad_luck<prob)
                        people[index+1].infect(5);
                }
            }
        }
        void random_interaction(int index, int amount){
            while(amount > 0){
                int random_person = rand() % num + 1;
                if(random_person != index){
                    if(!people[index].is_stable() || !people[random_person].is_stable()){
                        if(people[index].get_status() > 0){
                            float bad_luck = (float) rand()/(float)RAND_MAX;
                            if(bad_luck<prob && people[random_person].get_status() == 0)
                                people[random_person].infect(5);
                        }
                        if(people[random_person].get_status() > 0){
                            float bad_luck = (float) rand()/(float)RAND_MAX;
                            if(bad_luck<prob && people[index].get_status() == 0)
                                people[index].infect(5);
                        }
                    }
                    amount--;
                }
            }
        }
    public:
        void population(int npeople){
            num = npeople;
            people = vector<Person>(npeople);
        }
        void random_infection(){
            while(true){
                int person = rand() % num + 1;
                if(people[person].get_status() == 0){
                    people[person].infect(5);
                    break;
                }
            }
        }
        int count_infected(){
            int total = 0;
            int i = 0;
            #pragma omp parallel for private(i) reduction(+:total)
            for(i = 0; i < num; i++){
                if(/*people[i].get_status() > 0*/ people[i].status > 0)
                    total++;
            }
            return total;
        }
        void update(){
            int i = 0;
            #pragma omp parallel for private(i)
            for(i = 0; i < num; i++){
                //people[i].update();
                if(people[i].status > 1){
                    //#pragma omp single
                    people[i].status -= 1;
                }
                else if(people[i].status == 1){
                    //#pragma omp single
                    people[i].status = -1;
                }
            }
        }
        void transmission(){
            int i = 0;
            #pragma omp parallel for private(i)
            for(i = 0; i < num; i++){
                if(people[i].get_status()>0){
                    bool already_infected = false;
                    if(i+1 < num)
                        already_infected = people[i+1].get_status()>0;
                    infect_neighbors(i);
                    if(!already_infected)
                        i++;    // To prevent continuous spread to the right
                }
            }
        }
        void spreading(){
           int i = 0;
           int amount = 6;
           #pragma omp parallel for private(i)
           for(i = 0; i < num; i++){
                random_interaction(i, 6);
                /*amount = 6;
                while(amount > 0){
                int random_person = rand() % num + 1;
                if(random_person != index){
                    if(!people[index].is_stable() || !people[random_person].is_stable()){
                        if(people[index].get_status() > 0){
                            float bad_luck = (float) rand()/(float)RAND_MAX;
                            if(bad_luck<prob && people[random_person].get_status() == 0)
                                people[random_person].infect(5);
                        }
                        if(people[random_person].get_status() > 0){
                            float bad_luck = (float) rand()/(float)RAND_MAX;
                            if(bad_luck<prob && people[index].get_status() == 0)
                                people[index].infect(5);
                        }
                    }
                    amount--;
                }
                }*/
           }
        }
        void display(int step, int sick){
            cout << "In step " << step << " #sick: " << sick /*<< " : "*/;
            
            // Used to display visual representation of population.
            /*for(int i = 0; i < num; i++){
                if(people[i].get_status() == -1){
                    cout << "- ";
                }
                else if(people[i].get_status() == 0){
                    cout << "? ";
                }
                else if(people[i].get_status() == -2){
                    cout << "v ";
                }
                else{
                    cout << "+ ";
                }
            }*/
            cout << endl;
        }
        void set_probability_of_transfer(float probability){
            prob = probability;
        }
        void vaccinate(float vac){
            int vaccinated = vac*num;
            cout << "Amount vaccinated: "<< vaccinated << endl;
            while(vaccinated > 0){
                int person = rand() % num + 1;
                if(people[person].get_status() != -2){
                    people[person].vaccinate();
                    vaccinated--;
                }
            }
        }
};
