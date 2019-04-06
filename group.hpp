//
//  group.hpp
//  6883Project
//
//  Created by 黄子璇 on 2018/12/6.
//  Update: 12/11/2018 add DisplayPriceData() function (Bond)


#ifndef group_hpp
#define group_hpp

#include <stdio.h>
#include <iostream>
#include "fillData.hpp"
#include <string>
#include <vector>
#include "stock.hpp"
#include <algorithm>    // std::shuffle
#include <random>       // std::default_random_engine
#include <chrono>       // std::chrono::system_clock

class Group
{
private:
    map<string,vector<double>> groupData;//map<stockname,map<date,AR>> for all stocks belongs this group
    vector<vector<double>> AAR;//store each bootstrap's ARR result
    vector<vector<double>> CAAR;//store each bootstrap's CARR result
    vector<double> averageCAAR;//store the average result of each bootstrap's CARR
    vector<double> averageAAR;//store the average result of each bootstrap's ARR
public:
    Group():averageCAAR(240),averageAAR(240){}; // constructor
    map<string,vector<double>> RandSelection(); // random select 100 stocks
    vector<double> CalculateAAR(map<string,vector<double>> & groupData100Samples) const; // calculation the AAR for each stock
    vector<double> CalculateCAAR(vector<double> & AAR_) const; // calculation the CAAR for each stock
    vector<double> CalculateAverageCAAR(int n =5); // calcualte average of n CAAR results, default n = 5
    
    vector<double> getCAAR() const; // get CAAR for menu use
    vector<double> getAAR() const;// get AAR for menu use
    friend void GetAllGroupData(HistDataFromYahoo & data, Group & beat, Group & meet, Group & miss); // friend function 
};

//selecting stocks and fill in useful data to each group
void GetAllGroupData(HistDataFromYahoo & data, Group & beat, Group & meet, Group & miss);


#endif /* group_hpp */
