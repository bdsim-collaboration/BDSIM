/* * BDSIM code.    Version 1.0
   * Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   * Last modified 24.7.2002
   * Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 


   Author of this code: John C. Carter, Royal Holloway, Univ. of London.
   Last modified 12.10.2005
*/


#include "BDSGlobalConstants.hh" 

#include <cstdlib>
#include "BDSMaterials.hh"
#include "BDSMySQLTable.hh"
#include "BDSExecOptions.hh"

using namespace std;

#include"globals.hh"
#include<string>
#include<vector>

BDSMySQLTable::BDSMySQLTable (G4String aTableName)
{
  itsTableName = aTableName;
  itsNVariables = 0;
}

void BDSMySQLTable::AddVariable(G4String aName, G4String aType)
{
  itsVar.push_back(new BDSMySQLVariable(aName,aType));
  itsNVariables++;
}

BDSMySQLVariable* BDSMySQLTable::GetVariable(G4String aVarName)
{
  for (G4int i=0; i<(G4int)itsVar.size(); i++)
    {
      G4String::caseCompare cmpmode = G4String::ignoreCase;
      if( (itsVar[i])->GetName().compareTo(aVarName,cmpmode)==0) {
	return itsVar[i];
      }
    }
  return NULL; //if does not exist return null
}

BDSMySQLVariable* BDSMySQLTable::GetVariable(G4int aVarN)
{
  return itsVar[aVarN];
}



BDSMySQLTable::~BDSMySQLTable()
{
}
