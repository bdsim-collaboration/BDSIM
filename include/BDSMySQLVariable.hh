#ifndef BDSMYSQLVARIABLE_H
#define BDSMYSQLVARIABLE_H

#include "globals.hh"
#include <vector>

/**
 * @brief A variable representation for mysql loader.
 *
 * @author John Carter
 */

class BDSMySQLVariable
{
public: 
  BDSMySQLVariable(G4String aVarName, G4String aVarType);

  void AddValue(G4double aVarValue);
  void AddValue(G4String aVarValue);
  void AddValue(G4int aVarValue);

  /// @{ Accessor
  inline G4String GetStrValue(G4int itemN) const {return itsStrVar[itemN];}
  inline G4double GetDblValue(G4int itemN) const {return itsDblVar[itemN];}
  inline G4int    GetIntValue(G4int itemN) const {return itsIntVar[itemN];}
  inline G4int    GetNVariables() const {return itsNVariables;}
  inline G4String GetVarType()    const {return itsVarType;}
  inline G4String GetName()       const {return itsVarName;}
  /// @}

  /// @{ Setter
  inline void SetStrValue(G4int itemN, G4String newValue) {itsStrVar[itemN] = newValue;}
  inline void SetDblValue(G4int itemN, G4double newValue) {itsDblVar[itemN] = newValue;}
  inline void SetIntValue(G4int itemN, G4int newValue)    {itsIntVar[itemN] = newValue;}
  /// @}

  void Print();

  ~BDSMySQLVariable(){};
  
private:
  BDSMySQLVariable();
  
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

#endif
