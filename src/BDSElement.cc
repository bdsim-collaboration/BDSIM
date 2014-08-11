/* BDSIM code.    Version 1.0
   Author: John C. Carter, Royal Holloway, Univ. of London.
   Last modified 02.12.2004
   Copyright (c) 2004 by J.C.Carter.  ALL RIGHTS RESERVED. 
*/

#include "BDSGlobalConstants.hh" // must be first in include list
#include "BDSElement.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Torus.hh"
#include "G4Trd.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4UserLimits.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "BDSAcceleratorComponent.hh"
#include "BDS3DMagField.hh"
#include "BDSXYMagField.hh"
#include "BDSMagFieldSQL.hh"
#include "G4NystromRK4.hh"
#include "G4ClassicalRK4.hh"
#include "G4HelixImplicitEuler.hh"

// geometry drivers
#include "parser/enums.h"
#include "ggmad.hh"
#include "BDSGeometrySQL.hh"

#ifdef USE_LCDD
#include "BDSGeometryLCDD.hh"
#endif

#ifdef USE_GDML
#include "BDSGeometryGDML.hh"
#endif

#include <map>
#include <vector>

//============================================================

typedef std::map<G4String,int> LogVolCountMap;
extern LogVolCountMap* LogVolCount;

typedef std::map<G4String,G4LogicalVolume*> LogVolMap;
extern LogVolMap* LogVol;



//============================================================

BDSElement::BDSElement(G4String aName, G4String geometry, G4String bmap, G4double bmapZOffset,
		       G4double aLength, G4double bpRad, G4double outR, G4String aTunnelMaterial, G4double aTunnelRadius, G4double aTunnelOffsetX, G4String aTunnelCavityMaterial, G4int aPrecisionRegion):
  BDSAcceleratorComponent(
			  aName,
			  aLength,bpRad,0,0,
			  SetVisAttributes(), aTunnelMaterial, "", 0., 0., 0., 0., aTunnelRadius*CLHEP::m, aTunnelOffsetX*CLHEP::m, aTunnelCavityMaterial, aPrecisionRegion),
  fChordFinder(NULL), itsFStepper(NULL), itsFEquation(NULL), itsEqRhs(NULL), 
  itsMagField(NULL), itsCachedMagField(NULL), itsUniformMagField(NULL)
{
  itsFieldVolName="";
  itsFieldIsUniform=false;
  itsOuterR = outR;
  itsBmapZOffset = bmapZOffset;
  SetType(_ELEMENT);

  //Set marker volume lengths
  CalculateLengths();

  // WARNING: ALign in and out will only apply to the first instance of the
  //          element. Subsequent copies will have no alignment set.
  align_in_volume = NULL;
  align_out_volume = NULL;

  if(!(*LogVolCount)[itsName])
    {
#ifdef BDSDEBUG 
      G4cout<<"BDSElement : starting build logical volume "<<
        itsName<<G4endl;
#endif
      BuildGeometry(); // build element box
      
#ifdef BDSDEBUG 
      G4cout<<"BDSElement : end build logical volume "<<
        itsName<<G4endl;
#endif

      PlaceComponents(geometry,bmap); // place components (from file) and build filed maps
    }
  else
    {
      (*LogVolCount)[itsName]++;
      
      itsMarkerLogicalVolume=(*LogVol)[itsName];
    }
}


void BDSElement::BuildElementMarkerLogicalVolume(){
  
#ifdef BDSDEBUG 
  G4cout<<"BDSElement : creating logical volume"<<G4endl;
#endif
  G4double elementSizeX=itsOuterR+BDSGlobalConstants::Instance()->GetLengthSafety()/2, elementSizeY = itsOuterR+BDSGlobalConstants::Instance()->GetLengthSafety()/2;
  
  
  elementSizeX = std::max(elementSizeX, this->GetTunnelRadius()+2*std::abs(this->GetTunnelOffsetX()) + BDSGlobalConstants::Instance()->GetTunnelThickness()+BDSGlobalConstants::Instance()->GetTunnelSoilThickness() + 4*BDSGlobalConstants::Instance()->GetLengthSafety() );   
  elementSizeY = std::max(elementSizeY, this->GetTunnelRadius()+2*std::abs(BDSGlobalConstants::Instance()->GetTunnelOffsetY()) + BDSGlobalConstants::Instance()->GetTunnelThickness()+BDSGlobalConstants::Instance()->GetTunnelSoilThickness()+4*BDSGlobalConstants::Instance()->GetLengthSafety() );

  G4double elementSize=std::max(elementSizeX, elementSizeY); 
  

  itsMarkerLogicalVolume = 
    new G4LogicalVolume(new G4Box(itsName+"generic_element",
                                  elementSize,
                                  elementSize,   
				  itsLength/2.0),
			BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetVacuumMaterial()),
			itsName);

  
  //-------------------------------------------------------------------------------------------------------------


#ifdef BDSDEBUG 
  G4cout<<"marker volume : x/y="<<elementSize/CLHEP::m<<
    " m, l= "<<  (itsLength)/2/CLHEP::m <<" m"<<G4endl;
#endif


    //-----------------------------
    /*
      G4RotationMatrix* rotMatrix1 = new G4RotationMatrix();
  rotMatrix1->rotateY(itsPhiAngleIn);
  G4RotationMatrix* rotMatrix2 = new G4RotationMatrix();
  rotMatrix1->rotateY(itsPhiAngleOut);
  G4ThreeVector transVec1(0, 0, itsLength

  itsMarkerLogicalVolume = new G4SubtactionSolid(itsName+"_generic_element",
						 new G4SubtactionSolid(itsName+"_tempSolid1",
								       new G4Box(itsName+"tempSolid1a",
										 elementSize,
										 elementSize,   
										 itsLength/2),
								       new G4Box(itsName+"tempSolid1b", 
										 sin(itsPhiAngleIn)*elementSize*2, //Make sure it is wider than the solid being subtracted from
										 elementSize,
										 itsLength/2),
								       rotMatrix1,
								       transVec1),
						 new  G4Box(itsName+"tempSolid2", 
							    sin(itsPhiAngleOut)*elementSize*2, //Make sure it is wider than the solid being subtracted from
							    elementSize,
							    itsLength/2),
						 rotMatrix2,
						 transVec2);



  G4LogicalVolume* tempBox1 = new G4LogicalVolume(new G4Box(itsName+"tempBox1", 
							    sin(itsPhiAngleIn)*elementSize,
							    elementSize,
							    itsLength/2),
						  BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetVacuumMaterial()),
						  itsName+"tempBox1Log"
						  );
  

    */

  /*
  G4double xHalfLengthPlus, xHalfLengthMinus;
  xHalfLengthMinus = (itsLength + (elementSize/2.0)*(tan(itsPhiAngleIn) -tan(itsPhiAngleOut)))/2.0;
  xHalfLengthPlus = (itsLength +  (elementSize/2.0)*(tan(itsPhiAngleOut)-tan(itsPhiAngleIn )))/2.0;
  
  
  if((xHalfLengthPlus<0) || (xHalfLengthMinus<0)){
    G4cerr << "Bend radius in " << itsName << " too small for this tunnel/component geometry. Exiting." << G4endl;
    exit(1);
  }
  
  itsMarkerSolidVolume = new G4Trd(itsName+"_marker",
				   xHalfLengthPlus,     // x hlf lgth at +z
				   xHalfLengthMinus,    // x hlf lgth at -z
				   elementSize/2,           // y hlf lgth at +z
				   elementSize/2,           // y hlf lgth at -z
				   fabs(cos(itsAngle/2))*elementSize/2);// z hlf lgth
  
  G4String LocalLogicalName=itsName;
  
  itsMarkerLogicalVolume=    
    new G4LogicalVolume(itsMarkerSolidVolume,
			BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetVacuumMaterial()),
			LocalLogicalName+"_marker");
  */
  
  itsMarkerUserLimits = new G4UserLimits(DBL_MAX,DBL_MAX,DBL_MAX, BDSGlobalConstants::Instance()->GetThresholdCutCharged());
  G4double  maxStepFactor=5;
  itsMarkerUserLimits->SetMaxAllowedStep(itsLength*maxStepFactor);
  itsMarkerLogicalVolume->SetUserLimits(itsMarkerUserLimits);
  
  
  //
  // zero field in the marker volume
  //
  itsMarkerLogicalVolume->
    SetFieldManager(BDSGlobalConstants::Instance()->GetZeroFieldManager(),false);
}

void BDSElement::BuildGeometry()
{
  // multiple element instances not implemented yet!!!

  // Build the marker logical volume 
  BuildElementMarkerLogicalVolume();


  (*LogVolCount)[itsName] = 1;
  (*LogVol)[itsName] = itsMarkerLogicalVolume;
#ifndef NOUSERLIMITS
  itsOuterUserLimits = new G4UserLimits();
  G4double stepfactor=5;
  itsOuterUserLimits->SetMaxAllowedStep(itsLength*stepfactor);
  itsOuterUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->GetMaxTime());
  if(BDSGlobalConstants::Instance()->GetThresholdCutCharged()>0){
    itsOuterUserLimits->SetUserMinEkine(BDSGlobalConstants::Instance()->GetThresholdCutCharged());
  }
  itsMarkerLogicalVolume->SetUserLimits(itsOuterUserLimits);
#endif

  //Build the tunnel
  if(BDSGlobalConstants::Instance()->GetBuildTunnel()){
    BuildTunnel();
  }
}

// place components 
void BDSElement::PlaceComponents(G4String geometry, G4String bmap)
{

  G4String gFormat="", bFormat="";
  G4String gFile="", bFile="";

  // get geometry format and file
  if(geometry != ""){
    G4int pos = geometry.find(":");
    gFormat="none";
    if(pos<0) { 
      G4cerr<<"WARNING: invalid geometry reference format : "<<geometry<<G4endl;
    }
    else {
      gFormat = geometry.substr(0,pos);
      gFile = BDSGlobalConstants::Instance()->GetBDSIMHOME();
      G4String temp = geometry.substr(pos+1,geometry.length() - pos);     
#ifdef BDSDEBUG
      G4cout << "BDSElement::PlaceComponents SQL file is " << temp << G4endl;
#endif
#ifdef BDSDEBUG
      G4cout << "BDSElement::PlaceComponents Full path is " << gFile << G4endl;
#endif
      gFile+=temp;
#ifdef BDSDEBUG
      G4cout << "BDSElement::PlaceComponents Full path is " << gFile << G4endl;
#endif
    }
  }

  // get fieldmap format and file
  bFormat="none";
  if(bmap != ""){
    G4int pos = bmap.find(":");
    if(pos<0) {
      G4cerr<<"WARNING: invalid B map reference format : "<<bmap<<G4endl;
    }
    else {
      bFormat = bmap.substr(0,pos);
      bFile = BDSGlobalConstants::Instance()->GetBDSIMHOME();
      bFile += bmap.substr(pos+1,bmap.length() - pos); 
#ifdef BDSDEBUG
      G4cout << "BDSElement::PlaceComponents bmap file is " << bFile << G4endl;
#endif
    }
  }

#ifdef BDSDEBUG
  G4cout<<"placing components:\n geometry format - "<<gFormat<<G4endl<<
    "file - "<<gFile<<G4endl;
  G4cout<<"bmap format - "<<bFormat<<G4endl<<
    "file - "<<bFile<<G4endl;
#endif


  // get the geometry for the driver
  // different drivers may interpret the fieldmap differently
  // so a field map without geometry is not allowed

  if(gFormat=="gmad") {
    GGmadDriver *ggmad = new GGmadDriver(gFile);
    ggmad->Construct(itsMarkerLogicalVolume);

    // set sensitive volumes
    //vector<G4LogicalVolume*> SensComps = ggmad->SensitiveComponents;
    //for(G4int id=0; id<SensComps.size(); id++)
    //  SetMultipleSensitiveVolumes(SensComps[id]);

    
    SetMultipleSensitiveVolumes(itsMarkerLogicalVolume);

    // attach magnetic field if present
    if(bFormat=="3D"){
#ifdef BDSDEBUG
      G4cout << "BDSElement.cc> Making BDS3DMagField..." << G4endl;
#endif
      
      itsMagField = new BDS3DMagField(bFile, itsBmapZOffset);
      itsCachedMagField = new G4CachedMagneticField(itsMagField, 1*CLHEP::um);
      BuildMagField(true);
    }else if(bFormat=="XY"){
      itsMagField = new BDSXYMagField(bFile);
      itsCachedMagField = new G4CachedMagneticField(itsMagField, 1*CLHEP::um);

      // build the magnetic field manager and transportation
      BuildMagField(true);
    }
    delete ggmad;
  }
  else if(gFormat=="lcdd") {
#ifdef USE_LCDD
    BDSGeometryLCDD *LCDD = new BDSGeometryLCDD(gFile);
    //Make marker visible (temp debug)
    G4VisAttributes* VisAttLCDD = new G4VisAttributes(G4Colour(0.0, 1.0, 0.0));
    VisAttLCDD->SetForceSolid(true);  
    VisAttLCDD->SetVisibility(false);
    itsMarkerLogicalVolume->SetVisAttributes(VisAttLCDD);

    LCDD->Construct(itsMarkerLogicalVolume);
    SetMultipleSensitiveVolumes(itsMarkerLogicalVolume);
    if(bFormat=="XY"){
      itsMagField = new BDSXYMagField(bFile);
      itsCachedMagField = new G4CachedMagneticField(itsMagField, 1*CLHEP::um);

      // build the magnetic field manager and transportation
      BuildMagField(true);
    } else if ( bFormat == "mokka" ){
      G4ThreeVector uniField = G4ThreeVector(0,3.5*CLHEP::tesla,0);
      std::vector<G4ThreeVector> uniFieldVect;
      uniFieldVect.push_back(uniField);
      std::vector<G4double> nulld;
      std::vector<G4String> nulls;
      //      itsMagField = new BDSMagFieldSQL(bFile,itsLength,nulls, nulld, nulls, nulld, nulls, nulld, LCDD->FieldVol, uniFieldVect);
    } else if ( bFormat == "test" ){
    }
    else if ( bFormat == "none" ){
      itsFieldIsUniform=LCDD->GetFieldIsUniform();
      if(itsFieldIsUniform){
	G4cout << "BDSElement> using LCDD format uniform field..." << G4endl;
	itsUniformMagField=LCDD->GetUniformField();
      }else{
	itsMagField=LCDD->GetField();
	itsCachedMagField = new G4CachedMagneticField(itsMagField, 1*CLHEP::um);
	
      }
      itsFieldVolName=LCDD->GetFieldVolName();
      BuildMagField(true);
    }

    std::vector<G4LogicalVolume*> SensComps = LCDD->SensitiveComponents;
    for(G4int id=0; id<(G4int)SensComps.size(); id++)
      SetMultipleSensitiveVolumes(SensComps[id]);
    delete LCDD;
#else
    G4cout << "LCDD support not selected during BDSIM configuration" << G4endl;
    G4Exception("Please re-compile BDSIM with USE_LCDD flag", "-1", FatalException, "");
#endif
  }
  else if(gFormat=="mokka") {
#ifdef BDSDEBUG
    G4cout << "BDSElement.cc: loading geometry sql file: BDSGeometrySQL(" << gFile << "," << itsLength << ")" << G4endl;
#endif
    BDSGeometrySQL *Mokka = new BDSGeometrySQL(gFile,itsLength);
    Mokka->Construct(itsMarkerLogicalVolume);
    for(unsigned int i=0; i<Mokka->GetMultiplePhysicalVolumes().size(); i++){
      SetMultiplePhysicalVolumes(Mokka->GetMultiplePhysicalVolumes().at(i));
    }

    std::vector<G4LogicalVolume*> SensComps = Mokka->SensitiveComponents;
    for(G4int id=0; id<(G4int)SensComps.size(); id++)
      SetMultipleSensitiveVolumes(SensComps[id]);

    std::vector<G4LogicalVolume*> GFlashComps =Mokka->itsGFlashComponents;
    for(G4int id=0; id<(G4int)GFlashComps.size(); id++)
      SetGFlashVolumes(GFlashComps[id]);

    align_in_volume = Mokka->align_in_volume;
    align_out_volume = Mokka->align_out_volume;
    // attach magnetic field if present

    if(bFormat=="3D"){
#ifdef BDSDEBUG
      G4cout << "BDSElement.cc> Making BDS3DMagField..." << G4endl;
#endif
      itsMagField = new BDS3DMagField(bFile, 0);
      itsCachedMagField = new G4CachedMagneticField(itsMagField, 1*CLHEP::um);
      
      BuildMagField(true);
    } else if(bFormat=="XY"){
      itsMagField = new BDSXYMagField(bFile);
      itsCachedMagField = new G4CachedMagneticField(itsMagField, 1*CLHEP::um);
      
      // build the magnetic field manager and transportation
      BuildMagField(true);
    } else if(bFormat=="mokka" || bFormat=="none")
      {
	if(Mokka->HasFields || bFile!="")
	  // Check for field file or volumes with fields
	  // as there may be cases where there are no bFormats given
	  // in gmad file but fields might be set to volumes in SQL files
	  {
	    itsMagField = new BDSMagFieldSQL(bFile,itsLength,
					     Mokka->QuadVolBgrad,
					     Mokka->SextVolBgrad,
					     Mokka->OctVolBgrad,
					     Mokka->UniformFieldVolField,
					     Mokka->nPoleField,
					     Mokka->HasUniformField);
	    itsCachedMagField = new G4CachedMagneticField(itsMagField, 1*CLHEP::um);

	    
	    // build the magnetic field manager and transportation
	    BuildMagField(true);
	  }
      }
    delete Mokka;
  }
  else if(gFormat=="gdml") {
#ifdef USE_GDML
    BDSGeometryGDML *GDML = new BDSGeometryGDML(gFile);
    GDML->Construct(itsMarkerLogicalVolume);
    delete GDML;
#else
    G4cout << "GDML support not selected during BDSIM configuration" << G4endl;
    G4Exception("Please re-compile BDSIM with USE_GDML flag", "-1", FatalException, "");
#endif
  }
  else {
    G4cerr<<"geometry format "<<gFormat<<" not supported"<<G4endl;
  }
}



G4VisAttributes* BDSElement::SetVisAttributes()
{
  itsVisAttributes=new G4VisAttributes(G4Colour(0.5,0.5,1));
  return itsVisAttributes;
}


void BDSElement::BuildMagField(G4bool forceToAllDaughters)
{
#ifdef BDSDEBUG
  G4cout << "BDSElement.cc::BuildMagField Building magnetic field...." << G4endl;
#endif
  // create a field manager
  G4FieldManager* fieldManager = new G4FieldManager();


  if(!itsFieldIsUniform){
#ifdef BDSDEBUG
    G4cout << "BDSElement.cc> Building magnetic field..." << G4endl;
#endif
    itsEqRhs = new G4Mag_UsualEqRhs(itsCachedMagField);
    if( (itsMagField->GetHasUniformField())&!(itsMagField->GetHasNPoleFields() || itsMagField->GetHasFieldMap())){
      itsFStepper = new G4NystromRK4(itsEqRhs); 
    } else {
      itsFStepper = new G4NystromRK4(itsEqRhs);
    }
    fieldManager->SetDetectorField(itsCachedMagField );
  } else {
#ifdef BDSDEBUG
    G4cout << "BDSElement.cc> Building uniform magnetic field..." << G4endl;
#endif
    itsEqRhs = new G4Mag_UsualEqRhs(itsUniformMagField);
    itsFStepper = new G4NystromRK4(itsEqRhs); 
    fieldManager->SetDetectorField(itsUniformMagField );
  }

#ifdef BDSDEBUG
  G4cout << "BDSElement.cc> Setting stepping accuracy parameters..." << G4endl;
#endif
  
  if(BDSGlobalConstants::Instance()->GetDeltaOneStep()>0){
    fieldManager->SetDeltaOneStep(BDSGlobalConstants::Instance()->GetDeltaOneStep());
  }
  if(BDSGlobalConstants::Instance()->GetMaximumEpsilonStep()>0){
    fieldManager->SetMaximumEpsilonStep(BDSGlobalConstants::Instance()->GetMaximumEpsilonStep());
  }
  if(BDSGlobalConstants::Instance()->GetMinimumEpsilonStep()>=0){
    fieldManager->SetMinimumEpsilonStep(BDSGlobalConstants::Instance()->GetMinimumEpsilonStep());
  }
  if(BDSGlobalConstants::Instance()->GetDeltaIntersection()>0){
    fieldManager->SetDeltaIntersection(BDSGlobalConstants::Instance()->GetDeltaIntersection());
  }

  G4MagInt_Driver* fIntgrDriver = new G4MagInt_Driver(BDSGlobalConstants::Instance()->GetChordStepMinimum(),
						      itsFStepper, 
						      itsFStepper->GetNumberOfVariables() );
  fChordFinder = new G4ChordFinder(fIntgrDriver);
  
  fChordFinder->SetDeltaChord(BDSGlobalConstants::Instance()->GetDeltaChord());
  
  fieldManager->SetChordFinder( fChordFinder ); 
  
#ifdef BDSDEBUG
  G4cout << "BDSElement.cc> Setting the logical volume " << itsMarkerLogicalVolume->GetName() << " field manager... force to all daughters = " << forceToAllDaughters << G4endl;
#endif
  itsMarkerLogicalVolume->SetFieldManager(fieldManager,forceToAllDaughters);
}

// creates a field mesh in the reference frame of a physical volume
// from  b-field map value list 
// has to be called after the component is placed in the geometry
void BDSElement::PrepareField(G4VPhysicalVolume *referenceVolume)
{
  if(!itsMagField) return;
  itsMagField->Prepare(referenceVolume);
  itsCachedMagField = new G4CachedMagneticField(itsMagField, 1*CLHEP::um);
}

// Rotates and positions the marker volume before it is placed in
// BDSDetectorConstruction. It aligns the marker volume so that the
// the beamline goes through the specified daugther volume (e.g. for mokka)
void BDSElement::AlignComponent(G4ThreeVector& TargetPos, 
				G4RotationMatrix *TargetRot, 
				G4RotationMatrix& globalRotation,
				G4ThreeVector& rtot,
				G4ThreeVector& rlast,
				G4ThreeVector& localX,
				G4ThreeVector& localY,
				G4ThreeVector& localZ)
{
  
  
  if(align_in_volume == NULL)
    {
      if(align_out_volume == NULL)
	{
	  // advance co-ords in usual way if no alignment volumes found
	  
	  rtot = rlast + localZ*(itsLength/2);
	  rlast = rtot + localZ*(itsLength/2);
	  return;
	}
      else 
	{
#ifdef BDSDEBUG
	  G4cout << "BDSElement : Aligning outgoing to SQL element " 
		 << align_out_volume->GetName() << G4endl;
#endif
	  G4RotationMatrix Trot = *TargetRot;
	  G4RotationMatrix trackedRot;
	  G4RotationMatrix outRot = *(align_out_volume->GetFrameRotation());
	  trackedRot.transform(outRot.inverse());
	  trackedRot.transform(Trot.inverse());
	  globalRotation = trackedRot;

	  G4ThreeVector outPos = align_out_volume->GetFrameTranslation();
	  G4ThreeVector diff = outPos;

	  G4ThreeVector zHalfAngle = G4ThreeVector(0.,0.,1.);

	  zHalfAngle.transform(globalRotation);

	  //moving positioning to outgoing alignment volume
	  rlast = TargetPos - ((outPos.unit()).transform(Trot.inverse()) )*diff.mag();
	  localX.transform(outRot.inverse());
	  localY.transform(outRot.inverse());
	  localZ.transform(outRot.inverse());

	  localX.transform(Trot.inverse());
	  localY.transform(Trot.inverse());
	  localZ.transform(Trot.inverse());

	  //moving position in Z be at least itsLength/2 away
	  rlast +=zHalfAngle*(itsLength/2 + diff.z());
	  return;
	}
    }

  if(align_in_volume != NULL)
    {
#ifdef BDSDEBUG
      G4cout << "BDSElement : Aligning incoming to SQL element " 
      	     << align_in_volume->GetName() << G4endl;
#endif
      
      const G4RotationMatrix* inRot = align_in_volume->GetFrameRotation();
      TargetRot->transform((*inRot).inverse());
      
      G4ThreeVector inPos = align_in_volume->GetFrameTranslation();
      inPos.transform((*TargetRot).inverse());
      TargetPos+=G4ThreeVector(inPos.x(), inPos.y(), 0.0);
      
      if(align_out_volume == NULL)
	{
	  // align outgoing (i.e. next component) to Marker Volume
	  
	  G4RotationMatrix Trot = *TargetRot;
	  globalRotation.transform(Trot.inverse());
	  
	  G4ThreeVector zHalfAngle = G4ThreeVector(0.,0.,1.);
	  zHalfAngle.transform(Trot.inverse());
	  
	  rlast = TargetPos + zHalfAngle*(itsLength/2);
	  localX.transform(Trot.inverse());
	  localY.transform(Trot.inverse());
	  localZ.transform(Trot.inverse());
	  return;
	}

      else
	{
#ifdef BDSDEBUG
	  G4cout << "BDSElement : Aligning outgoing to SQL element " 
		 << align_out_volume->GetName() << G4endl;
#endif
	  G4RotationMatrix Trot = *TargetRot;
	  G4RotationMatrix trackedRot;
	  G4RotationMatrix outRot = *(align_out_volume->GetFrameRotation());
	  trackedRot.transform(outRot.inverse());
	  trackedRot.transform(Trot.inverse());
	  globalRotation = trackedRot;

	  G4ThreeVector outPos = align_out_volume->GetFrameTranslation();
	  G4ThreeVector diff = outPos;

	  G4ThreeVector zHalfAngle = G4ThreeVector(0.,0.,1.);

	  zHalfAngle.transform(globalRotation);

	  //moving positioning to outgoing alignment volume
	  rlast = TargetPos - ((outPos.unit()).transform(Trot.inverse()) )*diff.mag();
	  localX.transform(outRot.inverse());
	  localY.transform(outRot.inverse());
	  localZ.transform(outRot.inverse());

	  localX.transform(Trot.inverse());
	  localY.transform(Trot.inverse());
	  localZ.transform(Trot.inverse());

	  //moving position in Z be at least itsLength/2 away
	  rlast +=zHalfAngle*(itsLength/2 + diff.z());
	  return;
	}
    }
  
}

BDSElement::~BDSElement()
{
  delete fChordFinder;
  delete itsFStepper;
  delete itsFEquation;
}
