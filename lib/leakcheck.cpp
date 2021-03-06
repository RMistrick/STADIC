/******************************************************************************
 * Copyright (c) 2014-2015, The Pennsylvania State University
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

#include "leakcheck.h"
#include "logging.h"
#include <iostream>
#include <fstream>
#include "filepath.h"
#include "stadicprocess.h"
#include "gridmaker.h"
#include "functions.h"

namespace stadic{

LeakCheck::LeakCheck()
{
}
bool LeakCheck::isEnclosed(){
    if (!makeGrid()){
        return false;
    }
    if (!writeExtraRad()){
        return false;
    }

    if (!xformModifiers()){
        return false;
    }

    if (!createOct()){
        return false;
    }

    if (!runCalc()){
        return false;
    }
    std::ifstream iFile;
    iFile.open("Final.res");
    if (!iFile.is_open()){
        STADIC_ERROR("The opening of the results file failed.");
        return false;
    }
    std::string val;
    bool containsLeak=false;
    bool likelyEnclosed=false;
    for (int i=0;i<m_Points.size();i++){
        for (int j=0;j<m_Points[i].size();j++){
            iFile>>val;
            if (toDouble(val)>0 &&toDouble(val)<0.5){
                likelyEnclosed=true;
            }else if (toDouble(val)>=0.5){
                containsLeak=true;
            }
        }
    }
    iFile.close();

    if (containsLeak){
        STADIC_ERROR("The provided model either contains a leak or the provided point is outside the space.");
        return false;
    }else if (likelyEnclosed){
        STADIC_WARNING("The illuminance value is greater than 0 at the analysis point, but less than 0.5.\n\tIt will be assumed that there is no light leak.");
    }else{
        std::cout<<"The model is fully enclosed."<<std::endl;
    }
    return true;
}
//Setters
bool LeakCheck::setRadFile(std::vector<std::string> files){
    for (int i=0;i<files.size();i++){
        if(!isFile(files[i])){
            STADIC_ERROR("The rad file named "+files[i]+" does not exist.");
            return false;
        }
        m_RadFiles.push_back(files[i]);
    }
    return true;
}

void LeakCheck::setFloorLayers(std::vector<std::string> layers){
    m_FloorLayers=layers;
}
bool LeakCheck::setUnits(std::string unit){
    if (unit=="ft"||unit=="in"||unit=="mm"||unit=="m"){
        m_Units=unit;
        return true;
    }
    return false;
}

//Private
bool LeakCheck::makeGrid(){
    GridMaker grid(m_RadFiles);
    grid.setLayerNames(m_FloorLayers);
    if (m_Units=="in"){
        //Inches
        grid.setOffset(24);
        grid.setSpaceX(24);
        grid.setSpaceY(24);
        grid.setOffsetZ(30);
    }else if (m_Units=="ft"){
        //Feet
        grid.setOffset(2);
        grid.setSpaceX(2);
        grid.setSpaceY(2);
        grid.setOffsetZ(2.5);
    }else if (m_Units=="mm"){
        //Millimeters
        grid.setOffset(609.6);
        grid.setSpaceX(609.6);
        grid.setSpaceY(609.6);
        grid.setOffsetZ(762);
    }else{
        //Meters
        grid.setOffset(.6096);
        grid.setSpaceX(.6096);
        grid.setSpaceY(.6096);
        grid.setOffsetZ(.762);
    }
    if (!grid.makeGrid()){
        return false;
    }
    if (!grid.writePTS("Test.pts")){
        return false;
    }
    m_Points=grid.points();
    if (m_Points.empty()){
        STADIC_ERROR("The points vector is empty.");
        return false;
    }
    return true;
}

bool LeakCheck::xformModifiers(){
    std::vector<std::string> arguments;
    arguments.push_back("-m");
    arguments.push_back("modified");
    for (int i=0;i<m_RadFiles.size();i++){
        arguments.push_back(m_RadFiles[i]);
    }
    std::string programName="xform";
    Process xform(programName,arguments);
    xform.setStandardOutputFile("Mod.rad");
    xform.start();

    if (!xform.wait()){
        // This may be rather extreme, but can these failures be recovered from? Failure
        // to find the program is pretty bad, and any other failure indicates misuse of the
        // process class.
        if(xform.state() == Process::BadProgram) {
            STADIC_LOG(Severity::Fatal, "The xform program could not be found.");
        }
        STADIC_LOG(Severity::Fatal, "The xform of the modifiers has failed.");
    }

    return true;
}

bool LeakCheck::writeExtraRad(){
    std::ofstream oFile;
    oFile.open("Extra.rad");
    if (!oFile.is_open()){
        return false;
    }
    oFile<<"void plastic modified"<<std::endl;
    oFile<<"0"<<std::endl<<"0"<<std::endl;
    oFile<<"5 1 1 1 0 0"<<std::endl;
    oFile.close();

    return true;
}

bool LeakCheck::createOct(){
    std::vector<std::string> arguments;
    arguments.push_back("Extra.rad");
    arguments.push_back("Mod.rad");
    std::string programName="oconv";
    Process oconv(programName,arguments);

    oconv.setStandardOutputFile("Test.oct");
    oconv.start();
    if(!oconv.wait()){
        STADIC_ERROR("There was a problem creating the octree.");
        return false;
    }
    return true;
}

bool LeakCheck::runCalc(){
    std::vector<std::string> arguments;
    arguments.push_back("-I");
    arguments.push_back("-h");
    arguments.push_back("-ab");
    arguments.push_back("4");
    arguments.push_back("-ad");
    arguments.push_back("5000");
    arguments.push_back("-as");
    arguments.push_back("3000");
    arguments.push_back("-av");
    arguments.push_back("0");
    arguments.push_back("0");
    arguments.push_back("0");
    arguments.push_back("-aw");
    arguments.push_back("0");
    arguments.push_back("-aa");
    arguments.push_back("0.15");
    arguments.push_back("Test.oct");
    std::string programName="rtrace";
    Process rtrace(programName,arguments);
    rtrace.setStandardInputFile("Test.pts");

    std::vector<std::string> arguments2;
    arguments2.clear();
    arguments2.push_back("-e");
    arguments2.push_back(Process::quote("$1=179*($1*0.265+$2*0.670+$3*0.065)"));
    programName="rcalc";
    Process rcalc(programName,arguments2);
    rtrace.setStandardOutputProcess(&rcalc);
    rcalc.setStandardOutputFile("Final.res");
    rtrace.start();
    rcalc.start();
    if(!rtrace.wait()){
        STADIC_ERROR("The running of rtrace has failed.");
        return false;
    }
    if(!rcalc.wait()){
        STADIC_ERROR("The running of rcalc has failed.");
        return false;
    }
    return true;
}

}
