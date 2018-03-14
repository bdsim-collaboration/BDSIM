/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2018.

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
#include "BDSBeamPipeFactory.hh"
#include "BDSBeamPipeFactoryBase.hh"
#include "BDSBeamPipeFactoryCircular.hh"
#include "BDSBeamPipeFactoryCircularVacuum.hh"
#include "BDSBeamPipeFactoryElliptical.hh"
#include "BDSBeamPipeFactoryRectangular.hh"
#include "BDSBeamPipeFactoryLHC.hh"
#include "BDSBeamPipeFactoryLHCDetailed.hh"
#include "BDSBeamPipeFactoryOctagonal.hh"
#include "BDSBeamPipeFactoryRaceTrack.hh"
#include "BDSBeamPipeFactoryRectEllipse.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSBeamPipeType.hh"
#include "BDSDebug.hh"

#include "globals.hh"                        // geant4 globals / types

BDSBeamPipeFactory* BDSBeamPipeFactory::instance = nullptr;

BDSBeamPipeFactory* BDSBeamPipeFactory::Instance()
{
  if (instance == nullptr)
    {instance = new BDSBeamPipeFactory();}
  return instance;
}

BDSBeamPipeFactory::BDSBeamPipeFactory()
{;}

BDSBeamPipeFactory::~BDSBeamPipeFactory()
{
  delete BDSBeamPipeFactoryCircular::Instance();
  delete BDSBeamPipeFactoryElliptical::Instance();
  delete BDSBeamPipeFactoryRectangular::Instance();
  delete BDSBeamPipeFactoryLHC::Instance();
  delete BDSBeamPipeFactoryLHCDetailed::Instance();
  delete BDSBeamPipeFactoryRectEllipse::Instance();
  delete BDSBeamPipeFactoryRaceTrack::Instance();
  delete BDSBeamPipeFactoryOctagonal::Instance();
  delete BDSBeamPipeFactoryCircularVacuum::Instance();
  instance = nullptr;
}

BDSBeamPipeFactoryBase* BDSBeamPipeFactory::GetAppropriateFactory(BDSBeamPipeType type)
{
  switch(type.underlying())
    {
    case BDSBeamPipeType::circular:
      {return BDSBeamPipeFactoryCircular::Instance(); break;}
    case BDSBeamPipeType::elliptical:
      {return BDSBeamPipeFactoryElliptical::Instance(); break;}
    case BDSBeamPipeType::rectangular:
      {return BDSBeamPipeFactoryRectangular::Instance(); break;}
    case BDSBeamPipeType::lhc:
      {return BDSBeamPipeFactoryLHC::Instance(); break;}
    case BDSBeamPipeType::lhcdetailed:
      {return BDSBeamPipeFactoryLHCDetailed::Instance(); break;}
    case BDSBeamPipeType::rectellipse:
      {return BDSBeamPipeFactoryRectEllipse::Instance(); break;}
    case BDSBeamPipeType::racetrack:
      {return BDSBeamPipeFactoryRaceTrack::Instance(); break;}
    case BDSBeamPipeType::octagonal:
      {return BDSBeamPipeFactoryOctagonal::Instance(); break;}
    case BDSBeamPipeType::circularvacuum:
      {return BDSBeamPipeFactoryCircularVacuum::Instance(); break;}
    default:
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "unknown type \"" << type << "\" - circular beampipe factory by default" << G4endl;
#endif
      return BDSBeamPipeFactoryCircular::Instance();
      break;
    }
}

BDSBeamPipe* BDSBeamPipeFactory::CreateBeamPipe(G4String         name,
						G4double         length,
						BDSBeamPipeInfo* bpi)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "using beam pipe information" << G4endl;
#endif
  if ((bpi->inputFaceNormal.z() > -1) || (bpi->outputFaceNormal.z() < 1))
    {
      return CreateBeamPipe(bpi->beamPipeType,
			    name,
			    length,
			    bpi->inputFaceNormal,
			    bpi->outputFaceNormal,
			    bpi->aper1,
			    bpi->aper2,
			    bpi->aper3,
			    bpi->aper4,
			    bpi->vacuumMaterial,
			    bpi->beamPipeThickness,
			    bpi->beamPipeMaterial);
    }
  else
    {
      return CreateBeamPipe(bpi->beamPipeType,
			    name,
			    length,
			    bpi->aper1,
			    bpi->aper2,
			    bpi->aper3,
			    bpi->aper4,
			    bpi->vacuumMaterial,
			    bpi->beamPipeThickness,
			    bpi->beamPipeMaterial);
    }
}
  
BDSBeamPipe* BDSBeamPipeFactory::CreateBeamPipe(BDSBeamPipeType beamPipeType,
						G4String        name,
						G4double        length,
						G4double        aper1,
						G4double        aper2,
						G4double        aper3,
						G4double        aper4,
						G4Material*     vacuumMaterial,
						G4double        beamPipeThickness,
						G4Material*     beamPipeMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSBeamPipeFactoryBase* factory = GetAppropriateFactory(beamPipeType);
  return factory->CreateBeamPipe(name,length,aper1,aper2,aper3,aper4,
				 vacuumMaterial,beamPipeThickness,beamPipeMaterial);
}

BDSBeamPipe*  BDSBeamPipeFactory::CreateBeamPipe(BDSBeamPipeType beamPipeType,
						 G4String        name,
						 G4double        length,
						 G4ThreeVector   inputFaceNormal,
						 G4ThreeVector   outputFaceNormal,
						 G4double        aper1,
						 G4double        aper2,
						 G4double        aper3,
						 G4double        aper4,
						 G4Material*     vacuumMaterial,
						 G4double        beamPipeThickness,
						 G4Material*     beamPipeMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSBeamPipeFactoryBase* factory = GetAppropriateFactory(beamPipeType);
  return factory->CreateBeamPipe(name,length,inputFaceNormal,outputFaceNormal,aper1,
				 aper2,aper3,aper4,vacuumMaterial,beamPipeThickness,
				 beamPipeMaterial);
}
