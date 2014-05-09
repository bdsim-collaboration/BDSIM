#include "BDSDebug.hh"
#include "BDSExecOptions.hh"
#include "BDSGlobalConstants.hh" 
#include "BDSGeometrySQL.hh"
#include "G4Box.hh"
#include "G4Trap.hh"
#include "G4Tubs.hh"
#include "G4EllipticalTube.hh"
#include "G4Cons.hh"
#include "G4EllipticalCone.hh"
#include "G4Torus.hh"
#include "G4SubtractionSolid.hh"
#include "G4IntersectionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4Polycone.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4UserLimits.hh"
#include "G4ProductionCuts.hh"
#include "G4RegionStore.hh"
#include "BDSMySQLWrapper.hh"
#include "BDSMaterials.hh"
#include "G4SDManager.hh"
#include "BDSSamplerSD.hh"
#include "BDSSampler.hh"
#include "BDSOutput.hh"
#include "BDSPCLTube.hh"
#include <vector>
#include <cstdlib>
#include <cstring>
#include "parser/getEnv.h"

extern BDSSamplerSD* BDSSamplerSensDet;

extern BDSOutput* bdsOutput;
//extern BDSGlobalConstants* BDSGlobalConstants::Instance();

BDSGeometrySQL::BDSGeometrySQL(G4String DBfile, G4double markerlength):
  rotateComponent(NULL),itsMarkerVol(NULL)
{
  itsMarkerLength = markerlength;
#ifdef DEBUG
  G4cout << "BDSGeometrySQL constructor: loading SQL file " << DBfile << G4endl;
#endif
  G4String sBDSPATH = getEnv("BDSIMPATH");
  G4String fullPath = sBDSPATH + DBfile;
  ifs.open(fullPath.c_str());
  G4String exceptionString = "Unable to load SQL database file: " + DBfile;
  if(!ifs) G4Exception(exceptionString.c_str(), "-1", FatalException, "");
  align_in_volume = NULL;  //default alignment (does nothing)
  align_out_volume = NULL;  //default alignment (does nothing)
  HasFields = false;
  nPoleField = 0;
  HasUniformField = false;

  //Set up the precision region
  G4String pRegName="precisionRegionSQL";
  _precisionRegionSQL = G4RegionStore::GetInstance()->FindOrCreateRegion(pRegName);
  //  if(!_precisionRegionSQL->IsModified()){
    G4ProductionCuts* theProductionCuts = new G4ProductionCuts();
    if(BDSGlobalConstants::Instance()->GetProdCutPhotonsP()>0)
      theProductionCuts->SetProductionCut(BDSGlobalConstants::Instance()->GetProdCutPhotonsP(),G4ProductionCuts::GetIndex("gamma"));
    if(BDSGlobalConstants::Instance()->GetProdCutElectronsP()>0)
      theProductionCuts->SetProductionCut(BDSGlobalConstants::Instance()->GetProdCutElectronsP(),G4ProductionCuts::GetIndex("e-"));
    if(BDSGlobalConstants::Instance()->GetProdCutPositronsP()>0)
      theProductionCuts->SetProductionCut(BDSGlobalConstants::Instance()->GetProdCutPositronsP(),G4ProductionCuts::GetIndex("e+"));
    _precisionRegionSQL->SetProductionCuts(theProductionCuts);
    //  }
  //Set up the approximation region
  G4String vRegName="approximationRegionSQL";
  _approximationRegionSQL = G4RegionStore::GetInstance()->FindOrCreateRegion(vRegName);
  //  if(!_approximationRegionSQL->IsModified()){
    G4ProductionCuts* approxProductionCuts = new G4ProductionCuts();
    approxProductionCuts->SetProductionCut(BDSGlobalConstants::Instance()->GetProdCutPhotonsA(),G4ProductionCuts::GetIndex("gamma"));
    approxProductionCuts->SetProductionCut(BDSGlobalConstants::Instance()->GetProdCutElectronsA(),G4ProductionCuts::GetIndex("e-"));
    approxProductionCuts->SetProductionCut(BDSGlobalConstants::Instance()->GetProdCutPositronsA(),G4ProductionCuts::GetIndex("e+"));
    _approximationRegionSQL->SetProductionCuts(approxProductionCuts);
    //  }
}

BDSGeometrySQL::~BDSGeometrySQL(){
}

void BDSGeometrySQL::Construct(G4LogicalVolume *marker)
{
  itsMarkerVol = marker;
  VOL_LIST.push_back(itsMarkerVol);
  G4String file;
  char buffer[1000];
  while (ifs>>file)
    {
      if(file.contains("#")) ifs.getline(buffer,1000); // This is a comment line
      else{
	G4String sBDSPATH = getEnv("BDSIMPATH");
	G4String fullPath = sBDSPATH + file;
	BuildSQLObjects(fullPath);}
    }
  
  // Close Geomlist file
  ifs.close();
}

void BDSGeometrySQL::BuildSQLObjects(G4String file)
{
#ifdef DEBUG
  G4cout << "BDSGeometrySQL::BuildSQLObjects Loading file " << file << G4endl;
#endif

  G4String fullpath = BDSGlobalConstants::Instance()->GetBDSIMHOME();
  fullpath += file; 
#ifdef DEBUG
  G4cout << "BDSGeometrySQL::BuildSQLObjects Full path is " << fullpath << G4endl;
#endif

  BDSMySQLWrapper sql(fullpath);
  itsSQLTable=sql.ConstructTable();

  for (G4int i=0; i<(G4int)itsSQLTable.size(); i++)
    {
#ifdef DEBUG
      itsSQLTable[i]->Print();
#endif
      _TableName = itsSQLTable[i]->GetName();
#ifdef DEBUG
      G4cout << __METHOD_NAME__ << " i = " << i << ", TableName = " << _TableName << G4endl;
#endif
      G4int pos = _TableName.find("_");
      G4String ObjectType = _TableName.substr(pos+1,_TableName.length() - pos);
      G4String::caseCompare cmpmode = G4String::ignoreCase;
      _NVariables = itsSQLTable[i]->GetVariable(0)->GetNVariables();
      for(G4int k=0; k<_NVariables; k++){
	SetCommonParams(itsSQLTable[i], k);
	G4LogicalVolume* logVol;
	if(ObjectType.compareTo("CONE",cmpmode)==0) logVol = BuildCone(itsSQLTable[i],k);
	else if(ObjectType.compareTo("ELLIPTICALCONE",cmpmode)==0) logVol = BuildEllipticalCone(itsSQLTable[i],k);
	else if(ObjectType.compareTo("POLYCONE",cmpmode)==0) logVol = BuildPolyCone(itsSQLTable[i],k);
	else if(ObjectType.compareTo("BOX",cmpmode)==0) logVol = BuildBox(itsSQLTable[i],k);
	else if(ObjectType.compareTo("TRAP",cmpmode)==0) logVol =  BuildTrap(itsSQLTable[i],k);
	else if(ObjectType.compareTo("TORUS",cmpmode)==0) logVol = BuildTorus(itsSQLTable[i],k);
	else if(ObjectType.compareTo("SAMPLER",cmpmode)==0) logVol = BuildSampler(itsSQLTable[i],k);
	else if(ObjectType.compareTo("TUBE",cmpmode)==0) logVol =  BuildTube(itsSQLTable[i],k);
	else if(ObjectType.compareTo("ELLIPTICALTUBE",cmpmode)==0) logVol =  BuildEllipticalTube(itsSQLTable[i],k);
	else if(ObjectType.compareTo("PCLTUBE",cmpmode)==0) logVol =  BuildPCLTube(itsSQLTable[i],k);
	else {
	  G4cerr << __METHOD_NAME__ << ObjectType << " not known" << G4endl;
	  exit(1);
	}
	//Set the user limits and visual attributes
	SetLogVolAtt(logVol, _lengthUserLimit);
	VOL_LIST.push_back(logVol);
      }
      PlaceComponents(itsSQLTable[i], VOL_LIST);
    }
}

void BDSGeometrySQL::SetCommonParams(BDSMySQLTable* aSQLTable, G4int k){
  //Defaults
  _VisRed = _VisGreen = _VisBlue = 0.5;
  _VisAlpha = 0.5;
  _VisType = "S";
  _Material = "VACUUM";
  _Name="";
  _PrecisionRegion=0;
  _ApproximationRegion=0;

  if(aSQLTable->GetVariable("RED")!=NULL)
    _VisRed = aSQLTable->GetVariable("RED")->GetDblValue(k);
  if(aSQLTable->GetVariable("BLUE")!=NULL)
    _VisBlue = aSQLTable->GetVariable("BLUE")->GetDblValue(k);
  if(aSQLTable->GetVariable("GREEN")!=NULL)
    _VisGreen = aSQLTable->GetVariable("GREEN")->GetDblValue(k);
  if(aSQLTable->GetVariable("ALPHA")!=NULL)
    _VisAlpha = aSQLTable->GetVariable("ALPHA")->GetDblValue(k);
  if(aSQLTable->GetVariable("VISATT")!=NULL)
    _VisType = aSQLTable->GetVariable("VISATT")->GetStrValue(k);
  if(aSQLTable->GetVariable("MATERIAL")!=NULL)
    _Material = aSQLTable->GetVariable("MATERIAL")->GetStrValue(k);
  if(aSQLTable->GetVariable("PRECISIONREGION")!=NULL)
    _PrecisionRegion = aSQLTable->GetVariable("PRECISIONREGION")->GetIntValue(k);
  if(aSQLTable->GetVariable("APPROXIMATIONREGION")!=NULL)
    _ApproximationRegion = aSQLTable->GetVariable("APPROXIMATIONREGION")->GetIntValue(k);
  if(aSQLTable->GetVariable("NAME")!=NULL)
    _Name = aSQLTable->GetVariable("NAME")->GetStrValue(k);
  if(_Name=="_SQL") _Name = _TableName+BDSGlobalConstants::Instance()->StringFromInt(k) + "_SQL";
  if(_Name=="") _Name = _TableName+BDSGlobalConstants::Instance()->StringFromInt(k);
  _Name = itsMarkerVol->GetName()+"_"+_Name;
#ifdef DEBUG
  G4cout << __METHOD_NAME__ << " k = " << k << ", _Name = " << _Name << G4endl;
#endif
}

void BDSGeometrySQL::SetPlacementParams(BDSMySQLTable* aSQLTable, G4int k){
  //Defaults
      _PosX = _PosY = _PosZ = 0.;
      _RotPsi = _RotTheta = _RotPhi = 0.;
      _K1 = _K2 = _K3 = _K4 = 0.;
      _PARENTNAME = "";
      _InheritStyle = "";
      _Parameterisation = "";
      _align_in=0;
      _align_out=0;
      _SetSensitive=0;
      _MagType = "";
      _FieldX = _FieldY = _FieldZ = 0.0;
      _Name="";
      if(aSQLTable->GetVariable("PARENTNAME")!=NULL)
	_PARENTNAME = aSQLTable->GetVariable("PARENTNAME")->GetStrValue(k);
      if(aSQLTable->GetVariable("POSX")!=NULL)
	_PosX = aSQLTable->GetVariable("POSX")->GetDblValue(k);
      if(aSQLTable->GetVariable("POSY")!=NULL)
	_PosY = aSQLTable->GetVariable("POSY")->GetDblValue(k);
      if(aSQLTable->GetVariable("POSZ")!=NULL)
	_PosZ = aSQLTable->GetVariable("POSZ")->GetDblValue(k);
      if(aSQLTable->GetVariable("ROTPSI")!=NULL)
	_RotPsi = aSQLTable->GetVariable("ROTPSI")->GetDblValue(k);
      if(aSQLTable->GetVariable("ROTTHETA")!=NULL)
	_RotTheta = aSQLTable->GetVariable("ROTTHETA")->GetDblValue(k);
      if(aSQLTable->GetVariable("ROTPHI")!=NULL)
	_RotPhi = aSQLTable->GetVariable("ROTPHI")->GetDblValue(k);
      if(aSQLTable->GetVariable("K1")!=NULL)
	_K1 = aSQLTable->GetVariable("K1")->GetDblValue(k);
      if(aSQLTable->GetVariable("K2")!=NULL)
	_K2 = aSQLTable->GetVariable("K2")->GetDblValue(k);
      if(aSQLTable->GetVariable("K3")!=NULL)
	_K3 = aSQLTable->GetVariable("K3")->GetDblValue(k);
      if(aSQLTable->GetVariable("K4")!=NULL)
	_K4 = aSQLTable->GetVariable("K4")->GetDblValue(k);
      if(aSQLTable->GetVariable("MAGTYPE")!=NULL)
	_MagType = aSQLTable->GetVariable("MAGTYPE")->GetStrValue(k);
      if(aSQLTable->GetVariable("FIELDX")!=NULL)
	_FieldX = aSQLTable->GetVariable("FIELDX")->GetDblValue(k);
      if(aSQLTable->GetVariable("FIELDY")!=NULL)
	_FieldY = aSQLTable->GetVariable("FIELDY")->GetDblValue(k);
      if(aSQLTable->GetVariable("FIELDZ")!=NULL)
	_FieldZ = aSQLTable->GetVariable("FIELDZ")->GetDblValue(k);
      if(aSQLTable->GetVariable("ALIGNIN")!=NULL)
	_align_in = aSQLTable->GetVariable("ALIGNIN")->GetIntValue(k);
      if(aSQLTable->GetVariable("ALIGNOUT")!=NULL)
	_align_out = aSQLTable->GetVariable("ALIGNOUT")->GetIntValue(k);
      if(aSQLTable->GetVariable("SETSENSITIVE")!=NULL)
	_SetSensitive = aSQLTable->GetVariable("SETSENSITIVE")->GetIntValue(k);
      if(aSQLTable->GetVariable("INHERITSTYLE")!=NULL)
	_InheritStyle = aSQLTable->GetVariable("INHERITSTYLE")->GetStrValue(k);
      if(aSQLTable->GetVariable("PARAMETERISATION")!=NULL)
	_Parameterisation = aSQLTable->GetVariable("PARAMETERISATION")->GetStrValue(k);
      if(_PARENTNAME=="") _PosZ-=itsMarkerLength/2; //Move definition of PosZ to front of element
      _PARENTNAME=itsMarkerVol->GetName()+"_"+_PARENTNAME;
      if(aSQLTable->GetVariable("NAME")!=NULL)
	_Name = aSQLTable->GetVariable("NAME")->GetStrValue(k);
      if(_Name=="_SQL") _Name = _TableName+BDSGlobalConstants::Instance()->StringFromInt(k) + "_SQL";
      if(_Name=="") _Name = _TableName+BDSGlobalConstants::Instance()->StringFromInt(k);
      _Name = itsMarkerVol->GetName()+"_"+_Name;
#ifdef DEBUG
      G4cout << __METHOD_NAME__ << " k = " << k << ", _Name = " << _Name << G4endl;
#endif
}

G4VisAttributes* BDSGeometrySQL::VisAtt(){
  G4VisAttributes* VisAtt = 
    new G4VisAttributes(G4Colour(_VisRed, _VisGreen, _VisBlue, _VisAlpha));
  switch (_VisType(0))
    {
    case 'W': VisAtt->SetForceWireframe(true); break;
    case 'I': VisAtt->SetVisibility(false); break;
    case 'S': VisAtt->SetForceSolid(true); break;
    case 'w': VisAtt->SetForceWireframe(true); break;
    case 'i': VisAtt->SetVisibility(false); break;
    case 's': VisAtt->SetForceSolid(true); break;
    }
  return VisAtt;
}

G4UserLimits* BDSGeometrySQL::UserLimits(G4double var){
  G4UserLimits* UserLimits = new G4UserLimits();
  UserLimits->SetMaxAllowedStep(var*0.5);
  UserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->GetMaxTime());
  if(BDSGlobalConstants::Instance()->GetThresholdCutCharged()>0){
    UserLimits->SetUserMinEkine(BDSGlobalConstants::Instance()->GetThresholdCutCharged());
  }
  return UserLimits;
}

//Set logical volume attributes
void BDSGeometrySQL::SetLogVolAtt(G4LogicalVolume* logVol, G4double k){
  logVol->SetVisAttributes(VisAtt());
#ifndef NOUSERLIMITS
  logVol->SetUserLimits(UserLimits(k));
#endif
  SetLogVolRegion(logVol);
}

void BDSGeometrySQL::SetLogVolRegion(G4LogicalVolume* logVol){
  if(_PrecisionRegion){
    logVol->SetRegion(_precisionRegionSQL);
    _precisionRegionSQL->AddRootLogicalVolume(logVol);
  }
  if(_ApproximationRegion){
    logVol->SetRegion(_approximationRegionSQL);
    _approximationRegionSQL->AddRootLogicalVolume(logVol);
  }
}

G4LogicalVolume* BDSGeometrySQL::BuildCone(BDSMySQLTable* aSQLTable, G4int k)
{
  G4double length;
  G4double rInnerStart;
  G4double rInnerEnd;
  G4double rOuterStart;
  G4double rOuterEnd;
  G4double sphi;
  G4double dphi;
  
  //Defaults
  sphi =0.0;
  dphi = CLHEP::twopi*CLHEP::radian;
  length = rOuterStart = rOuterEnd = 10.*CLHEP::mm;
  rInnerStart = rInnerEnd = 0.0;
  
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
  
  G4Cons* aCone = new G4Cons(_Name+"_Cone",
			     rInnerStart,
			     rOuterStart,
			     rInnerEnd,
			     rOuterEnd,
			     length/2,
			     sphi,
			     dphi);
  
  G4LogicalVolume* aConeVol = 
    new G4LogicalVolume(aCone,
			BDSMaterials::Instance()->GetMaterial(_Material),
			_Name+"_LogVol");
  _lengthUserLimit=length;
  return aConeVol;
}

G4LogicalVolume* BDSGeometrySQL::BuildEllipticalCone(BDSMySQLTable* aSQLTable, G4int k){

  G4double lengthZ = 0;
  G4double pxSemiAxis = 0;
  G4double pySemiAxis = 0;
  G4double pzTopCut = 0;
  
  //Defaults
  lengthZ = 10.*CLHEP::mm;
  
  if(aSQLTable->GetVariable("LENGTHZ")!=NULL)
    lengthZ = aSQLTable->GetVariable("LENGTHZ")->GetDblValue(k);
  if(aSQLTable->GetVariable("XSEMIAXIS")!=NULL)
    pxSemiAxis = aSQLTable->GetVariable("XSEMIAXIS")->GetDblValue(k);
  if(aSQLTable->GetVariable("YSEMIAXIS")!=NULL)
    pySemiAxis = aSQLTable->GetVariable("YSEMIAXIS")->GetDblValue(k);
  if(aSQLTable->GetVariable("ZCUT")!=NULL)
    pzTopCut = aSQLTable->GetVariable("ZCUT")->GetDblValue(k);
  
  G4EllipticalCone* aEllipticalCone = new G4EllipticalCone(_Name+"_EllipticalCone",
							   pxSemiAxis,
							   pySemiAxis,
							       lengthZ/2,
							   pzTopCut);
  
  G4LogicalVolume* aEllipticalConeVol = 
    new G4LogicalVolume(aEllipticalCone,
			BDSMaterials::Instance()->GetMaterial(_Material),
			_Name+"_LogVol");

  _lengthUserLimit=lengthZ*0.5;
  return aEllipticalConeVol;
}

G4LogicalVolume* BDSGeometrySQL::BuildPolyCone(BDSMySQLTable* aSQLTable, G4int k)
{
  G4int numZplanes;
  G4double* rInner = NULL;
  G4double* rOuter = NULL;
  G4double* zPos = NULL;
  G4double sphi;
  G4double dphi;
  
  //Defaults
  sphi = 0.0;
  dphi = CLHEP::twopi*CLHEP::radian;
  numZplanes = 0;
  
  if(aSQLTable->GetVariable("NZPLANES")!=NULL)
    numZplanes = aSQLTable->GetVariable("NZPLANES")->GetIntValue(k);
  rInner = new G4double[numZplanes+1];
  rOuter = new G4double[numZplanes+1];
  zPos = new G4double[numZplanes+1];
      
  for(G4int planenum=0; planenum<numZplanes; planenum++)
    {
      G4String rInner_ID = "RINNER" + BDSGlobalConstants::Instance()->StringFromInt(planenum+1);
      G4String rOuter_ID = "ROUTER" + BDSGlobalConstants::Instance()->StringFromInt(planenum+1);
      G4String zPos_ID = "PLANEPOS" + BDSGlobalConstants::Instance()->StringFromInt(planenum+1);
      
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
  
  G4Polycone* aPolyCone = new G4Polycone(_Name+"_PolyCone",
					 sphi,
					 dphi,
					 numZplanes,
					 zPos,
					 rInner,
					 rOuter);

  G4LogicalVolume* aPolyConeVol = 
    new G4LogicalVolume(aPolyCone,
			BDSMaterials::Instance()->GetMaterial(_Material),
			_Name+"_LogVol");

  _lengthUserLimit=fabs(zPos[0]-zPos[numZplanes-1])/2;
  
  delete [] rInner;
  rInner = NULL;
  delete [] rOuter;
  rOuter = NULL;
  delete [] zPos;
  zPos = NULL;
  
  return aPolyConeVol;
}

G4LogicalVolume* BDSGeometrySQL::BuildBox(BDSMySQLTable* aSQLTable, G4int k)
{
#ifdef DEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif

  G4double lengthX;
  G4double lengthY;
  G4double lengthZ;

  lengthX = lengthY = lengthZ = 10.*CLHEP::mm;
  
  if(aSQLTable->GetVariable("LENGTHX")!=NULL)
    lengthX = aSQLTable->GetVariable("LENGTHX")->GetDblValue(k);
  if(aSQLTable->GetVariable("LENGTHY")!=NULL)
    lengthY = aSQLTable->GetVariable("LENGTHY")->GetDblValue(k);
  if(aSQLTable->GetVariable("LENGTHZ")!=NULL)
    lengthZ = aSQLTable->GetVariable("LENGTHZ")->GetDblValue(k);
  
  G4Box* aBox = new G4Box(_Name+"_Box",
			  lengthX/2,
			  lengthY/2,
			  lengthZ/2);
  
  G4LogicalVolume* aBoxVol = 
    new G4LogicalVolume(aBox,
			BDSMaterials::Instance()->GetMaterial(_Material),
			_Name+"_LogVol");
  
  _lengthUserLimit = lengthZ;
  return aBoxVol;
}

G4LogicalVolume* BDSGeometrySQL::BuildTrap(BDSMySQLTable* aSQLTable, G4int k)
{
  
  G4double trapTheta = 0; //Angle between faces of trapezoid
  G4double lengthXPlus = 0;
  G4double lengthXMinus = 0;
  G4double lengthYPlus = 0;
  G4double lengthYMinus = 0;
  G4double lengthZ = 0;
  
  if(aSQLTable->GetVariable("TRAPTHETA")!=NULL)
    trapTheta = aSQLTable->GetVariable("TRAPTHETA")->GetDblValue(k);
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
  
  G4Trap* aTrap = new G4Trap(_Name+"_Trd",
			     lengthZ/2,
			     trapTheta, 0,
			     lengthYPlus/2,
			     lengthXPlus/2,
			     lengthXPlus/2,
			     0,
			     lengthYMinus/2,
			     lengthXMinus/2,
			     lengthXMinus/2,
			     0);
  
  
  G4LogicalVolume* aTrapVol = 
    new G4LogicalVolume(aTrap,
			BDSMaterials::Instance()->GetMaterial(_Material),
			_Name+"_LogVol");

  _lengthUserLimit = lengthZ*0.5;
  return aTrapVol;
}

G4LogicalVolume* BDSGeometrySQL::BuildTorus(BDSMySQLTable* aSQLTable, G4int k)
{
  
  G4double rInner;
  G4double rOuter;
  G4double rSwept;
  G4double sphi;
  G4double dphi;
  
  //Defaults
  rSwept = 20.*CLHEP::mm;
  rOuter = 10.*CLHEP::mm;
  rInner = 0.0;
  sphi = 0.0;
  dphi=2*CLHEP::pi*CLHEP::radian;
  
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
  
  G4Torus* aTorus = new G4Torus(_Name+"_Torus",
				rInner,
				rOuter,
				rSwept,
				sphi,
				dphi);
  

  G4LogicalVolume* aTorusVol = 
    new G4LogicalVolume(aTorus,
			BDSMaterials::Instance()->GetMaterial(_Material),
			_Name+"_LogVol");

  _lengthUserLimit = rOuter*0.5;
  return aTorusVol;
}

G4LogicalVolume* BDSGeometrySQL::BuildSampler(BDSMySQLTable* aSQLTable, G4int k)
{
  
  G4double length;
  G4double rInnerStart;
  G4double rInnerEnd;
  G4double rOuterStart;
  G4double rOuterEnd;
  
  //Defaults
  length = rOuterStart = rOuterEnd = 10.*CLHEP::mm;
  rInnerStart = rInnerEnd = 0.0;
  
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
  if(aSQLTable->GetVariable("NAME")!=NULL)
    {
      _Name = aSQLTable->GetVariable("NAME")->GetStrValue(k);
      aSQLTable->GetVariable("NAME")->SetStrValue(k,_Name+"_SQL");
      _Name = aSQLTable->GetVariable("NAME")->GetStrValue(k);
    }
  if(_Name=="_SQL") _Name = _TableName+BDSGlobalConstants::Instance()->StringFromInt(k)+"_SQL";
  // make sure that each name is unique!
  _Name = itsMarkerVol->GetName()+"_"+_Name;
     
  G4Cons* aSampler = new G4Cons(_Name+"_samp",
				rInnerStart,
				rOuterStart,
				rInnerEnd,
				rOuterEnd,
				length/2,
				0,
				CLHEP::twopi*CLHEP::radian);

  G4LogicalVolume* aSamplerVol = 
    new G4LogicalVolume(aSampler,
			BDSMaterials::Instance()->GetMaterial(_Material),
			_Name+"_LogVol");

  _lengthUserLimit = length*0.5;
  
  G4SDManager* SDMan = G4SDManager::GetSDMpointer();
  if(BDSSampler::GetNSamplers()==0){
    BDSSamplerSensDet = new BDSSamplerSD(_Name, "plane");
    SDMan->AddNewDetector(BDSSamplerSensDet);
  }
  aSamplerVol->SetSensitiveDetector(BDSSamplerSensDet);
  //SPM bdsOutput->nSamplers++;
  BDSSampler::AddExternalSampler();
  bdsOutput->SampName.push_back(BDSGlobalConstants::Instance()->StringFromInt(
									      BDSSampler::GetNSamplers())+"_"+_Name+"_1");
  
  return aSamplerVol;
}

G4LogicalVolume* BDSGeometrySQL::BuildTube(BDSMySQLTable* aSQLTable, G4int k)
{
 
  G4double rInner;
  G4double rOuter;
  G4double length;
  G4double sphi;
  G4double dphi;
  
  //Defaults 
  length = 100.*CLHEP::mm;
  rOuter = 10.*CLHEP::mm;
  rInner = 0.0;
  sphi = 0.0;
  dphi=2*CLHEP::pi*CLHEP::radian;
  
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
  
  G4Tubs* aTubs = new G4Tubs(_Name+"_Tubs",
			     rInner,
			     rOuter,
			     length/2,
			     sphi,
			     dphi);
  
  G4LogicalVolume* aTubsVol = 
    new G4LogicalVolume(aTubs,
			BDSMaterials::Instance()->GetMaterial(_Material),
			_Name+"_LogVol");
  
  _lengthUserLimit = length*0.5;
  return aTubsVol;
}

G4LogicalVolume* BDSGeometrySQL::BuildEllipticalTube(BDSMySQLTable* aSQLTable, G4int k)
{
  G4double lengthX;
  G4double lengthY;
  G4double lengthZ;
  
  //Defaults 
  lengthX = 100.*CLHEP::mm;
  lengthY = 50.*CLHEP::mm;
  lengthZ = 200.*CLHEP::mm;

  if(aSQLTable->GetVariable("LENGTHX")!=NULL)
    lengthX = aSQLTable->GetVariable("LENGTHX")->GetDblValue(k);
  if(aSQLTable->GetVariable("LENGTHY")!=NULL)
    lengthY = aSQLTable->GetVariable("LENGTHY")->GetDblValue(k);
  if(aSQLTable->GetVariable("LENGTHZ")!=NULL)
    lengthZ = aSQLTable->GetVariable("LENGTHZ")->GetDblValue(k);
  
  G4EllipticalTube* aEllipticalTube = new G4EllipticalTube(_Name+"_EllipticalTube",
							   lengthX/2,
							   lengthY/2,
							   lengthZ/2
							   );
  
  
  G4LogicalVolume* aEllipticalTubeVol = 
    new G4LogicalVolume(aEllipticalTube,
			BDSMaterials::Instance()->GetMaterial(_Material),
			_Name+"_LogVol");
  G4double maxLength = lengthX;
  if (lengthY>lengthX&&lengthY>lengthZ){
    maxLength = lengthY;
  }
  else if(lengthZ>lengthY&&lengthZ>lengthX){
    maxLength = lengthZ;
  }
  _lengthUserLimit = maxLength*0.5;
  return aEllipticalTubeVol;
}


G4LogicalVolume* BDSGeometrySQL::BuildPCLTube(BDSMySQLTable* aSQLTable, G4int k)
{
  G4double aperX;
  G4double aperYUp;
  G4double aperYDown;
  G4double aperDy; 
  G4double thickness;
  G4double length;
  
  //Defaults 
  aperX = 100.*CLHEP::mm;
  aperYUp = 50.*CLHEP::mm;
  aperYDown = 200.*CLHEP::mm;
  aperDy = 0.*CLHEP::mm;
  thickness = BDSGlobalConstants::Instance()->GetBeampipeThickness();
  length = 200.0*CLHEP::mm;
  
  if(aSQLTable->GetVariable("APERX")!=NULL)
    aperX = aSQLTable->GetVariable("APERX")->GetDblValue(k);
  if(aSQLTable->GetVariable("APERYUP")!=NULL)
    aperYUp = aSQLTable->GetVariable("APERYUP")->GetDblValue(k);
  if(aSQLTable->GetVariable("APERYDOWN")!=NULL)
    aperYDown = aSQLTable->GetVariable("APERYDOWN")->GetDblValue(k);
  if(aSQLTable->GetVariable("APERDY")!=NULL)
    aperDy = aSQLTable->GetVariable("APERDY")->GetDblValue(k);
  if(aSQLTable->GetVariable("THICKNESS")!=NULL)
    thickness = aSQLTable->GetVariable("THICKNESS")->GetDblValue(k);
  if(aSQLTable->GetVariable("LENGTH")!=NULL)
    length = aSQLTable->GetVariable("LENGTH")->GetDblValue(k);
  
  BDSPCLTube* aPCLTubeBuilder = new BDSPCLTube(aperX, aperYUp, aperYDown, aperDy, thickness, length, _Name+"_PCLTube");
  G4VSolid* aPCLTube = aPCLTubeBuilder->GetSolid();
  delete aPCLTubeBuilder;

  G4LogicalVolume* aPCLTubeVol = 
    new G4LogicalVolume(aPCLTube,
			BDSMaterials::Instance()->GetMaterial(_Material),
			_Name+"_LogVol");
  G4double totalYLength = aperDy+aperYUp+aperYDown+thickness;
  G4double totalXLength = aperX+thickness;
  G4double maxLength = length;
  if (totalYLength>length&&totalYLength>totalXLength){
    maxLength = totalYLength;
  }
  else if(totalXLength>totalYLength&&totalXLength>length){
    maxLength = totalXLength;
  }
  _lengthUserLimit = maxLength*0.5;
  return aPCLTubeVol;
}

G4RotationMatrix* BDSGeometrySQL::RotateComponent(G4double psi,G4double phi,G4double theta)
{
  rotateComponent = new G4RotationMatrix;
  if(psi==0 && phi==0 && theta==0) return rotateComponent;

  G4RotationMatrix* LocalRotation = new G4RotationMatrix;
  G4ThreeVector* localX = new G4ThreeVector(1.,0.,0.);
  G4ThreeVector* localY = new G4ThreeVector(0.,1.,0.);
  G4ThreeVector* localZ = new G4ThreeVector(0.,0.,1.);
  
  LocalRotation->rotate(psi,*localZ);
  localX->rotate(psi,*localZ);
  localY->rotate(psi,*localZ);
  
  
  LocalRotation->rotate(phi,*localY);
  localX->rotate(phi,*localY);
  localZ->rotate(phi,*localY);
  
  
  LocalRotation->rotate(theta,*localX);
  localY->rotate(theta,*localX);
  localZ->rotate(theta,*localX);
  
  rotateComponent->transform(*LocalRotation);
  rotateComponent->invert();
  
  return rotateComponent;
}


void BDSGeometrySQL::PlaceComponents(BDSMySQLTable* aSQLTable, std::vector<G4LogicalVolume*> VOL_LIST)
{
  G4String::caseCompare cmpmode = G4String::ignoreCase;
  for(G4int k=0; k<_NVariables; k++) // Now run through and place according to
    { 
      SetPlacementParams(aSQLTable, k);
      G4int PARENTID=0;
      if(_PARENTNAME!=""){
	_PARENTNAME+="_LogVol";
	for(G4int i=0; i<(G4int)VOL_LIST.size(); i++)
	  {
	    if(_PARENTNAME.compareTo(VOL_LIST[i]->GetName(),cmpmode)==0)
	      {
		PARENTID = i;
		continue;
	      }
	  }
      }
      
      // to being in line with logvol names (needed for name checking loop
      G4String tmpname = _Name+"_LogVol";
      G4int ID=0;
      for(G4int i=0; i<(G4int)VOL_LIST.size(); i++)
	{
	  if(tmpname.compareTo(VOL_LIST[i]->GetName(),cmpmode)==0)
	    {
	      ID = i;
	      continue;
	    }
	}

      if(_SetSensitive) SensitiveComponents.push_back(VOL_LIST[ID]);

      G4ThreeVector PlacementPoint(_PosX,_PosY,_PosZ);

      if(_InheritStyle.compareTo("",cmpmode)){ //True if InheritStyle is set
	if(_InheritStyle.compareTo("SUBTRACT",cmpmode)==0)
	  {
	    G4VSolid* original = VOL_LIST[PARENTID]->GetSolid();
	    G4VSolid* sub = VOL_LIST[ID]->GetSolid();
	    VOL_LIST[PARENTID]->SetSolid(new G4SubtractionSolid(VOL_LIST[PARENTID]->GetName(),
								original,
								sub,
								RotateComponent(_RotPsi,_RotPhi,_RotTheta),
								PlacementPoint));
	    
	  }else if(_InheritStyle.compareTo("INTERSECT",cmpmode)==0){
	  G4VSolid* original = VOL_LIST[PARENTID]->GetSolid();
	  G4VSolid* sub = VOL_LIST[ID]->GetSolid();
	  VOL_LIST[PARENTID]->SetSolid(new G4IntersectionSolid(VOL_LIST[PARENTID]->GetName(),
							       original,
							       sub,
							       RotateComponent(_RotPsi,_RotPhi,_RotTheta),
							       PlacementPoint));
	
	} else if(_InheritStyle.compareTo("UNION",cmpmode)==0)
	  {
	    G4VSolid* original = VOL_LIST[PARENTID]->GetSolid();
	    G4VSolid* sub = VOL_LIST[ID]->GetSolid();
	    VOL_LIST[PARENTID]->SetSolid(new G4UnionSolid(VOL_LIST[PARENTID]->GetName(),
							  original,
							  sub,
							  RotateComponent(_RotPsi,_RotPhi,_RotTheta),
							  PlacementPoint));
	  }
      }

      if(_Parameterisation.compareTo("GFLASH",cmpmode)==0){       
	itsGFlashComponents.push_back(VOL_LIST[ID]);
      }

#ifdef DEBUG
      G4cout << __METHOD_NAME__ << " k = " << k << ", volume = " << VOL_LIST[ID]->GetName() << G4endl;
#endif
      
	G4VPhysicalVolume* PhysiComp = 
	  new G4PVPlacement(RotateComponent(_RotPsi,_RotPhi,_RotTheta),
			    PlacementPoint,
			    VOL_LIST[ID],
			    _Name,
			    VOL_LIST[PARENTID],
			    false,
			    0, BDSGlobalConstants::Instance()->GetCheckOverlaps());
	SetMultiplePhysicalVolumes(PhysiComp);
      if(_align_in)
	{
	  // Make sure program stops and informs user if more than one alignment vol.
	  if(align_in_volume!=NULL)
	    {
	      G4cerr<<"\nBDSGeometrySQL.cc:486: Trying to align in-beam to SQL volume to " << PhysiComp->GetName() << " but alignment already set to " << align_in_volume->GetName() << G4endl;
	      G4Exception("Aborting Program", "-1", FatalException, "");

	    }

	  else
	    align_in_volume=PhysiComp;

	}

      if(_align_out)
	{
	  if(align_out_volume!=NULL)
	    {
	      G4cerr<<"\nBDSGeometrySQL.cc:486: Trying to align out-beam to SQL volume to " << PhysiComp->GetName() << " but alignment already set to " << align_out_volume->GetName() << G4endl;
	      G4Exception("Aborting Program", "-1", FatalException, "");

	    }

	  else
	    align_out_volume=PhysiComp;
	}

//      G4double P0 = BDSGlobalConstants::Instance()->GetBeamTotalEnergy();
//      G4double brho=
//	sqrt(pow(P0,2)- pow(electron_mass_c2,2))/(0.299792458 * (CLHEP::GeV/(CLHEP::tesla*CLHEP::m)));

      // compute magnetic rigidity brho
      // formula: B(Tesla)*rho(m) = p(GeV)/(0.299792458 * |charge(e)|)
      //
      // charge (in |e| units)
      G4double charge = BDSGlobalConstants::Instance()->GetParticleDefinition()->GetPDGCharge();  
      // momentum (in GeV/c)   
      G4double momentum = BDSGlobalConstants::Instance()->GetBeamMomentum();
      // rigidity (in T*m)
      G4double brho = ( (momentum/CLHEP::GeV) / (0.299792458 * charge));
      // rigidity (in Geant4 units)
      brho *= (CLHEP::tesla*CLHEP::m);

      if(_MagType.compareTo("QUAD",cmpmode)==0)
	{
	  HasFields = true;
	  nPoleField = 1;
	  QuadBgrad.push_back(- brho * _K1 / CLHEP::m2);
	  Quadvol.push_back(PhysiComp->GetName());
	  QuadVolBgrad[PhysiComp->GetName()]=(- brho * _K1 / CLHEP::m2);
	}

      if(_MagType.compareTo("SEXT",cmpmode)==0)
	{
	  HasFields = true;
	  nPoleField = 2;
	  SextBgrad.push_back(- brho * _K2 / CLHEP::m3);
	  Sextvol.push_back(PhysiComp->GetName());
	  SextVolBgrad[PhysiComp->GetName()]=(- brho * _K2 / CLHEP::m3);
	}

      if(_MagType.compareTo("OCT",cmpmode)==0)
	{
	  HasFields = true;
	  nPoleField = 3;
	  OctBgrad.push_back(- brho * _K3 / (CLHEP::m2*CLHEP::m2));
	  Octvol.push_back(PhysiComp->GetName());
	  OctVolBgrad[PhysiComp->GetName()]=(- brho * _K3 / (CLHEP::m2*CLHEP::m2));
	}

      if(_FieldX || _FieldY || _FieldZ) //if any vols have non-zero field components
	{
	  HasFields = true;
	  HasUniformField=true;
#ifdef DEBUG
	  G4cout << "BDSGeometrySQL> volume " << PhysiComp->GetName() << " has the following uniform field: " << _FieldX << " " << _FieldY << " " << _FieldZ << " " << G4endl;
#endif
	  UniformField.push_back(G4ThreeVector(_FieldX*CLHEP::tesla,_FieldY*CLHEP::tesla,_FieldZ*CLHEP::tesla));
	  Fieldvol.push_back(PhysiComp->GetName());
	  UniformFieldVolField[PhysiComp->GetName()]=G4ThreeVector(_FieldX*CLHEP::tesla,_FieldY*CLHEP::tesla,_FieldZ*CLHEP::tesla);
	}
  }
}
