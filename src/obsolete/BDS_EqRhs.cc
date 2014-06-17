/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/
#include "BDS_EqRhs.hh"
#include "G4MagneticField.hh"
#include "G4EquationOfMotion.hh"

// Constructor Implementation
//
BDS_EqRhs::BDS_EqRhs( BDSSbendMagField *magField ) 
   : G4Mag_EqRhs(magField),itsField(magField)
{}


BDS_EqRhs::~BDS_EqRhs() { }
