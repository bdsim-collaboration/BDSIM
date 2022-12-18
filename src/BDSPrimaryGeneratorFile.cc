/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2022.

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
#include "BDSBunch.hh"
#include "BDSBunchEventGenerator.hh"
#include "BDSBunchType.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSGlobalConstants.hh"
#include "BDSPrimaryGeneratorFile.hh"
#include "BDSROOTSamplerReader.hh"
#include "BDSUtilities.hh"

#ifdef USE_HEPMC3
#include "BDSHepMC3Reader.hh"
#endif

#include "geomdefs.hh"
#include "G4LogicalVolume.hh"
#include "G4Navigator.hh"
#include "G4ThreeVector.hh"
#include "G4TransportationManager.hh"
#include "G4Types.hh"
#include "G4VPhysicalVolume.hh"

#include "parser/beam.h"

BDSPrimaryGeneratorFile::BDSPrimaryGeneratorFile():
  endOfFileReached(false),
  matchFileLength(false),
  loopFile(false),
  vertexGeneratedSuccessfully(false),
  currentFileEventIndex(0),
  nEventsInFile(0),
  nEventsReadThatPassedFilters(0),
  worldSolid(nullptr)
{;}

BDSPrimaryGeneratorFile::~BDSPrimaryGeneratorFile()
{;}

G4bool BDSPrimaryGeneratorFile::GeneratePrimaryVertexSafe(G4Event* event)
{
  vertexGeneratedSuccessfully = false;

  while (! (endOfFileReached || vertexGeneratedSuccessfully) )
    {GeneratePrimaryVertex(event);} // derived class provides this; it must update vertexGeneratedSuccessfully
  return vertexGeneratedSuccessfully;
}

G4long BDSPrimaryGeneratorFile::NEventsLeftInFile() const
{
  return nEventsInFile - currentFileEventIndex;
}

BDSPrimaryGeneratorFile* BDSPrimaryGeneratorFile::ConstructGenerator(const GMAD::Beam& beam,
                                                                     BDSBunch* bunchIn,
                                                                     G4bool recreate,
                                                                     G4int eventOffset)
{
  BDSPrimaryGeneratorFile* generatorFromFile = nullptr;
  
  BDSBunchType egf = BDSBunchType::eventgeneratorfile;
  G4bool useEventGeneratorFile = BDS::StrContains(G4String(beam.distrType), egf.ToString());
  BDSBunchType samp = BDSBunchType::bdsimsampler;
  G4bool useSamplerLoader = BDS::StrContains(beam.distrType, samp.ToString());
  
  if (useEventGeneratorFile || useSamplerLoader)
    {
      if (beam.distrFile.empty())
	{throw BDSException(__METHOD_NAME__, "no distrFile specified for event generator beam distribution.");}
      G4String filename = BDS::GetFullPath(beam.distrFile, false, beam.distrFileFromExecOptions);
      BDSBunchEventGenerator* beg = dynamic_cast<BDSBunchEventGenerator*>(bunchIn);
      if (!beg)
	{throw BDSException(__METHOD_NAME__, "must be used with a BDSBunchEventGenerator instance");}
      if (useEventGeneratorFile)
	{
#ifdef USE_HEPMC3
	  generatorFromFile = new BDSHepMC3Reader(beam.distrType,
						  filename,
						  beg,
						  beam.removeUnstableWithoutDecay,
						  beam.eventGeneratorWarnSkippedParticles);
#else
	  throw BDSException(__METHOD_NAME__, "event generator file being used but BDSIM not compiled with HEPMC3");
#endif
	}
      else // must be useSamplerLoader is true
	{
	  generatorFromFile = new BDSROOTSamplerReader(beam.distrType,
						       filename,
						       beg,
						       beam.removeUnstableWithoutDecay,
						       beam.eventGeneratorWarnSkippedParticles);
	  
	}
      
      // common bits
      if (recreate)
	{generatorFromFile->RecreateAdvanceToEvent(eventOffset);}
      if (beam.distrFileMatchLength)
	{BDSGlobalConstants::Instance()->SetNumberToGenerate((G4int)generatorFromFile->NEventsLeftInFile());}
    }
  
  return generatorFromFile; // could be nullptr
}

G4bool BDSPrimaryGeneratorFile::VertexInsideWorld(const G4ThreeVector& pos) const
{
  if (!worldSolid)
    {// cache the world solid
      G4Navigator* navigator = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
      G4VPhysicalVolume* world = navigator->GetWorldVolume();
      worldSolid = world->GetLogicalVolume()->GetSolid();
    }
  EInside qinside = worldSolid->Inside(pos);
  return qinside == EInside::kInside;
}
