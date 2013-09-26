/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 25.12.2003
   Copyright (c) 2003 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/

#include "BDSGlobalConstants.hh" 

#include "BDSMagFieldSQL.hh"
#include "G4Navigator.hh"
#include "G4TransportationManager.hh"
#include "G4RotationMatrix.hh"
#include "G4VPhysicalVolume.hh"
#include "G4TouchableHistoryHandle.hh"
#include "G4TouchableHistory.hh"
#include "G4NavigationHistory.hh"
#include <string>
#include <vector>
#include <map>

using namespace std;

#if 0
BDSMagFieldSQL::BDSMagFieldSQL(const G4String& aFieldFile,
			       G4double aMarkerLength,
			       list<G4String> Quadvol, list<G4double> QuadBgrad,
			       list<G4String> Sextvol, list<G4double> SextBgrad,
			       list<G4String> Octvol, list<G4double> OctBgrad,
			       list<G4String> Fieldvol, list<G4ThreeVector> UniformField)

  :ifs(aFieldFile.c_str()),itsMarkerLength(aMarkerLength),FieldFile(aFieldFile),
   itsQuadBgrad(QuadBgrad), itsQuadVol(Quadvol),
   itsSextBgrad(SextBgrad), itsSextVol(Sextvol),
   itsOctBgrad(OctBgrad), itsOctVol(Octvol),
   itsUniformField(UniformField), itsFieldVol(Fieldvol)
{
}
#endif

G4bool BDSMagFieldSQL::GetHasNPoleFields(){return itsHasNPoleFields;}
G4bool BDSMagFieldSQL::GetHasUniformField(){return itsHasUniformField;}
G4bool BDSMagFieldSQL::GetHasFieldMap(){return itsHasFieldMap;}

BDSMagFieldSQL::BDSMagFieldSQL(const G4String& aFieldFile,
			       G4double aMarkerLength,
			       std::map<G4String, G4double> aQuadVolBgrad,
			       std::map<G4String, G4double> aSextVolBgrad,
			       std::map<G4String, G4double> aOctVolBgrad,
			       std::map<G4String, G4ThreeVector> aUniformFieldVolField, G4bool aHasNPoleFields, G4bool aHasUniformField)
  :itsHasNPoleFields(aHasNPoleFields),itsHasUniformField(aHasUniformField),itsHasFieldMap(false),ifs(aFieldFile.c_str()),
   itsMarkerLength(aMarkerLength), FieldFile(aFieldFile), itsUniformFieldVolField(aUniformFieldVolField),
   itsQuadVolBgrad(aQuadVolBgrad), itsSextVolBgrad(aSextVolBgrad), itsOctVolBgrad(aOctVolBgrad), itsdz(0.0)
{
  //Define alternate navigator (see geant4 application developers manual section 4.1.8.2)
  itsIRNavigator=new G4Navigator();
}

BDSMagFieldSQL::~BDSMagFieldSQL(){
  delete itsIRNavigator;
}


void BDSMagFieldSQL::GetFieldValue( const G4double Point[4],
		       G4double *Bfield ) const
{
  //    G4Navigator* itsIRNavigator=
  //    G4TransportationManager::GetTransportationManager()-> 
  //    GetNavigatorForTracking();

  itsIRNavigator->SetWorldVolume(G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking()->GetWorldVolume());

  G4ThreeVector LocalR, LocalB, RLocalR, FieldB, NPoleB, GlobalR(Point[0], Point[1], Point[2]);
  //  GlobalR.setX(Point[0]);
  //  GlobalR.setY(Point[1]);
  //  GlobalR.setZ(Point[2]);

  itsIRNavigator->LocateGlobalPointAndSetup(GlobalR);
  //  G4TouchableHistory* aTouchable = itsIRNavigator->CreateTouchableHistory();
  G4TouchableHistoryHandle aTouchable = itsIRNavigator->CreateTouchableHistoryHandle();
  const G4AffineTransform GlobalToMarker=aTouchable->GetHistory()->GetTransform(1);
  //  const G4AffineTransform MarkerToGlobal=GlobalToMarker.Inverse();
  RLocalR=GlobalToMarker.TransformPoint(GlobalR);
  
  if( fabs(RLocalR.z()) > fabs(itsMarkerLength/2) ){
    // Outside of mokka region - field should be zero. This is needed
    // because sometimes RKStepper asks for overly large steps (1km)
    Bfield[0] = 0;
    Bfield[1] = 0;
    Bfield[2] = 0;
    Bfield[3] = 0;
    Bfield[4] = 0;
    Bfield[5] = 0;
    return;
  }

  G4bool inNPole = false;
  G4bool inField = false;
  G4String VolName = aTouchable->GetVolume()->GetName();

  if(itsHasUniformField || itsHasNPoleFields){
    G4AffineTransform GlobalAffine, LocalAffine;
    GlobalAffine=itsIRNavigator->GetGlobalToLocalTransform();
    LocalAffine=itsIRNavigator->GetLocalToGlobalTransform();
    LocalR=GlobalAffine.TransformPoint(GlobalR); 
    LocalR.setY(-LocalR.y());
    LocalR.setX(-LocalR.x());	  // -ve signs because of Geant Co-ord System
    if(itsHasNPoleFields){
      std::map<G4String, G4double>::const_iterator iter = itsQuadVolBgrad.find(VolName);
      if(iter!=itsQuadVolBgrad.end()){
	NPoleB.setX(iter->second*LocalR.y());
	NPoleB.setY(iter->second*LocalR.x());
      } else {
	iter = itsSextVolBgrad.find(VolName);
	if(iter!=itsSextVolBgrad.end()){
	  NPoleB.setX(LocalR.x()*LocalR.y()*iter->second);
	  NPoleB.setY(-(LocalR.x()*LocalR.x()-LocalR.y()*LocalR.y())*iter->second/2.);
	} else {
	  iter = itsOctVolBgrad.find(VolName);
	  if(iter!=itsOctVolBgrad.end()){
	    NPoleB.setX((3*LocalR.x()*LocalR.x()*LocalR.y() - 
			 LocalR.y()*LocalR.y()*LocalR.y())*iter->second/6.);
	    NPoleB.setY((LocalR.x()*LocalR.x()*LocalR.x() -
			 LocalR.x()*LocalR.y()*LocalR.y())*iter->second/6.);
	  } 
	}
      }
    }
    if(itsHasUniformField){
      std::map<G4String, G4ThreeVector>::const_iterator iter_u = itsUniformFieldVolField.find(VolName);  
      if(iter_u!=itsUniformFieldVolField.end()){
	FieldB = iter_u->second;
	FieldB = LocalAffine.TransformAxis(FieldB);
	inField=true;
      }
    }
    NPoleB.setZ(0.0);
    NPoleB = LocalAffine.TransformAxis(NPoleB);
    inNPole=true;
  }

  if(itsHasFieldMap){
    if(itsMarkerLength>0) RLocalR.setZ(RLocalR.z()+itsMarkerLength/2);
    else RLocalR.setZ( -(RLocalR.z()+fabs(itsMarkerLength)/2) + fabs(itsMarkerLength));
    G4double tempz = RLocalR.z()/CLHEP::cm;
    if(tempz<0)  //Mokka region resets Z to be positive at starting from one
      //Edge of the region
      {
	// This should NEVER happen. If it does, then the cause is either that
	// the mokka region length is not set properly, or that the BDSRKStepper
	// is asking for a step length greater than the Mokka marker length
	G4cout << "Z position in Mokka region less than 0 - check step lengths!!" << G4endl;
	G4Exception("Quitting BDSIM in BDSMagFieldSQL.cc", "-1", FatalException, "");
      }
    G4double zlow = floor(tempz);
    G4int ilow = (G4int)(zlow);
    G4double zhi = zlow + 1.0;
    if (ilow > (G4int)itsBz.size() ||
	itsBz.size()==0) LocalB = G4ThreeVector(0.,0.,0.);
    else
      {
	// Calculate the field local to MarkerVolume
	// Interpolate the value of the field nearest to the point
	G4double fieldBrr_r = ( (zhi-tempz)*itsBr_over_r[ilow] +
				(tempz-zlow)*itsBr_over_r[ilow+1]);
	// then should divide by (zhi-zlow) but this = 1
	
	G4double fieldBzz = ( (zhi-tempz)*itsBz[ilow] +
			      (tempz-zlow)*itsBz[ilow+1]);
	// then should divide by (zhi-zlow) but this = 1
	LocalB.setX(fieldBrr_r*(RLocalR.x()));
	LocalB.setY(fieldBrr_r*(RLocalR.y()));
	LocalB.setZ(fieldBzz);
	// Now rotate to give BField on Global Reference Frame
	LocalB.transform(rotation.inverse());
      }
    //LocalB=G4ThreeVector(0.,0.,0.); //turn Bfield from Solenoid off
  }

  if(inField) LocalB+=FieldB;
  if(inNPole) LocalB+=NPoleB;
  // b-field
  Bfield[0] = LocalB.x();
  Bfield[1] = LocalB.y();
  Bfield[2] = LocalB.z();
  
  /*
    G4cout << "BField: " << LocalB << G4endl;
    G4cout << itsMarkerLength << G4endl;
    G4cout << RLocalR << G4endl;
    G4cout << ilow << G4endl;
    G4cout << QuadB << G4endl;
    G4cout << SextB << G4endl;
    G4cout << OctB << G4endl;
    G4cout << G4endl;
  */
  
  
#ifdef DEBUG 
  LocalB.rotateY(10e-3); //to track from incoming beamline perspective
  // this needs the be the crossing angle plus any marker rotation applied
  // for IR solenoid case
  G4cout << RLocalR.x()/CLHEP::m << " "<<RLocalR.y()/CLHEP::m << " "<<RLocalR.z()/CLHEP::m << " "<< LocalB.x()/CLHEP::tesla << " " << LocalB.y()/CLHEP::tesla << " " << LocalB.z()/CLHEP::tesla << G4endl;
#endif
  //  delete aTouchable;
  //  aTouchable = NULL;
  return;
}


void BDSMagFieldSQL::Prepare(G4VPhysicalVolume *referenceVolume)
{
  if(FieldFile==""){
    itsHasFieldMap=false;
  } else {
    itsHasFieldMap=true;
    G4cout<<"BDSElement:: creating SQL field map"<<G4endl;
    
    if(!ifs)
      {
	G4cerr<<"\nBDSMagFieldSQL.cc: Unable to open Field Map File: " << FieldFile << G4endl;
	G4Exception("Aborting Program", "-1", FatalException, "");
      }
    else
      G4cout << "Loading SQL Field Map file: " << FieldFile << G4endl;
    
    if(FieldFile.contains("inverse")) itsMarkerLength*=-1;
    double temp_z=0.0;
    double temp_Bz=0.0;
    double temp_solB=0.0;
    while(!ifs.eof()){
      if(FieldFile.contains("SiD"))
	ifs >> temp_z >> temp_Bz >> temp_solB;
      
      if(FieldFile.contains("LD"))
	ifs >> temp_z >> temp_Bz >> temp_solB >> temp_solB;
      
      if(FieldFile.contains("TESLA"))
	ifs >> temp_z >> temp_Bz;
      
      itsZ.push_back(temp_z*CLHEP::m);
      itsBz.push_back(temp_Bz*CLHEP::tesla);
    }
    
    itsdz = itsZ[1] - itsZ[0];
    
    //first element:
    itsdBz_by_dz.push_back( (itsBz[1] - itsBz[0]) / itsdz );
    itsBr_over_r.push_back(0.5 * itsdBz_by_dz[0] );
    
    for(G4int j=1; j<(G4int)itsBz.size()-2; j++)
      {
	  itsdBz_by_dz.push_back( (itsBz[j+1] - itsBz[j-1]) / (2*itsdz) );
	  itsBr_over_r.push_back(0.5 * itsdBz_by_dz[j] );
	}
      
      //last element:
      itsdBz_by_dz.push_back( (itsBz[itsBz.size()-1] - itsBz[itsBz.size()-2]) / itsdz );
      itsBr_over_r.push_back(0.5 * itsdBz_by_dz[itsdBz_by_dz.size()-1] );
    }
  
  SetOriginRotation(*(referenceVolume->GetFrameRotation()));
  SetOriginTranslation(referenceVolume->GetFrameTranslation());
}
