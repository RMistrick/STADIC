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

#ifndef METRICS_H
#define METRICS_H

#include "spacecontrol.h"
#include "buildingcontrol.h"
#include <vector>
#include <string>
#include "dayill.h"

#include "stadicapi.h"

namespace stadic {
class STADIC_API Metrics
{
public:
    explicit Metrics(BuildingControl *model);                         //Constructor that takes a Control object as an argument                                                           //Function to simulate the daylight
    bool processMetrics();

private:

    bool calculateDA(Control *model, DaylightIlluminanceData *dayIll);
    bool calculatecDA(Control *model, DaylightIlluminanceData *dayIll);
    bool calculateDF(Control *model, DaylightIlluminanceData *dayIll);
    bool calculateUDI(Control *model, DaylightIlluminanceData *dayIll);
    bool calculatesDA(Control *model, DaylightIlluminanceData *dayIll);
    bool calculateOccsDA(Control *model, DaylightIlluminanceData *dayIll);
    bool parseOccupancy(std::string file, double threshold);
    BuildingControl *m_Model;
    std::vector<bool> m_Occupancy;

};

}
#endif // METRICS_H
