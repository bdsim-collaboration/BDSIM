//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file eventgenerator/HepMC/HepMCEx01/src/HepMCG4Interface.cc§
/// \brief Implementation of the HepMCG4Interface class
//
// $Id: HepMCG4Interface.cc 77801 2013-11-28 13:33:20Z gcosmo $
//

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

#include "CLHEP/Units/SystemOfUnits.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
HepMCG4Interface::HepMCG4Interface()
  : hepmcEvent(0)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
HepMCG4Interface::~HepMCG4Interface()
{
  delete hepmcEvent;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4bool HepMCG4Interface::CheckVertexInsideWorld
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
void HepMCG4Interface::HepMC2G4(const HepMC3::GenEvent* hepmcevt,
                                G4Event* g4event)
{
  for (const auto& vertexShrPtr : hepmcevt->vertices())
  {
      const HepMC3::GenVertex *v = vertexShrPtr.get();
      HepMC3::FourVector pos = v->position();
      G4LorentzVector xvtx(pos.x(), pos.y(), pos.z(), pos.t());
      if (!CheckVertexInsideWorld(xvtx.vect() * CLHEP::mm)) { continue; }

      G4PrimaryVertex *g4vtx = new G4PrimaryVertex(xvtx.x() * CLHEP::mm, xvtx.y() * CLHEP::mm, xvtx.z() * CLHEP::mm,
                                                   xvtx.t() * CLHEP::mm / CLHEP::c_light);

      const auto particles = v->particles_out();

      for (const auto particlePtr : particles)
      {
          const HepMC3::GenParticle *particle = particlePtr.get();
          int pdgcode = particle->pdg_id();

          pos = particle->momentum();
          G4LorentzVector p(pos.px(), pos.py(), pos.pz(), pos.e());
          G4PrimaryParticle *g4prim = new G4PrimaryParticle(pdgcode, p.x() * GeV, p.y() * GeV, p.z() * GeV);

          g4vtx->SetPrimary(g4prim);
      }
      g4event-> AddPrimaryVertex(g4vtx);
  }

      /*
    G4bool qvtx=false;
    for (HepMC3::GenVertex::particle_iterator
           pitr= (*vitr)->particles_begin(HepMC3::children);
         pitr != (*vitr)->particles_end(HepMC3::children); ++pitr) {

      if (!(*pitr)->end_vertex() && (*pitr)->status()==1) {
        qvtx=true;
        break;
      }
    }
    if (!qvtx) continue;
*/
      /*
    // check world boundary
    HepMC3::FourVector pos= (*vitr)-> position();
    G4LorentzVector xvtx(pos.x(), pos.y(), pos.z(), pos.t());
    if (! CheckVertexInsideWorld(xvtx.vect()*mm)) continue;

    // create G4PrimaryVertex and associated G4PrimaryParticles
    G4PrimaryVertex* g4vtx=
      new G4PrimaryVertex(xvtx.x()*mm, xvtx.y()*mm, xvtx.z()*mm,
                          xvtx.t()*mm/c_light);
                          */

      /*
    for (HepMC3::GenVertex::particle_iterator
           vpitr= (*vitr)->particles_begin(HepMC3::children);
         vpitr != (*vitr)->particles_end(HepMC3::children); ++vpitr) {

      if( (*vpitr)->status() != 1 ) continue;

      G4int pdgcode= (*vpitr)-> pdg_id();
      pos= (*vpitr)-> momentum();
      G4LorentzVector p(pos.px(), pos.py(), pos.pz(), pos.e());
      G4PrimaryParticle* g4prim=
        new G4PrimaryParticle(pdgcode, p.x()*GeV, p.y()*GeV, p.z()*GeV);

      g4vtx-> SetPrimary(g4prim);
    }
    g4event-> AddPrimaryVertex(g4vtx);
  }
       */
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
HepMC3::GenEvent* HepMCG4Interface::GenerateHepMCEvent()
{
  HepMC3::GenEvent* aevent = new HepMC3::GenEvent();
  return aevent;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void HepMCG4Interface::GeneratePrimaryVertex(G4Event* anEvent)
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
