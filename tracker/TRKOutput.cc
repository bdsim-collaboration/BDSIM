#include <string>

#include "BDSParser.hh"
#include "BDSOutput.hh"
#include "BDSOutputFactory.hh"
#include "BDSGlobalConstants.hh"

#include "TRKOutput.hh"

TRKOutput::TRKOutput(std::string file) :
  nevents(0),
  filename(file)
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
    output->WriteTrackerBunch("primaries", bunch, true);
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

void TRKOutput::PushBackSampler()
{
  samplers.PushBackSampler();
}

void TRKOutput::WriteEvents()
{
  FillSamplerOutputStructures();
}

void TRKOutput::FillSamplerOutputStructures()
{
  
  for (int i = 0; i < nevents; ++i)  // Loop event IDs
    {
      // Loop sampler indices
      for (int isampler = 0; isampler < samplers.NSamplers(); ++isampler)
	{
        auto range = samplers.GetSamplerData(isampler).EventRange(i);
        // Loop particles recorded in that sampler for this event ID.
        for (auto it = range.first; it != range.second; ++it) {
	  std::cout << "EventID: " << i << ", sampler: " << isampler << it->particle << "\n";
          output->FillSamplerHitsTracker(isampler, it->particle, it->s);
        }
      }
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
