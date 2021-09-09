/*
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway,
University of London 2001 - 2021.

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

#include "BDSDicomVFile.hh"

#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcpixel.h"
#include "dcmtk/dcmdata/dcpxitem.h"
#include "dcmtk/dcmdata/dcpixseq.h"

BDSDicomVFile::BDSDicomVFile(DcmDataset* dset):
  theDataset(dset)
{;}

std::vector<G4double> BDSDicomVFile::Read1Data( DcmDataset * dset, DcmTagKey tagKey, G4int nData )
{
    std::vector<G4double> dataV;

    for(int ii=0; ii<nData; ++ii) {
        G4double data;
        Uint16 datai;
        // see  http://support.dcmtk.org/docs/classDcmItem.html for types
        if (dset->findAndGetFloat64(tagKey, data,ii).good() ) {
            dataV.push_back(data);
        } else if (dset->findAndGetUint16(tagKey, datai,ii).good() ) {
            dataV.push_back(datai);
        } else {
            G4cout <<"ERROR  (" << std::showbase // show the 0x prefix
                   << std::internal // fill between the prefix and the number
                   << std::setfill('0') << std::hex << std::setw(4) << tagKey.getGroup()
                   << "," << tagKey.getElement() << ") "<< std::dec << ii << std::endl;
            G4Exception("DicomHandler::ReadData",
                        "",
                        JustWarning,
                        (std::to_string(data) +G4String(" Have not read (")
                         + std::to_string(tagKey.getGroup())+","+std::to_string(tagKey.getElement())
                         +")"+" : "+std::to_string(ii)).c_str());
        }
    }

    return dataV;
}

OFString BDSDicomVFile::Read1DataStr( DcmDataset * dset, DcmTagKey tagKey )
{
    //  const char* data = "";
    OFString data;
    // see  http://support.dcmtk.org/docs/classDcmItem.html for types
    if (dset->findAndGetOFString(tagKey, data).good() ) {
    } else {
        G4cout <<"ERROR  (" << std::showbase // show the 0x prefix
               << std::internal // fill between the prefix and the number
               << std::setfill('0') << std::hex << std::setw(4) << tagKey.getGroup() << ","
               << tagKey.getElement() << ") "<< std::dec << std::endl;
        G4Exception("DicomHandler::ReadData",
                    "",
                    JustWarning,
                    (" Have not read (" + std::to_string(tagKey.getGroup())+","
                     +std::to_string(tagKey.getElement())+")"+" : ").c_str());
    }

    return data.c_str();
}