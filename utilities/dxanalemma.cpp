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

#include "analemma.h"
#include "logging.h"
#include "functions.h"
#include <iostream>
#include <string>

void usage(){
    std::cerr << "Usage: dxanalemma [OPTIONS]" << std::endl;
    std::cerr << stadic::wrapAtN("Generate the necessary files for the five phase .smx file including a material"
        " and geometry file.") << std::endl;
    std::cerr<<std::endl;
    std::cerr << stadic::wrapAtN("-f name   Set the input weather file to name. This file can either be an epw or"
        " tmy file.", 72, 10, true) << std::endl;
    std::cerr << stadic::wrapAtN("-m name   Set the material file to name.  This is the output location for all of"
        " the sun materials.", 72, 10, true) << std::endl;
    std::cerr << stadic::wrapAtN("-g name   Set the geometry file to name.  This is the output location for all of"
        " the sun geometry locations.", 72, 10, true) << std::endl;
    std::cerr << stadic::wrapAtN("-s name   Set the output smx file to name.  This is the output location for the"
        " smx file which is needed for the direct sun portion of the five phase analysis.", 72, 10, true) << std::endl;
    std::cerr << stadic::wrapAtN("-r angle  Set the rotation of the building.  A positive angle will result in a"
        " counter-clockwise rotation of the building (a clockwise rotation of the sky).  This follows the right"
        " hand rule.", 72, 10, true) << std::endl;
}

int main (int argc, char *argv[])
{
    if(argc == 1) {
        usage();
        return EXIT_FAILURE;
    }
    std::string weaFile;
    std::string matFile;
    std::string geoFile;
    std::string smxFile;
    double rotation=0;

    for (int i=1;i<argc;i++){
        if (std::string("-f")==argv[i]){
            i++;
            weaFile=argv[i];
        }else if (std::string("-m")==argv[i]){
            i++;
            matFile=argv[i];
        }else if (std::string("-g")==argv[i]){
            i++;
            geoFile=argv[i];
        }else if (std::string("-s")==argv[i]){
            i++;
            smxFile=argv[i];
        }else if (std::string("-r")==argv[i]){
            i++;
            rotation=atof(argv[i]);
        }else{
            std::string temp=argv[i];
            STADIC_ERROR("Invalid option \""+temp+"\".  Run with no arguments to get usage.");
            return EXIT_FAILURE;
        }
    }
    if (geoFile.empty()){
        STADIC_ERROR("The gometry file must be specified.");
        usage();
        return EXIT_FAILURE;
    }
    if (matFile.empty()){
        STADIC_ERROR("The material file must be specified.");
        usage();
        return EXIT_FAILURE;
    }
    if (smxFile.empty()){
        STADIC_ERROR("The smx file must be specified.");
        usage();
        return EXIT_FAILURE;
    }

    stadic::Analemma suns(weaFile);
    suns.setGeoFile(geoFile);
    suns.setMatFile(matFile);
    suns.setRotation(rotation);
    suns.setSMXFile(smxFile);
    if (!suns.genSun()){
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
