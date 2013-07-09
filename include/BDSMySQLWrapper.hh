/* * BDSIM code.    Version 1.0
   * Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   * Last modified 24.7.2002
   * Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 


   Author of this code: John C. Carter, Royal Holloway, Univ. of London.
   Last modified 12.10.2005
*/


#ifndef BDSMySQLWrapper_h
#define BDSMySQLWrapper_h 1

#include "BDSAcceleratorComponent.hh"
#include "BDSMySQLTable.hh"
#include <fstream>
#include <vector>

class BDSMySQLWrapper 
{
public:
  BDSMySQLWrapper (const G4String& SQLFileName);
  ~BDSMySQLWrapper();
  
  std::vector<BDSMySQLTable*> ConstructTable();

  G4int GetComponentN() {return ComponentN;}
 
  std::vector<BDSMySQLTable*> table;

protected:
  
private:
  // Data Members for Class Attributes

  std::ifstream ifs;
  
  G4String CurrentTableName;
  G4String InsertTableName;
  G4int ComponentN;

  G4int tableN;

  G4int ReadComponent ();
};

#endif
