#ifdef USE_HEPMC3

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
/// \file eventgenerator/HepMC/HepMCEx01/src/HepMCG4AsciiReader.cc
/// \brief Implementation of the HepMCG4AsciiReader class
//
// $Id: HepMCG4AsciiReader.cc 77801 2013-11-28 13:33:20Z gcosmo $
//

#include "BDSHepMCG4AsciiReader.hh"
#include "HepMC3/ReaderAsciiHepMC2.h"

#include <iostream>
#include <fstream>
#include "globals.hh"
//class ReaderAsciiHepMC2;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
HepMCG4AsciiReader::HepMCG4AsciiReader(const G4String& fileNameIn,
G4int verbosityIn)
  :  filename(fileNameIn), verbose(verbosityIn)
{
  //asciiInput= new HepMC3::ReaderAsciiHepMC(filename.c_str(), std::ios::in);
  asciiInput= new HepMC3::ReaderAsciiHepMC2(filename.c_str());

  //messenger= new HepMCG4AsciiReaderMessenger(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
HepMCG4AsciiReader::~HepMCG4AsciiReader()
{
  delete asciiInput;
  //delete messenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void HepMCG4AsciiReader::Initialize()
{
  delete asciiInput;

  asciiInput= new HepMC3::ReaderAsciiHepMC2(filename.c_str());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
HepMC3::GenEvent* HepMCG4AsciiReader::GenerateHepMCEvent()
{
  HepMC3::GenEvent* evt = new HepMC3::GenEvent();
  //HepMC3::ReaderAsciiHepMC2* evt= asciiInput-> read_event();
  asciiInput->read_event(*evt);
    return evt;
  //if(!evt) return 0; // no more event

  //if(verbose>0) evt-> print();

  //return evt;
}

#endif
