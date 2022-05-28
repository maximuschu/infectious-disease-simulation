#include <iostream>
#include "disease.h"
#include <omp.h>
using std::cout;
using std::endl;
using std::cin;

int main(){
    double start, end, timing;
    #pragma omp parallel
    {
        printf("This is thread %d\n", omp_get_thread_num());
    }
    #pragma omp parallel
    {
        #pragma omp single
        cout << "Number of threads is: " << omp_get_num_threads() << endl;
    }
    srand(time(NULL));
    cout << "Enter a probability: " << endl;
    float probability;
    cin >> probability;
    Population people;
    cout << "Enter a population size: " << endl;
    int size;
    cin >> size;
    people.population(size);
    people.set_probability_of_transfer(probability);
    cout << "Enter percentage of people vaccinated: " << endl;
    float vaccinate;
    cin >> vaccinate;
    people.vaccinate(vaccinate);
    cout << "Size of population: " << size << endl;
    int step = 1;
    start = omp_get_wtime();
    for( ; ; step++){
        people.update();
        //people.spreading();
        people.transmission();
        if(step == 1)
            people.random_infection();
        people.display(step, people.count_infected());
        if(people.count_infected() == 0)
            break;
    }
    cout << "Disease ran its course by step " << step << endl;
    end = omp_get_wtime();
    timing = (double)(end - start);
    cout << "Time to run simulation (secs): " << timing << endl;
}

