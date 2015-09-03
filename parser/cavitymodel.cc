#include "cavitymodel.h"

#include <iomanip>
#include <iostream>

CavityModel::CavityModel()
{;}

void CavityModel::clear()
{
  name = "";
  type = "";
  
}

void CavityModel::print()const
{
  std::cout << "cavitymodel: "
	    << name          << " "
	    << type          << " "
	    << std::endl;
}

void CavityModel::set_value(std::string property, double value )
{
#ifdef BDSDEBUG
  std::cout << "parser> Setting value " << std::setw(25) << std::left << property << value << std::endl;
#endif

  //  if (property=="radius" || property == "aper1") {aper1 = value; return;}
  
  std::cerr << "Error: parser> unknown cavitymodel option \"" << property << "\" with value " << value << std::endl; 
  exit(1);
}

void CavityModel::set_value(std::string property, std::string value )
{
  #ifdef BDSDEBUG
  std::cout << "parser> Setting value " << std::setw(25) << std::left << property << value << std::endl;
#endif

  if (property=="name")         {name = value; return;}
  if (property=="type")         {type = value; return;}

  std::cerr << "Error: parser> unknown cavitymodel option \"" << property << "\" with value " << value << std::endl;
  exit(1);
}
