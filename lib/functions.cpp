/******************************************************************************
 * Copyright (c) 2014-2015, The Pennsylvania State University
 * Copyright (c) 2015, Jason W. DeGraw
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission of the
 *    respective copyright holder or contributor.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
 * AND NONINFRINGEMENT OF INTELLECTUAL PROPERTY ARE EXPRESSLY DISCLAIMED. IN
 * NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *****************************************************************************/

#include "functions.h"
#include <iostream>
#include <sstream>
#include <boost/optional.hpp>

namespace stadic{
//String splitting function based on a delimiter that returns a vector of standard strings
std::vector<std::string> split(std::string line, char delimiter)
{
    std::vector<std::string> stringList;
    if(line.size() > 0) {
        if(*line.begin() == delimiter) {
            line = std::string(line.begin() + 1, line.end());
        }
        std::stringstream stream(line);
        std::string string;
        while(std::getline(stream, string, delimiter)) {
            stringList.push_back(string);
        }
    }
    return stringList;
}

std::vector<std::string> trimmedSplit(std::string line, char delimiter)
{
    std::vector<std::string> stringList;
    std::stringstream stream(line);
    std::string string;
    while(std::getline(stream, string, delimiter)) {
        stringList.push_back(trim(string));
    }
    return stringList;
}

//String trimming function that clears the white space from the beginning or the end of a string
std::string trim(std::string string)
{
    std::size_t found=string.find_first_not_of(" \t\f\v\n\r");
    if(found >= string.length()) {
        return std::string();
    }
    if (found!=string.npos){
        string.erase(string.begin(),string.begin()+found);
    }
    found=string.find_last_not_of(" \t\f\v\n\r");
    if (found!=string.npos){
        string.erase(string.begin()+found+1,string.end());
    }
    return string;
}

// Generic convert string to double function, mainly in case we need to drop C++11
double toDouble(const std::string &string, bool *ok)
{
    double value;
    try {
        value = std::stod(string);
        if(ok != nullptr) {
            *ok = true;
        }
    }
    catch(std::exception&)
    {
        value = 0;
        if(ok != nullptr) {
            *ok = false;
        }
    }
    return value;
}

// Generic convert string to integer function, mainly in case we need to drop C++11
int toInteger(const std::string &string, bool *ok)
{
    int value;
    try {
        value = std::stoi(string);
        if(ok != nullptr) {
            *ok = true;
        }
    }
    catch(std::exception&)
    {
        value = 0;
        if(ok != nullptr) {
            *ok = false;
        }
    }
    return value;
}

// Break a string at every N characters and insert a new line.
// This implementation relies on the current behavior of split
// in breaking up consecutive spaces:
//
//   "two spaces:  " -> "two", "spaces", ""
//
// Consecutive spaces will be preserved, except at the beginning
// of new lines. To get indentation, use the indent parameter.
std::string wrapAtN(const std::string &text, unsigned N, unsigned indent, bool hangingIndent)
{
    std::string skip;
    if(indent) {
        skip = std::string(indent, ' ');
    }
    std::vector<std::string> list = split(text, ' ');
    std::stringstream stream;
    unsigned length = 0;
    if(!hangingIndent) {
        stream << skip;
        length = indent;
    }
    for(std::string &entry : list) {
        if(length + entry.size() < N) {
            if(length + entry.size() == 0) {
                continue;
            }
            stream << entry;
            length += entry.size();
            if(length + 1 < N) {
                stream << " ";
                length++;
            } else {
                stream << std::endl << skip;
                length = indent;
            }
        } else {
            stream << std::endl << skip << entry;
            length = indent + entry.size();
            if(length + 1 < N) {
                stream << " ";
                length++;
            } else {
                stream << std::endl << skip;
                length = indent;
            }
        }
    }
    return stream.str();
}

std::pair<std::string, std::string> stringPartition(const std::string &string, char delimiter)
{
    std::pair<std::string, std::string> pair;
    std::size_t found = string.find(delimiter);
    if(found >= string.length()) {
        pair.first = string;
        return pair;
    }
    pair.first = std::string(string.begin(), string.begin() + found);
    pair.second = std::string(string.begin()+found+1, string.end());
    return pair;
}

void tokenize(std::queue<std::string> &container, const std::string &string)
{
    std::stringstream stream(string);
    std::string token;
    while(!stream.eof()) {
        stream >> token;
        if(!token.empty()) {
            container.push(token);
        }
    }
}

}
