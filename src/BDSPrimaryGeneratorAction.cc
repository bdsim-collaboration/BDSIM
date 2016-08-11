#include "BDSGlobalConstants.hh"
#include "BDSPrimaryGeneratorAction.hh"
#include "BDSBunch.hh"
#include "BDSParticle.hh"
#include "BDSDebug.hh"
#include "BDSRandom.hh"
#include "BDSGlobalConstants.hh"
#include "CLHEP/Random/Random.h"
#include <fstream>

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleDefinition.hh"

BDSPrimaryGeneratorAction::BDSPrimaryGeneratorAction(BDSBunch* bdsBunchIn):
  G4VUserPrimaryGeneratorAction(),
  weight(1),
  bdsBunch(bdsBunchIn)
{
  particleGun  = new G4ParticleGun(1); // 1-particle gun

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "Primary particle is "
	 << BDSGlobalConstants::Instance()->GetParticleDefinition()->GetParticleName() << G4endl;
#endif
  
  particleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));
  particleGun->SetParticlePosition(G4ThreeVector(0.*CLHEP::cm,0.*CLHEP::cm,0.*CLHEP::cm));
  particleGun->SetParticleEnergy(BDSGlobalConstants::Instance()->BeamKineticEnergy());
  particleGun->SetParticleTime(0);
}

BDSPrimaryGeneratorAction::~BDSPrimaryGeneratorAction()
{delete particleGun;}

void BDSPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  // save the seed state in a file to recover potentially unrecoverable events
  std::ofstream f = std::ofstream();
  f.open("evtseed.txt");
  std::stringstream ss1;
  CLHEP::HepRandom::saveFullState(ss1);
  f << ss1.str();
  f.close();


  //this function is called at the begining of event

  G4double x0=0.0, y0=0.0, z0=0.0, xp=0.0, yp=0.0, zp=0.0, t=0.0, E=0.0;

  particleGun->SetParticleDefinition(BDSGlobalConstants::Instance()->GetParticleDefinition());
  bdsBunch->GetNextParticle(x0,y0,z0,xp,yp,zp,t,E,weight); // get next starting point
  G4double mass = particleGun->GetParticleDefinition()->GetPDGMass();
  G4double EK = E - mass;
  
  if(EK<0)
    {
      G4cout << __METHOD_NAME__ << "Particle kinetic energy smaller than 0! This will not be tracked." << G4endl;
      anEvent->SetEventAborted();
    }
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
  particleGun->SetParticleEnergy(EK);
  particleGun->SetParticleMomentumDirection(PartMomDir);
  particleGun->SetParticleTime(t);
  
  particleGun->GeneratePrimaryVertex(anEvent);

  //Set the weight
#ifdef BDSDEBUG
  G4cout << "BDSPrimaryGeneratorAction: setting weight = " << weight << G4endl;
#endif
  anEvent->GetPrimaryVertex()->SetWeight(weight);
  
#ifdef BDSDEBUG
  G4cout
    << "BDSPrimaryGeneratorAction: " << G4endl
    << "  position= " << particleGun->GetParticlePosition()/CLHEP::m<<" m"<<G4endl
    << "  total energy= " << E/CLHEP::GeV << " GeV" << G4endl
    << "  kinetic energy= " << EK/CLHEP::GeV << " GeV" << G4endl
    << "  momentum direction= " << PartMomDir << G4endl
    << "  weight= " << anEvent->GetPrimaryVertex()->GetWeight() << G4endl;
#endif

  // save initial values outside scope for entry into the samplers:
  BDSParticle initialPoint(x0,y0,z0,xp,yp,zp,E,t,weight);
  BDSGlobalConstants::Instance()->SetInitialPoint(initialPoint);
}
