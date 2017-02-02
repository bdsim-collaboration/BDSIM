#include <cstdlib>
#include <string>
#include <vector>

#include "BDSMySQLTable.hh"
#include "BDSDebug.hh"

BDSMySQLTable::BDSMySQLTable (G4String aTableName)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " - creating new table named: " << aTableName << G4endl;
#endif
  SetValidVarTypes();
  itsTableName = aTableName;
  itsNVariables = 0;
}

void BDSMySQLTable::AddVariable(G4String aName, G4String aType){
  if(aName == ""){
    return;
  }
  if(!isValidVarType(aType)){
    return;
  }
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " - adding variable: " << aName << " " << aType << " to table " << GetName() << G4endl;
#endif
  itsVar.push_back(new BDSMySQLVariable(aName,aType));
  itsNVariables++;
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " - nVariables = " << GetNVariables() << G4endl;
#endif
}

void BDSMySQLTable::Print(){
  G4cout << __METHOD_NAME__ << G4endl;
  G4cout << GetName() << G4endl;
  for(int i=0; i<itsNVariables; i++){
    GetVariable(i)->Print();
  }
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
  return nullptr; //if does not exist return null
}

BDSMySQLVariable* BDSMySQLTable::GetVariable(G4int aVarN)
{
  return itsVar[aVarN];
}

void BDSMySQLTable::SetValidVarTypes(){
  _validVarTypes.push_back((G4String)"DOUBLE");
  _validVarTypes.push_back((G4String)"INTEGER");
  _validVarTypes.push_back((G4String)"STRING");

}

G4bool BDSMySQLTable::isValidVarType(G4String val){
  G4bool result = false;
  for(std::vector<G4String>::const_iterator iter = _validVarTypes.begin(); iter != _validVarTypes.end(); ++iter){
    if((*iter) == val) result = true;
  }
  return result;
}

BDSMySQLTable::~BDSMySQLTable()
{
}


void BDSMySQLVariable::Print(){
  G4cout << __METHOD_NAME__ << G4endl;
  std::string varType=GetVarType();
  G4cout << GetName() << " " << GetVarType() << " " << G4endl;
  for(int i=0; i<GetNVariables(); i++){
    if(varType=="DOUBLE") G4cout << GetDblValue(i) << G4endl;
    if(varType=="INTEGER")G4cout << GetIntValue(i) << G4endl;
    if(varType=="STRING")G4cout << GetStrValue(i) << G4endl;
  }
}

