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

#include "metrics.h"
#include "logging.h"
#include "dayill.h"
#include <fstream>
#include "functions.h"
#include "gridmaker.h"

namespace stadic {
Metrics::Metrics(BuildingControl *model) :
    m_Model(model)
{
}

bool Metrics::processMetrics()
{
    std::vector<std::shared_ptr<Control>> spaces=m_Model->spaces();
    for (int i=0;i<spaces.size();i++){
        DaylightIlluminanceData daylightIll;
        daylightIll.parseTimeBased(spaces[i].get()->spaceDirectory()+spaces[i].get()->resultsDirectory()+spaces[i].get()->spaceName()+".ill");
        //Test whether Daylight Autonomy needs to be calculated
        if (spaces[i].get()->runDA()){
            if(calculateDA(spaces[i].get(), &daylightIll)){
                spaces[i].get()->setCalcDA(false);      //Set calculate to false for DA in control file if returned true
            }
        }

        if (spaces[i].get()->runcDA()){
            if (calculatecDA(spaces[i].get(), &daylightIll)){
                spaces[i].get()->setCalccDA(false);
            }
        }

        if (spaces[i].get()->runDF()){
            if (calculateDF(spaces[i].get(), &daylightIll)){
                spaces[i].get()->setDF(false);
            }
        }

        if (spaces[i].get()->runUDI()){
            if (calculateUDI(spaces[i].get(), &daylightIll)){
                spaces[i].get()->setCalcUDI(false);
            }
        }

        if (spaces[i].get()->runsDA()){
            if (calculatesDA(spaces[i].get(), &daylightIll)){
                spaces[i].get()->setCalcsDA(false);
            }
        }

        if (spaces[i].get()->runOccsDA()){
            if (calculateOccsDA(spaces[i].get(), &daylightIll)){
                spaces[i].get()->setCalcOccsDA(false);
            }
        }
    }

    return true;
}

bool Metrics::calculateDA(Control *model, DaylightIlluminanceData *dayIll)
{
    std::vector<int> pointCount;
    for (int i=0;i<dayIll->illuminance()[0].lux().size();i++){
        pointCount.push_back(0);
    }
    int hourCount=0;
    for (int i=0;i<dayIll->illuminance().size();i++){
        if(m_Occupancy[i]){
            hourCount++;
            if (model->illumUnits()=="lux"){
                for (int j=0;j<dayIll->illuminance()[i].lux().size();j++){
                    if (dayIll->illuminance()[i].lux()[j]>model->DAIllum()){
                        pointCount[j]++;
                    }
                }
            }else{
                for (int j=0;j<dayIll->illuminance()[i].fc().size();j++){
                    if (dayIll->illuminance()[i].fc()[j]>model->DAIllum()){
                        pointCount[j]++;
                    }
                }
            }
        }
    }

    std::ofstream outDA;
    std::string tmpFileName;
    tmpFileName=model->spaceDirectory()+model->resultsDirectory()+model->spaceName()+"_DA.res";
    outDA.open(tmpFileName);
    if (!outDA.is_open()){
        STADIC_LOG(Severity::Error, "The opening of the Daylight Autonomy results file "+tmpFileName +" has failed.");
        return false;
    }
    for (int i=0;i<dayIll->illuminance()[0].lux().size();i++){
        outDA<<double(pointCount[i])/hourCount<<std::endl;
    }
    outDA.close();
    return true;
}
bool Metrics::calculatecDA(Control *model, DaylightIlluminanceData *dayIll)
{
    std::vector<double> pointCount;
    for (int i=0;i<dayIll->illuminance()[0].lux().size();i++){
        pointCount.push_back(0);
    }
    int hourCount=0;
    for (int i=0;i<dayIll->illuminance().size();i++){
        if(m_Occupancy[i]){
            hourCount++;
            if (model->illumUnits()=="lux"){
                for (int j=0;j<dayIll->illuminance()[i].lux().size();j++){
                    pointCount[j]=pointCount[j]+dayIll->illuminance()[i].lux()[j]/model->cDAIllum();
                }
            }else{
                for (int j=0;j<dayIll->illuminance()[i].fc().size();j++){
                    pointCount[j]=pointCount[j]+dayIll->illuminance()[i].fc()[j]/model->cDAIllum();
                }
            }
        }
    }

    std::ofstream outcDA;
    std::string tmpFileName;
    tmpFileName=model->spaceDirectory()+model->resultsDirectory()+model->spaceName()+"_cDA.res";
    outcDA.open(tmpFileName);
    if (!outcDA.is_open()){
        STADIC_LOG(Severity::Error, "The opening of the Continuous Daylight Autonomy results file "+tmpFileName +" has failed.");
        return false;
    }
    for (int i=0;i<dayIll->illuminance()[0].lux().size();i++){
        outcDA<<double(pointCount[i])/hourCount<<std::endl;
    }
    outcDA.close();

    return true;
}
bool Metrics::calculateDF(Control *model, DaylightIlluminanceData *dayIll)
{

    return true;
}
bool Metrics::calculateUDI(Control *model, DaylightIlluminanceData *dayIll)
{
    std::vector<double> countWithin;
    std::vector<double> countBelow;
    std::vector<double> countAbove;
    for (int i=0;i<dayIll->illuminance()[0].lux().size();i++){
        countWithin.push_back(0);
        countBelow.push_back(0);
        countAbove.push_back(0);
    }
    int hourCount=0;
    for (int i=0;i<dayIll->illuminance().size();i++){
        if(m_Occupancy[i]){
            hourCount++;
            if (model->illumUnits()=="lux"){
                for (int j=0;j<dayIll->illuminance()[i].lux().size();j++){
                    if (dayIll->illuminance()[i].lux()[j]<model->UDIMin()){
                        countBelow[j]++;
                    }else if(dayIll->illuminance()[i].lux()[j]<=model->UDIMax()){
                        countWithin[j]++;
                    }else{
                        countAbove[j]++;
                    }
                }
            }else{
                for (int j=0;j<dayIll->illuminance()[i].fc().size();j++){
                    if (dayIll->illuminance()[i].fc()[j]<model->UDIMin()){
                        countBelow[j]++;
                    }else if(dayIll->illuminance()[i].fc()[j]<=model->UDIMax()){
                        countWithin[j]++;
                    }else{
                        countAbove[j]++;
                    }
                }
            }
        }
    }

    std::ofstream outUDIbelow;
    std::string tmpFileName;
    tmpFileName=model->spaceDirectory()+model->resultsDirectory()+model->spaceName()+"_below_UDI.res";
    outUDIbelow.open(tmpFileName);
    if (!outUDIbelow.is_open()){
        STADIC_LOG(Severity::Error, "The opening of the below UDI results file "+tmpFileName +" has failed.");
        return false;
    }
    for (int i=0;i<dayIll->illuminance()[0].lux().size();i++){
        outUDIbelow<<double(countBelow[i])/hourCount<<std::endl;
    }
    outUDIbelow.close();

    std::ofstream outUDI;
    tmpFileName=model->spaceDirectory()+model->resultsDirectory()+model->spaceName()+"_UDI.res";
    outUDI.open(tmpFileName);
    if (!outUDI.is_open()){
        STADIC_LOG(Severity::Error, "The opening of the UDI results file "+tmpFileName +" has failed.");
        return false;
    }
    for (int i=0;i<dayIll->illuminance()[0].lux().size();i++){
        outUDI<<double(countWithin[i])/hourCount<<std::endl;
    }
    outUDI.close();

    std::ofstream outUDIabove;
    tmpFileName=model->spaceDirectory()+model->resultsDirectory()+model->spaceName()+"_above_UDI.res";
    outUDIabove.open(tmpFileName);
    if (!outUDIabove.is_open()){
        STADIC_LOG(Severity::Error, "The opening of the above UDI results file "+tmpFileName +" has failed.");
        return false;
    }
    for (int i=0;i<dayIll->illuminance()[0].lux().size();i++){
        outUDIabove<<double(countAbove[i])/hourCount<<std::endl;
    }
    outUDIabove.close();

    return true;
}
bool Metrics::calculatesDA(Control *model, DaylightIlluminanceData *dayIll)
{
    //Calculate the area of the floor polygons
    GridMaker gridSize(model->spaceDirectory()+model->geoDirectory()+model->geoFile());
    if (model->identifiers()){
        gridSize.setIdentifiers(model->identifiers().get());
    }else if (model->modifiers()){
        gridSize.setLayerNames(model->modifiers().get());
    }else{
        STADIC_LOG(Severity::Warning, "The calculation of sDA for "+model->spaceName()+" cannot be completed without a list of floor geometry.");
        return false;
    }
    if (!gridSize.calcArea()){
        STADIC_LOG(Severity::Error, "The calculation of the floor area for "+model->spaceName()+" has failed.");
        return false;
    }
    double area;
    area=gridSize.area();
    //Correct all values to be in feet.
    if (model->importUnits()=="mm"){
        area=area*0.00328084*0.00328084;
    }else if (model->importUnits()=="in"){
        area=area/144;
    }else if (model->importUnits()=="m"){
        area=area*3.28084*3.28084;
    }

    //Calculate the target sDA Percentage to stay under with direct sun.
    double sDAPercent=0.02;
    if (area<200){
        sDAPercent=2.0/dayIll->illuminance()[0].lux().size();
    }else if (area<500){
        sDAPercent=4.0/dayIll->illuminance()[0].lux().size();
    }else if (area<1000){
        sDAPercent=6.0/dayIll->illuminance()[0].lux().size();
    }
    if (sDAPercent<0.02){
        sDAPercent=0.2;
    }

    //Create possible shade combinations array
    std::vector<std::vector<bool>> combinations;
    combinations.resize(int(std::pow(2, model->windowGroups().size())));        //Resizes the main vector to the number of combinations
    for (int i=0;i<combinations.size();i++){
        combinations[i].resize(model->windowGroups().size());                   //Resizes the inner vectors to the number of window groups
    }
    for (int i=0;i<model->windowGroups().size();i++){                           //This section fills the first window group with [on, off, on, off,...]
        int count=0;                                                            //The second window group with [on, on, off, off, ...]
        bool shadesEmployed=true;
        for (int j=0;j<combinations.size();j++){
            count++;
            combinations[j][i]=shadesEmployed;
            if (count==int(std::pow(2,i))){
                count=0;
                if (shadesEmployed==true){
                    shadesEmployed=false;
                }else{
                    shadesEmployed=true;
                }
            }
        }
    }

    //Parse the direct illuminance files and store objects in vectors for both base and setting cases.
    std::vector<DaylightIlluminanceData> baseDirectIlls;
    for (int i=0;i<model->windowGroups().size();i++){
        DaylightIlluminanceData illBase;
        illBase.parseTimeBased(model->spaceDirectory()+model->resultsDirectory()+model->spaceName()+"_"+model->windowGroups()[i].name()+"_base_direct.ill");
        baseDirectIlls.push_back(illBase);
    }

    //Find the combination closest to the sDAPercent without going over
    std::vector<std::vector<int>> shadeSchedule;
    shadeSchedule.resize(baseDirectIlls[0].illuminance().size());
    for (int i=0;i<baseDirectIlls[0].illuminance().size();i++){
        shadeSchedule[i].resize(model->windowGroups().size());
    }
    for (int i=0;i<baseDirectIlls[0].illuminance().size();i++){         //Loop through the number of hours in the year

    }


    return true;
}
bool Metrics::calculateOccsDA(Control *model, DaylightIlluminanceData *dayIll)
{

    return true;
}
bool Metrics::parseOccupancy(std::string file, double threshold){
    std::ifstream occFile;
    occFile.open(file);
    if (!occFile.is_open()){
        STADIC_LOG(Severity::Error, "The opening of the occupancy file "+file+" has failed.");
        return false;
    }
    std::string line;
    while (std::getline(occFile, line)){
        std::vector<std::string> vals;
        vals=split(line, ',');
        if (toDouble(vals[3])<threshold){
            m_Occupancy.push_back(false);
        }else{
            m_Occupancy.push_back(true);
        }
    }
    occFile.close();
    return true;
}

}
