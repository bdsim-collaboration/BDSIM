/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 28.12.2004
   Copyright (c) 2004 by G.A.Blair.  ALL RIGHTS RESERVED. 

   Modified 22.03.05 by J.C.Carter, Royal Holloway, Univ. of London.
   Added GABs SynchGen code
*/

//==================================================================
//==================================================================
#include "BDSGlobalConstants.hh"
#include "BDSPrimaryGeneratorAction.hh"
#include "BDSBunch.hh"
#include "BDSParticle.hh"
#include "BDSDebug.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleDefinition.hh"

BDSPrimaryGeneratorAction::BDSPrimaryGeneratorAction(BDSBunch* bdsBunchIn):
  weight(1),
  bdsBunch(bdsBunchIn)
{
  particleGun  = new G4ParticleGun(1); // 1-particle gun

#ifdef BDSDEBUG
  G4cout << "BDSPrimaryGeneratorAction.cc: Primary particle is " << BDSGlobalConstants::Instance()->GetParticleDefinition()->GetParticleName() << G4endl;
#endif
  
#ifdef BDSDEBUG
  G4cout << "Setting momentum..." << G4endl;
#endif
  particleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));
  particleGun->SetParticlePosition(G4ThreeVector(0.*CLHEP::cm,0.*CLHEP::cm,0.*CLHEP::cm));
  particleGun->SetParticleEnergy(BDSGlobalConstants::Instance()->GetBeamKineticEnergy());
  particleGun->SetParticleTime(0);
}

//===================================================

BDSPrimaryGeneratorAction::~BDSPrimaryGeneratorAction()
{
  delete particleGun;
}

//===================================================

void BDSPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  //this function is called at the begining of event

  G4double x0=0.0, y0=0.0, z0=0.0, xp=0.0, yp=0.0, zp=0.0, t=0.0, E=0.0;

  particleGun->SetParticleDefinition(BDSGlobalConstants::Instance()->GetParticleDefinition());
  bdsBunch->GetNextParticle(x0,y0,z0,xp,yp,zp,t,E,weight); // get next starting point
  if(E==0) G4cout << __METHOD_NAME__ << "Particle energy is 0! This will not be tracked." << G4endl;
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ 
	 << x0 << " " << y0 << " " << z0 << " " 
	 << xp << " " << yp << " " << zp << " " 
	 << t  << " " << E  << " " << weight << G4endl;
#endif

  G4ThreeVector LocalPos;
  G4ThreeVector LocalMomDir;
  
  G4ThreeVector PartMomDir(xp,yp,zp);
  G4ThreeVector PartPosition(x0,y0,z0);
  
  particleGun->SetParticlePosition(PartPosition);
  particleGun->SetParticleEnergy(E);
  particleGun->SetParticleMomentumDirection(PartMomDir);
  particleGun->SetParticleTime(t);
  
  particleGun->GeneratePrimaryVertex(anEvent);

  //Set the weight
#ifdef BDSDEBUG
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
#ifdef BDSDEBUG
  G4cout
    << "BDSPrimaryGeneratorAction: " << G4endl
    << "  position= " << particleGun->GetParticlePosition()/CLHEP::m<<" m"<<G4endl
    << "  kinetic energy= " << E/CLHEP::GeV << " GeV" << G4endl
    << "  total energy= " << totalE/CLHEP::GeV << " GeV" << G4endl
    << "  momentum direction= " << PartMomDir << G4endl
    << "  weight= " << anEvent->GetPrimaryVertex()->GetWeight() << G4endl;
#endif

  // save initial values outside scope for entry into the samplers:
  BDSParticle initialPoint(x0,y0,z0,xp,yp,zp,totalE,t,weight);
  BDSGlobalConstants::Instance()->SetInitialPoint(initialPoint);
}

//===================================================
