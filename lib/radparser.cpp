/******************************************************************************
 * Copyright (c) 2015, Alliance for Sustainable Energy
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

#include "radparser.h"
#include "functions.h"
#include <iostream>

namespace stadic {

RadParser::RadParser(std::istream &stream) : m_file(stream), m_linenumber(0)
{
    // Fill the internal stream
    fillStream();
}

bool RadParser::fillStream()
{
    std::string string;
    do {
        if(m_file.get().eof()) {
            //std::cout << "Out of stream to read" << std::endl;
            return false;
        }
        std::getline(m_file.get(), string);
        m_linenumber++;
        //std::cout << "fillStream line " << m_linenumber << ": " << string << std::endl;
        auto parts = stringPartition(string, '#');
        string = trim(parts.first);
    } while(string.empty());
    m_stream = std::stringstream(string);
    return true;
}

boost::optional<std::string> RadParser::next()
{
    std::string string;
    // Try to fill the internal string stream if needed
    if(m_stream.eof()) {
        //std::cout << "stream at EOF" << std::endl;
        if(!fillStream()) {
            return boost::none;
        }
    }
    //std::cout << "Have data in stream..." << std::endl;
    // Try to get something from the internal string stream
    m_stream >> string;
    if(string.empty()) {
        return boost::none;
    }
    return boost::optional<std::string>(string);
}

}
