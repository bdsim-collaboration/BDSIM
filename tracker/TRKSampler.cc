#include <string>
#include <iostream>

#include "TRKSampler.hh"

TRKSampler::TRKSampler(std::string nameIn):TRKElement(nameIn+"_sampler",0,NULL,NULL)
{}

TRKSampler::~TRKSampler() 
{}

std::ostream& operator<< (std::ostream& out, const TRKSampler& element) {
  return out << "Sampler: " << element.name << " ";
}
