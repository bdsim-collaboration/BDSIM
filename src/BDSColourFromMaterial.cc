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
#include "BDSColours.hh"
#include "BDSColourFromMaterial.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Colour.hh"
#include "G4Material.hh"
#include "G4PhysicsOrderedFreeVector.hh"

#include <map>

BDSColourFromMaterial* BDSColourFromMaterial::instance = nullptr;

BDSColourFromMaterial* BDSColourFromMaterial::Instance()
{
  if (!instance)
    {instance = new BDSColourFromMaterial();}
  return instance;
}

BDSColourFromMaterial::~BDSColourFromMaterial()
{
  instance = nullptr;
  delete generalDensity;
}

BDSColourFromMaterial::BDSColourFromMaterial()
{
  BDSColours* c = BDSColours::Instance();
  defines["cu"]       = c->GetColour("coil");
  defines["copper"]   = c->GetColour("coil");
  defines["air"]      = c->GetColour("air:0.95 0.95 0.95 0.05");
  defines["boron"]    = c->GetColour("reallyreallydarkgrey");
  defines["carbon"]   = c->GetColour("reallyreallydarkgrey");
  defines["sulphur"]  = c->GetColour("yellow");
  defines["chlorine"] = c->GetColour("yellow");
  std::vector<G4double> densities = {1e-4, 1e-3, 1e-2, 1,   1e2};
  std::vector<G4double> values    = {1,    10,   100,  180, 255};
  generalDensity = new G4PhysicsOrderedFreeVector(&densities[0], &values[0], values.size());
}

G4Colour* BDSColourFromMaterial::GetColour(const G4Material* material)
{
  G4String materialName = material->GetName();
  materialName.toLower();
  auto search = defines.find(materialName);
  if (search != defines.end())
    {return search->second;}
  else
    {
      G4double alpha = 1;
       G4State state = material->GetState();
       switch (state)
         {
           case G4State::kStateGas:
             {alpha = 0.05; break;}
           case G4State::kStateSolid:
             {alpha = 1.0; break;}
           case G4State::kStateLiquid:
             {alpha = 0.5; break;}
           default:
             {alpha = 1.0; break;}
         }
       G4double density = material->GetDensity();
       G4double value   = generalDensity->Value(density);
       G4String vs      = G4String(std::to_string(value));
       G4Colour* result = BDSColours::Instance()->GetColour(materialName + ":" +
           vs + " " +
           vs + " " +
           vs + " " +
           std::to_string(alpha));
      return result;
    }
}
