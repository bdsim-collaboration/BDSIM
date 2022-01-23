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
#ifndef EVENTOUTPUT_H
#define EVENTOUTPUT_H

#include <string>

#include "TRKOutputSamplers.hh"

class TRKBunch;
class BDSOutput;

namespace trk
{
  class EventOutput
  {
  public:
    EventOutput(std::string file);
    ~EventOutput();
    void NewFile();
    void CloseFile();
    void WritePrimaries(TRKBunch* bunch);

    void FillSamplerHitsTracker(int samplerIndex,
				TRKParticle& particle,
				double s);

  
    void RecordSamplerHit(int samplerIndex, TRKParticle const &particle, int turn,
			  double s);

    void PushBackSampler(const std::string& name);
    void PushBackSampler(const std::string& name, std::size_t reserved);
  

    void WriteEvents();
    int currentTurn;

  private:
    std::string filename;
    BDSOutput* output;
    TRKSamplerData primaries;
    TRKOutputSamplers samplers;
    int nevents;

    void FillEventOutputStructures();
    void PrimaryFillOutput(int eventID);
    void SamplersFillOutput(int eventID);

  };
}

#endif /* EVENTOUTPUT_H */
