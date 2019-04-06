//
//  LoadData.hpp
//  6883Project
//
//  Created by xuemengheng on 2018/12/10.
//

#ifndef loadData_h
#define loadData_h

#include <iostream>
#include "stock.hpp"
#include <fstream>

//extract useful data from eps.csv
//the stock list, stock announcement date, stock aEPS and stock eEPS information are all from eps.csv
void StockLoading(map<string, Stock>& stockList,vector<string>& calendar)
{
    ifstream file("eps.csv");
    string line;
    int num = 0;
    getline(file, line,'\r');
    while (getline(file, line,'\r')) {
        istringstream ss(line);
        string cur;
        Stock curs;
        getline(ss, cur, ',');
        cur.erase(std::remove(cur.begin(), cur.end(), '\n'), cur.end());
        curs.SetSymbol(cur);
        getline(ss, cur, ',');
        string date = cur;
        curs.SetReportDate(date,calendar);
        getline(ss, cur, ',');//name
        //getline(ss, cur, ',');//price
        getline(ss, cur, ',');//BEst EPS
        string BEstEPS=cur;
        getline(ss, cur, ',');//EPS
        string EPS=cur;
        //double surprise = strtod(EPS.c_str(), NULL)/strtod(BEstEPS.c_str(), NULL)-1;
        stockList[curs.getSymbol()] = curs;
        stockList[curs.getSymbol()].CalculateSurp(BEstEPS,EPS);
        num++;
    }
    cout << "Finished Loading "<<num<<" Stocks" << endl;
}

//we need use the trading calendar date instead of the normal calendr date when we count the trading date
 // load the calendar to get trading calendar, so that I can get the start date and end date for each stock base on the eps date
void LoadTradingCalendar(vector<string> &calendar)
{
    ifstream file("trading calendar.csv");
    string line;
    getline(file, line,'\r');
    while (getline(file, line,'\r')) {
        string date = line.substr(0, line.find_first_of('\r'));
        calendar.push_back(date);
    }
}

#endif /* LoadData_h */
