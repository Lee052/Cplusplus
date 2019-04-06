//
//  Data.cpp
//  6883Project
//
//  Created by Xiang Zou on 12/1/18.
//
#include <string.h>
#include <stdlib.h>
#include "fillData.hpp"
#include <unistd.h>
#include <functional>
#include <iostream>
#include<algorithm>
using namespace std;

size_t write_data(void *ptr, int size, int nmemb, FILE *stream)
{
    size_t written;
    written = fwrite(ptr, size, nmemb, stream);
    return written;
}

void *myrealloc(void *ptr, size_t size)
{
    /* There might be a realloc() out there that doesn't like reallocing
     NULL pointers, so we take care of it here */
    if (ptr)
        return realloc(ptr, size);
    else
        return malloc(size);
}


size_t write_data2(void *ptr, size_t size, size_t nmemb, void *data)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)data;
    
    mem->memory = (char *)myrealloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory) {
        memcpy(&(mem->memory[mem->size]), ptr, realsize);
        mem->size += realsize;
        mem->memory[mem->size] = 0;
    }
    return realsize;
}

string getTimeinSeconds(string Time)
{
    std::tm t = {0};
    std::istringstream ssTime(Time);
    char time[100];
    memset(time, 0, 100);
    if (ssTime >> std::get_time(&t, "%Y-%m-%dT%H:%M:%S"))
    {
        //cout << std::put_time(&t, "%c %Z") << "\n" << std::mktime(&t) << "\n";
        sprintf (time, "%ld", mktime(&t));
        return string(time);
    }
    else
    {
        cout << "Parse failed\n";
        return "";
    }
}

//run this function at the beginning to get Cookies and Crumb(only run one time)
void HistDataFromYahoo::GetCookieAndCrumb()
{
    struct MemoryStruct data;
    data.memory = NULL;
    data.size = 0;
    FILE *fp;
    CURL * handle;
    CURLcode result;
    curl_global_init(CURL_GLOBAL_ALL);
    handle = curl_easy_init();
    if (handle)
    {

        curl_easy_setopt(handle, CURLOPT_URL, "https://finance.yahoo.com/quote/AMZN/history");
        curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
        curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);
        curl_easy_setopt(handle, CURLOPT_COOKIEJAR, "cookies.txt");
        curl_easy_setopt(handle, CURLOPT_COOKIEFILE, "cookies.txt");
        curl_easy_setopt(handle, CURLOPT_ENCODING, "");
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data2);
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&data);
        
        /* perform, then store the expected code in 'result'*/
        result = curl_easy_perform(handle);
        
        /* Check for errors */
        if (result != CURLE_OK)
        {
            /* if errors have occured, tell us what is wrong with 'result'*/
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
            //return 1;
        }
        
        char cKey[] = "CrumbStore\":{\"crumb\":\"";
        char *ptr1 = strstr(data.memory, cKey);
        char *ptr2 = ptr1 + strlen(cKey);
        char *ptr3 = strstr(ptr2, "\"}");
        if (ptr3 != NULL)
            *ptr3 = NULL;
        
        sCrumb = ptr2;
        fp = fopen("cookies.txt", "r");
        char cCookies[100];
        if (fp) {
            while (fscanf(fp, "%s", cCookies) != EOF);
            fclose(fp);
        }
        else
            cerr << "cookies.txt does not exists" << endl;
        
        sCookies = cCookies;
        free(data.memory);
        data.memory = NULL;
        data.size = 0;
    }
}

//get data from yahoo, input is a map storing each stock, this function would retrive the tradeData from yahoo( it's single thread)
//after run this function, we will get all the trade data, return data in stockmap.
int HistDataFromYahoo::FillStockData(map<string,Stock> &s)
{
    //in oreder to calculate the AR for each stock, we need get the index return first.
    FillAssetData();
    map<string,Stock>::iterator itr = s.begin();
    
    /* declaration of an object CURL */
    CURL *handle;
    
    /* result of the whole process */
    CURLcode result;
    
    /* the first functions */
    /* set up the program environment that libcurl needs */
    curl_global_init(CURL_GLOBAL_ALL);
    handle = curl_easy_init();
    /* curl_easy_init() returns a CURL easy handle that you're gonna reuse in other easy functions*/
    //handle = curl_easy_init();
    /* if everything's all right with the easy handle... */
    if (handle)
    {
        if(sCookies.length() == 0 || sCrumb.length() == 0)
        {
            GetCookieAndCrumb();
        }
        while (true)
        {
            struct MemoryStruct data;
            data.memory = NULL;
            data.size = 0;
            
            if (itr == s.end())
                break;
            string startDate = itr->second.GetStartDate();
            string endDate = itr->second.GetEndDate();
            string startTime = getTimeinSeconds(startDate.append("T16:00:00"));
            string endTime = getTimeinSeconds(endDate.append("T16:00:00"));
            string urlA = "https://query1.finance.yahoo.com/v7/finance/download/";
            string symbol = itr->first;
            //cout << symbol << endl;
            string urlB = "?period1=";
            string urlC = "&period2=";
            string urlD = "&interval=1d&events=history&crumb=";
            
            string url = urlA + symbol + urlB + startTime + urlC + endTime + urlD + sCrumb;
            const char * cURL = url.c_str();
            const char * cookies = sCookies.c_str();
            curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
            curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);
            curl_easy_setopt(handle, CURLOPT_COOKIE, cookies);   // Only needed for 1st stock
            curl_easy_setopt(handle, CURLOPT_URL, cURL);
            curl_easy_setopt(handle, CURLOPT_COOKIEFILE, "cookies.txt");
            curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data2);
            curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&data);
            result = curl_easy_perform(handle);
            
            /* Check for errors */
            if (result != CURLE_OK)
            {
                /* if errors have occurred, tell us what is wrong with 'result'*/
                fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
                return 1;
            }
            
            stringstream sData(data.memory);
            string line;
            string sValue, sDate;
            vector<TradeData> trades;
            double dValue;
            
            getline(sData, line);
            //cout << line << endl;
            getline(sData, line);
            sDate = line.substr(0, line.find_first_of(','));
            line.erase(line.find_last_of(','));
            sValue = line.substr(line.find_last_of(',') + 1);
            dValue = strtod(sValue.c_str(), NULL);
            if (dValue>10000) {
                cout<< dValue;  dValue = 0 ;
            }
            
            TradeData aTrade(sDate,dValue);
            trades.push_back(aTrade);

            
            for (; getline(sData, line); ){
                //cout << line << endl;
                sDate = line.substr(0, line.find_first_of(','));
                line.erase(line.find_last_of(','));
                sValue = line.substr(line.find_last_of(',') + 1);
                dValue = strtod(sValue.c_str(), NULL);
                if (dValue>10000) {
                    cout<< dValue;  dValue = 0 ;
                }
        
                TradeData aTrade(sDate,dValue);
                trades.push_back(aTrade);
                };
            
            //calculate the daily return
            double returns;
            vector<TradeData>::iterator itrtrades=trades.begin();
            itrtrades->SetRevenue(NULL);
            itrtrades++;
            
            for( ;itrtrades!=trades.end();itrtrades++)
            {
                returns=itrtrades->GetAdjClosePrice()/(itrtrades-1)->GetAdjClosePrice()-1;
                itrtrades->SetRevenue(returns);
            }
            
            if (!trades.empty()){
                //fill in data in stockmap from s
                stockMap[symbol] = s[symbol];
                //fill in trades data in stockmap from trades, which download from yahoo
                stockMap[symbol].SetTrades(trades);
                //fill in AR data in stockmap
                stockMap[symbol].SetMapARet(IWB);
                //fill in surprise data in stockmap from s
                stockMap[symbol].SetSurp(s[symbol].GetSurp());
                //fill in CheckVaildData in stockmap
                stockMap[symbol].CheckValidData();
            }
            else{
                cout<<symbol<<"no data received"<<endl;
            }
            
            
            free(data.memory);
            data.size= 0;
            itr++;
        }
    }
    else
    {
        fprintf(stderr, "Curl init failed!\n");
        return 1;
    }
    
    /* cleanup since you've used curl_easy_init */
    //curl_easy_cleanup(handle);
    curl_easy_cleanup(handle);
    /* this function releases resources acquired by curl_global_init() */
    curl_global_cleanup();
    return 0;
}

//this function using to download the index data
int HistDataFromYahoo::FillAssetData()
{
    /* declaration of an object CURL */
    CURL *handle;
    
    /* result of the whole process */
    CURLcode result;
    
    /* the first functions */
    /* set up the program environment that libcurl needs */
    curl_global_init(CURL_GLOBAL_ALL);
    handle = curl_easy_init();
    /* curl_easy_init() returns a CURL easy handle that you're gonna reuse in other easy functions*/
    //handle = curl_easy_init();
    /* if everything's all right with the easy handle... */
    if (handle)
    {
        if(sCookies.length() == 0 || sCrumb.length() == 0)
        {
            GetCookieAndCrumb();
        }
            struct MemoryStruct data;
            data.memory = NULL;
            data.size = 0;
        
            string startTime = getTimeinSeconds("2015-12-01T16:00:00");
            string endTime = getTimeinSeconds("2018-12-05T16:00:00");
            string urlA = "https://query1.finance.yahoo.com/v7/finance/download/";
            string symbol = "IWB";
            //cout << symbol << endl;
            string urlB = "?period1=";
            string urlC = "&period2=";
            string urlD = "&interval=1d&events=history&crumb=";
            
            string url = urlA + symbol + urlB + startTime + urlC + endTime + urlD + sCrumb;
            const char * cURL = url.c_str();
            const char * cookies = sCookies.c_str();
            curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
            curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);
            curl_easy_setopt(handle, CURLOPT_COOKIE, cookies);   // Only needed for 1st stock
            curl_easy_setopt(handle, CURLOPT_URL, cURL);
            curl_easy_setopt(handle, CURLOPT_COOKIEFILE, "cookies.txt");
            curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data2);
            curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&data);
            result = curl_easy_perform(handle);
            
            /* Check for errors */
            if (result != CURLE_OK)
            {
                /* if errors have occurred, tell us what is wrong with 'result'*/
                fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
                return 1;
            }
            
            stringstream sData(data.memory);
            string line;
            string sValue, sDate;
            vector<TradeData> trades;
            double dValue;
            
            getline(sData, line);
            //cout << line << endl;
            getline(sData, line);
            sDate = line.substr(0, line.find_first_of(','));
            line.erase(line.find_last_of(','));
            sValue = line.substr(line.find_last_of(',') + 1);
            dValue = strtod(sValue.c_str(), NULL);
            if (dValue>10000) {
                cout<< dValue;  dValue = 0 ;
            }
            
            TradeData aTrade(sDate,dValue);
            trades.push_back(aTrade);
            
            
            for (; getline(sData, line); ){
                //cout << line << endl;
                sDate = line.substr(0, line.find_first_of(','));
                line.erase(line.find_last_of(','));
                sValue = line.substr(line.find_last_of(',') + 1);
                dValue = strtod(sValue.c_str(), NULL);
                if (dValue>10000) {
                    cout<< dValue;  dValue = 0 ;
                }
                
                TradeData aTrade(sDate,dValue);
                trades.push_back(aTrade);
            };
            
            double returns;
            vector<TradeData>::iterator itrtrades=trades.begin();
            itrtrades->SetRevenue(NULL);
            itrtrades++;
            
            for( ;itrtrades!=trades.end();itrtrades++)
            {
                returns=itrtrades->GetAdjClosePrice()/(itrtrades-1)->GetAdjClosePrice()-1;
                itrtrades->SetRevenue(returns);
            }
            
            if (!trades.empty()){
                //cout << symbol<<endl;
                IWB.SetSymbol("IWB");
                IWB.SetTrades(trades);
            }
            else{
                cout<<symbol<<"no data received"<<endl;
            }
            
            
            free(data.memory);
            data.size= 0;
        
    }
    else
    {
        fprintf(stderr, "Curl init failed!\n");
        return 1;
    }
    
    /* cleanup since you've used curl_easy_init */
    //curl_easy_cleanup(handle);
    curl_easy_cleanup(handle);
    /* this function releases resources acquired by curl_global_init() */
    curl_global_cleanup();
    return 0;
}


//multi thread to get the data
int HistDataFromYahoo::parellel(map<string,Stock>&s){
    unsigned long const length = distance(s.begin(),s.end());
    if(!length){
        return 1;
    }
    
    unsigned long const min_per_thread=25;
    unsigned long const max_threads=(length+min_per_thread-1)/min_per_thread;   //max_threads
    unsigned long const hardware_threads=thread::hardware_concurrency();   //CPU cores
    //unsigned long const num_threads=6;
    unsigned long const num_threads=std::min(hardware_threads!=0?hardware_threads:2,max_threads);//num_of_threads
    
    unsigned long const block_size=length/num_threads;
    vector<HistDataFromYahoo> results(num_threads);
    
    map<string,Stock>::iterator block_start= s.begin();
    vector<thread> v_threads(num_threads-1);
    cout<< "Multithreading loading! Number of threads: " << num_threads << endl;
    for(unsigned long i=0;i <num_threads-1;++i)
    {
        
        map<string,Stock>::iterator block_end=block_start;
        advance(block_end,block_size);
        map<string,Stock> subMap;
        for(map<string,Stock>::iterator itr= block_start; itr != block_end; itr++){
            subMap[itr->first] = itr->second;
        }
        v_threads[i]= std::thread(std::bind(&HistDataFromYahoo::FillStockData, &results[i],subMap));
        block_start=block_end;
    }
    
    map<string,Stock> resMap;
    for(map<string,Stock>::iterator itr= block_start; itr != s.end(); itr++){
        resMap[itr->first] = itr->second;
    }
    
    HistDataFromYahoo::FillStockData(resMap);    //calculate the rest
    
    for_each(v_threads.begin(),v_threads.end(),mem_fn(&std::thread::join));//wait until all threads join
    for(int i=0; i<num_threads-1; i++){
        *this = *this + results[i];
    }
    
    
    cout<<"Loading Completed!"<<endl;
    cout<<"Waiting for next instruction!"<<endl;
    return 0;
}

void HistDataFromYahoo::setStockTag(string ticker, string tag)
{
    (*this).stockMap[ticker].SetGroupName(tag);
}
