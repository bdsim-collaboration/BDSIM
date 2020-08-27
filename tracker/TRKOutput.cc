#include <string>

#include "BDSParser.hh"
#include "BDSOutput.hh"
#include "BDSOutputFactory.hh"
#include "BDSGlobalConstants.hh"

#include "TRKOutput.hh"

TRKOutput::TRKOutput(std::string file) : filename(file)
{
  output = BDSOutputFactory::CreateOutput(
      BDSGlobalConstants::Instance()->OutputFormat(), filename);
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

void TRKOutput::FillSamplerHitsTracker(G4int samplerIndex,
                                       TRKParticle &particle, double s)
{
  output->FillSamplerHitsTracker(samplerIndex, particle, s);
}
