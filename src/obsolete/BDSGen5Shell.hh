/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/
#ifndef BDSGen5Shell_h
#define BDSGen5Shell_h 
#include "BDSGlobalConstants.hh" 
#include "globals.hh"
#include "Randomize.hh"
#include <vector>

//typedef vector<G4double> vDbl;

class BDSGen5Shell
{

public:
  BDSGen5Shell(G4double InnerRad,G4double OuterRad);
  ~BDSGen5Shell();


  G4double* GenPoint();


private:

  G4double GenSin2();
  G4double GenSin3();
  G4double GenRadius();

  G4double itsVec[5];

  G4double Theta0,Theta1,Theta2,Theta3;

  G4double area2,area3,areaR;
  G4double Gen2Lower,Gen2Upper,Gen3Lower,Gen3Upper;
  G4double GenRLower;
  G4double ShellInnerRadius,ShellOuterRadius;

};


#endif
