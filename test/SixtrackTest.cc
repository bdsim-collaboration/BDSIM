/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2020.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published 
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "BDSBunchSixTrackLink.hh"
#include "BDSException.hh"
#include "BDSIMLink.hh"
#include "BDSIonDefinition.hh"
#include "BDSParticleCoordsFull.hh"
#include "BDSParticleDefinition.hh"
#include "BDSPhysicsUtilities.hh"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4Types.hh"

#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <vector>

struct Collimator
{
  std::string name;
  std::string materialName;
  double length;
  double aperture;
  double rotation;
  double xOffset;
  double yOffset;
};

std::vector<Collimator> ReadFile(const std::string& filename);
void AddParticle(BDSBunchSixTrackLink* stp);
void Summarise(BDSIMLink* bds);

int main(int /*argc2*/, char** /*argv2*/)
{
  std::vector<std::string> arguments = {"--file=/Users/nevay/physics/reps/bdsim-tracker-coupling/test/lhccrystals.gmad","--file=/Users/nevay/physics/reps/bdsim-tracker-coupling/test/lhccrystals.gmad", "--vis_debug", "--batch"};
  std::vector<char*> argv;
  for (const auto& arg : arguments)
    {argv.push_back((char*)arg.data());}
  argv.push_back(nullptr);
  
  BDSBunchSixTrackLink* stp = new BDSBunchSixTrackLink();
  BDSIMLink* bds = new BDSIMLink(stp);
  try
    {bds->Initialise(argv.size() - 1, argv.data());}
  catch (const BDSException& exception)
    {
      std::cerr << std::endl << exception.what() << std::endl;
      delete bds;
      exit(1);
    }
  catch (const std::exception& exception)
    {
      std::cerr << std::endl << exception.what() << std::endl;
      delete bds;
      exit(1);
    }

  std::vector<Collimator> collimators = ReadFile("/Users/nevay/physics/reps/bdsim-tracker-coupling/test/allColls.dat");

  for (const auto& c : collimators)
    {
      bds->AddLinkCollimator(c.name,
			     c.materialName,
			     c.length,
			     c.aperture,
			     c.rotation,
			     c.xOffset,
			     c.yOffset);
    }

  for (const auto& collimator : collimators)
    {
      bds->ClearSamplerHits();
      stp->ClearParticles();

      AddParticle(stp);
      
      bds->SelectLinkElement(collimator.name);
      bds->BeamOn((G4int)stp->Size());
      Summarise(bds);
    }
  
  return 0;
}

std::vector<Collimator> ReadFile(const std::string& filename)
{
  std::ifstream file;
  file.open(filename);
  std::vector<Collimator> result;

  std::string line;
  std::string dummy, cn, mn;
  double le, ap, to, xo, yo;
  while (std::getline(file, line))
    {
      std::stringstream liness(line);
      liness >> dummy >> cn >> mn >> le >> ap >> to >> xo >> yo;
      Collimator co = {cn,mn,le,ap,to,xo,yo};
      result.push_back(co);
    }

  return result;
}

void AddParticle(BDSBunchSixTrackLink* stp)
{
  G4double totalEnergy = 123*CLHEP::GeV;
  BDSParticleCoordsFull coords = BDSParticleCoordsFull(1e-3, 1e-3, 0,
                                                       0, 0, 1,
                                                       0, 0,
                                                       totalEnergy,
                                                       1);

  long long int pdgID = 2212;
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particleDef = particleTable->FindParticle(pdgID);
  if (!particleDef)
    {throw BDSException("BDSBunchUserFile> Particle \"" + std::to_string(pdgID) + "\" not found");}

  BDSIonDefinition* ionDef = nullptr;
  if (BDS::IsIon(particleDef))
    {
      G4int a = 208;
      G4int z = 82;
      G4double q = 82 * CLHEP::eplus;
      ionDef = new BDSIonDefinition(a,z,q);
    }

  // Wrap in our class that calculates momentum and kinetic energy.
  // Requires that one of E, Ek, P be non-zero (only one).
  BDSParticleDefinition* particleDefinition = nullptr;
  try
    {
      particleDefinition = new BDSParticleDefinition(particleDef, totalEnergy, 0, 0, 1, ionDef);
    }
  catch (const BDSException& e)
    {// if we throw an exception the object is invalid for the delete on the next loop
      particleDefinition = nullptr; // reset back to nullptr for safe delete
      return;
    }

  if (particleDefinition)
    {stp->AddParticle(particleDefinition, coords);}
}

void Summarise(BDSIMLink* bds)
{
  const BDSHitsCollectionSamplerLink* hits = bds->SamplerHits();

  //const BDSParticleCoordsFull& coords = hit->coords;
}
