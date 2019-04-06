//
//  main.cpp
//  6883Project
//
//  Created: 12/4/18   (Xiang Zou)
// Update: 12/05/2018 change  (MENNGHENG)

#include <iostream>
#include "fillData.hpp"
#include "stock.hpp"
#include "group.hpp"
#include "plot.hpp"
#include <fstream>
#include "loadData.hpp"



int main(void) {
    map<string,Stock> s; // change to stock 
    vector<string> calendar;
    LoadTradingCalendar(calendar);
    StockLoading(s,calendar);
    HistDataFromYahoo data;
    Group beat;
    Group meet;
    Group miss;
    vector<vector<vector<double>>> matrixToStoreAARCAAR;
    vector<vector<double>> matrixBeat,matrixMeet,matrixMiss;
    vector<double> plot_miss,plot_meet,plot_beat;
    
    // ===================MENU=======================
    cout << "==================MENU PART=====================" << endl;
    bool excution = true;
    int option = 0;
    string name;
    map<string,Stock> stockMap;
    cout << "Please selete one option below (1-5):" << endl;
    cout << "1: Retrieve historical price data for all stocks" << endl;
    cout << "2: Information for one stock" << endl;
    cout << "3: Show AAR or CAAR for one group" << endl;
    cout << "4: Show the graph with CAAR for all 3 groups" << endl;
    cout << "5: Exit the program" << endl << endl;
    while (excution)
    {
        cin >> option;
        switch (option)
        {
            case 1:{
                data.parellel(s);
                GetAllGroupData(data,beat,meet,miss);
                // get ready for the plotting
                plot_beat=beat.CalculateAverageCAAR();
                plot_meet=meet.CalculateAverageCAAR();
                plot_miss=miss.CalculateAverageCAAR();
                //get ready for retriving the AAR and CAAR
                matrixBeat.push_back(beat.getAAR());
                matrixBeat.push_back(beat.getCAAR());
                matrixMeet.push_back(meet.getAAR());
                matrixMeet.push_back(meet.getCAAR());
                matrixMiss.push_back(miss.getAAR());
                matrixMiss.push_back(miss.getCAAR());
                matrixToStoreAARCAAR.push_back(matrixBeat);
                matrixToStoreAARCAAR.push_back(matrixMeet);
                matrixToStoreAARCAAR.push_back(matrixMiss);
                continue;
            }
            case 2:{
                cout<<"please enter the stock name you wanted to look"<<endl;
                cin >> name;
                stockMap = data.GetStockMap();
                if(stockMap.count(name)>0)
                {
                    stockMap[name].Display();
                }
                else
                {
                    cout<<"Search failed, there is no such stock. Please double check!"<<endl;
                }
                continue;
            }
            case 3:{
                int x,y;
                cout<<"Please choose which group you want to access from the following"<<endl;
                cout<<"1.Beat     2.Meet     3.Miss"<<endl;
                cin>>x;
                cout<<"Please choose 1.AAR or 2.CAAR"<<endl;
                cin>>y;
                if(x<1||x>3||y<1||y>2)
                {
                    cout<<"You are doing something stupid. Please read carefully"<<endl;
                    continue;
                }
                vector<double> target = matrixToStoreAARCAAR[x-1][y-1];
                for(auto itr = target.begin();itr!=target.end();itr++)
                {
                    cout<<*itr<<endl;
                }
                continue;
            }
            case 4:{
                Plot(plot_beat, plot_meet, plot_miss);
                continue;
            }
            case 5:{
                cout << "Done." << endl;
                excution = false;
                return 0;
            }
            default:{
                cout<<"Please just enter 1-5. Thanks for your cooperation"<<endl;
            }
                
        }
    }
    
    return 0;
    

}

