// Written By Sebastian Matiz

#include <string>
#include <fstream>
#include <utility>
#include <iostream>
#include <vector>
#include <string> 
#include <bits/stdc++.h>
#include <chrono>
#include "label_k_transactions.h"

#define K 5

using namespace std;
using namespace std::chrono;

string dateFolder = "./dates";
string indexFolder = "./price_data_indexing";
string dataFolder = "./unlabeled_price_data_csv";
string labeledDataFolderSeq = "./labeled_price_data_csv_seq"; 

int main(int argc, char * argv[]) {
    ifstream date, index, data;
    ofstream data_label;
    
    date.open(dateFolder + "/dates.txt");
    
    string currDate;
    string currIndexLength;

    auto tik = high_resolution_clock::now();
    int i = 0;
    while (getline(date, currDate)) {
        
        index.open(indexFolder + "/" + currDate + ".csv");
        
        // fill prices vector for week
        vector<double>  prices;
        string price;
        data.open(dataFolder + "/" + currDate + ".csv");
        while(getline(data, price)) {
            prices.push_back(stod(price)); 
        }

        // give indices of days in the week
        int start = 0;
        int end;
        data_label.open(labeledDataFolderSeq + "/" + currDate + ".csv");
        int weeklyMaxProfit = 0;
        while (getline(index, currIndexLength)) {
            int * buyOrSell = (int *) calloc(stoi(currIndexLength), sizeof(int)); 
            end = stoi(currIndexLength);
            vector<double> dailyPartition;
            for (int i = start; i < end; i++) {
                dailyPartition.push_back(prices[i]);
            }
            int x = maxProfit(K, dailyPartition, buyOrSell);
            weeklyMaxProfit+=x;
            int length = dailyPartition.size();
            for (int i =  0; i < length; i++) {
                data_label << prices[i] << "," << buyOrSell[i] << "\n"; 
            }
            start = end;
            
        }
        index.close();
        data.close();
        data_label.close();
        i++;
        cout << "Total Profit for week, " << currDate << ": $" << weeklyMaxProfit << endl; 
    }
    date.close();
    auto tok = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(tok - tik);
    cout << "Total Time Seq: " << duration.count() << " milliseconds" << endl;
    exit(EXIT_SUCCESS); 
}