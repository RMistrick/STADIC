#include "weatherdata.h"
#include "logging.h"

#include <iostream>
#include <string>

void usage()
{
    std::cout << "dxmakewea - Convert TMY or EPW files to WEA" << std::endl;
    std::cout << "usage: dxmakewea <input file> <output file>" << std::endl;
}

int main (int argc, char *argv[]){
    if (argc < 3){
        usage();
        return EXIT_FAILURE;
    }
    std::string fileName=argv[1];
    std::string resultsFile=argv[2];
    stadic::WeatherData wea;
    if (!wea.parseWeather(fileName)){
        return EXIT_FAILURE;
    }
    if (!wea.writeWea(resultsFile)){
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
