//
//  tradeData.hpp
//  6883Project
//
//  Created: 12/4/18   Li Ji
//  Update:  12/9/2018

#ifndef tradeData_hpp
#define tradeData_hpp

#include <stdio.h>
#include <string>
#include<fstream>

using namespace std;

// TradeData class to store the trading data for each stock
// the structure is like map<string,double>, each string date corresponding to a double adjusted close price
// it used in asset.hpp to constructor protect member vector<TradeData> trades
class TradeData
{
private:
    double     adjClosePrice; // adjusted close price
    string     sDate; // start date
    double     ret; // return，can not be initialized
public:
    TradeData() : sDate(""), adjClosePrice(0.0) {} // default constructor
    TradeData(string sDate_, double adjClosePrice_) : sDate(sDate_),adjClosePrice(adjClosePrice_){} // constructor with parameters
    
    void SetRevenue(double ret_){ret=ret_;} // set function for return
    double GetAdjClosePrice(){return adjClosePrice;} // get function for AdjClosePrice
    double GetRet(){return ret;} // get function for return
    string GetDate(){return sDate;}; // get function for start date
};

#endif /* tradeData_hpp */


