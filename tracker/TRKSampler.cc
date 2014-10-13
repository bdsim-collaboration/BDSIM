#include <string>
#include <iostream>

#include "TRKSampler.hh"
#include "TRKBunch.hh"
#include "TRKStrategy.hh"
#include "TRKBunch.hh"

#include "BDSOutputBase.hh"
#include "BDSDebug.hh"

extern BDSOutputBase* trkOutput;

TRKSampler::TRKSampler(std::string nameIn):TRKElement(nameIn+"_sampler",0,NULL,NULL)
{}

TRKSampler::~TRKSampler() 
{}

std::ostream& operator<< (std::ostream& out, const TRKSampler& element) {
  return out << "Sampler: " << element.name << " ";
}

void TRKSampler::Track(TRKBunch* bunch, TRKStrategy* /*strategy*/)
{
#ifdef TRKDEBUG
  std::cout << __METHOD_NAME__ << std::endl;
#endif
  //overload this and do it differently for the sampler
  //normally pass bunch to strategy along with *this but
  //instead we write to output here
  //in effect, the strategy is implemented here
  trkOutput->WriteTrackerBunch(name,bunch);
}

void TRKSampler::CheckAperture(TRKBunch* /*bunch*/)
{
//overload this from the base class but to do nothing
#ifdef TRKDEBUG
  std::cout << __METHOD_NAME__ << std::endl;
#endif
}
