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

using namespace std;

#include"globals.hh"
#include<string>
#include<vector>

void Log(const G4String& tag, int depth, ostream& os)
{
  static const char* tab = "----|";
  while(depth--) os<<tab;
  os<<' '<<tag<<endl;
}

void StripHeader(istream& is)
{
  //Need to read the first 6 lines of the SQL file
  char buffer[256];
  for (int i=0; i<6; i++)
    is.getline(buffer,256);
}

inline G4String StripQuotes(const G4String& text)
{
  if(text=="") return text;
  return text.substr(1,text.length()-2);
}

inline vector<G4String> StripComma(const G4String& text)
{
  vector<G4String> strippedtext;
  G4String comma_text = text;
  if(text==',')
    {
      return strippedtext;
    }
  
  G4int length = comma_text.length();
  G4int curr=0;
  while(comma_text.contains(','))
    {
      curr = comma_text.first(',');
      //G4cout << "Init: " << comma_text << G4endl;
      if(curr!=0) strippedtext.push_back(comma_text.substr(0,curr));
      //if(curr!=0) G4cout << "Res: " << comma_text.substr(0,curr) << G4endl;

      if(curr+1>=length) 
	{
	  comma_text="";
	  break;
	}
      else comma_text = comma_text.substr(curr+1,length);
    }
  if(comma_text.length()>0) strippedtext.push_back(comma_text);
  //if(comma_text.length()>0) G4cout << "Res: " << comma_text << G4endl;
  return strippedtext;

}
inline G4String StripFirst(const G4String& text)
{
  if(text=="") return text;
  return text.substr(1,text.length());
}
inline G4String StripEnd(const G4String& text)
{
  if(text=="") return text;
  return text.substr(0,text.length()-2);
}

BDSMySQLWrapper::BDSMySQLWrapper (const G4String& SQLFileName)
  : ifs(SQLFileName.c_str()), ComponentN(0), tableN(-1)
  
{
  if(ifs) G4cout<<"BDSMySQLWrapper contructor: Loading SQL Filename="<<SQLFileName<<G4endl;
  else G4cout<<"BDSMySQLWrapper constructor: Unable to load SQL file: "<<SQLFileName<<G4endl;
}

vector<BDSMySQLTable*> BDSMySQLWrapper::ConstructTable ()
{
  // reset the stream pointer
  ifs.seekg(0);
  ComponentN=0;
  tableN=-1;
  while(ifs) ComponentN+=ReadComponent();
  ifs.close();
  return table;
}


BDSMySQLWrapper::~BDSMySQLWrapper()
{
}

G4int BDSMySQLWrapper::ReadComponent()
{
#define  _READ(value) if(!(ifs>>value)) return 0;


#define CMD_CREATE   "CREATE"
#define CMD_TABLE    "TABLE"
#define CMD_INSERT   "INSERT"
#define CMD_INTO     "INTO"
#define CMD_VALUES   "VALUES"
#define CMD_DROP     "DROP"
#define CMD_DATABASE "DATABASE"
#define CMD_USE      "USE"
#define CMD_IF       "IF"
#define CMD_EXISTS   "EXISTS"

  G4String input;
  G4String varname;
  G4String vartype;
  
  char buffer[255];
  _READ(input);
  G4cout << __METHOD_NAME__ << " reading input: " << input << G4endl;
  if(input.contains("#")){// This is a comment line  
    ifs.getline(buffer,255);
    G4cout << __METHOD_NAME__ << " returning 0..." << G4endl;
    return 0;
  }

  // not using these commands yet...
  if(input==CMD_DROP) {   G4cout << __METHOD_NAME__ << " returning 0..." << G4endl; return 0;}
  else if(input==CMD_DATABASE) {   G4cout << __METHOD_NAME__ << " returning 0..." << G4endl; return 0;}
  else if(input==CMD_IF) {   G4cout << __METHOD_NAME__ << " returning 0..." << G4endl; return 0;}
  else if(input==CMD_EXISTS) {   G4cout << __METHOD_NAME__ << " returning 0..." << G4endl; return 0;}
  else if(input==CMD_USE) {   G4cout << __METHOD_NAME__ << " returning 0..." << G4endl; return 0;}

  else if(input==CMD_CREATE)
    {
      _READ(input);
      G4cout << __METHOD_NAME__ << " reading input: " << input << G4endl;
      if(input==CMD_DATABASE) {
	G4cout << __METHOD_NAME__ << " returning 0..." << G4endl;
	return 0;
      }
      else if(input==CMD_TABLE)
	{
	  _READ(CurrentTableName);
	  G4cout << __METHOD_NAME__ << " reading CurrentTableName: " << CurrentTableName << G4endl;
	  ifs.getline(buffer,255); // dumping rest of line.
	  table.push_back(new BDSMySQLTable(CurrentTableName));
	  tableN++;

	  do
	    {
	      _READ(varname);
	      G4cout << __METHOD_NAME__ << " reading varname: " << varname << G4endl;
	      if(!varname.contains(";"))
		{
		  _READ(vartype);
		  G4cout << __METHOD_NAME__ << " reading vartype: " << vartype << G4endl;
		  if(vartype.contains("DOUBLE")) vartype="DOUBLE";
		  else if(vartype.contains("VARCHAR")) vartype="STRING";
		  else if(vartype.contains("INTEGER")) vartype="INTEGER";
		  table[tableN]->AddVariable(varname,vartype);
		  ifs.getline(buffer,255); // dumping rest of line
		}
	    }
	  while (!varname.contains(";")); // skipping to end of SQL table creation
	}
      {   G4cout << __METHOD_NAME__ << " returning 0..." << G4endl; return 0;}
    }
  
  else if(input==CMD_INSERT)
    {
      _READ(input);
      G4cout << __METHOD_NAME__ << " reading input: " << input << G4endl;
      if(input==CMD_INTO)
	{
	  _READ(InsertTableName);
	  G4cout << __METHOD_NAME__ << " reading InsertTableName: " << InsertTableName << G4endl;	  

	  _READ(input);
	  G4cout << __METHOD_NAME__ << " reading input: " << input << G4endl;	  
	  if(input==CMD_VALUES)
	    {
	      for(G4int j=0; j<(G4int)table.size(); j++)
		{
		  if(table[j]->GetName()==InsertTableName)
		    {
		      for(G4int k=0; k<table[j]->GetNVariables(); k++)
			{
			  _READ(input);
			  G4cout << __METHOD_NAME__ << " reading input: " << input << G4endl;	  
			  //G4cout << "Input: " << input << "\t";
			  if(input=="(") {
			    _READ(input)
			      G4cout << __METHOD_NAME__ << " reading input: " << input << G4endl;	  
			  }
			  if(input.contains("(")) input = StripFirst(input);
			  if(input.contains(");")) input = StripEnd(input);

			  if(input.contains(","))
			    {
			      vector<G4String> vctInput = StripComma(input);
			      if(vctInput.size()==0) k--;
			      for(G4int i=0; i<(G4int)vctInput.size(); i++)
				{
				  //G4cout << "Res: " << vctInput[i] << G4endl;
				  if(table[j]->GetVariable(k)->GetVarType()=="DOUBLE")
				    table[j]->GetVariable(k)->AddValue(atof(vctInput[i])*mm);
				  else if(table[j]->GetVariable(k)->GetVarType()=="STRING")
				    table[j]->GetVariable(k)->AddValue(StripQuotes(vctInput[i]));
				  else if(table[j]->GetVariable(k)->GetVarType()=="INTEGER")
				    table[j]->GetVariable(k)->AddValue(atoi(vctInput[i]));
				  if(i!=(G4int)vctInput.size()-1) k++;
				}
			    }
			  else
			    {
			      //G4cout << "Res: " << input << G4endl;
			      if(table[j]->GetVariable(k)->GetVarType()=="DOUBLE")
				table[j]->GetVariable(k)->AddValue(atof(input)*mm);
			      else if(table[j]->GetVariable(k)->GetVarType()=="STRING")
				table[j]->GetVariable(k)->AddValue(StripQuotes(input));
			      else if(table[j]->GetVariable(k)->GetVarType()=="INTEGER")
				table[j]->GetVariable(k)->AddValue(atoi(input));

			    }
			  
			}
		    }

		}
	    }
	} else {
	G4cout << __METHOD_NAME__ << " input: " << input << " does not equal command " << CMD_INTO << G4endl;
      }
      return 1;
    }
  else {   G4cout << __METHOD_NAME__ << " returning 0..." << G4endl; return 0;}
}
