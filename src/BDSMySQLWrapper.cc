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

BDSMySQLWrapper::BDSMySQLWrapper (const G4String& SQLFileName)
  : ifs(SQLFileName.c_str()), ComponentN(0), tableN(-1)
  
{
  _startOfFile=true;
  BeginTokens();

#ifdef DEBUG
  if(ifs) G4cout<<"BDSMySQLWrapper contructor: Loading SQL Filename="<<SQLFileName<<G4endl;
  else G4cout<<"BDSMySQLWrapper constructor: Unable to load SQL file: "<<SQLFileName<<G4endl;
#endif
}

BDSMySQLWrapper::~BDSMySQLWrapper()
{
}

std::vector<BDSMySQLTable*> BDSMySQLWrapper::ConstructTable ()
{
  ComponentN=0;
  tableN=-1;
  while(NextToken()){
    ParseComponent();
  }
  return table;
}

void BDSMySQLWrapper::TokenizeLine(){
  std::string token;
  _tokens.clear();
  typedef boost::tokenizer<boost::char_separator<char> > tokenizer;  
  boost::char_separator<char> sep(", ",";()\n");
  tokenizer tok(_currentLine, sep);
  for(tokenizer::iterator tok_iter=tok.begin(); tok_iter != tok.end(); ++tok_iter){
    token = *tok_iter;
    RemoveQuotesFromLine(token);
    RemoveWhitespace(token);
    _tokens.push_back(token);
#ifdef DEBUG
    G4cout << __METHOD_NAME__ << " - token: = <" << token << ">" << G4endl;
#endif
  }
  BeginTokens();
}

bool BDSMySQLWrapper::NextToken(){
  std::string line;
  ++_tokens_iter;
  if(_startOfFile){
    --_tokens_iter;
  }
  if(_startOfFile || EndTokens()){
    if(ifs.good()){
      ReadLine();
      TokenizeLine();
#ifdef DEBUG
      G4cout << __METHOD_NAME__ << " - Token() = " << Token() << G4endl;
#endif
    }else{
      return false;
    }
  } else {
#ifdef DEBUG
    G4cout << __METHOD_NAME__ << " - Token() = " << Token() << G4endl;
#endif
  }
  return true;
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
#ifdef DEBUG
  G4cout << __METHOD_NAME__ << " reading CurrentTableName: " << CurrentTableName << G4endl;
#endif
  ProceedToEndOfLine();
  table.push_back(new BDSMySQLTable(CurrentTableName));
  tableN++;
  
  while((!varname.contains(";")) && (!vartype.contains(";"))){
    //Get next variable, skipping blanks.
    do{
      _NEXTINPUT
	} while((EmptyToken()) || (EndOfLine()));
    varname=Token();
#ifdef DEBUG
    G4cout << __METHOD_NAME__ << " reading varname: " << varname << G4endl;
#endif
    if (varname.contains(";")) return; //Semicolon indicates end of table.
    _NEXTINPUT
      vartype=Token();
#ifdef DEBUG
    G4cout << __METHOD_NAME__ << " reading vartype: " << vartype << G4endl;
#endif
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
#ifdef DEBUG
    G4cout << __METHOD_NAME__ << " reading input: " << Token() << G4endl;
#endif
  if(Token()==CMD_DATABASE) {CreateDatabase();}
  else if(Token()==CMD_TABLE) {CreateTable();}
}

void BDSMySQLWrapper::Insert(){
  _NEXT
#ifdef DEBUG
    G4cout << __METHOD_NAME__ << " reading input: " << Token() << G4endl;
#endif
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
	  //	  std::stringstream excptnSstrm;
	  //	  excptnSstrm<<__METHOD_NAME__<< " - expected " << table[j]->GetNVariables() << " values for insertion into table " << InsertTableName << "\n";
	  //	  G4Exception(excptnSstrm.str().c_str(), "-1", FatalException, "");
	  //	  return;
	}
#ifdef DEBUG
	G4cout << __METHOD_NAME__ << " inserting value " << Token() << G4endl;
#endif
	if(table[j]->GetVariable(k)->GetVarType()=="DOUBLE")
	  table[j]->GetVariable(k)->AddValue(atof(Token().c_str())*CLHEP::mm);
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

bool BDSMySQLWrapper::NextInputToken(){
  if(ifs.good()){
    bool status;
    do{
      status=NextToken();
    } while (EndOfLine());
    return status;
  } else {
    return false;
  }
}

bool BDSMySQLWrapper::EndTokens(){
  return(_tokens_iter==_tokens.end());
}

bool BDSMySQLWrapper::EndOfLine(){
  return((*_tokens_iter)=="\n");
}

bool BDSMySQLWrapper::EmptyToken(){
  return((*_tokens_iter)=="");
}

std::string BDSMySQLWrapper::Token(){
  return *_tokens_iter;
}

void BDSMySQLWrapper::ProceedToEndOfLine(){
#ifdef DEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  while(!EndOfLine()){
    _NEXT
#ifdef DEBUG
      G4cout << __METHOD_NAME__ << " " << Token() << G4endl;
#endif
  }
#ifdef DEBUG
  G4cout << __METHOD_NAME__ << " finished." << G4endl;
#endif
}

void BDSMySQLWrapper::ReadLine(){
  _currentLine.clear();
  _startOfFile=false;
  //returns non zero if unsuccessful
  char buffer[512];
  ifs.getline(buffer,512);
  _currentLine=buffer;
  RemoveCommentsFromLine(_currentLine);
  //Put back the new line character
  _currentLine += ' ';
  _currentLine += '\n';
#ifdef DEBUG
  G4cout << __METHOD_NAME__ << " - _currentLine = " << _currentLine << G4endl;
#endif
}

void BDSMySQLWrapper::RemoveCommentsFromLine(std::string& value){
  //Strip all characters after and including '#'
  unsigned pos = value.find('#');
  std::string uncommented = value.substr(0,pos);
  value=uncommented;
}

void BDSMySQLWrapper::RemoveQuotesFromLine(std::string& value){
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




 
