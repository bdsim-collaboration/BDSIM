/* * BDSIM code.    Version 1.0
   * Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   * Last modified 24.7.2002
   * Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 


   Author of this code: John C. Carter, Royal Holloway, Univ. of London.
   Last modified 12.10.2005
*/


#ifndef BDSMySQLTable_h
#define BDSMySQLTable_h 1

#include <vector>

class BDSMySQLVariable
{

public:
  
  BDSMySQLVariable(G4String aVarName, G4String aVarType);

  void AddValue(G4double aVarValue);
  void AddValue(G4String aVarValue);
  void AddValue(G4int aVarValue);

  G4String GetStrValue(G4int itemN);
  G4double GetDblValue(G4int itemN);
  G4int GetIntValue(G4int itemN);

  void SetStrValue(G4int itemN, G4String newValue);
  void SetDblValue(G4int itemN, G4double newValue);
  void SetIntValue(G4int itemN, G4int newValue);

  G4int GetNVariables();

  G4String GetVarType();
  G4String GetName();

  ~BDSMySQLVariable(){};
  
protected:

private:

  G4int itsNVariables;
  G4String itsVarName;
  G4String itsVarType;
  std::vector<G4double> itsDblVar;
  std::vector<G4String> itsStrVar;
  std::vector<G4int> itsIntVar;

};

inline BDSMySQLVariable::BDSMySQLVariable(G4String aVarName, G4String aVarType)
{
  itsVarName=aVarName;
  itsVarType=aVarType;
  itsNVariables=0;
}

inline G4int BDSMySQLVariable::GetNVariables()
{return itsNVariables;}

inline G4String BDSMySQLVariable::GetName()
{return itsVarName;}

inline G4String BDSMySQLVariable::GetVarType()
{return itsVarType;}

inline void BDSMySQLVariable::AddValue(G4double aVarValue)
{
  itsNVariables++;
  // Check is to avoid unecessary confusion
  if(itsVarType=="DOUBLE")
    itsDblVar.push_back(aVarValue);
}

inline void BDSMySQLVariable::AddValue(G4int aVarValue)
{
  itsNVariables++;
  // Check is to avoid unecessary confusion
  if(itsVarType=="INTEGER")
    itsIntVar.push_back(aVarValue);
}

inline void BDSMySQLVariable::AddValue(G4String aVarValue)
{
  itsNVariables++;
  itsStrVar.push_back(aVarValue);
}

inline  G4String BDSMySQLVariable::GetStrValue(G4int itemN)
{return itsStrVar[itemN];}

inline  G4double BDSMySQLVariable::GetDblValue(G4int itemN)
{
  return itsDblVar[itemN];
}

inline  G4int BDSMySQLVariable::GetIntValue(G4int itemN)
{return itsIntVar[itemN];}

inline  void BDSMySQLVariable::SetStrValue(G4int itemN, G4String newValue)
{itsStrVar[itemN] = newValue;}

inline  void BDSMySQLVariable::SetDblValue(G4int itemN, G4double newValue)
{itsDblVar[itemN] = newValue;}

inline  void BDSMySQLVariable::SetIntValue(G4int itemN, G4int newValue)
{itsIntVar[itemN] = newValue;}

class BDSMySQLTable 
{
public:
  BDSMySQLTable (G4String aTableName);
  ~BDSMySQLTable();
  

  void AddVariable(G4String aVarName, G4String aType);

  G4String GetName();
  G4int GetNVariables();

  BDSMySQLVariable* GetVariable(G4String aVarName);
  BDSMySQLVariable* GetVariable(G4int aVarN);

protected:
  
private:
  G4int itsNVariables;
  G4String itsTableName;
  std::vector<BDSMySQLVariable*> itsVar;
  
};

inline G4String BDSMySQLTable::GetName()
{return itsTableName;}

inline G4int BDSMySQLTable::GetNVariables()
{return itsNVariables;}

#endif
