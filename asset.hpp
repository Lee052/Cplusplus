//
//  Asset.hpp
//  6883Project
//
//  Creat: 2018/12/9 Mengheng Xue
//

#ifndef asset_h
#define asset_h

#include "asset.hpp"
#include "tradeData.hpp"
#include <vector>
#include <iostream>
using namespace std;


// basis class asset to store the IWB Index
class Asset
{
protected: // use protect to avoid get set function in the derived Stock class
    string symbol; // stock name
    vector<TradeData> trades; // vector of tradedata objects
    
public:
    Asset(): symbol("") {} // default constructor
    Asset(string symbol_, const vector<TradeData>& trades_): symbol(symbol_), trades(trades_){} // constructor with parameters
    
    void SetSymbol(string symbol_) { symbol = symbol_; } // set Symbol function
    void SetTrades(vector<TradeData> trades_) {trades = trades_;}; // set function for trades
    string getSymbol() const { return symbol; } // getter function for symbol
    vector<TradeData> getTrades() const { return trades; } // getter function for trades
};

#endif /* Asset_h */

