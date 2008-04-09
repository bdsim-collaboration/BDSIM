#include "BDSGlobalConstants.hh" // must be first in include list
#include "BDSGeometrySQL.hh"
#include "G4Box.hh"
#include "G4Trap.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4EllipticalCone.hh"
#include "G4Torus.hh"
#include "G4SubtractionSolid.hh"
#include "G4Polycone.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4UserLimits.hh"
#include "BDSMySQLWrapper.hh"
#include "BDSMaterials.hh"
#include "G4SDManager.hh"
#include "BDSSamplerSD.hh"
#include "BDSSampler.hh"
#include "BDSOutput.hh"
#include <vector>
#include <map>
#include <cstdlib>
#include "G4ClassicalRK4.hh"
#include <cstring>

using namespace std;

extern BDSSamplerSD* BDSSamplerSensDet;

extern BDSMaterials* theMaterials;
extern G4RotationMatrix* RotY90;
extern BDSOutput bdsOutput;
extern BDSGlobalConstants* BDSGlobals;

BDSGeometrySQL::BDSGeometrySQL(G4String DBfile, G4double markerlength)
{
  itsMarkerLength = markerlength;
  ifs.open(DBfile.c_str());
  if(!ifs) G4Exception("Unable to load SQL database file: " + DBfile);
  align_in_volume = NULL;  //default alignment (does nothing)
  align_out_volume = NULL;  //default alignment (does nothing)
  HasFields = false;
}

BDSGeometrySQL::~BDSGeometrySQL()
{;}

void BDSGeometrySQL::Construct(G4LogicalVolume *marker)
{
  itsMarkerVol = marker;
  VOL_LIST.push_back(itsMarkerVol);
  G4String file;
  char buffer[1000];
  while (ifs>>file)
    {
      if(file.contains("#")) ifs.getline(buffer,1000); // This is a comment line
      else BuildSQLObjects(file);
    }
  
  // Close Geomlist file
  ifs.close();
}

void BDSGeometrySQL::BuildSQLObjects(G4String file)
{

  BDSMySQLWrapper sql(file);
  itsSQLTable=sql.ConstructTable();

  for (G4int i=0; i<(G4int)itsSQLTable.size(); i++)
    {

      G4String TableName = itsSQLTable[i]->GetName();
      G4int pos = TableName.find("_");
      G4String ObjectType = TableName.substr(pos+1,TableName.length() - pos);
      G4String::caseCompare cmpmode = G4String::ignoreCase;
      if(ObjectType.compareTo("CONE",cmpmode)==0) BuildCone(itsSQLTable[i]);
      else if(ObjectType.compareTo("ELLIPTICALCONE",cmpmode)==0) BuildEllipticalCone(itsSQLTable[i]);
      else if(ObjectType.compareTo("POLYCONE",cmpmode)==0) BuildPolyCone(itsSQLTable[i]);
      else if(ObjectType.compareTo("BOX",cmpmode)==0) BuildBox(itsSQLTable[i]);
      else if(ObjectType.compareTo("TRAP",cmpmode)==0) BuildTrap(itsSQLTable[i]);
      else if(ObjectType.compareTo("TORUS",cmpmode)==0) BuildTorus(itsSQLTable[i]);
      else if(ObjectType.compareTo("SAMPLER",cmpmode)==0) BuildSampler(itsSQLTable[i]);
      else if(ObjectType.compareTo("TUBE",cmpmode)==0) BuildTube(itsSQLTable[i]);
    }

}

void BDSGeometrySQL::BuildCone(BDSMySQLTable* aSQLTable)
{
  G4int NVariables = aSQLTable->GetVariable("LENGTH")->GetNVariables();

  G4double length;
  G4double rInnerStart;
  G4double rInnerEnd;
  G4double rOuterStart;
  G4double rOuterEnd;
  G4double VisRed; 
  G4double VisGreen;
  G4double VisBlue;
  G4String VisType;
  G4String Material;
  G4String TableName = aSQLTable->GetName();
  G4String Name;
  G4double sphi;
  G4double dphi;

  for(G4int k=0; k<NVariables; k++)
    {
      //Defaults
      sphi =0.0;
      dphi = twopi*radian;
      length = rOuterStart = rOuterEnd = 10.*mm;
      rInnerStart = rInnerEnd = 0.0;
      VisRed = VisGreen = VisBlue = 0.;
      VisType = "S";
      Material = "VACUUM";

      if(aSQLTable->GetVariable("RED")!=NULL)
	VisRed = aSQLTable->GetVariable("RED")->GetDblValue(k);
      if(aSQLTable->GetVariable("BLUE")!=NULL)
	VisBlue = aSQLTable->GetVariable("BLUE")->GetDblValue(k);
      if(aSQLTable->GetVariable("GREEN")!=NULL)
	VisGreen = aSQLTable->GetVariable("GREEN")->GetDblValue(k);
      if(aSQLTable->GetVariable("VISATT")!=NULL)
	VisType = aSQLTable->GetVariable("VISATT")->GetStrValue(k);
      if(aSQLTable->GetVariable("LENGTH")!=NULL)
	length = aSQLTable->GetVariable("LENGTH")->GetDblValue(k);
      if(aSQLTable->GetVariable("RINNERSTART")!=NULL)
	rInnerStart = aSQLTable->GetVariable("RINNERSTART")->GetDblValue(k);
      if(aSQLTable->GetVariable("RINNEREND")!=NULL)
	rInnerEnd = aSQLTable->GetVariable("RINNEREND")->GetDblValue(k);
      if(aSQLTable->GetVariable("ROUTERSTART")!=NULL)
	rOuterStart = aSQLTable->GetVariable("ROUTERSTART")->GetDblValue(k);
      if(aSQLTable->GetVariable("ROUTEREND")!=NULL)
	rOuterEnd = aSQLTable->GetVariable("ROUTEREND")->GetDblValue(k);
      if(aSQLTable->GetVariable("STARTPHI")!=NULL)
	sphi = aSQLTable->GetVariable("STARTPHI")->GetDblValue(k);
      if(aSQLTable->GetVariable("DELTAPHI")!=NULL)
	dphi = aSQLTable->GetVariable("DELTAPHI")->GetDblValue(k);
      if(aSQLTable->GetVariable("MATERIAL")!=NULL)
	Material = aSQLTable->GetVariable("MATERIAL")->GetStrValue(k);
      if(aSQLTable->GetVariable("NAME")!=NULL)
	Name = aSQLTable->GetVariable("NAME")->GetStrValue(k);

      if(Name=="") Name = TableName+BDSGlobals->StringFromInt(k);

      // make sure that each name is unique!
      Name = itsMarkerVol->GetName()+"_"+Name;

      G4Cons* aCone = new G4Cons(Name+"_Cone",
				 rInnerStart,
				 rOuterStart,
				 rInnerEnd,
				 rOuterEnd,
				 length/2,
				 sphi,
				 dphi);

      G4LogicalVolume* aConeVol = 
	new G4LogicalVolume(aCone,
			    theMaterials->GetMaterial(Material),
			    Name+"_LogVol");
      
      G4UserLimits* ConeUserLimits = new G4UserLimits();
      ConeUserLimits->SetMaxAllowedStep(length);
      aConeVol->SetUserLimits(ConeUserLimits);
      G4VisAttributes* VisAtt = 
	new G4VisAttributes(G4Colour(VisRed, VisGreen, VisBlue));
      switch (VisType(0))
	{
	case 'W': VisAtt->SetForceWireframe(true); break;
	case 'I': VisAtt->SetVisibility(false); break;
	case 'S': VisAtt->SetForceSolid(true); break;
	case 'w': VisAtt->SetForceWireframe(true); break;
	case 'i': VisAtt->SetVisibility(false); break;
	case 's': VisAtt->SetForceSolid(true); break;
	}
      aConeVol->SetVisAttributes(VisAtt);

      VOL_LIST.push_back(aConeVol);

    }

  PlaceComponents(aSQLTable, VOL_LIST);
}

void BDSGeometrySQL::BuildEllipticalCone(BDSMySQLTable* aSQLTable)
{
  G4int NVariables = aSQLTable->GetVariable("LENGTHZ")->GetNVariables();

  G4double lengthZ = 0;
  G4double pxSemiAxis = 0;
  G4double pySemiAxis = 0;
  G4double pzTopCut = 0;
  G4double VisRed; 
  G4double VisGreen;
  G4double VisBlue;
  G4String VisType;
  G4String Material;
  G4String TableName = aSQLTable->GetName();
  G4String Name = "";

  for(G4int k=0; k<NVariables; k++)
    {
      //Defaults
      lengthZ = 10.*mm;
      VisRed = VisGreen = VisBlue = 0.;
      VisType = "S";
      Material = "VACUUM";

      if(aSQLTable->GetVariable("RED")!=NULL)
	VisRed = aSQLTable->GetVariable("RED")->GetDblValue(k);
      if(aSQLTable->GetVariable("BLUE")!=NULL)
	VisBlue = aSQLTable->GetVariable("BLUE")->GetDblValue(k);
      if(aSQLTable->GetVariable("GREEN")!=NULL)
	VisGreen = aSQLTable->GetVariable("GREEN")->GetDblValue(k);
      if(aSQLTable->GetVariable("VISATT")!=NULL)
	VisType = aSQLTable->GetVariable("VISATT")->GetStrValue(k);
      if(aSQLTable->GetVariable("LENGTHZ")!=NULL)
	lengthZ = aSQLTable->GetVariable("LENGTHZ")->GetDblValue(k);
      if(aSQLTable->GetVariable("XSEMIAXIS")!=NULL)
	pxSemiAxis = aSQLTable->GetVariable("XSEMIAXIS")->GetDblValue(k);
      if(aSQLTable->GetVariable("YSEMIAXIS")!=NULL)
	pySemiAxis = aSQLTable->GetVariable("YSEMIAXIS")->GetDblValue(k);
      if(aSQLTable->GetVariable("ZCUT")!=NULL)
	pzTopCut = aSQLTable->GetVariable("ZCUT")->GetDblValue(k);
      if(aSQLTable->GetVariable("MATERIAL")!=NULL)
	Material = aSQLTable->GetVariable("MATERIAL")->GetStrValue(k);
      if(aSQLTable->GetVariable("NAME")!=NULL)
	Name = aSQLTable->GetVariable("NAME")->GetStrValue(k);

      if(Name=="") Name = TableName+BDSGlobals->StringFromInt(k);

      // make sure that each name is unique!
      Name = itsMarkerVol->GetName()+"_"+Name;

      G4EllipticalCone* aEllipticalCone = new G4EllipticalCone(Name+"_EllipticalCone",
							       pxSemiAxis,
							       pySemiAxis,
							       lengthZ/2,
							       pzTopCut);

      G4LogicalVolume* aEllipticalConeVol = 
	new G4LogicalVolume(aEllipticalCone,
			    theMaterials->GetMaterial(Material),
			    Name+"_LogVol");
      
      G4UserLimits* EllipticalConeUserLimits = new G4UserLimits();
      EllipticalConeUserLimits->SetMaxAllowedStep(lengthZ);
      aEllipticalConeVol->SetUserLimits(EllipticalConeUserLimits);
      G4VisAttributes* VisAtt = 
	new G4VisAttributes(G4Colour(VisRed, VisGreen, VisBlue));
      switch (VisType(0))
	{
	case 'W': VisAtt->SetForceWireframe(true); break;
	case 'I': VisAtt->SetVisibility(false); break;
	case 'S': VisAtt->SetForceSolid(true); break;
	case 'w': VisAtt->SetForceWireframe(true); break;
	case 'i': VisAtt->SetVisibility(false); break;
	case 's': VisAtt->SetForceSolid(true); break;
	}
      aEllipticalConeVol->SetVisAttributes(VisAtt);

      VOL_LIST.push_back(aEllipticalConeVol);

    }

  PlaceComponents(aSQLTable, VOL_LIST);
}

void BDSGeometrySQL::BuildPolyCone(BDSMySQLTable* aSQLTable)
{
  G4int NVariables = aSQLTable->GetVariable("NZPLANES")->GetNVariables();

  G4int numZplanes;
  G4double* rInner = NULL;
  G4double* rOuter = NULL;
  G4double* zPos = NULL;
  G4double VisRed; 
  G4double VisGreen;
  G4double VisBlue;
  G4String VisType;
  G4String Material;
  G4String TableName = aSQLTable->GetName();
  G4String Name;
  G4double sphi;
  G4double dphi;

  for(G4int k=0; k<NVariables; k++)
    {
      //Defaults
      sphi = 0.0;
      dphi = twopi*radian;
      numZplanes = 0;
      VisRed = VisGreen = VisBlue = 0.;
      VisType = "S";
      Material = "VACUUM";

      if(aSQLTable->GetVariable("NZPLANES")!=NULL)
	numZplanes = aSQLTable->GetVariable("NZPLANES")->GetIntValue(k);
      rInner = new G4double[numZplanes+1];
      rOuter = new G4double[numZplanes+1];
      zPos = new G4double[numZplanes+1];
      
      for(G4int planenum=0; planenum<numZplanes; planenum++)
	{
	  G4String rInner_ID = "RINNER" + BDSGlobals->StringFromInt(planenum+1);
	  G4String rOuter_ID = "ROUTER" + BDSGlobals->StringFromInt(planenum+1);
	  G4String zPos_ID = "PLANEPOS" + BDSGlobals->StringFromInt(planenum+1);

	  if(aSQLTable->GetVariable(rInner_ID)!=NULL)
	    rInner[planenum] = aSQLTable->GetVariable(rInner_ID)->GetDblValue(k);
	  if(aSQLTable->GetVariable(rOuter_ID)!=NULL)
	    rOuter[planenum] = aSQLTable->GetVariable(rOuter_ID)->GetDblValue(k);

	  if(aSQLTable->GetVariable(zPos_ID)!=NULL)
	    zPos[planenum] = aSQLTable->GetVariable(zPos_ID)->GetDblValue(k);
	}

      if(aSQLTable->GetVariable("STARTPHI")!=NULL)
	sphi = aSQLTable->GetVariable("STARTPHI")->GetDblValue(k);
      if(aSQLTable->GetVariable("DELTAPHI")!=NULL)
	dphi = aSQLTable->GetVariable("DELTAPHI")->GetDblValue(k);
      if(aSQLTable->GetVariable("RED")!=NULL)
	VisRed = aSQLTable->GetVariable("RED")->GetDblValue(k);
      if(aSQLTable->GetVariable("BLUE")!=NULL)
	VisBlue = aSQLTable->GetVariable("BLUE")->GetDblValue(k);
      if(aSQLTable->GetVariable("GREEN")!=NULL)
	VisGreen = aSQLTable->GetVariable("GREEN")->GetDblValue(k);
      if(aSQLTable->GetVariable("VISATT")!=NULL)
	VisType = aSQLTable->GetVariable("VISATT")->GetStrValue(k);
      if(aSQLTable->GetVariable("MATERIAL")!=NULL)
	Material = aSQLTable->GetVariable("MATERIAL")->GetStrValue(k);
      if(aSQLTable->GetVariable("NAME")!=NULL)
	Name = aSQLTable->GetVariable("NAME")->GetStrValue(k);

      if(Name=="") Name = TableName+BDSGlobals->StringFromInt(k);

      // make sure that each name is unique!
      Name = itsMarkerVol->GetName()+"_"+Name;

      G4Polycone* aPolyCone = new G4Polycone(Name+"_PolyCone",
					     sphi,
					     dphi,
					     numZplanes,
					     zPos,
					     rInner,
					     rOuter);

      G4LogicalVolume* aPolyConeVol = 
	new G4LogicalVolume(aPolyCone,
			    theMaterials->GetMaterial(Material),
			    Name+"_LogVol");
      
      G4UserLimits* ConeUserLimits = new G4UserLimits();
      ConeUserLimits->SetMaxAllowedStep(fabs(zPos[0]-zPos[numZplanes-1]));
      aPolyConeVol->SetUserLimits(ConeUserLimits);
      G4VisAttributes* VisAtt = 
	new G4VisAttributes(G4Colour(VisRed, VisGreen, VisBlue));
      switch (VisType(0))
	{
	case 'W': VisAtt->SetForceWireframe(true); break;
	case 'I': VisAtt->SetVisibility(false); break;
	case 'S': VisAtt->SetForceSolid(true); break;
	case 'w': VisAtt->SetForceWireframe(true); break;
	case 'i': VisAtt->SetVisibility(false); break;
	case 's': VisAtt->SetForceSolid(true); break;
	}
      aPolyConeVol->SetVisAttributes(VisAtt);

      VOL_LIST.push_back(aPolyConeVol);
      
      delete [] rInner;
      rInner = NULL;
      delete [] rOuter;
      rOuter = NULL;
      delete [] zPos;
      zPos = NULL;

    }

  PlaceComponents(aSQLTable, VOL_LIST);
}

void BDSGeometrySQL::BuildBox(BDSMySQLTable* aSQLTable)
{
  G4int NVariables = aSQLTable->GetVariable("LENGTHX")->GetNVariables();

  G4double lengthX;
  G4double lengthY;
  G4double lengthZ;
  G4double VisRed;
  G4double VisGreen;
  G4double VisBlue;
  G4String VisType;
  G4String Material;
  G4String TableName = aSQLTable->GetName();

  G4String Name;

  for(G4int k=0; k<NVariables; k++)
    {
      lengthX = lengthY = lengthZ = 10.*mm;
      VisRed = VisGreen = VisBlue = 0.;
      VisType = "S";
      Material = "VACUUM";
      if(aSQLTable->GetVariable("RED")!=NULL)
	VisRed = aSQLTable->GetVariable("RED")->GetDblValue(k);
      if(aSQLTable->GetVariable("BLUE")!=NULL)
	VisBlue = aSQLTable->GetVariable("BLUE")->GetDblValue(k);
      if(aSQLTable->GetVariable("GREEN")!=NULL)
	VisGreen = aSQLTable->GetVariable("GREEN")->GetDblValue(k);
      if(aSQLTable->GetVariable("VISATT")!=NULL)
	VisType = aSQLTable->GetVariable("VISATT")->GetStrValue(k);
      if(aSQLTable->GetVariable("LENGTHX")!=NULL)
	lengthX = aSQLTable->GetVariable("LENGTHX")->GetDblValue(k);
      if(aSQLTable->GetVariable("LENGTHY")!=NULL)
	lengthY = aSQLTable->GetVariable("LENGTHY")->GetDblValue(k);
      if(aSQLTable->GetVariable("LENGTHZ")!=NULL)
	lengthZ = aSQLTable->GetVariable("LENGTHZ")->GetDblValue(k);
      if(aSQLTable->GetVariable("MATERIAL")!=NULL)
	Material = aSQLTable->GetVariable("MATERIAL")->GetStrValue(k);
      if(aSQLTable->GetVariable("NAME")!=NULL)
	Name = aSQLTable->GetVariable("NAME")->GetStrValue(k);

      if(Name=="") Name = TableName+BDSGlobals->StringFromInt(k);

      // make sure that each name is unique!
      Name = itsMarkerVol->GetName()+"_"+Name;

      G4Box* aBox = new G4Box(Name+"_Box",
			      lengthX/2,
			      lengthY/2,
			      lengthZ/2);
      
      G4LogicalVolume* aBoxVol = 
	new G4LogicalVolume(aBox,
			    theMaterials->GetMaterial(Material),
			    Name+"_LogVol");
      
      G4UserLimits* BoxUserLimits = new G4UserLimits();
      BoxUserLimits->SetMaxAllowedStep(lengthZ);
      aBoxVol->SetUserLimits(BoxUserLimits);
      G4VisAttributes* VisAtt = 
	new G4VisAttributes(G4Colour(VisRed, VisGreen, VisBlue));
      switch (VisType(0))
	{
	case 'W': VisAtt->SetForceWireframe(true); break;
	case 'I': VisAtt->SetVisibility(false); break;
	case 'S': VisAtt->SetForceSolid(true); break;
	case 'w': VisAtt->SetForceWireframe(true); break;
	case 'i': VisAtt->SetVisibility(false); break;
	case 's': VisAtt->SetForceSolid(true); break;
	}
      aBoxVol->SetVisAttributes(VisAtt);

      VOL_LIST.push_back(aBoxVol);
    }

  PlaceComponents(aSQLTable, VOL_LIST);
}

void BDSGeometrySQL::BuildTrap(BDSMySQLTable* aSQLTable)
{
  G4int NVariables = aSQLTable->GetVariable("LENGTHXPLUS")->GetNVariables();

  G4double lengthXPlus = 0;
  G4double lengthXMinus = 0;
  G4double lengthYPlus = 0;
  G4double lengthYMinus = 0;
  G4double lengthZ = 0;
  G4double VisRed;
  G4double VisGreen;
  G4double VisBlue;
  G4String VisType;
  G4String Material;
  G4String TableName = aSQLTable->GetName();

  G4String Name = "";

  for(G4int k=0; k<NVariables; k++)
    {
      VisRed = VisGreen = VisBlue = 0.;
      VisType = "S";
      Material = "VACUUM";
      if(aSQLTable->GetVariable("RED")!=NULL)
	VisRed = aSQLTable->GetVariable("RED")->GetDblValue(k);
      if(aSQLTable->GetVariable("BLUE")!=NULL)
	VisBlue = aSQLTable->GetVariable("BLUE")->GetDblValue(k);
      if(aSQLTable->GetVariable("GREEN")!=NULL)
	VisGreen = aSQLTable->GetVariable("GREEN")->GetDblValue(k);
      if(aSQLTable->GetVariable("VISATT")!=NULL)
	VisType = aSQLTable->GetVariable("VISATT")->GetStrValue(k);
      if(aSQLTable->GetVariable("LENGTHXPLUS")!=NULL)
	lengthXPlus = aSQLTable->GetVariable("LENGTHXPLUS")->GetDblValue(k);
      if(aSQLTable->GetVariable("LENGTHXMINUS")!=NULL)
	lengthXMinus = aSQLTable->GetVariable("LENGTHXMINUS")->GetDblValue(k);
      if(aSQLTable->GetVariable("LENGTHYPLUS")!=NULL)
	lengthYPlus = aSQLTable->GetVariable("LENGTHYPLUS")->GetDblValue(k);
      if(aSQLTable->GetVariable("LENGTHYMINUS")!=NULL)
	lengthYMinus = aSQLTable->GetVariable("LENGTHYMINUS")->GetDblValue(k);
      if(aSQLTable->GetVariable("LENGTHZ")!=NULL)
	lengthZ = aSQLTable->GetVariable("LENGTHZ")->GetDblValue(k);
      if(aSQLTable->GetVariable("MATERIAL")!=NULL)
	Material = aSQLTable->GetVariable("MATERIAL")->GetStrValue(k);
      if(aSQLTable->GetVariable("NAME")!=NULL)
	Name = aSQLTable->GetVariable("NAME")->GetStrValue(k);

      if(Name=="") Name = TableName+BDSGlobals->StringFromInt(k);

      // make sure that each name is unique!
      Name = itsMarkerVol->GetName()+"_"+Name;

      G4Trap* aTrap = new G4Trap(Name+"_Trd",
				 lengthXPlus/2,
				 lengthXMinus/2,
				 lengthYPlus/2,
				 lengthYMinus/2,
				 lengthZ/2);
      
      G4LogicalVolume* aTrapVol = 
	new G4LogicalVolume(aTrap,
			    theMaterials->GetMaterial(Material),
			    Name+"_LogVol");
      
      G4UserLimits* TrapUserLimits = new G4UserLimits();
      TrapUserLimits->SetMaxAllowedStep(lengthZ);
      aTrapVol->SetUserLimits(TrapUserLimits);
      G4VisAttributes* VisAtt = 
	new G4VisAttributes(G4Colour(VisRed, VisGreen, VisBlue));
      switch (VisType(0))
	{
	case 'W': VisAtt->SetForceWireframe(true); break;
	case 'I': VisAtt->SetVisibility(false); break;
	case 'S': VisAtt->SetForceSolid(true); break;
	case 'w': VisAtt->SetForceWireframe(true); break;
	case 'i': VisAtt->SetVisibility(false); break;
	case 's': VisAtt->SetForceSolid(true); break;
	}
      aTrapVol->SetVisAttributes(VisAtt);

      VOL_LIST.push_back(aTrapVol);
    }

  PlaceComponents(aSQLTable, VOL_LIST);
}

void BDSGeometrySQL::BuildTorus(BDSMySQLTable* aSQLTable)
{
  G4int NVariables = aSQLTable->GetVariable("RINNER")->GetNVariables();

  G4double rInner;
  G4double rOuter;
  G4double rSwept;
  G4double sphi;
  G4double dphi;
  G4double VisRed; 
  G4double VisGreen;
  G4double VisBlue;
  G4String VisType;
  G4String Material;
  G4String TableName = aSQLTable->GetName();
  G4String Name;

  for(G4int k=0; k<NVariables; k++)
    {
      //Defaults
      rSwept = 20.*mm;
      rOuter = 10.*mm;
      rInner = 0.0;
      sphi = 0.0;
      dphi=2*pi*radian;
      VisRed = VisGreen = VisBlue = 0.;
      VisType = "S";
      Material = "VACUUM";

      if(aSQLTable->GetVariable("RED")!=NULL)
	VisRed = aSQLTable->GetVariable("RED")->GetDblValue(k);
      if(aSQLTable->GetVariable("BLUE")!=NULL)
	VisBlue = aSQLTable->GetVariable("BLUE")->GetDblValue(k);
      if(aSQLTable->GetVariable("GREEN")!=NULL)
	VisGreen = aSQLTable->GetVariable("GREEN")->GetDblValue(k);
      if(aSQLTable->GetVariable("VISATT")!=NULL)
	VisType = aSQLTable->GetVariable("VISATT")->GetStrValue(k);
      if(aSQLTable->GetVariable("RINNER")!=NULL)
	rInner = aSQLTable->GetVariable("RINNER")->GetDblValue(k);
      if(aSQLTable->GetVariable("ROUTER")!=NULL)
	rOuter = aSQLTable->GetVariable("ROUTER")->GetDblValue(k);
      if(aSQLTable->GetVariable("RSWEPT")!=NULL)
	rSwept = aSQLTable->GetVariable("RSWEPT")->GetDblValue(k);
      if(aSQLTable->GetVariable("STARTPHI")!=NULL)
	sphi = aSQLTable->GetVariable("STARTPHI")->GetDblValue(k);
      if(aSQLTable->GetVariable("DELTAPHI")!=NULL)
	dphi = aSQLTable->GetVariable("DELTAPHI")->GetDblValue(k);
      if(aSQLTable->GetVariable("MATERIAL")!=NULL)
	Material = aSQLTable->GetVariable("MATERIAL")->GetStrValue(k);
      if(aSQLTable->GetVariable("NAME")!=NULL)
	Name = aSQLTable->GetVariable("NAME")->GetStrValue(k);

      if(Name=="") Name = TableName+BDSGlobals->StringFromInt(k);

      // make sure that each name is unique!
      Name = itsMarkerVol->GetName()+"_"+Name;

      G4Torus* aTorus = new G4Torus(Name+"_Torus",
				    rInner,
				    rOuter,
				    rSwept,
				    sphi,
				    dphi);


      G4LogicalVolume* aTorusVol = 
	new G4LogicalVolume(aTorus,
			    theMaterials->GetMaterial(Material),
			    Name+"_LogVol");
      
      G4UserLimits* TorusUserLimits = new G4UserLimits();
      TorusUserLimits->SetMaxAllowedStep(rInner);
      aTorusVol->SetUserLimits(TorusUserLimits);
      G4VisAttributes* VisAtt = 
	new G4VisAttributes(G4Colour(VisRed, VisGreen, VisBlue));
      switch (VisType(0))
	{
	case 'W': VisAtt->SetForceWireframe(true); break;
	case 'I': VisAtt->SetVisibility(false); break;
	case 'S': VisAtt->SetForceSolid(true); break;
	case 'w': VisAtt->SetForceWireframe(true); break;
	case 'i': VisAtt->SetVisibility(false); break;
	case 's': VisAtt->SetForceSolid(true); break;
	}
      aTorusVol->SetVisAttributes(VisAtt);

      VOL_LIST.push_back(aTorusVol);

    }

  PlaceComponents(aSQLTable, VOL_LIST);
}

void BDSGeometrySQL::BuildSampler(BDSMySQLTable* aSQLTable)
{
  G4int NVariables = aSQLTable->GetVariable("LENGTH")->GetNVariables();

  G4double length;
  G4double rInnerStart;
  G4double rInnerEnd;
  G4double rOuterStart;
  G4double rOuterEnd;
  G4double VisRed; 
  G4double VisGreen;
  G4double VisBlue;
  G4String VisType;
  G4String Material;
  G4String TableName = aSQLTable->GetName();
  G4String Name;

  for(G4int k=0; k<NVariables; k++)
    {
      //Defaults
      length = rOuterStart = rOuterEnd = 10.*mm;
      rInnerStart = rInnerEnd = 0.0;
      VisRed = VisGreen = VisBlue = 0.;
      VisType = "S";
      Material = "VACUUM";

      if(aSQLTable->GetVariable("RED")!=NULL)
	VisRed = aSQLTable->GetVariable("RED")->GetDblValue(k);
      if(aSQLTable->GetVariable("BLUE")!=NULL)
	VisBlue = aSQLTable->GetVariable("BLUE")->GetDblValue(k);
      if(aSQLTable->GetVariable("GREEN")!=NULL)
	VisGreen = aSQLTable->GetVariable("GREEN")->GetDblValue(k);
      if(aSQLTable->GetVariable("VISATT")!=NULL)
	VisType = aSQLTable->GetVariable("VISATT")->GetStrValue(k);
      if(aSQLTable->GetVariable("LENGTH")!=NULL)
	length = aSQLTable->GetVariable("LENGTH")->GetDblValue(k);
      if(aSQLTable->GetVariable("RINNERSTART")!=NULL)
	rInnerStart = aSQLTable->GetVariable("RINNERSTART")->GetDblValue(k);
      if(aSQLTable->GetVariable("RINNEREND")!=NULL)
	rInnerEnd = aSQLTable->GetVariable("RINNEREND")->GetDblValue(k);
      if(aSQLTable->GetVariable("ROUTERSTART")!=NULL)
	rOuterStart = aSQLTable->GetVariable("ROUTERSTART")->GetDblValue(k);
      if(aSQLTable->GetVariable("ROUTEREND")!=NULL)
	rOuterEnd = aSQLTable->GetVariable("ROUTEREND")->GetDblValue(k);
      if(aSQLTable->GetVariable("MATERIAL")!=NULL)
	Material = aSQLTable->GetVariable("MATERIAL")->GetStrValue(k);
      if(aSQLTable->GetVariable("NAME")!=NULL)
	{
	  Name = aSQLTable->GetVariable("NAME")->GetStrValue(k);
	  aSQLTable->GetVariable("NAME")->SetStrValue(k,Name+"_SQL");
	  Name = aSQLTable->GetVariable("NAME")->GetStrValue(k);
	}
      if(Name=="_SQL") Name = TableName+BDSGlobals->StringFromInt(k)+"_SQL";

      // make sure that each name is unique!
      Name = itsMarkerVol->GetName()+"_"+Name;
     
      G4Cons* aSampler = new G4Cons(Name+"_samp",
				    rInnerStart,
				    rOuterStart,
				    rInnerEnd,
				    rOuterEnd,
				    length/2,
				    0,
				    twopi*radian);

      G4LogicalVolume* aSamplerVol = 
	new G4LogicalVolume(aSampler,
			    theMaterials->GetMaterial(Material),
			    Name+"_LogVol");
      
      G4UserLimits* SamplerUserLimits = new G4UserLimits();
      SamplerUserLimits->SetMaxAllowedStep(length);
      aSamplerVol->SetUserLimits(SamplerUserLimits);
      G4VisAttributes* VisAtt = 
	new G4VisAttributes(G4Colour(VisRed, VisGreen, VisBlue));
      switch (VisType(0))
	{
	case 'W': VisAtt->SetForceWireframe(true); break;
	case 'I': VisAtt->SetVisibility(false); break;
	case 'S': VisAtt->SetForceSolid(true); break;
	case 'w': VisAtt->SetForceWireframe(true); break;
	case 'i': VisAtt->SetVisibility(false); break;
	case 's': VisAtt->SetForceSolid(true); break;
	}
      aSamplerVol->SetVisAttributes(VisAtt);

      G4SDManager* SDMan = G4SDManager::GetSDMpointer();
      if(BDSSampler::GetNSamplers==0){
	BDSSamplerSensDet = new BDSSamplerSD(Name, "plane");
	SDMan->AddNewDetector(BDSSamplerSensDet);
      }
      aSamplerVol->SetSensitiveDetector(BDSSamplerSensDet);
//SPM bdsOutput.nSamplers++;
      BDSSampler::AddExternalSampler();
      bdsOutput.SampName.push_back(BDSGlobals->StringFromInt(
					BDSSampler::GetNSamplers())+"_"+Name+"_1");

      VOL_LIST.push_back(aSamplerVol);
    }

  PlaceComponents(aSQLTable, VOL_LIST);
}

void BDSGeometrySQL::BuildTube(BDSMySQLTable* aSQLTable)
{
  G4int NVariables = aSQLTable->GetVariable("RINNER")->GetNVariables();

  G4double rInner;
  G4double rOuter;
  G4double length;
  G4double sphi;
  G4double dphi;
  G4double VisRed; 
  G4double VisGreen;
  G4double VisBlue;
  G4String VisType;
  G4String Material;
  G4String TableName = aSQLTable->GetName();
  G4String Name;

  for(G4int k=0; k<NVariables; k++)
    {
      //Defaults 
      length = 100.*mm;
      rOuter = 10.*mm;
      rInner = 0.0;
      sphi = 0.0;
      dphi=2*pi*radian;
      VisRed = VisGreen = VisBlue = 0.;
      VisType = "S";
      Material = "VACUUM";

      if(aSQLTable->GetVariable("RED")!=NULL)
	VisRed = aSQLTable->GetVariable("RED")->GetDblValue(k);
      if(aSQLTable->GetVariable("BLUE")!=NULL)
	VisBlue = aSQLTable->GetVariable("BLUE")->GetDblValue(k);
      if(aSQLTable->GetVariable("GREEN")!=NULL)
	VisGreen = aSQLTable->GetVariable("GREEN")->GetDblValue(k);
      if(aSQLTable->GetVariable("VISATT")!=NULL)
	VisType = aSQLTable->GetVariable("VISATT")->GetStrValue(k);
      if(aSQLTable->GetVariable("RINNER")!=NULL)
	rInner = aSQLTable->GetVariable("RINNER")->GetDblValue(k);
      if(aSQLTable->GetVariable("ROUTER")!=NULL)
	rOuter = aSQLTable->GetVariable("ROUTER")->GetDblValue(k);
      if(aSQLTable->GetVariable("LENGTH")!=NULL)
	length = aSQLTable->GetVariable("LENGTH")->GetDblValue(k);
      if(aSQLTable->GetVariable("STARTPHI")!=NULL)
	sphi = aSQLTable->GetVariable("STARTPHI")->GetDblValue(k);
      if(aSQLTable->GetVariable("DELTAPHI")!=NULL)
	dphi = aSQLTable->GetVariable("DELTAPHI")->GetDblValue(k);
      if(aSQLTable->GetVariable("MATERIAL")!=NULL)
	Material = aSQLTable->GetVariable("MATERIAL")->GetStrValue(k);
      if(aSQLTable->GetVariable("NAME")!=NULL)
	Name = aSQLTable->GetVariable("NAME")->GetStrValue(k);

      if(Name=="") Name = TableName+BDSGlobals->StringFromInt(k);

      // make sure that each name is unique!
      Name = itsMarkerVol->GetName()+"_"+Name;

      G4Tubs* aTubs = new G4Tubs(Name+"_Tubs",
				    rInner,
				    rOuter,
				    length/2,
				    sphi,
				    dphi);


      G4LogicalVolume* aTubsVol = 
	new G4LogicalVolume(aTubs,
			    theMaterials->GetMaterial(Material),
			    Name+"_LogVol");
      
      G4UserLimits* TubsUserLimits = new G4UserLimits();
      TubsUserLimits->SetMaxAllowedStep(length);
      aTubsVol->SetUserLimits(TubsUserLimits);
      G4VisAttributes* VisAtt = 
	new G4VisAttributes(G4Colour(VisRed, VisGreen, VisBlue));
      switch (VisType(0))
	{
	case 'W': VisAtt->SetForceWireframe(true); break;
	case 'I': VisAtt->SetVisibility(false); break;
	case 'S': VisAtt->SetForceSolid(true); break;
	case 'w': VisAtt->SetForceWireframe(true); break;
	case 'i': VisAtt->SetVisibility(false); break;
	case 's': VisAtt->SetForceSolid(true); break;
	}
      aTubsVol->SetVisAttributes(VisAtt);

      VOL_LIST.push_back(aTubsVol);

    }

  PlaceComponents(aSQLTable, VOL_LIST);
}

G4RotationMatrix* BDSGeometrySQL::RotateComponent(G4double psi,G4double phi,G4double theta)
{
  G4RotationMatrix *rotateComponent = new G4RotationMatrix;
  if(psi==0 && phi==0 && theta==0) return rotateComponent;

  G4RotationMatrix LocalRotation;
  G4ThreeVector localX = G4ThreeVector(1.,0.,0.);
  G4ThreeVector localY = G4ThreeVector(0.,1.,0.);
  G4ThreeVector localZ = G4ThreeVector(0.,0.,1.);
  
  LocalRotation.rotate(psi,localZ);
  localX.rotate(psi,localZ);
  localY.rotate(psi,localZ);
  
  
  LocalRotation.rotate(phi,localY);
  localX.rotate(phi,localY);
  localZ.rotate(phi,localY);
  
  
  LocalRotation.rotate(theta,localX);
  localY.rotate(theta,localX);
  localZ.rotate(theta,localX);
  
  rotateComponent->transform(LocalRotation);
  rotateComponent->invert();
  
  return rotateComponent;
}


void BDSGeometrySQL::PlaceComponents(BDSMySQLTable* aSQLTable, vector<G4LogicalVolume*> VOL_LIST)
{
  G4double PosX;
  G4double PosY;
  G4double PosZ;
  G4double RotPsi;
  G4double RotTheta;
  G4double RotPhi;
  G4double K1,K2,K3,K4;
  G4String PARENTNAME;
  G4String InheritStyle;
  G4String Name;
  G4String MagType;
  G4String TableName = aSQLTable->GetName();
  G4int NVariables = aSQLTable->GetVariable("POSX")->GetNVariables();
  G4int align_in;
  G4int align_out;
  G4int SetSensitive;
  G4double FieldX, FieldY, FieldZ;

  for(G4int k=0; k<NVariables; k++) // Now run through and place according to
    { 
      //Defaults
      PosX = PosY = PosZ = 0.;
      RotPsi = RotTheta = RotPhi = 0.;
      K1 = K2 = K3 = K4 = 0.;
      PARENTNAME = "";
      InheritStyle = "";
      align_in=0;
      align_out=0;
      SetSensitive=0;
      MagType = "";
      FieldX = FieldY = FieldZ = 0.0;
      if(aSQLTable->GetVariable("PARENTNAME")!=NULL)
	PARENTNAME = aSQLTable->GetVariable("PARENTNAME")->GetStrValue(k);
      if(aSQLTable->GetVariable("POSX")!=NULL)
	PosX = aSQLTable->GetVariable("POSX")->GetDblValue(k);
      if(aSQLTable->GetVariable("POSY")!=NULL)
	PosY = aSQLTable->GetVariable("POSY")->GetDblValue(k);
      if(aSQLTable->GetVariable("POSZ")!=NULL)
	PosZ = aSQLTable->GetVariable("POSZ")->GetDblValue(k);
      if(aSQLTable->GetVariable("ROTPSI")!=NULL)
	RotPsi = aSQLTable->GetVariable("ROTPSI")->GetDblValue(k);
      if(aSQLTable->GetVariable("ROTTHETA")!=NULL)
	RotTheta = aSQLTable->GetVariable("ROTTHETA")->GetDblValue(k);
      if(aSQLTable->GetVariable("ROTPHI")!=NULL)
	RotPhi = aSQLTable->GetVariable("ROTPHI")->GetDblValue(k);
      if(aSQLTable->GetVariable("K1")!=NULL)
	K1 = aSQLTable->GetVariable("K1")->GetDblValue(k);
      if(aSQLTable->GetVariable("K2")!=NULL)
	K2 = aSQLTable->GetVariable("K2")->GetDblValue(k);
      if(aSQLTable->GetVariable("K3")!=NULL)
	K3 = aSQLTable->GetVariable("K3")->GetDblValue(k);
      if(aSQLTable->GetVariable("K4")!=NULL)
	K4 = aSQLTable->GetVariable("K4")->GetDblValue(k);
      if(aSQLTable->GetVariable("MAGTYPE")!=NULL)
	MagType = aSQLTable->GetVariable("MAGTYPE")->GetStrValue(k);
      if(aSQLTable->GetVariable("FIELDX")!=NULL)
	FieldX = aSQLTable->GetVariable("FIELDX")->GetDblValue(k);
      if(aSQLTable->GetVariable("FIELDY")!=NULL)
	FieldY = aSQLTable->GetVariable("FIELDY")->GetDblValue(k);
      if(aSQLTable->GetVariable("FIELDZ")!=NULL)
	FieldZ = aSQLTable->GetVariable("FIELDZ")->GetDblValue(k);
      if(aSQLTable->GetVariable("ALIGNIN")!=NULL)
	align_in = aSQLTable->GetVariable("ALIGNIN")->GetIntValue(k);
      if(aSQLTable->GetVariable("ALIGNOUT")!=NULL)
	align_out = aSQLTable->GetVariable("ALIGNOUT")->GetIntValue(k);
      if(aSQLTable->GetVariable("SETSENSITIVE")!=NULL)
	SetSensitive = aSQLTable->GetVariable("SETSENSITIVE")->GetIntValue(k);
      if(aSQLTable->GetVariable("INHERITSTYLE")!=NULL)
	InheritStyle = aSQLTable->GetVariable("INHERITSTYLE")->GetStrValue(k);
      if(aSQLTable->GetVariable("NAME")!=NULL)
	Name = aSQLTable->GetVariable("NAME")->GetStrValue(k);
      if(Name=="_SQL") Name = TableName+BDSGlobals->StringFromInt(k) + "_SQL";
      if(Name=="") Name = TableName+BDSGlobals->StringFromInt(k);

      // make sure that each name is unique!
      Name = itsMarkerVol->GetName()+"_"+Name;

      if(PARENTNAME=="") PosZ-=itsMarkerLength/2; //Move definition of PosZ to front of element
            
      PARENTNAME=itsMarkerVol->GetName()+"_"+PARENTNAME;
      G4String::caseCompare cmpmode = G4String::ignoreCase;

      G4int PARENTID=0;
      if(PARENTNAME!=""){
	PARENTNAME+="_LogVol";
	for(G4int i=0; i<(G4int)VOL_LIST.size(); i++)
	  {
	    if(PARENTNAME.compareTo(VOL_LIST[i]->GetName(),cmpmode)==0)
	      {
		PARENTID = i;
		continue;
	      }
	  }
      }
      
      // to being in line with logvol names (needed for name checking loop
      G4String tmpname = Name+"_LogVol";
      G4int ID=0;
      for(G4int i=0; i<(G4int)VOL_LIST.size(); i++)
	{
	  if(tmpname.compareTo(VOL_LIST[i]->GetName(),cmpmode)==0)
	    {
	      ID = i;
	      continue;
	    }
	}

      if(SetSensitive) SensitiveComponents.push_back(VOL_LIST[ID]);

      G4ThreeVector PlacementPoint(PosX,PosY,PosZ);

      if(InheritStyle.compareTo("SUBTRACT",cmpmode)==0)
	{
	  G4VSolid* original = VOL_LIST[PARENTID]->GetSolid();
	  G4VSolid* sub = VOL_LIST[ID]->GetSolid();
	  VOL_LIST[PARENTID]->SetSolid(new G4SubtractionSolid(VOL_LIST[PARENTID]->GetName(),
							 original,
							 sub,
							 RotateComponent(RotPsi,RotPhi,RotTheta),
							 PlacementPoint));
	  continue;
	}


      G4VPhysicalVolume* PhysiComp=
	new G4PVPlacement(RotateComponent(RotPsi,RotPhi,RotTheta),
			  PlacementPoint,
			  VOL_LIST[ID],
			  Name,
			  VOL_LIST[PARENTID],
			  false,
			  0);

      if(align_in)
	{
	  // Make sure program stops and informs user if more than one alignment vol.
	  if(align_in_volume!=NULL)
	    {
	      G4cerr<<"\nBDSGeometrySQL.cc:486: Trying to align in-beam to SQL volume to " << PhysiComp->GetName() << " but alignment already set to " << align_in_volume->GetName() << G4endl;
	      G4Exception("Aborting Program");

	    }

	  else
	    align_in_volume=PhysiComp;

	}

      if(align_out)
	{
	  if(align_out_volume!=NULL)
	    {
	      G4cerr<<"\nBDSGeometrySQL.cc:486: Trying to align out-beam to SQL volume to " << PhysiComp->GetName() << " but alignment already set to " << align_out_volume->GetName() << G4endl;
	      G4Exception("Aborting Program");

	    }

	  else
	    align_out_volume=PhysiComp;
	}

      G4double P0 = BDSGlobals->GetBeamTotalEnergy();
      G4double brho=
	sqrt(pow(P0,2)- pow(electron_mass_c2,2))/(0.299792458 * (GeV/(tesla*m)));
      if(MagType.compareTo("QUAD",cmpmode)==0)
	{
	  HasFields = true;
	  QuadBgrad.push_back(brho * K1 * tesla / m);
	  Quadvol.push_back(PhysiComp->GetName());
	}

      if(MagType.compareTo("SEXT",cmpmode)==0)
	{
	  HasFields = true;
	  SextBgrad.push_back(brho * K2 * tesla / (m*m));
	  Sextvol.push_back(PhysiComp->GetName());
	}

      if(MagType.compareTo("OCT",cmpmode)==0)
	{
	  HasFields = true;
	  OctBgrad.push_back(brho * K3 * tesla / (m*m*m));
	  Octvol.push_back(PhysiComp->GetName());
	}

      if(FieldX || FieldY || FieldZ) //if any vols have non-zero field components
	{
	  HasFields = true;
	  UniformField.push_back(G4ThreeVector(FieldX*tesla,FieldY*tesla,FieldZ*tesla));
	  Fieldvol.push_back(PhysiComp->GetName());
	}
  }
}
