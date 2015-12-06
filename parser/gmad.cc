 /*
 * GMAD interface
 *
 */

#include "gmad.h"

#include "parser.h"

#include <cstdio>
#include <iostream>

using namespace GMAD;

int main(int argc, char *argv[])
{
  if(argc<2) {
    std::cout << "GMAD parser needs an input file" << std::endl;
    return 1;
  }
  if(argc>2) {
    std::cout << "GMAD parser needs only one input file" << std::endl;
    return 1;
  }
  Parser::Instance(std::string(argv[1]));
  return 0;
}

/** Python interface, need to match pybdsim/Gmad.py **/ 
int GMAD::GmadParser_c(char *name) 
{
  Parser::Instance(std::string(name));
  return 0;
}

int GMAD::GetNElements() 
{
  return Parser::Instance()->GetBeamline().size();
}  

const char* GMAD::GetName(int i) 
{
  std::list<Element>::const_iterator it = Parser::Instance()->GetBeamline().begin();
  std::advance(it, i);
  return (it->name).c_str();
}

int GMAD::GetType(int i) 
{
  std::list<Element>::const_iterator it = Parser::Instance()->GetBeamline().begin();
  std::advance(it, i);
  return static_cast<int>(it->type);
}

double GMAD::GetLength(int i) 
{
  std::list<Element>::const_iterator it = Parser::Instance()->GetBeamline().begin();
  std::advance(it, i);
  return it->l;
}

double GMAD::GetAngle(int i) 
{
  std::list<Element>::const_iterator it = Parser::Instance()->GetBeamline().begin();
  std::advance(it, i);
  return it->angle;  
}

double* GMAD::GetKs(int i)
{
  std::list<Element>::const_iterator it = Parser::Instance()->GetBeamline().begin();
  std::advance(it, i);
  double* result = new double[6];
  result[0] = it->ks;
  result[1] = it->k0;
  result[2] = it->k1;
  result[3] = it->k2;
  result[4] = it->k3;
  result[5] = it->k4;
  return result;
}

double GMAD::GetAper1(int i) 
{
  std::list<Element>::const_iterator it = Parser::Instance()->GetBeamline().begin();
  std::advance(it, i);
  return it->aper1;
}

double GMAD::GetAper2(int i) 
{
  std::list<Element>::const_iterator it = Parser::Instance()->GetBeamline().begin();
  std::advance(it, i);
  return it->aper2;
}

double GMAD::GetBeampipeThickness(int i)
{
  std::list<Element>::const_iterator it = Parser::Instance()->GetBeamline().begin();
  std::advance(it, i);
  return it->beampipeThickness;
}
