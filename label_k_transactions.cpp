// Inspired by https://leetcode.com/problems/best-time-to-buy-and-sell-stock-iv/discuss/54118/C%2B%2B-Solution-with-O(n-%2B-klgn)-time-using-Max-Heap-and-Stack
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <pthread.h>
#include <omp.h>
#include <vector>
#include <queue>
#include <stack>
#include "label_k_transactions.h"
using namespace std;

class buySellpair {
    public:
    double profit;
    int buyIndex, sellIndex;
};

struct profitOrdering {
  bool operator()(const buySellpair& lhs, const buySellpair& rhs) const
  {
    return lhs.profit < rhs.profit;
  }
};

double maxProfit(int k, vector<int> &prices);

// this main function was used for testing outside of main program
// vector<double> price_data = {3,2,6,5,0,3,5,0,20,5,5,6,7,15,21,4,3,21,18,40};

// int main(int argc, char *argv[]) {    
//     int *buySellBuff = (int *) calloc(price_data.size(), sizeof(int));
//     double max = maxProfit(3, price_data, buySellBuff);
//     int length = price_data.size();
//     for (int i = 0; i < length; i++) {
//         cout << buySellBuff[i] << ", ";
//     }
//     cout << "Max Profit: " << max <<  endl; 
// }
 
double maxProfit(int k, vector<double> &prices, int *buySellBuff) {
    int n = (int)prices.size(), v, p = 0;
    double ret = 0;
    priority_queue<buySellpair, vector<buySellpair>, profitOrdering>  profits;
    stack<pair<int, int> > vp_pairs;
    while (p < n) {
        // find next valley/peak pair
        for (v = p; v < n - 1 && prices[v] >= prices[v+1]; v++);
        for (p = v + 1; p < n && prices[p] >= prices[p-1]; p++);
        // save profit of 1 transaction at last v/p pair, if current v is lower than last v
        while (!vp_pairs.empty() && prices[v] < prices[vp_pairs.top().first]) {
            buySellpair curr;
            curr.buyIndex = vp_pairs.top().first;
            curr.sellIndex = vp_pairs.top().second-1;
            curr.profit = prices[vp_pairs.top().second-1] - prices[vp_pairs.top().first];
            profits.push(curr);
            vp_pairs.pop();
        }
        // save profit difference between 1 transaction (last v and current p) and 2 transactions (last v/p + current v/p),
        // if current v is higher than last v and current p is higher than last p
        while (!vp_pairs.empty() && prices[p-1] >= prices[vp_pairs.top().second-1]) {
            buySellpair curr;
            curr.buyIndex = v;
            curr.sellIndex = vp_pairs.top().second-1;
            curr.profit = prices[vp_pairs.top().second-1] - prices[v];
            profits.push(curr);
            v = vp_pairs.top().first;
            vp_pairs.pop();
        }
        vp_pairs.push(pair<int, int>(v, p));
    }
    // save profits of the rest v/p pairs
    while (!vp_pairs.empty()) {
        buySellpair curr;
        curr.buyIndex = vp_pairs.top().first;
        curr.sellIndex = vp_pairs.top().second-1;
        curr.profit = prices[vp_pairs.top().second-1] - prices[vp_pairs.top().first];
        profits.push(curr);
        vp_pairs.pop();
    }
    // sum up first k highest profits
    for (int i = 0; i < k && !profits.empty(); i++) {
        ret += profits.top().profit;
        buySellBuff[profits.top().buyIndex] = 1;
        buySellBuff[profits.top().sellIndex] = -1;
        // cout << "Buy: " << profits.top().buyIndex 
        //      << ", Sell: " << profits.top().sellIndex 
        //      << ", Profit: " << prices[profits.top().sellIndex] - prices[profits.top().buyIndex] 
        //      << endl;
        
        profits.pop();
    }
    return ret;
}
