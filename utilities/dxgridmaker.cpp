#include "gridmaker.h"
#include "logging.h"
#include <vector>

int main (int argc, char *argv[]){
    QString fileName;
    fileName.clear();
    QString resultFile;
    resultFile.clear();
    QString csvFile;
    csvFile.clear();
    QString polyFile;
    polyFile.clear();
    std::vector<QString> layerNames;
    double Sx, Sy, Ox, Oy, Z;
    Sx=0;
    Sy=0;
    Ox=0;
    Oy=0;
    Z=0;
    for (int i=1;i<argc;i++){
        if (QString(argv[i])=="-f"){
            i++;
            fileName=argv[i];
        }else if (QString(argv[i])=="-Sx"){
            i++;
            Sx=atof(argv[i]);
        }else if (QString(argv[i])=="-Sy"){
            i++;
            Sy=atof(argv[i]);
        }else if (QString(argv[i])=="-Ox"){
            i++;
            Ox=atof(argv[i]);
        }else if (QString(argv[i])=="-Oy"){
            i++;
            Oy=atof(argv[i]);
        }else if (QString(argv[i])=="-Z"){
            i++;
            Z=atof(argv[i]);
        }else if (QString(argv[i])=="-r"){
            i++;
            resultFile=argv[i];
        }else if(QString(argv[i])=="-l"){
            i++;
            layerNames.push_back(QString(argv[i]));
        }else if(QString(argv[i])=="-p"){
            i++;
            polyFile=argv[i];
        }else if(QString(argv[i])=="-csv"){
            i++;
            csvFile=argv[i];
        }else{
            QString temp=argv[i];
            WARNING(QString("The argument "+temp+" is an unkown argument."));
        }
    }
    if (Sx==0 ||Sy==0){
        ERROR(QString("The x and y spacing are needed to complete the calculation.\n\tSpecify with \"-Sx\" and \"-Sy\"."));
        EXIT_FAILURE;
    }
    if (fileName.isEmpty()){
        ERROR(QString("The rad file name must be specified.\n\tSpecify with \"-f\"."));
        EXIT_FAILURE;
    }
    if (resultFile.isEmpty()){
        ERROR(QString("The result file name must be specified.\n\tSpecify with \"-r\"."));
        EXIT_FAILURE;
    }
    //Instantiate GridMaker Object
    GridMaker grid;
    for (int i=0;i<layerNames.size();i++){
        grid.setLayerNames(layerNames.at(i));
    }
    grid.setSpaceX(Sx);
    grid.setSpaceY(Sy);
    grid.setOffsetX(Ox);
    grid.setOffsetY(Oy);
    grid.setZHeight(Z);
    if (!grid.parseRad(fileName)){
        ERROR(QString("The parsing of the rad file failed."));
        EXIT_FAILURE;
    }
    if (!grid.makeGrid()){
        ERROR(QString("The creation of the grid failed."));
        EXIT_FAILURE;
    }
    if (!grid.writePTS(resultFile)){
        ERROR(QString("The writing of the points file failed."));
        EXIT_FAILURE;
    }
    if (!polyFile.isEmpty()){
        if (!grid.writeRadPoly(polyFile)){
            EXIT_FAILURE;
        }
    }
    if (!csvFile.isEmpty()){
        if (!grid.writePTScsv(csvFile)){
            EXIT_FAILURE;
        }
    }

    EXIT_SUCCESS;
}