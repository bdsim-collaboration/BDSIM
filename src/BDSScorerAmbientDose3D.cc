/*
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway,
University of London 2001 - 2019.

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
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSHistBinMapper3D.hh"
#include "BDSScorerConversionLoader.hh"
#include "BDSScorerAmbientDose3D.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4PhysicsVector.hh"
#include "G4SystemOfUnits.hh"
#include "G4VSolid.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VPVParameterisation.hh"
#include "G4UnitsTable.hh"

#include <fstream>
#include <string>

#ifdef USE_GZSTREAM
#include "src-external/gzstream/gzstream.h"
#endif

BDSScorerAmbientDose3D::BDSScorerAmbientDose3D(const G4String            scorer_name,
					       const BDSHistBinMapper3D* mapperIn,
					       const G4String            pathname,
					       G4int ni,
					       G4int nj,
					       G4int nk,
					       G4int depi,
					       G4int depj,
					       G4int depk):
  BDSScorerQuantity3D(scorer_name, mapperIn, "nosinglefileused", ni, nj, nk, depi, depj, depk)
{
  if (pathname.empty())
    {throw BDSException(__METHOD_NAME__, "no conversionFactorPath provided for \"" + scorer_name + "\" - required");}
  
  G4String filePath = BDS::GetFullPath(pathname);
  
  BDSScorerConversionLoader<std::ifstream> loader;
  
  //TODO use an option instead of hardcoding the filename
  conversionFactor_protons = loader.Load(filePath+"/h10protons.dat");
  conversionFactor_neutrons = loader.Load(filePath+"/h10neutrons.dat");
  conversionFactor_gammas = loader.Load(filePath+"/h10photons.dat");
  conversionFactor_electrons = loader.Load(filePath+"/h10electrons.dat");
}

BDSScorerAmbientDose3D::~BDSScorerAmbientDose3D()
{;}

G4double BDSScorerAmbientDose3D::GetConversionFactor(G4int particleID, G4double energy) const
{
  switch (particleID){
  case 2212 : //protons
    return conversionFactor_protons->Value(energy);
    break;
  case 2112 : //neutrons
    return conversionFactor_neutrons->Value(energy);
    break;
  case 22 : //gamma
    return conversionFactor_gammas->Value(energy);
    break;
  case 11 : //electron
    return conversionFactor_electrons->Value(energy);
    break;
  default:
    return 0;
    break;
  }
}
