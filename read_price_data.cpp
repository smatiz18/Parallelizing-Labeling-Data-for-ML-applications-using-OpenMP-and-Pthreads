// Written By Sebastian Matiz
#include <string>
#include <fstream>
#include <vector>
#include <utility>
#include <iostream>
#include <vector>
#include <bits/stdc++.h>

using namespace std;
int main(int argc, char *argv[]) {
    ifstream priceData;
    // raw data: unix time since epoch,price,amount
    priceData.open("./unlabeled_price_data_csv/ETHUSD.csv");

    // date,price,amount
    string line;     
    
    // price
    string data;     
    // previous date we were on, on a weekly basis
    string prevDate = "./unlabeled_price_data_csv/6-30-2021:EDT.csv";  
    string prevDayIndexing = "./price_data_indexing/6-30-2021:EDT.csv";
    string dates = "./dates/dates.txt";
    int prevDay = 30;
    int currDay;
    // current date that we are on
    string date;    
    ofstream newFile, newFileIndexing, datesFile;
    newFile.open(prevDate);
    newFileIndexing.open(prevDayIndexing);
    
    datesFile.open(dates);
    datesFile << "6-30-2021:EDT\n";
    bool flag = false;
    int tradeCountPerDay = 0;

    while (getline(priceData, line)) {
        stringstream check1(line);
        getline(check1, data, ',');
        time_t t = (time_t) stoi(data);
        struct tm res;
        localtime_r(&t, &res);
        currDay = res.tm_mday;
        date = to_string(res.tm_mon+1);
        date += "-";
        date += to_string(res.tm_mday);
        date += "-";
        date += to_string(1900 + res.tm_year);
        date += ":";
        date += res.tm_zone;
        if (currDay != prevDay && currDay % 7 == 0 && flag == false) {
            prevDay = currDay;
            newFileIndexing << tradeCountPerDay << "\n";
            tradeCountPerDay = 0;
            newFileIndexing.close();
            string csvFileName = "./price_data_indexing/"; 
            csvFileName += date;
            csvFileName += ".csv";
            newFileIndexing.open(csvFileName);
        }
        else if (currDay != prevDay) {
            prevDay = currDay;
            newFileIndexing << tradeCountPerDay << "\n";
        }
        if (currDay % 7 != 0 && flag == true) flag = false;
        if (currDay % 7 == 0 && flag == false) {
            newFile.close();
            datesFile << date << "\n";
            prevDate = date; 
            string csvFileName = "./unlabeled_price_data_csv/"; 
            csvFileName += prevDate;
            csvFileName += ".csv";
            newFile.open(csvFileName);
            flag = true;
        }
        getline(check1, data, ',');
        // newFile << date << "," << data << "\n";  
        newFile << data << "\n";
        tradeCountPerDay++;
    }
    newFile.close();
    newFileIndexing.close();
    datesFile.close();
    priceData.close();
}