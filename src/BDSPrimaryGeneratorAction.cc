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
#include "BDSSamplerHit.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4Track.hh"

#include "Randomize.hh"

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

BDSPrimaryGeneratorAction::BDSPrimaryGeneratorAction(BDSDetectorConstruction* /*BDSDC*/)
  :itsBDSSynchrotronRadiation(NULL)
{
  particleGun  = new G4ParticleGun(1); // 1-particle gun

  // initialize with default values... 
  // they will be overridden in GeneratePrimaries function

  //  particleGun->SetParticleDefinition(BDSGlobalConstants::Instance()->
  //                                      GetParticleDefinition());

#ifdef BDSDEBUG
  G4cout << "BDSPrimaryGeneratorAction.cc: Primary particle is " << BDSGlobalConstants::Instance()->GetParticleDefinition()->GetParticleName() << G4endl;
  //  G4cout << "BDSPrimaryGeneratorAction.cc: Setting particle definition for gun..." << G4endl;
  //  particleGun->SetParticleDefinition(G4ParticleTable::GetParticleTable()->
  //				     FindParticle("e-"));
#endif
  
//   if(BDSGlobalConstants::Instance()->GetUseSynchPrimaryGen()) // synchrotron radiation generator
//     {
//       itsBDSSynchrotronRadiation=new BDSSynchrotronRadiation("tmpSynRad");
// #ifdef BDSDEBUG
//       G4cout << "BDSPrimaryGeneratorAction.cc: Setting synch rad..." << G4endl;
//       G4double R=BDSGlobalConstants::Instance()->GetSynchPrimaryLength()/
// 	BDSGlobalConstants::Instance()->GetSynchPrimaryAngle();   
//       G4double SynchCritEng=3./2.*CLHEP::hbarc/pow(CLHEP::electron_mass_c2,3)*
// 	pow(BDSGlobalConstants::Instance()->GetBeamKineticEnergy(),3)/R;
//       G4cout<<" BDSPrimaryGeneratorAction:  Critical Energy="<<
// 	SynchCritEng/CLHEP::keV<<" keV"<<G4endl;
// #endif
//       particleGun->SetParticleDefinition(G4ParticleTable::GetParticleTable()->
// 					 FindParticle("gamma"));
//     }
  
#ifdef BDSDEBUG
  G4cout << "Setting momentum..." << G4endl;
#endif
  particleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));
  particleGun->SetParticlePosition(G4ThreeVector(0.*CLHEP::cm,0.*CLHEP::cm,0.*CLHEP::cm));
  particleGun->SetParticleEnergy(BDSGlobalConstants::Instance()->GetBeamKineticEnergy());
  particleGun->SetParticleTime(0);
  weight = 1;

  //Set up the hits collection for storing the primaries
  itsSamplerHitsCollection = new BDSSamplerHitsCollection((G4String)"inputSampler",(G4String)"inputCollection");
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

  G4double x0=0.0, y0=0.0, z0=0.0, xp=0.0, yp=0.0, zp=0.0, t=0.0, E=0.0;

  particleGun->SetParticleDefinition(BDSGlobalConstants::Instance()->GetParticleDefinition());

  if(!BDSGlobalConstants::Instance()->getReadFromStack()){
    bdsBunch.GetNextParticle(x0,y0,z0,xp,yp,zp,t,E,weight); // get next starting point
  }  else if(BDSGlobalConstants::Instance()->holdingQueue.size()!=0){
    BDSParticle holdingParticle = BDSGlobalConstants::Instance()->holdingQueue.front();
    BDSParticle outputParticle  = BDSGlobalConstants::Instance()->outputQueue.front();
    x0 = outputParticle.GetX(); //
    y0 = outputParticle.GetY(); //
    z0 = outputParticle.GetZ(); //
    t  = holdingParticle.GetTime();  //
    xp = holdingParticle.GetXp();
    yp = holdingParticle.GetYp();
    zp = holdingParticle.GetZp();
    E  = holdingParticle.GetEnergy();
    weight = holdingParticle.GetWeight();

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

#ifdef BDSDEBUG 
    printf("Particles left %i: %f %f %f %f %f %f %f %f\n",
           (int)BDSGlobalConstants::Instance()->holdingQueue.size(),x0,y0,z0,xp,yp,zp,t,E);
#endif
  }
  else G4Exception("No new particles to fire...\n", "-1", FatalException, "");

  if(E==0) G4cout << "Particle energy is 0! This will not be tracked." << G4endl;

  G4ThreeVector PartMomDir(0,0,1);
  G4ThreeVector PartPosition(0,0,0);

  G4ThreeVector LocalPos;
  G4ThreeVector LocalMomDir;

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
  initial_x  = x0;
  initial_xp = xp;
  initial_y  = y0;
  initial_yp = yp;
  initial_t  = t;
  initial_z  = z0;
  initial_zp = zp;
  // total energy is used elsewhere:
  initial_E  = totalE;
  // weight
  initial_weight=weight;
}

//===================================================

