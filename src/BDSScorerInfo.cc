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
#include "parser/scorer.h"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSScorerInfo.hh"
#include "BDSScorerType.hh"
#include "G4ParticleTable.hh"
#include "globals.hh"

BDSScorerInfo::BDSScorerInfo(const GMAD::Scorer& scorer):
particle(nullptr)
{
    scorerType = BDS::DetermineScorerType(scorer.type);
    name = scorer.name;
    minimumEnergy = scorer.minimumEnergy;
    maximumEnergy = scorer.maximumEnergy;
    filename = scorer.conversionFactorFile;

    G4bool error = false;

    if(scorer.particlePDGID != 0)
    {
        G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
        particle = particleTable->FindParticle(scorer.particlePDGID);
        error = !particle;
    }
    else if(!(scorer.particleName.empty()))
    {
        G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
        particle = particleTable->FindParticle(scorer.particleName);
        error = !particle;
    }
    if (error)
    {throw BDSException(__METHOD_NAME__,"Particle not found for scorer "+ scorer.name);}
}