//
//  stock.hpp
//  6883Project
//
//  Created by Xiang Zou on 12/1/18.
// 

#ifndef stock_hpp
#define stock_hpp
#include <stdio.h>
#include <string>
#include <iostream>
#include <map>
#include <vector>
#include "tradeData.hpp"
#include "asset.hpp"
using namespace std;

// the derived class from Asset to store each stock

class Stock:public Asset
{
private:
    string sEPSdate; // day0 (EPS annoucment day)
    string startDate; // day -120, 120 calendar days before EPS annoucment day
    string endDate; // day 120, 120 calendar days after EPS annoucment day
    double eEPS; //estimated eps
    double aEPS; // actual eps
    double surprise; // actural EPS /estimate EPS - 1, used to classify groups
    bool validDataCheck; // used for clearing 0 data and trading data which have not enough data
    vector<double> ARet; // abnormal return
    string groupName;

    
public:
    Stock() : Asset() {} // default constructor
    Stock(string symbol_, const vector<TradeData>& trades_): Asset(symbol_,trades_){} // constructor with parameters

    void SetSurp(double surprise_){surprise=surprise_;}; // setter function for surpirse
    double GetSurp(){ return surprise;} // getter function for surprise
    bool GetCheck(){ return validDataCheck;} // getter function for validDataCheck
    string GetStartDate() const { return startDate; } // getter function for startDate
    string GetEndDate() const {return endDate;} // getter function for endDate
    const vector<double> & GetARetMap() const {return ARet;} // getter function for ARet
    void SetGroupName(string name_){groupName = name_;}// set the group name beat miss or meet
    
    void SetMapARet(Asset & Index); // fill in abnormal return Map ARet
    void SetReportDate(string Eps_date,vector<string>& calendar); // define day0 and set startDate/endDate
    void CheckValidData(); // function for data cleaning
    void CalculateSurp(string BEstEPS_, string EPS_); // calculate surprise value
    
    void Display();
};



#endif /* stock_hpp */
