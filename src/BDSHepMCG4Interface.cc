
#ifdef USE_HEPMC3
#include "BDSHepMCG4Interface.hh"

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

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
BDSHepMC3Reader::BDSHepMC3Reader()
  : hepmcEvent(0)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
BDSHepMC3Reader::~BDSHepMC3Reader()
{
  delete hepmcEvent;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4bool BDSHepMC3Reader::CheckVertexInsideWorld
                         (const G4ThreeVector& pos) const
{
  G4Navigator* navigator= G4TransportationManager::GetTransportationManager()
                                                 -> GetNavigatorForTracking();

  G4VPhysicalVolume* world= navigator-> GetWorldVolume();
  G4VSolid* solid= world-> GetLogicalVolume()-> GetSolid();
  EInside qinside= solid-> Inside(pos);

  if( qinside != kInside) return false;
  else return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void BDSHepMC3Reader::HepMC2G4(const HepMC3::GenEvent* hepmcevt,
                                G4Event* g4event)
{
    G4PrimaryVertex* g4vtx = new G4PrimaryVertex(0, 0, 0, 0);


    for (const auto &particlePtr : hepmcevt->particles())
    {
        const HepMC3::GenParticle* particle = particlePtr.get();
        int pdgcode = particle->pdg_id();
        std::vector<HepMC3::ConstGenParticlePtr> child = particle->children();
        if (child.size() > 0){
            continue;
        }
        else{
            //G4cout << "0 children: " << child.size() << G4endl;
            HepMC3::FourVector fv = particle->momentum();
            G4LorentzVector p(fv.px(), fv.py(), fv.pz(), fv.e());
            G4PrimaryParticle* g4prim = new G4PrimaryParticle(pdgcode, p.x() * CLHEP::GeV, p.y() * CLHEP::GeV, p.z() * CLHEP::GeV);
            g4vtx->SetPrimary(g4prim);
        }

    }
    g4event->AddPrimaryVertex(g4vtx);

}
/*
    G4bool qvtx=false;
    for (HepMC3::GenVertex::   particle_iterator
           pitr= (*vitr)->particles_begin(HepMC3::children);
         pitr != (*vitr)->particles_end(HepMC3::children); ++pitr)
    {

      if (!(*pitr)->end_vertex() && (*pitr)->status()==1) {
        qvtx=true;
        break;
      }
    }
    if (!qvtx) continue;


    // check world boundary
    HepMC3::FourVector pos= (*vitr)-> position();
    G4LorentzVector xvtx(pos.x(), pos.y(), pos.z(), pos.t());
    if (! CheckVertexInsideWorld(xvtx.vect()*mm)) continue;

    // create G4PrimaryVertex and associated G4PrimaryParticles
    G4PrimaryVertex* g4vtx=
      new G4PrimaryVertex(xvtx.x()*mm, xvtx.y()*mm, xvtx.z()*mm, xvtx.t()*mm/c_light);



    for (HepMC3::GenVertex::particle_iterator
           vpitr= (*vitr)->particles_begin(HepMC3::children);
         vpitr != (*vitr)->particles_end(HepMC3::children); ++vpitr) {

      if( (*vpitr)->status() != 1 ) continue;

      G4int pdgcode= (*vpitr)-> pdg_id();
      pos= (*vpitr)-> momentum();
      G4LorentzVector p(pos.px(), pos.py(), pos.pz(), pos.e());
      G4PrimaryParticle* g4prim= new G4PrimaryParticle(pdgcode, p.x()*GeV, p.y()*GeV, p.z()*GeV);

      g4vtx-> SetPrimary(g4prim);
    }
    g4event-> AddPrimaryVertex(g4vtx);
  }

}*/

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
HepMC3::GenEvent* BDSHepMC3Reader::GenerateHepMCEvent()
{
  HepMC3::GenEvent* aevent = new HepMC3::GenEvent();
  return aevent;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void BDSHepMC3Reader::GeneratePrimaryVertex(G4Event* anEvent)
{
  // delete previous event object
  delete hepmcEvent;

  // generate next event
  hepmcEvent = GenerateHepMCEvent();
  if(! hepmcEvent) {
    G4cout << "HepMCInterface: no generated particles. run terminated..."
           << G4endl;
    G4RunManager::GetRunManager()-> AbortRun();
    return;
  }
  HepMC2G4(hepmcEvent, anEvent);
}

#endif