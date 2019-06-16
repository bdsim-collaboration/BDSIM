
#ifdef USE_HEPMC3
#include "BDSHepMC3Reader.hh"

#include "G4RunManager.hh"
#include "G4LorentzVector.hh"
#include "G4Event.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
#include "G4TransportationManager.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include "HepMC3/GenParticle.h"
#include "HepMC3/GenVertex.h"
#include "HepMC3/Attribute.h"

#include "CLHEP/Units/SystemOfUnits.h"

BDSHepMC3Reader::BDSHepMC3Reader():
  hepmcEvent(nullptr)
{;}

BDSHepMC3Reader::~BDSHepMC3Reader()
{
  delete hepmcEvent;
}

G4bool BDSHepMC3Reader::CheckVertexInsideWorld(const G4ThreeVector& pos) const
{
  G4Navigator* navigator= G4TransportationManager::GetTransportationManager()
                                                 -> GetNavigatorForTracking();

  G4VPhysicalVolume* world= navigator-> GetWorldVolume();
  G4VSolid* solid= world-> GetLogicalVolume()-> GetSolid();
  EInside qinside= solid-> Inside(pos);

  if( qinside != kInside) return false;
  else return true;
}

void BDSHepMC3Reader::HepMC2G4(const HepMC3::GenEvent* hepmcevt,
                                G4Event* g4event)
{
  G4PrimaryVertex* g4vtx = new G4PrimaryVertex(0, 0, 0, 0);

  for (const auto &particlePtr : hepmcevt->particles())
    {
      const HepMC3::GenParticle* particle = particlePtr.get();
      if (!particle->children.empty())
	{continue;}
      int pdgcode = particle->pdg_id();
      HepMC3::FourVector fv = particle->momentum();
      G4LorentzVector p(fv.px(), fv.py(), fv.pz(), fv.e());
      G4PrimaryParticle* g4prim = new G4PrimaryParticle(pdgcode,
							p.x() * CLHEP::GeV,
							p.y() * CLHEP::GeV,
							p.z() * CLHEP::GeV);
      g4vtx->SetPrimary(g4prim);
    }
  g4event->AddPrimaryVertex(g4vtx);
}

HepMC3::GenEvent* BDSHepMC3Reader::GenerateHepMCEvent()
{
  HepMC3::GenEvent* aevent = new HepMC3::GenEvent();
  return aevent;
}

void BDSHepMC3Reader::GeneratePrimaryVertex(G4Event* anEvent)
{
  // delete previous event object
  delete hepmcEvent;

  // generate next event
  hepmcEvent = GenerateHepMCEvent();
  if(! hepmcEvent)
    {
      G4cout << "HepMCInterface: no generated particles. run terminated..." << G4endl;
      G4RunManager::GetRunManager()-> AbortRun();
      return;
    }
  HepMC2G4(hepmcEvent, anEvent);
}

#endif
