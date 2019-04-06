//
//  plot.hpp
//  6883Project
//
//  Created by xuemengheng on 2018/12/10.
//

#ifndef plot_hpp
#define plot_hpp

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>



//using Gnuplot to draw CAAR trend
void Plot(vector<double> plot_beat, vector<double> plot_meet, vector<double> plot_miss) 
{
    ofstream fout;
    fout.open("CAAR.txt");
    for (int i = 0; i < plot_beat.size(); i++)
    {
         fout << i - 120 << " " << plot_beat[i] << " " << plot_meet[i] << " " << plot_miss[i] << endl;
    }
    fout.close();
    
    fout.open("Plot.txt");
    fout << "set title \"CAAR\"" << endl;
    fout << "plot 'CAAR.txt' using 1:3 title 'Meet' with lines, 'CAAR.txt' using 1:2 title 'Beat' with lines, 'CAAR.txt' using 1:4 title 'Miss' with lines" << endl;
    fout << "pause -1" << endl;
    fout.close();
    system("/opt/local/bin/gnuplot Plot.txt");
}


#endif /* plot_hpp */
