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
#include <map>
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

  const std::map<std::string, G4int> files = {{"h10protons.dat",   2212},
					      {"h10neutrons.dat",  2112},
					      {"h10photons.dat",   22},
					      {"h10electrons.dat", 11},
					      {"h10positrons.dat", -11}};

  G4cout << "Scorer \"" << GetName() << "\" - adding conversionFiles:" << G4endl;
  BDSScorerConversionLoader<std::ifstream> loader;
  for (const auto& filePDG : files)
    {
      G4String uncompressedFile = filePath + filePDG.first;
      G4String compressedFile   = filePath + filePDG.first+".gz";
      if (BDS::FileExists(uncompressedFile))
	{
	  conversionFactors[filePDG.second] = loader.Load(uncompressedFile);
	  G4cout << __METHOD_NAME__ << "adding: " << uncompressedFile << G4endl;
	}
      else if (BDS::FileExists(compressedFile))
	{
#ifdef USE_GZSTREAM
	  BDSScorerConversionLoader<igzstream> loaderC;
	  conversionFactors[filePDG.second] = loaderC.Load(compressedFile);
	  G4cout << __METHOD_NAME__ << "adding: " << compressedFile << G4endl;
#else
	  throw BDSException(__METHOD_NAME__, "Compressed file loading - but BDSIM not compiled with ZLIB.");
#endif
	}
    }
}

BDSScorerAmbientDose3D::~BDSScorerAmbientDose3D()
{
  for (auto conversionFactor : conversionFactors)
    {delete conversionFactor.second;}
}

G4double BDSScorerAmbientDose3D::GetConversionFactor(G4int particleID, G4double energy) const
{
  auto search = conversionFactors.find(particleID);
  if (search != conversionFactors.end())
    {return search->second->Value(energy);}
  else
    {return 0;}
}
