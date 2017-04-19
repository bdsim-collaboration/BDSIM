#ifndef BDSMYSQLTABLE_H
#define BDSMYSQLTABLE_H

#include "globals.hh"
#include <vector>

class BDSMySQLVariable;

/**
 * @brief Representation of a mysql table.
 *
 * @author John Carter
 */

class BDSMySQLTable 
{
public:
  explicit BDSMySQLTable (G4String aTableName);
  ~BDSMySQLTable();

  void AddVariable(G4String aVarName, G4String aType);

  inline G4String GetName()       const {return itsTableName;}
  inline G4int    GetNVariables() const {return itsNVariables;}

  BDSMySQLVariable* GetVariable(G4String aVarName);
  BDSMySQLVariable* GetVariable(G4int aVarN);

  void Print();
  
private:
  G4int itsNVariables;
  G4String itsTableName;
  std::vector<BDSMySQLVariable*> itsVar;

  //A list of valid variable types (STRING, DOUBLE, INTEGER...)
  void SetValidVarTypes();
  std::vector<G4String> _validVarTypes;
  //To check is a var type is valid...
  G4bool isValidVarType(G4String);
};

#endif
