//
//  stock.cpp
//  6883Project
//
//  Created by Xiang Zou on 12/1/18.
//

#include "stock.hpp"
#include <math.h>


// function to define day0 and set startDate/endDate
void Stock::SetReportDate(string EpsDate,vector<string> & calendar)
{
    sEPSdate = EpsDate;
    int day0 = 0;
    // based on the EpsDate, find the closed trading date based on the calendar and define it as day0
    // as one company(ON) released its report on weekends
    for(int i = 0;i < calendar.size(); i++)
    {
        if(calendar[i] == EpsDate)
        {
            day0 = i;
        }
    }
    startDate = calendar[day0 - 120]; // set startDate, day0 - 120
    endDate = calendar[day0 + 120]; // set endDate, day0 + 120
}

//calculate surprise
void Stock::CalculateSurp(string BEstEPS_, string EPS_)
{
    double estimateEPS=strtod(BEstEPS_.c_str(), NULL); // convert string into double
    double acturalEPS=strtod(EPS_.c_str(), NULL); // convert string into double
    eEPS = estimateEPS;
    aEPS = acturalEPS;
    //if surprise is nan(if estimateEPS and acturalEPS are with opposite signs, we set surprise nan, then data will be ignored.
    if(estimateEPS*acturalEPS<0.0)
    {
        surprise=0.0/0.0; // nan
    }
    else{
        surprise = acturalEPS/estimateEPS-1; // surprise = actural EPS /estimate EPS - 1
    }
    
}

// fill in abnormal return (AR) Map ARet
// MapARet : key is start date, value is AR
void Stock::SetMapARet(Asset & Index)
{
    vector<TradeData> index=Index.getTrades();
    for(vector<TradeData>::iterator itr=index.begin();itr!=index.end();itr++)
    {
        if(itr->GetDate()==startDate)
        {
            
            for(auto itrtrades=trades.begin();itrtrades!=trades.end();itrtrades++)
            {
                double aret=itrtrades->GetRet()-itr->GetRet(); // ARet = Ri-Rmt, Rmt is the return fo IWB
                ARet.push_back(aret); // ARet becomes a vector contains 240 days of abnormal return
                itr++;
            }
            break;
        }
    }
}

// function for data cleaning
// if the result is fasle, these stock data will not be selected into the groupdata.
void Stock::CheckValidData()
{
    bool tempCheck= true;
    // or if trading days are not above 240, data will also be igored
    if(isnan(surprise) || ARet.size()<240)
        tempCheck=false;
    
    // if download data is missing, then data will be igored
    for(auto itr=ARet.begin();itr!=ARet.end();itr++)
    {
        if(isnan(*itr)|| isinf(*itr))
        {
            tempCheck=false;
            break;
        }
    }
    
    validDataCheck=tempCheck; // store validDataCheck
}

void Stock::Display()
{
    cout<<"You are looking for " <<this->symbol<<endl;
    cout<<"This stock's eps is released on "<<this->sEPSdate<<endl;
    cout<<"The estimated EPS is "<<this->eEPS<<endl;
    cout<<"The actual EPS is " <<this->aEPS<<endl;
    cout<<"The surprise is "<<this->surprise<<endl;
    cout<<"This stock belongs to "<<this->groupName<<" group"<<endl;
    for(auto itr = this->trades.begin();itr!=this->trades.end();itr++)
    {
        cout<<itr->GetDate()<<" "<<itr->GetAdjClosePrice()<<endl;
    }
}
