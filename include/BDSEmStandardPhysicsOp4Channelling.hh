//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
// $Id: G4EmStandardPhysics_option4_channeling.hh 98736 2016-08-09 10:55:12Z gcosmo $
//
//---------------------------------------------------------------------------
//
// ClassName:   G4EmStandardPhysics_option4_channeling
//
// Author:      V.Ivanchenko 28.09.2012
//
// Modified:
//
//----------------------------------------------------------------------------
//
// This class provides construction of EM physics using the best models
// of standard and low-energy packages and set of 
// the most adavced options allowing precise simulation at low
// and intermediate energies
//
#include "G4Version.hh"
// just exclude whole physics list for < 10.4 as it's only used with channelling that's in 10.4
#if G4VERSION_NUMBER > 1039

#ifndef BDSSTANDARDPHYSICSOP4CHANNELLING_H
#define BDSSTANDARDPHYSICSOP4CHANNELLING_H

#include "G4VPhysicsConstructor.hh"
#include "globals.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

/**
 * @brief Copy of crystal channelling em physics from Geant4.
 */

class BDSEmStandardPhysicsOp4Channelling : public G4VPhysicsConstructor
{
public:

  explicit BDSEmStandardPhysicsOp4Channelling(G4int ver=1, const G4String& name="");

  virtual ~BDSEmStandardPhysicsOp4Channelling();

  virtual void ConstructParticle();
  virtual void ConstructProcess();

private:
  G4int  verbose;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

#endif
