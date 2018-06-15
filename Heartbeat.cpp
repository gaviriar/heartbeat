//
//  Heartbeat.cpp
//  Heartbeat
//
//  Created by Philipp Rouast on 4/06/2016.
//  Copyright © 2016 Philipp Roüast. All rights reserved.
//

#include "Heartbeat.hpp"

#include <opencv2/imgcodecs.hpp>
#include "opencv.hpp"

//using namespace cv;

Heartbeat::Heartbeat(int argc_, char * argv_[], bool switches_on_) {
    argc = argc_;
    argv.resize(argc);
    copy(argv_, argv_ + argc, argv.begin());
    switches_on = switches_on_;

    // map the switches to the actual
    // arguments if necessary
    if (switches_on) {

        vector<string>::iterator it1, it2;
        it1 = argv.begin();
        it2 = it1 + 1;

        while (true) {

            if (it1 == argv.end()) break;
            if (it2 == argv.end()) break;

            if ((*it1)[0] == '-')
                switch_map[*it1] = *(it2);

            it1++;
            it2++;
        }
    }
}

string Heartbeat::get_arg(int i) {

    if (i >= 0 && i < argc)
        return argv[i];

    return "";
}

string Heartbeat::get_arg(string s) {

    if (!switches_on) return "";

    if (switch_map.find(s) != switch_map.end())
        return switch_map[s];

    return "";
}