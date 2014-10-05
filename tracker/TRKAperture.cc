#include "TRKAperture.hh"
#include <iterator>
#include <vector>
#include <iostream>
#include <algorithm> //for swap


void TRKAperture::CheckAperture(TRKBunch* bunch)
{
  //loop over bunch and check aperture
  TRKBunchIter iter       = bunch->begin();
  TRKBunchIter end        = bunch->end();
  TRKBunchIter backactive = bunch->end()--; //used to denote the last useful particle

  //make an output vector of particles
  std::vector<TRKParticle> lostparticles;
  //reserve say 1000 particles for quick push_back
  lostparticles.reserve(1000);
  
  for (;iter != end; ++iter)
    {
      if (OutsideAperture(*iter)) {
	  //note we're using the OutsideAperture virtual function in each derived class
	  lostparticles.push_back(*iter); //copy to output vector
	  std::swap(iter,backactive);   //copy swap the bad one with the last good one
	  backactive--; //change mark of where last good one is
	}
    }
  //note this method only swaps bad particles to the end of the vector / container
  //std::remove_if for example does the same but causes a copy of all subsequent 
  //items in the vector along one slot -> time (N).  This is quicker and clearer
  //std::remove_if also would require a static object or plain funciton and hence
  //doesn't play nicely with polymorphism - we have to create a dummy functor class
  //for each type of aperture

  //erase all the bad particles at the back of the vector in one foul swoop
  bunch->Erase(backactive,bunch->end());
  
  //now publish / shift all output particles to BDSIM here - TBC
  // for now lets just output them
  if (lostparticles.size() > 0) {
    std::cout << "Lost particles: " << std::endl;
    std::cout << TRKBunch(lostparticles) << std::endl;
    std::cout << "End of lost particles" << std::endl;
  }
}

