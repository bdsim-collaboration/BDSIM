/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2017.

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
#ifndef BDSOUTPUTROOTEVENTHEADER_H
#define BDSOUTPUTROOTEVENTHEADER_H

#include "TROOT.h"
#include "TObject.h"

/** 
 * @brief Information about the software and the file.
 * 
 * @author Laurie Nevay
 */

class BDSOutputROOTEventHeader: public TObject
{
public:
  BDSOutputROOTEventHeader();
  virtual ~BDSOutputROOTEventHeader();
  void Flush();
  
  std::string bdsimVersion;
  std::string geant4Version;
  std::string rootVersion;
  std::string clhepVersion;
  std::string timeStamp;
  std::string fileType;

#ifndef __ROOTBUILD__   
  void Fill();
#endif

  ClassDef(BDSOutputROOTEventHeader,1);
};

#endif
