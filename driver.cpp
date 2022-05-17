// Written By Sebastian Matiz

#include <string>
#include <fstream>
#include <utility>
#include <iostream>
#include <vector>
#include <string> 
#include <bits/stdc++.h>
#include <pthread.h>
#include <chrono>
#include "label_k_transactions.h"
#include "omp.h"

#define NUM_WEEKS 13
#define K 5


using namespace std;
using namespace std::chrono;

string dateFolder = "./dates";
string indexFolder = "./price_data_indexing";
string dataFolder = "./unlabeled_price_data_csv";
string labeledDataFolder = "./labeled_price_data_csv"; 


struct thread_week_args {
    int thread_id;
    vector<int> partitions;
    string week;
};

void * launchThreadsPerWeek(void * thread_week_args);


int main(int argc, char * argv[]) {
    ifstream date, index, data;
    ofstream data_label;
    
    date.open(dateFolder + "/dates.txt");


    
    string currDate;
    string currIndexLength;

    pthread_t threads[NUM_WEEKS];
    struct thread_week_args twa[NUM_WEEKS];
    int i =  0;
    int thread_create;

    // launch 1 thread for each week
    auto tik = high_resolution_clock::now();
    while (getline(date, currDate)) {
        twa[i].thread_id = i;   
        twa[i].week = currDate;
        index.open(indexFolder + "/" + currDate + ".csv");
        string length;
        vector<int> partitions;
        // give indices of days in the week.. so we know which openMP thread gets what partition
        while (getline(index, currIndexLength)) {
            partitions.push_back(stoi(currIndexLength));
        }
        twa[i].partitions = partitions;
        index.close();
        // launch thread
        thread_create = pthread_create(&threads[i], NULL, launchThreadsPerWeek, (void *) &twa[i]);

        if (thread_create) {
            cout << "Error: unable to create thread, " << thread_create << endl;
            exit(-1);
        }
        i++;
    }
    // join threads
    for (int i = 0; i < NUM_WEEKS; i++) {
        pthread_join(threads[i], NULL); 
    }
    date.close();
    auto tok = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(tok - tik);
    cout << "Total Time Multi-Thread: " << duration.count() << " milliseconds" <<  endl;
    exit(EXIT_SUCCESS);
}

void * launchThreadsPerWeek(void * args) {
    struct thread_week_args *curr_args;
    curr_args = (struct thread_week_args *) args;
    
    vector<int> partitions = curr_args->partitions;
    
    int numThreads = partitions.size();
    int sizeOfFile = partitions[numThreads-1]; 
    
    // populating whole price buff for given week
    string week = curr_args->week;
    string fileLocation = dataFolder + "/" + week + ".csv";
    ifstream data;
    data.open(fileLocation);
    double * prices = (double *) calloc(sizeOfFile, sizeof(double));
    int * buyOrSell = (int *) calloc(sizeOfFile, sizeof(int));
    string currPrice;
    
    
    int i = 0;
    while(getline(data, currPrice)) {
        prices[i] = stod(currPrice);
        i++;
    }

    data.close();

    double totalProfitWeek = 0;
    omp_set_dynamic(0);     // Explicitly disable dynamic teams
    // Spawn numThreads threads for this parallel region only
    #pragma omp parallel num_threads(numThreads)
    {
        int start = omp_get_thread_num() == 0 ? 0 : partitions[omp_get_thread_num()-1];
        int end = partitions[omp_get_thread_num()];
        vector<double> currPartition;
        for (int i = start; i < end; i++) {
            currPartition.push_back(prices[i]);
        }
        int * labels = (int *) calloc(currPartition.size(), sizeof(int));
        int x = maxProfit(K, currPartition, labels);
        

        for (int i = start, j = 0; i < end; i++, j++) {
            buyOrSell[i] = labels[j];
        }
        #pragma omp critical 
        {
            totalProfitWeek += x;
            // cout << "Thread: " << omp_get_thread_num() << ", Max Profit:  $" << x << endl;     
        }
    }
    ofstream labeledData;
    labeledData.open(labeledDataFolder + "/" + week + ".csv");
    for (int i = 0; i < sizeOfFile; i++) {
        labeledData << prices[i] << "," << buyOrSell[i] << "\n"; 
    }
    labeledData.close();
    free(prices);
    free(buyOrSell); 

    cout << "Total Profit for week, " << week << ": $" << totalProfitWeek << endl; 
    return NULL; 
}


