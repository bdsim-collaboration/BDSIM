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
#include "BDSMySQLWrapper.hh"
#include "BDSMySQLTable.hh"
#include "BDSExecOptions.hh"

#include<iostream>
#include<boost/tokenizer.hpp>

#include"globals.hh"
#include<string>
#include<vector>
#include<sstream>

using namespace std;
using namespace boost;

BDSMySQLWrapper::BDSMySQLWrapper (const G4String& SQLFileName)
  : ifs(SQLFileName.c_str()), ComponentN(0), tableN(-1)
  
{
  if(ifs) G4cout<<"BDSMySQLWrapper contructor: Loading SQL Filename="<<SQLFileName<<G4endl;
  else G4cout<<"BDSMySQLWrapper constructor: Unable to load SQL file: "<<SQLFileName<<G4endl;
}

BDSMySQLWrapper::~BDSMySQLWrapper()
{
}

vector<BDSMySQLTable*> BDSMySQLWrapper::ConstructTable ()
{
  TokenizeFile();
  BeginTokens();
  ComponentN=0;
  tableN=-1;
  while(NextToken()){
    ParseComponent();
  }
  return table;
}

void BDSMySQLWrapper::TokenizeFile(){
  string line;
  string token;
  typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
  boost::char_separator<char> sep(", ",";()\n");
  while(ReadLine(line)){
    tokenizer tokens(line, sep);
    for(tokenizer::iterator tok_iter=tokens.begin(); tok_iter != tokens.end(); ++tok_iter){
      token = *tok_iter;
      RemoveQuotesFromLine(token);
      RemoveWhitespace(token);
      _tokens.push_back(token);
    }
  }
}

G4int BDSMySQLWrapper::ParseComponent()
{
  // not using these commands yet...
  if(Token()==CMD_DROP) { return 0;}
  else if(Token()==CMD_DATABASE) { return 0;}
  else if(Token()==CMD_IF) { return 0;}
  else if(Token()==CMD_EXISTS) { return 0;}
  else if(Token()==CMD_USE) { return 0;}
  //The following commands are used...
  else if(Token()==CMD_CREATE) {Create(); return 0;}
  else if(Token()==CMD_INSERT) {Insert(); ComponentN++; return 0;}
  return 0;
}


void BDSMySQLWrapper::CreateDatabase(){
}

void BDSMySQLWrapper::CreateTable(){
  G4String varname;
  G4String vartype;

  _NEXTINPUT
    CurrentTableName=Token();
  G4cout << __METHOD_NAME__ << " reading CurrentTableName: " << CurrentTableName << G4endl;
  ProceedToEndOfLine();
  table.push_back(new BDSMySQLTable(CurrentTableName));
  tableN++;
  
  while((!varname.contains(";")) && (!vartype.contains(";"))){
    //Get next variable, skipping blanks.
    do{
      _NEXTINPUT
	} while((EmptyToken()) || (EndOfLine()));
    varname=Token();
    G4cout << __METHOD_NAME__ << " reading varname: " << varname << G4endl;
    if (varname.contains(";")) return; //Semicolon indicates end of table.
    _NEXTINPUT
      vartype=Token();
    G4cout << __METHOD_NAME__ << " reading vartype: " << vartype << G4endl;
    if (vartype.contains(";")) return; //Semicolon indicates end of table.
    if(vartype.contains("DOUBLE")) vartype="DOUBLE";
    else if(vartype.contains("VARCHAR")) vartype="STRING";
    else if(vartype.contains("INTEGER")) vartype="INTEGER";
    table[tableN]->AddVariable(varname,vartype);
    ProceedToEndOfLine();
  }
}


void BDSMySQLWrapper::Create(){
  _NEXT
    G4cout << __METHOD_NAME__ << " reading input: " << Token() << G4endl;
  if(Token()==CMD_DATABASE) {CreateDatabase();}
  else if(Token()==CMD_TABLE) {CreateTable();}
}

void BDSMySQLWrapper::Insert(){
  _NEXT
    G4cout << __METHOD_NAME__ << " reading input: " << Token() << G4endl;
  if(Token()==CMD_INTO){InsertInto();}
}

void BDSMySQLWrapper::InsertInto() {
  _NEXT
    InsertTableName=Token();
  _NEXT
    if(Token()==CMD_VALUES){Values();}
}
 
void BDSMySQLWrapper::Values() {
  for(G4int j=0; j<(G4int)table.size(); j++){
    if(table[j]->GetName()==InsertTableName){
      for(G4int k=0; k<table[j]->GetNVariables(); k++){
	_NEXTINPUT
	  //Skip first bracket...
	while (Token()=="(") {
	  _NEXTINPUT
	}
	if (Token()==")") {
	  std::stringstream excptnSstrm;
	  excptnSstrm<<__METHOD_NAME__<< " - expected " << table[j]->GetNVariables() << " values for insertion into table " << InsertTableName << "\n";
	  G4Exception(excptnSstrm.str().c_str(), "-1", FatalException, "");
	  return;
	}
	G4cout << __METHOD_NAME__ << " inserting value " << Token() << G4endl;
	if(table[j]->GetVariable(k)->GetVarType()=="DOUBLE")
	  table[j]->GetVariable(k)->AddValue(atof(Token().c_str())*mm);
	else if(table[j]->GetVariable(k)->GetVarType()=="STRING")
	  table[j]->GetVariable(k)->AddValue(Token().c_str());
	else if(table[j]->GetVariable(k)->GetVarType()=="INTEGER")
	  table[j]->GetVariable(k)->AddValue(atoi(Token().c_str()));
      }
    }
  }
}

void BDSMySQLWrapper::BeginTokens(){
  _tokens_iter=_tokens.begin();
}

bool BDSMySQLWrapper::NextToken(){
  ++_tokens_iter;
  if(!EndTokens()) return 0;
  return 1;
}

bool BDSMySQLWrapper::NextInputToken(){
  G4int returnVal;
  do{
    returnVal=NextToken();
  } while (EndOfLine());
  return returnVal;;
}

bool BDSMySQLWrapper::EndTokens(){
  return(_tokens_iter!=_tokens.end());
}

bool BDSMySQLWrapper::EndOfLine(){
  return((*_tokens_iter)=="\n");
}

bool BDSMySQLWrapper::EmptyToken(){
  return((*_tokens_iter)=="");
}

string BDSMySQLWrapper::Token(){
  return *_tokens_iter;
}

void BDSMySQLWrapper::ProceedToEndOfLine(){
  G4cout << __METHOD_NAME__ << G4endl;
  while(!EndOfLine()){
    _NEXT
      G4cout << __METHOD_NAME__ << " " << Token() << G4endl;
  }
  G4cout << __METHOD_NAME__ << " finished." << G4endl;
}

bool BDSMySQLWrapper::ReadLine(string& value){
  //returns non zero if unsuccessful
  char buffer[512];
  bool result = ifs.getline(buffer,512);
  value=buffer;
  RemoveCommentsFromLine(value);
  //Put back the new line character
  value += ' ';
  value += '\n';
  return result;
}

void BDSMySQLWrapper::RemoveCommentsFromLine(string& value){
  //Strip all characters after and including '#'
  unsigned pos = value.find('#');
  string uncommented = value.substr(0,pos);
  value=uncommented;
}

void BDSMySQLWrapper::RemoveQuotesFromLine(string& value){
  //Strip all "
  value.erase (std::remove(value.begin(), value.end(), '\"'), value.end());
}


void BDSMySQLWrapper::RemoveWhitespace(G4String& val){
  std::string s = val;
  RemoveWhitespace(s);
  val=(G4String)s;
}

void BDSMySQLWrapper::RemoveWhitespace(std::string& val){
  val.erase( std::remove_if( val.begin(), val.end(), ::isblank ), val.end() );
}

/*
void Log(const G4String& tag, int depth, ostream& os)
{
  static const char* tab = "----|";
  while(depth--) os<<tab;
  os<<' '<<tag<<endl;
}
*/




 
