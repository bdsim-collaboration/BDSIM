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
#include "BDSScorerInfo.hh"
#include "BDSScorerType.hh"

#include "parser/scorer.h"

#include "globals.hh"
#include "G4ParticleTable.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <map>
#include <string>


BDSScorerInfo::BDSScorerInfo(const GMAD::Scorer& scorer,
			     G4bool upgradeTo3D):
  particle(nullptr)
{
  const std::map<std::string, std::string> replacements = {
							   {"depositeddose",   "depositeddose3d"},
							   {"depositedenergy", "depositedenergy3d"}
  };
  
  std::string scorerTypeNameOriginal = scorer.type;
  std::string scorerTypeName         = scorerTypeNameOriginal; // default copy
  if (upgradeTo3D)
    {
      auto search = replacements.find(scorerTypeNameOriginal);
      if (search != replacements.end())
	{scorerTypeName = search->second;}
    }
  
  scorerType    = BDS::DetermineScorerType(G4String(scorerTypeName));
  name          = scorer.name;
  minimumEnergy = scorer.minimumEnergy*CLHEP::GeV;
  maximumEnergy = scorer.maximumEnergy*CLHEP::GeV;
  filename      = scorer.conversionFactorFile;
  minimumTime   = scorer.minimumTime*CLHEP::second;
  maximumTime   = scorer.maximumTime*CLHEP::second;
  
  G4bool error = false;
  
  if (scorer.particlePDGID != 0)
    {
      G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
      particle = particleTable->FindParticle(scorer.particlePDGID);
      error = !particle;
    }
  else if (!(scorer.particleName.empty()))
    {
      G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
      particle = particleTable->FindParticle(scorer.particleName);
      error = !particle;
    }
  if (error)
    {throw BDSException(__METHOD_NAME__,"Particle not found for scorer "+ scorer.name);}
}
