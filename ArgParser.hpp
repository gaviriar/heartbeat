//
//  ArgParser.hpp
//  ArgParser
//
//  Created by Philipp Rouast on 4/06/2016.
//  Copyright © 2016 Philipp Roüast. All rights reserved.
//

#ifndef ArgParser_hpp
#define ArgParser_hpp

#include <stdio.h>
#include <iostream>
#include <vector>
#include <map>
#include <string>

#include "RPPG.hpp"

using namespace std;

/**
 * Argument parser which allows you to parse argumets by index or name
 */
class ArgParser {
    
public:
    
    ArgParser(int argc_, char * argv_[]);
    ~ArgParser(){}

    string get_arg(string s);
    
private:
    
    int argc;
    vector<string> argv;
    map<string, string> switch_map;
    
};

#endif /* ArgParser_hpp */
