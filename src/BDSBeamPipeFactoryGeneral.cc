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
#include "BDSAperture.hh"
#include "BDSApertureFactory.hh"
#include "BDSBeamPipeFactoryGeneral.hh"
#include "BDSBeamPipe.hh"
#include "BDSBeamPipeInfo2.hh"
#include "BDSExtent.hh"

#include "globals.hh"                      // geant4 globals / types
#include "G4CutTubs.hh"
#include "G4LogicalVolume.hh"
#include "G4SubtractionSolid.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "G4VSolid.hh"

#include <cmath>
#include <set>
#include <utility>               // for std::pair

BDSBeamPipeFactoryGeneral::BDSBeamPipeFactoryGeneral()
{;}

BDSBeamPipe* BDSBeamPipeFactoryGeneral::CreateBeamPipe(const G4String& name,
                                                G4double length,
                                                BDSBeamPipeInfo2* bpi)
{
  CleanUp();
  BDSApertureFactory fac;
  vacuumSolid = fac.CreateSolid(name+"_vacuum", length, bpi->aperture, bpi->apertureOut, bpi->inputFaceNormal, bpi->outputFaceNormal);
  // we ignore angled faces and make it longer
  BDSAperture* apContSubIn  = bpi->aperture; // += 3; TBC
  BDSAperture* apContSubOut = bpi->apertureOut ? bpi->apertureOut : nullptr;

  delete apContSubIn; delete apContSubOut;
  
  if (!bpi->vacuumOnly)
  {
    BDSAperture* apBpInInside   = bpi->aperture;
    BDSAperture* apBpOutInside  = bpi->apertureOut;
    BDSAperture* apBpInOutside  = bpi->aperture;
    BDSAperture* apBpOutOutside = bpi->apertureOut;
    beamPipeSolid = fac.CreateSolidWithInner(name, length,
                                             apBpInInside, apBpInOutside,
                                             apBpOutInside, apBpOutOutside,
                                             bpi->inputFaceNormal, bpi->outputFaceNormal);
    delete apBpInInside; delete apBpOutInside; delete apBpOutInside; delete apBpOutOutside;
  }
  
  containerSubtractionSolid = fac.CreateSolid(name+"_container_sub",
                                              length, apContSubIn, apContSubOut,
                                              nullptr, nullptr,
                                              0.2*length);
  
  BDSBeamPipeFactoryBase::CommonConstruction(name, bpi->vacuumMaterial, bpi->beamPipeMaterial, length);

  // record extents
  BDSExtent ext = fac.
  //BDSExtent ext = BDSExtent(containerRadiusIn, containerRadiusIn, lengthIn*0.5);

  // true for containerIsGeneral - true for this factory
  return BDSBeamPipeFactoryBase::BuildBeamPipeAndRegisterVolumes(ext, containerRadiusIn, true);
}