#ifndef RADPRIMITIVE_H
#define RADPRIMITIVE_H

#include <QFile>
#include <QTextStream>
#include<array>
#include <vector>
#include <string>
#include <sstream>

#include "stadicapi.h"

namespace stadic {

class STADIC_API RadPrimitive
{
public:
    enum Type {Source, Sphere, Bubble, Polygon, Cone, Cup, Cylinder, Tube, Ring, Instance,
               Mesh, Light, Illum, Glow, Spotlight, Mirror, Prism1, Prism2, Plastic, Metal,
               Trans, Plastic2, Metal2, Trans2, Mist, Dielectric, Interface, Glass, Plasfunc,
               Metfunc, Transfunc, BRTDfunc, Plasdata, Metdata, Transdata, BSDF, Antimatter,
               Texfunc, Texdata, Colorfunc, Brightfunc, Colordata, Brightdata, Colorpict, Colortext,
               Brighttext, Mixfunc, Mixdata, Mixpict, Mixtext, Unknown};
    RadPrimitive();

    bool isGeometry() const;                                            //Boolean that determines whether the primitive is of a geometry type
    bool isMaterial() const;                                            //Boolean that determines whether the primitive is of a material type

    //Setters
    void setModifier(std::string modifier);                             //Function to set the modifier
    virtual bool setType(std::string type);                             //Function to set the type
    void setName(std::string name);                                     //Function to set the name
    bool setArg1(std::vector<std::string> vals);                        //Function to set the argumens on line one from a vector of strings
    bool setArg1(std::string arg, int position);                        //Function to set an argument on line one given the position of the argument
    bool setArg2(std::vector<std::string> vals);                        //Function to set the arguments on line two from a vector of strings
    bool setArg2(std::string arg, int position);                        //Function to set an argument on line two given the position of the argument
    bool setArg3(std::vector<std::string> vals);                        //Function to set the arguments on line three from a vector of strings
    bool setArg3(std::string arg, int position);                        //Function to set an argument on line three given the position of the argument

    virtual bool setArg(int number, std::string value, int position);

    //Getters
    std::string modifier() const;                                       //Function that returns the modifier as a string
    Type type() const;                                                  //Function that returns the type as a type
    std::string typeString() const;                                     //Function that returns the type as a string
    std::string name() const;                                           //Function that returns the name as a string
    std::vector<std::string> arg1() const;                              //Function that returns the first line of arguments as a vector
    std::vector<std::string> arg2() const;                              //Function that returns the second line of arguments as a vector
    std::vector<std::string> arg3() const;                              //Function that returns the third line of arguments as a vector

    virtual std::string getArg1(int position) const;                    //Function that returns a given argument from the first line as a string
    virtual std::string getArg2(int position) const;                    //Function that returns a given argument from the second line as a string
    virtual std::string getArg3(int position) const;                    //Function that returns a given argument from the third line as a string
    virtual std::string getArg(int number, int position) const;         //Function that returns a given argument from a given line

    static RadPrimitive *fromRad(std::stringstream &data);

protected:
    void initArg(int number, std::vector<std::string> arg);

private:
    virtual bool validateArg(int number, std::string value, int position) const {return true;}
    virtual bool validateArg(int number, std::vector<std::string> value) const {return true;}
    static std::array<std::string,51> s_typeStrings;
    static Type typeFromString(std::string string);

    std::string m_Modifier;                                             //Variable holding the modifier
    std::string m_TypeString;                                           //Variable holding the type
    std::string m_Name;                                                 //Variable holding the name
    std::vector<std::string> m_Arg1;                                    //Vector holding arguments on line 1
    std::vector<std::string> m_Arg2;                                    //Vector holding arguments on line 2
    std::vector<std::string> m_Arg3;                                    //Vector holding arguments on line 3

};

}

#endif // RADPRIMITIVE_H
