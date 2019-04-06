//
//  Data.h
//  6883Project
//
//  Created by Xiang Zou on 12/1/18.
//  Update: 12/11/2018 add DisplayPriceData() function
//

#ifndef fillData_hpp
#define fillData_hpp
#include <stdio.h>
#include "stock.hpp"
#include <sstream>
#include <vector>
#include <locale>
#include <iomanip>
#include <mutex>
#include <thread>
#include "curl/curl.h"

struct MemoryStruct {
    char *memory;
    size_t size;
};

// data class to get data from yahoo finance and store the data in the class
class HistDataFromYahoo{
private:
    string sCookies;
    string sCrumb;
    map<string,Stock> stockMap;
    Asset IWB;

public:
    HistDataFromYahoo():sCookies(""),sCrumb(""){}; // default constructor
    
    const map<string,Stock> & GetStockMap() const {return stockMap;} // getter function of stockMap
    void GetCookieAndCrumb(); // getter function of CookieAndCrumb
    void setStockTag(string ticker,string tag); 
    int FillStockData(map<string,Stock> &s);
    int FillAssetData();// return for IWB
    

        
    
    //for Multithreading
    HistDataFromYahoo operator+(const HistDataFromYahoo& histData){
        for(map<string,Stock>::const_iterator itr = histData.stockMap.begin(); itr != histData.stockMap.end(); itr++){
            this->stockMap[itr->first]=itr->second;
        }
        return *this;
    }
    
    int parellel(map<string,Stock>&s);
};






#endif /* Data_hpp */
