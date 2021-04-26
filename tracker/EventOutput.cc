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
#include <cstddef>
#include <string>
#include <vector>

#include "BDSParser.hh"
#include "BDSOutput.hh"
#include "BDSOutputFactory.hh"
#include "BDSGlobalConstants.hh"

#include "TRKBunch.hh"
#include "EventOutput.hh"
#include "TRKParticle.hh"

namespace trk
{

EventOutput::EventOutput(std::string file) :
  filename(file),
  nevents(0)
{
  output = BDSOutputFactory::CreateOutput(
      BDSGlobalConstants::Instance()->OutputFormat(), filename);
  nevents = BDSGlobalConstants::Instance()->NGenerate();
}

EventOutput::~EventOutput()
{
  delete output;
}

void EventOutput::NewFile()
{
  output->InitialiseGeometryDependent();
  output->NewFile();
  output->FillModel();
  output->FillBeam(BDSParser::Instance()->GetBeamBase());
}

void EventOutput::CloseFile()
{
  output->CloseFile();
}

void EventOutput::WritePrimaries(TRKBunch* bunch)
{
  for (const auto &p: *bunch)
    primaries.RecordParticle(p, 0, 0); // FIXME: account for non-zero S-start
}

void EventOutput::FillSamplerHitsTracker(int samplerIndex,
                                       TRKParticle &particle, double s)
{
  output->FillSamplerHitsTracker(samplerIndex, particle, s);
}

void EventOutput::RecordSamplerHit(int samplerIndex,
				 TRKParticle const &particle,
				 int turn,
                                 double s)
{
  samplers.RecordParticle(samplerIndex, particle, currentTurn, s);
}

void EventOutput::PushBackSampler(const std::string& name)
{
  samplers.PushBackSampler(name);
}

void EventOutput::PushBackSampler(const std::string &name,
                                std::size_t reserved)
{
  samplers.PushBackSampler(name);
  samplers.LastSampler().reserve(reserved);
}

void EventOutput::WriteEvents()
{
  FillEventOutputStructures();
}

void EventOutput::FillEventOutputStructures()
{
  for (int eventid = 0; eventid < nevents; ++eventid)  // Loop event IDs
    {
      // Loop sampler indices
      PrimaryFillOutput(eventid);
      SamplersFillOutput(eventid);
      const std::map<G4String, G4THitsMap<G4double>*> scorerhits;
      output->FillEvent(nullptr, // EventInfo
			nullptr, // vertex
			nullptr, // sampler hits plane
			nullptr, // sampler hits cylinder
			nullptr, // sampler hits link
			nullptr, // energy loss
			nullptr, // energy loss full
			nullptr, // energy loss vacuum
			nullptr, // energy loss tunnel
			nullptr, // energy loss world
			nullptr, // energy loss contents
			nullptr, // world exit hits
                        std::vector<const BDSTrajectoryPointHit*>(), // primary hit
                        std::vector<const BDSTrajectoryPointHit*>(), // primary loss
			nullptr, // trajectories
			nullptr, // collimator hits
			nullptr, // aperture impacts
			scorerhits, // scorer hits map
			10); // turns taken
    }
}

void EventOutput::PrimaryFillOutput(int eventid)
{
  auto range = primaries.EventRange(eventid);
  std::for_each(range.first, range.second,
		[this](const TRKSamplerData::TRKSamplerDatum &d)
		{this->output->FillPrimary(d.particle, d.s);});
}

void EventOutput::SamplersFillOutput(int eventid)
{
  for (int isampler = 0; isampler < samplers.NSamplers(); ++isampler) {
    auto samplerdata = samplers.GetSamplerData(isampler);
    auto range = samplerdata.EventRange(eventid);
    // Loop particles recorded in that sampler for this event ID.
    for (auto it = range.first; it != range.second; ++it) {
      output->FillSamplerHitsTracker(isampler, it->particle, it->s);
    }
  }
}

}
