#include "radprimitive.h"
#include <QTextStream>
#include "geometryprimitives.h"
#include "materialprimitives.h"
#include <iostream>
#include "functions.h"
#include <sstream>

namespace stadic {

std::array<std::string,51> RadPrimitive::s_typeStrings = {"source", "sphere", "bubble", "polygon", "cone", "cup",
                                                      "cylinder", "tube", "ring", "instance", "mesh", "light",
                                                      "illum", "glow", "spotlight", "mirror", "prism1", "prism2",
                                                      "plastic", "metal", "trans", "plastic2", "metal2", "trans2",
                                                      "mist", "dielectric", "interface", "glass", "plasfunc",
                                                      "metfunc", "transfunc", "brtdfunc", "plasdata", "metdata",
                                                      "transdata", "bsdf", "antimatter", "texfunc", "texdata",
                                                      "colorfunc", "brightfunc", "colordata", "brightdata",
                                                      "colorpict", "colortext", "brighttext", "mixfunc", "mixdata",
                                                      "mixpict", "mixtext", "unknown"};

RadPrimitive::RadPrimitive()
{
}

bool RadPrimitive::isGeometry() const
{
    switch(type()) {
    case Source:
    case Sphere:
    case Bubble:
    case Polygon:
    case Cone:
    case Cup:
    case Cylinder:
    case Tube:
    case Ring:
    case Instance:
    case Mesh:
        return true;
    }
    return false;
}

bool RadPrimitive::isMaterial() const
{
    switch(type()) {
    case Light:
    case Illum:
    case Glow:
    case Spotlight:
    case Mirror:
    case Prism1:
    case Prism2:
    case Plastic:
    case Metal:
    case Trans:
    case Plastic2:
    case Metal2:
    case Trans2:
    case Mist:
    case Dielectric:
    case Interface:
    case Glass:
    case Plasfunc:
    case Metfunc:
    case Transfunc:
    case BRTDfunc:
    case Plasdata:
    case Metdata:
    case Transdata:
    case BSDF:
    case Antimatter:
        return true;
    }
    return false;
}

//Setters
void RadPrimitive::setModifier(std::string modifier){
    m_Modifier=modifier;
}

bool RadPrimitive::setType(std::string type){
    m_TypeString=type;
    return true;
}
void RadPrimitive::setName(std::string name){
    m_Name=name;
}

bool RadPrimitive::setArg1(std::vector<std::string> vals){
    if(validateArg(1,vals)) {
      m_Arg1=vals;
      return true;
    }
    return false;
}
bool RadPrimitive::setArg1(std::string arg, int position) {
    if(position<m_Arg1.size()) {
        if(!validateArg(1,arg,position)) {
            return false;
        }
        m_Arg1[position] = arg;
        return true;
    }
    return false;
}
bool RadPrimitive::setArg2(std::vector<std::string> vals){
    if(validateArg(2,vals)) {
      m_Arg2=vals;
      return true;
    }
    return false;
}
bool RadPrimitive::setArg2(std::string arg, int position) {
    if(position<m_Arg2.size()) {
        if(!validateArg(2,arg,position)) {
            return false;
        }
        m_Arg2[position] = arg;
        return true;
    }
    return false;
}
bool RadPrimitive::setArg3(std::vector<std::string> vals){
    if(validateArg(3,vals)) {
      m_Arg3=vals;
      return true;
    }
    return false;
}
bool RadPrimitive::setArg3(std::string arg, int position) {
    if(position<m_Arg3.size()) {
        if(!validateArg(3,arg,position)) {
            return false;
        }
        m_Arg3[position] = arg;
        return true;
    }
    return false;
}

bool RadPrimitive::setArg(int number, std::string value, int position)
{
    std::vector<std::string> *arg;
    switch(number) {
    case 1:
        arg = &m_Arg1;
        break;
    case 2:
        arg = &m_Arg2;
        break;
    case 3:
        arg = &m_Arg3;
        break;
    default:
        // Error/warning message?
        return false;
    }
    if(validateArg(number,value,position)) {
        if(position>=arg->size()) {
            arg->resize(position+1);
        }
        (*arg)[position] = value;
        return true;
    } else {
        // Error/warning message?
    }
    return false;
}

//Getters
std::string RadPrimitive::modifier() const {
    return m_Modifier;
}
RadPrimitive::Type RadPrimitive::type() const{
    return typeFromString(m_TypeString);
}
std::string RadPrimitive::typeString() const {
    return m_TypeString;
}
std::string RadPrimitive::name() const {
    return m_Name;
}
std::vector<std::string> RadPrimitive::arg1() const {
    return m_Arg1;
}
std::vector<std::string> RadPrimitive::arg2() const {
    return m_Arg2;
}
std::vector<std::string> RadPrimitive::arg3() const{
    return m_Arg3;
}

static std::string nextNonComment(std::stringstream &data)
{
  std::string string;
  while(!data.eof()) {
    std::getline(data,string);
    string = trim(string);
    //std::cout << "### " << string.toStdString() << std::endl;
    if(!string.empty()) {
        if(string.find('#')==string.npos){
            return string;
        }
    }
  }
  std::string empty;
  return empty;
}

RadPrimitive* RadPrimitive::fromRad(std::stringstream &data)
{
    RadPrimitive *rad;

    std::string string = nextNonComment(data);
    if (string.empty()){
        return nullptr;
    }
    std::vector<std::string> list =trimmedSplit(string,' ');    // string.split(QRegExp("\\s+")); // This should be "modifier type identifier"?
    if(list.size() != 3) {
        return nullptr;
    }

    switch(typeFromString(list[1])) {
    case Polygon:
        rad = new PolygonGeometry();
        break;
    case Plastic:
        rad = new PlasticMaterial();
        break;
    case BSDF:
        rad = new BSDFMaterial();
        break;
    default:
        rad = new RadPrimitive();
        rad->setType(list[1]);
        break;
    }
    rad->setModifier(list[0]);
    rad->setName(list[2]);

    int nargs;
    data>>string;   //Reads number of arguments from first line
    nargs = atoi(string.c_str());
    if (nargs>0){
        std::vector<std::string> args;
        for (int i=0;i<nargs;i++){
            data>>string;
            args.push_back(string);
        }
        rad->setArg1(args);
    }

    data>>string;   //Reads number of arguments from second line
    nargs = atoi(string.c_str());
    if (nargs>0){
        std::vector<std::string> args;
        for (int i=0;i<nargs;i++){
            data>>string;
            args.push_back(string);
        }
        rad->setArg2(args);
    }

    data>>string;   //Reads number of arguments from third line
    nargs = atoi(string.c_str());
    if (nargs>0){
        std::vector<std::string> args;
        for (int i=0;i<nargs;i++){
            data>>string;
            args.push_back(string);
        }
        rad->setArg3(args);
    }
    return rad;
}

RadPrimitive::Type RadPrimitive::typeFromString(std::string string)
{
    for(unsigned i=0;i<s_typeStrings.size();i++) {
        if(s_typeStrings[i] == string) {
            return (RadPrimitive::Type)i;
        }
    }
    return Unknown;
}

std::string RadPrimitive::getArg1(int position) const
{
    if(position<0 || position>=m_Arg1.size()) {
        std::string empty;
        return empty;
    }
    return m_Arg1[position];
}

std::string RadPrimitive::getArg2(int position) const
{
    if(position<0 || position>=m_Arg2.size()) {
        std::string empty;
        return empty;
    }
    return m_Arg2[position];
}

std::string RadPrimitive::getArg3(int position) const
{
    if(position<0 || position>=m_Arg3.size()) {
        std::string empty;
        return empty;
    }
    return m_Arg3[position];
}

std::string RadPrimitive::getArg(int number, int position) const
{
    std::vector<std::string> *arg;
    switch(number) {
    case 1:
        return getArg1(position);
    case 2:
        return getArg2(position);
    case 3:
        return getArg3(position);
    }
    // Error/warning message?
    std::string empty;
    return empty;
}

void RadPrimitive::initArg(int number, std::vector<std::string> arg)
{
    switch(number) {
    case 1:
        m_Arg1 = arg;
        break;
    case 2:
        m_Arg2 = arg;
        break;
    case 3:
        m_Arg3 = arg;
        break;
    default:
        // Error message?
        break;
    }
}

}
