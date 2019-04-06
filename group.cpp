//
//  group.cpp
//  6883Project
//
//  Created:  2018/12/6 (黄子璇)
//  Update:

#include "group.hpp"
vector<double> operator+(const vector<double>& V,const vector<double>& W)
{
    unsigned long d = V.size();
    vector<double> U(d);
    for (int j=0; j<d; j++) U[j] = V[j] + W[j];
    return U;
}
vector<double> operator/(const vector<double>& V,int n)
{
    unsigned long d = V.size();
    vector<double> U(d);
    for (int j=0; j<d; j++) U[j] = V[j]/n;
    return U;
}



// friend function of class group to selectstocks and fill in stock symmbol and daily AR of each stock to each group
// instead of filling the data one by one, using this function, we can fill all three groups at one time.
void GetAllGroupData(HistDataFromYahoo & data, Group & beat, Group & meet, Group & miss) //call by reference since friend function
{
    map<string,Stock> stockMap= data.GetStockMap();
    
    // using the surprise value to classify the group, also clear the bad data
    for(auto itr=stockMap.begin();itr!=stockMap.end();itr++)
    {
        if((itr->second).GetSurp()>0.015 && (itr->second).GetCheck()) // beat group data fill in
        {
            beat.groupData[(itr->second).getSymbol()]=(itr->second).GetARetMap(); // key: stock symbol, value: AR
            data.setStockTag((itr->second).getSymbol(), "Beat");
        }
        else if((itr->second).GetSurp()<-0.015 && (itr->second).GetCheck()) // miss group data fill in
        {
            miss.groupData[(itr->second).getSymbol()]=(itr->second).GetARetMap();
            data.setStockTag((itr->second).getSymbol(), "Miss");
        }
        else if((itr->second).GetCheck()) // meet group data fill in
        {
            meet.groupData[(itr->second).getSymbol()]=(itr->second).GetARetMap();
            data.setStockTag((itr->second).getSymbol(), "Meet");
        }
    }
}

// sampling the stock by randomly selecting 100 stocks for bootstrapping
// return groupData100Samples
map<string,vector<double>> Group::RandSelection()
{
    map<string,vector<double>> groupData100Samples;
    vector<string> stockNames;
    
    // store stock names in stockNames
    for(auto itr=groupData.begin();itr!=groupData.end();itr++)
    {
        stockNames.push_back(itr->first);
    }
    
    // obtain a time-based seed for shuffle()
    short unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    // uses shuffle function shuffle to stockNames and select front 100 stocks
    shuffle (stockNames.begin(), stockNames.end(), default_random_engine(seed));
    stockNames.resize(100);
    for(vector<string>::iterator itrv=stockNames.begin();itrv!=stockNames.end();itrv++)
    {
        groupData100Samples[*itrv] = groupData[*itrv];
    }
    return groupData100Samples;
 }

// calculation the AAR
// def. of AAR: daily abnormal returns for each group of stocks for all 240 referencs days
vector<double> Group::CalculateAAR(map<string,vector<double>>& groupData100Samples) const
{
    // because when we calcualte the stock price, we directly assign the return of the first day zero.
    // Thus,we have to delete the first elements of vector tempARRT at the end.
    auto totalDays = (groupData100Samples.begin())->second.size();
    vector<double> tempAARt(241);
    int countDay=0;
    while(countDay<totalDays)
    {
        for(auto itr=groupData100Samples.begin();itr!=groupData100Samples.end();itr++)//stock name
        {
            tempAARt[countDay]=tempAARt[countDay]+(itr->second[countDay])/100;
        }
        countDay++;
    }
    tempAARt.erase(tempAARt.begin()); // delete the first element of vector tempARRt
    return tempAARt;
}

// calculate CARR
// def. CARR:cumulate the return on the first T days to CARR;
vector<double> Group::CalculateCAAR(vector<double> & AAR_) const
{
    int countDay=0;
    vector<double> CAARt_(240);
    for(vector<double>::iterator itr=AAR_.begin();itr!=AAR_.end();itr++)//day
    {
        
        if (countDay==0)
            CAARt_[countDay]= *itr;
        else
        {
            CAARt_[countDay]=CAARt_[countDay-1]+(*itr);
        }
        countDay++;
        
    }
    return CAARt_;
}

// we have repeat the process 5 (default n=5) times, we need to calcualte average CAAR
vector<double> Group::CalculateAverageCAAR(int n)
{
    for(int i=0;i<n;i++){
        map<string,vector<double>> groupData100Samples=RandSelection();
        vector<double> aar = CalculateAAR(groupData100Samples);
        AAR.push_back(aar);
        CAAR.push_back(CalculateCAAR(aar));
    }
    vector<double> tempCAAR = CAAR[0];
    vector<double> tempAAR = AAR[0];
    for(int i=1;i<n;i++){
        tempCAAR = tempCAAR+CAAR[i]; // += overloading for matrix operation
        tempAAR =tempAAR+ AAR[i];
    }
    averageCAAR = tempCAAR/n;
    averageAAR = tempAAR/n;
    return averageCAAR;
}

// get CAAR for menu use
vector<double> Group::getCAAR() const
{
    return this->averageCAAR;
}
// get AAR for menu use
vector<double> Group::getAAR() const
{
    return this->averageAAR;
}


