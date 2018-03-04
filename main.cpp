//
// Created by Rui Zhou on 2/3/18.
//

#include <string>
#include <fstream>
#include <iostream>
#include "error_monitor.h"
#include "feed_handler.h"

int main(int argc, char **argv)
{
    CS::FeedHandler feed;
    std::string line;
    const std::string filename(argv[1]);
    std::ifstream infile(filename.c_str(), std::ios::in);
    int counter = 0;

    while (std::getline(infile, line)) {
        feed.ProcessMessage(line);
        if (++counter % 10 == 0) {
            feed.PrintCurrentOrderBook(std::cout);
            CS::ErrorMonitor::GetInstance().PrintStats();
        }
    }
    feed.PrintCurrentOrderBook(std::cout);
    return 0;
}
