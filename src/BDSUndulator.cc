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
#include "globals.hh" // geant4 globals / types

#include "BDSAcceleratorComponent.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSComponentFactory.hh"
#include "BDSDebug.hh"
#include "BDSFieldType.hh"
#include "BDSGlobalConstants.hh"
#include "BDSIntegratorSet.hh"
#include "BDSIntegratorType.hh"
#include "BDSIntegratorSetType.hh"
#include "BDSMagnetStrength.hh"
#include "BDSLine.hh"
#include "BDSMagnet.hh"
#include "BDSMagnetOuterInfo.hh"
#include "BDSUndulator.hh"
#include "BDSUtilities.hh"

#include "parser/element.h"
#include "parser/elementtype.h"

#include "G4Transform3D.hh"

using namespace GMAD;

BDSAcceleratorComponent* BDS::BuildUndulator(const G4String&         elementName,
                                             const Element*          element,
                                             BDSMagnetStrength*      st,
                                             const G4double          brho,
                                             const BDSIntegratorSet* integratorSet)
{
  const G4String    baseName = elementName;
  const G4bool    yokeOnLeft = BDSComponentFactory::YokeOnLeft(element,st);
  G4double   arcLength = element->l * CLHEP::m;
  G4double      period = element->undulatorPeriod * CLHEP::m;

  if (BDS::IsFinite(fmod(arcLength, period)))
    {
        G4cerr << __METHOD_NAME__ << "Undulator length \"arcLength\" does not divide into an integer number of "
                "undulator periods (length \"period\"" <<  G4endl;
        exit(1);
    }

  G4int nFullMagnets = (arcLength / period) - 1.0;

  // Note for tilted undulators, the geometry is tilted but the curvilinear world isn't,
  // therefore we tilt the field to match the geometry.
  G4Transform3D fieldTiltOffset = BDSComponentFactory::CreateFieldTransform(element);

  // Line for all the undulator magnets
  BDSLine* undulatorLine  = new BDSLine(baseName); // line for resultant sbend

  //single magnet length
  G4double magnetLength = arcLength / (G4double) nFullMagnets;

  BDSMagnetStrength* upStrengthFull = new BDSMagnetStrength(*st); // the copy is crucial to copy the field strength
  (*upStrengthFull)["length"] = magnetLength;

  BDSMagnetStrength* downStrengthFull = new BDSMagnetStrength(*st); // the copy is crucial to copy the field strength
  (*downStrengthFull)["field"]  *= -1.0;
  (*downStrengthFull)["length"] = magnetLength;

  BDSMagnetStrength* upStrengthHalf = new BDSMagnetStrength(*upStrengthFull); // the copy is crucial to copy the field strength
  (*upStrengthHalf)["length"] = magnetLength/2.0;

  BDSMagnetStrength* downStrengthHalf = new BDSMagnetStrength(*downStrengthFull); // the copy is crucial to copy the field strength
  (*downStrengthHalf)["length"] = magnetLength/2.0;


  G4String halfNameIn = baseName + "_halfLengthEntrance";

  BDSMagnet* entranceHalf = BDS::BuildUndulatorMagnet(element, halfNameIn, magnetLength/2.0, upStrengthHalf,
                                        brho, integratorSet, yokeOnLeft);
  undulatorLine->AddComponent(entranceHalf);

    G4int numberOfUniqueComponents = 1; // used for naming purposes
  BDSMagnet* oneBend = nullptr;
  for (G4int i = 0; i < nFullMagnets; ++i)
    {
      G4String name = baseName + "_"+std::to_string(numberOfUniqueComponents);
      numberOfUniqueComponents++;
      if (i % 2 == 0)
        {
          oneBend = BDS::BuildUndulatorMagnet(element, name, magnetLength, downStrengthFull,
                                              brho, integratorSet, !yokeOnLeft);
        }
      else
        {
          oneBend = BDS::BuildUndulatorMagnet(element, name, magnetLength, upStrengthFull,
                                              brho, integratorSet, yokeOnLeft);
        }
      // append to the line
      undulatorLine->AddComponent(oneBend);
    }
  G4String halfNameOut = baseName + "_halfLengthExit";
  BDSMagnet* exitHalf = BDS::BuildUndulatorMagnet(element, halfNameOut, magnetLength/2.0, upStrengthHalf,
                                                        brho, integratorSet, yokeOnLeft);
  undulatorLine->AddComponent(exitHalf);

  return undulatorLine;
}


BDSMagnet* BDS::BuildUndulatorMagnet(const GMAD::Element*     element,
                                     const G4String           name,
                                     const G4double           magnetLength,
                                     const BDSMagnetStrength* strength,
                                     const G4double           brho,
                                     const BDSIntegratorSet*  integratorSet,
                                     const G4bool             yokeOnLeft)
{
  auto bpInfo = BDSComponentFactory::PrepareBeamPipeInfo(element, 0, 0);

  BDSMagnetStrength* strengthCopy = new BDSMagnetStrength(*strength); // the copy is crucial to copy the field strength
  auto  magnetOuterInfo = BDSComponentFactory::PrepareMagnetOuterInfo(name, element, 0, 0, bpInfo, yokeOnLeft);
  // set the name to the desired one rather than the one from the element
  magnetOuterInfo->name = name;

  G4Transform3D fieldTiltOffset = BDSComponentFactory::CreateFieldTransform(element);

  BDSFieldType dipoleFieldType = BDSFieldType::dipole;
  BDSIntegratorType intType = integratorSet->undulator;
  BDSFieldInfo* vacuumField = new BDSFieldInfo(dipoleFieldType,
                                               brho,
                                               intType,
                                               strengthCopy,
                                               true,
                                               fieldTiltOffset);

  BDSMagnet* magnet = new BDSMagnet(BDSMagnetType::rectangularbend,
                                    name,
                                    magnetLength,
                                    bpInfo,
                                    magnetOuterInfo,
                                    vacuumField,
                                    0);

  return magnet;
}