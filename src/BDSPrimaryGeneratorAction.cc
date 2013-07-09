/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 28.12.2004
   Copyright (c) 2004 by G.A.Blair.  ALL RIGHTS RESERVED. 

   Modified 22.03.05 by J.C.Carter, Royal Holloway, Univ. of London.
   Added GABs SynchGen code
*/

//==================================================================
//==================================================================
#include "BDSGlobalConstants.hh" // must be first in include list
#include "BDSPrimaryGeneratorAction.hh"

#include "BDSDetectorConstruction.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4EventManager.hh"
#include "G4StackManager.hh"
#include "G4Track.hh"
#include "G4Trajectory.hh"

#include "Randomize.hh"

//#include "BDSExtract.hh"
#include "BDSBunch.hh"

#include<iostream>

extern BDSBunch bdsBunch;

//===================================================
// Keep initial point in phase space for diagnostics
G4double
  initial_x, initial_xp,
  initial_y, initial_yp,
  initial_z, initial_zp,
  initial_E, initial_t,
  initial_weight;

BDSPrimaryGeneratorAction::BDSPrimaryGeneratorAction(
					      BDSDetectorConstruction* BDSDC)
  :BDSDetector(BDSDC), itsBDSSynchrotronRadiation(NULL)
{
 
  particleGun  = new G4ParticleGun(1); // 1-particle gun

  // initialize with default values... 
  // they will be overridden in GeneratePrimaries function

  //  particleGun->SetParticleDefinition(BDSGlobalConstants::Instance()->
  //                                      GetParticleDefinition());

#ifdef DEBUG
  G4cout << "BDSPrimaryGeneratorAction.cc: Primary particle is " << BDSGlobalConstants::Instance()->GetParticleDefinition()->GetParticleName() << G4endl;
  G4cout << "BDSPrimaryGeneratorAction.cc: Setting particle definition for gun..." << G4endl;
  particleGun->SetParticleDefinition(G4ParticleTable::GetParticleTable()->
				     FindParticle("e-"));
  G4cout << "BDSPrimaryGeneratorAction.cc: Setting synch rad..." << G4endl;
#endif
  
  if(BDSGlobalConstants::Instance()->GetUseSynchPrimaryGen()) // synchrotron radiation generator
    {
      itsBDSSynchrotronRadiation=new BDSSynchrotronRadiation("tmpSynRad");
      G4double R=BDSGlobalConstants::Instance()->GetSynchPrimaryLength()/
	BDSGlobalConstants::Instance()->GetSynchPrimaryAngle();   
      itsSynchCritEng=3./2.*hbarc/pow(electron_mass_c2,3)*
	pow(BDSGlobalConstants::Instance()->GetBeamKineticEnergy(),3)/R;
#ifdef DEBUG
      G4cout<<" BDSPrimaryGeneratorAction:  Critical Energy="<<
	itsSynchCritEng/keV<<" keV"<<G4endl;
#endif
      particleGun->SetParticleDefinition(G4ParticleTable::GetParticleTable()->
					 FindParticle("gamma"));
    }
  
#ifdef DEBUG
  G4cout << "Setting momentum..." << G4endl;
#endif
  particleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));
  particleGun->SetParticlePosition(G4ThreeVector(0.*cm,0.*cm,0.*cm));
  particleGun->SetParticleEnergy(BDSGlobalConstants::Instance()->GetBeamKineticEnergy());
  particleGun->SetParticleTime(0);
  weight = 1;
}

//===================================================

BDSPrimaryGeneratorAction::~BDSPrimaryGeneratorAction()
{
  delete particleGun;
  delete itsBDSSynchrotronRadiation;
}

//===================================================

void BDSPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  //this function is called at the begining of event

  G4double x0, y0, z0, xp, yp, zp, t, E;

  particleGun->SetParticleDefinition(BDSGlobalConstants::Instance()->GetParticleDefinition());

  if(!BDSGlobalConstants::Instance()->getReadFromStack()){
    bdsBunch.GetNextParticle(x0,y0,z0,xp,yp,zp,t,E,weight); // get next starting point
  }
  else if(BDSGlobalConstants::Instance()->holdingQueue.size()!=0){
    tmpParticle holdingParticle = BDSGlobalConstants::Instance()->holdingQueue.front();
    tmpParticle outputParticle  = BDSGlobalConstants::Instance()->outputQueue.front();
    x0 = outputParticle.x; //
    y0 = outputParticle.y; //
    z0 = outputParticle.z; //
    t  = holdingParticle.t;  //
    xp = holdingParticle.xp;
    yp = holdingParticle.yp;
    zp = holdingParticle.zp;
    E  = holdingParticle.E;
    weight = holdingParticle.weight;

    //flag for secondaries from previous runs
    //    if(outputParticle.parentID != 0)
      //      anEvent->SetEventID(-(anEvent->GetEventID()));

    if(E<0){
      particleGun->SetParticleDefinition(
		G4ParticleTable::GetParticleTable()->FindParticle(
			BDSGlobalConstants::Instance()->GetParticleDefinition()->
				GetAntiPDGEncoding()));
      E*=-1;
    }

#ifdef DEBUG 
    printf("Particles left %i: %f %f %f %f %f %f %f %f\n",
           (int)BDSGlobalConstants::Instance()->holdingQueue.size(),x0,y0,z0,xp,yp,zp,t,E);
#endif
  }
  else if(!BDSGlobalConstants::Instance()->isReference) G4Exception("No new particles to fire...\n", "-1", FatalException, "");

  if(E==0) G4cout << "Particle energy is 0! This will not be tracked." << G4endl;

  G4ThreeVector PartMomDir(0,0,1);
  G4ThreeVector PartPosition(0,0,0);

  G4ThreeVector LocalPos;
  G4ThreeVector LocalMomDir;

  if(!BDSGlobalConstants::Instance()->isReference){
    PartMomDir=G4ThreeVector(xp,yp,zp);
    PartPosition=G4ThreeVector(x0,y0,z0);

    if(BDSGlobalConstants::Instance()->GetRefVolume()!=""){
      const G4AffineTransform* tf = BDSGlobalConstants::Instance()->GetRefTransform();
      LocalPos = tf->TransformPoint(PartPosition);
      LocalMomDir = tf->TransformAxis(PartMomDir);
#ifdef DEBUG 
      G4cout << PartPosition << G4endl;
      G4cout << PartMomDir << G4endl;
      G4cout << LocalPos << G4endl;
      G4cout << LocalMomDir << G4endl;
#endif
      PartPosition = LocalPos;
      PartMomDir = LocalMomDir;
    }

    particleGun->SetParticlePosition(PartPosition);
    particleGun->SetParticleEnergy(E);
    particleGun->SetParticleMomentumDirection(PartMomDir);
    particleGun->SetParticleTime(t);
  }


  particleGun->GeneratePrimaryVertex(anEvent);

  //Set the weight
#ifdef DEBUG
  G4cout << "BDSPrimaryGeneratorAction: setting weight = " << weight << G4endl;
#endif
  anEvent->GetPrimaryVertex()->SetWeight(weight);
  
  if(BDSGlobalConstants::Instance()->holdingQueue.size()!=0){

//    anEvent->    GetTrack()->SetTrackID(outputParticle.trackID);
//    anEvent->    GetTrack()->SetParentID(outputParticle.parentID);
    
    BDSGlobalConstants::Instance()->holdingQueue.pop_front();
    BDSGlobalConstants::Instance()->outputQueue.pop_front();
  }

  G4double totalE = E+particleGun->GetParticleDefinition()->GetPDGMass();
#ifdef DEBUG
  G4cout
    << "BDSPrimaryGeneratorAction: " << G4endl
    << "  position= " << particleGun->GetParticlePosition()/m<<" m"<<G4endl
    << "  kinetic energy= " << E/GeV << " GeV" << G4endl
    << "  total energy= " << totalE/GeV << " GeV" << G4endl
    << "  momentum direction= " << PartMomDir << G4endl
    << "  weight= " << anEvent->GetPrimaryVertex()->GetWeight() << G4endl;
#endif

  // save initial values outside scope for entry into the samplers:
  initial_x=x0;
  initial_xp=xp;
  initial_y=y0;
  initial_yp=yp;
  initial_t=t;
  initial_z=z0;
  initial_zp=zp;
  // total energy is used elsewhere:
  initial_E=totalE;
  // weight
  initial_weight=weight;
}


//===================================================

