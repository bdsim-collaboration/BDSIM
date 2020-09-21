#include <cstddef>
#include <string>

#include "BDSParser.hh"
#include "BDSOutput.hh"
#include "BDSOutputFactory.hh"
#include "BDSGlobalConstants.hh"


#include "TRKBunch.hh"
#include "TRKOutput.hh"
#include "TRKParticle.hh"

TRKOutput::TRKOutput(std::string file) :
  filename(file),
  nevents(0)
{
  output = BDSOutputFactory::CreateOutput(
      BDSGlobalConstants::Instance()->OutputFormat(), filename);
  nevents = BDSGlobalConstants::Instance()->NGenerate();
}

TRKOutput::~TRKOutput()
{
  delete output;
}

void TRKOutput::NewFile()
{
  output->InitialiseGeometryDependent();
  output->NewFile();
  output->FillModel();
  output->FillBeam(BDSParser::Instance()->GetBeamBase());
}

void TRKOutput::CloseFile()
{
  output->CloseFile();
}

void TRKOutput::WritePrimaries(TRKBunch* bunch)
{
  for (const auto &p: *bunch)
    primaries.RecordParticle(p, 0, 0); // FIXME: account for non-zero S-start
}

void TRKOutput::FillSamplerHitsTracker(int samplerIndex,
                                       TRKParticle &particle, double s)
{
  output->FillSamplerHitsTracker(samplerIndex, particle, s);
}

void TRKOutput::RecordSamplerHit(int samplerIndex,
				 TRKParticle const &particle,
				 int turn,
                                 double s)
{
  samplers.RecordParticle(samplerIndex, particle, currentTurn, s);
}

void TRKOutput::PushBackSampler(const std::string& name)
{
  samplers.PushBackSampler(name);
}

void TRKOutput::PushBackSampler(const std::string &name,
                                std::size_t reserved)
{
  samplers.PushBackSampler(name);
  samplers.LastSampler().reserve(reserved);
}

void TRKOutput::WriteEvents()
{
  FillEventOutputStructures();
}

void TRKOutput::FillEventOutputStructures()
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
			nullptr, // energy loss
			nullptr, // energy loss full
			nullptr, // energy loss vacuum
			nullptr, // energy loss tunnel
			nullptr, // energy loss world
			nullptr, // energy loss contents
			nullptr, // world exit hits
			nullptr, // primary hit
			nullptr, // primary loss
			nullptr, // trajectories
			nullptr, // collimator hits
			nullptr, // aperture impacts
			scorerhits, // scorer hits map
			10); // turns taken

    }
}

void TRKOutput::PrimaryFillOutput(int eventid)
{
  auto range = primaries.EventRange(eventid);
  std::for_each(range.first, range.second,
		[this](const TRKSamplerData::TRKSamplerDatum &d)
		{this->output->FillPrimary(d.particle, d.s);});
}

void TRKOutput::SamplersFillOutput(int eventid)
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
