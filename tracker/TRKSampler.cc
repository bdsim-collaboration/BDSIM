#include <string>
#include <iostream>

#include "TRKSampler.hh"
#include "TRKStrategy.hh"

TRKSampler::TRKSampler(std::string nameIn):TRKElement(nameIn+"_sampler",0,NULL,NULL)
{}

TRKSampler::~TRKSampler() 
{}

void TRKSampler::Track(TRKBunch* bunch, TRKStrategy* strategy) {
  strategy->Track(this,bunch);
}

void TRKSampler::Print(std::ostream &out)const {
  out << "Sampler: " << name << " ";
}
