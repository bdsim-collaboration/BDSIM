/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2019.

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
#include "TRKAperture.hh"
#include <iterator>
#include <vector>
#include <iostream>
#include <algorithm> //for swap

// Loss output
#include "BDSDebug.hh"
#include "BDSOutputBase.hh"
#include "BDSGlobalConstants.hh"

extern BDSOutputBase* trkOutput;

std::ostream& operator<<(std::ostream& out, const TRKAperture& aperture)
{
  aperture.PrintDetails(out);
  return out;
}

TRKBunch* TRKAperture::CheckAperture(TRKBunch* bunch)
{
  //loop over bunch and check aperture
  TRKBunchIter iter       = bunch->begin();
  TRKBunchIter backactive = --(bunch->end()); //used to denote the last useful particle
  //don't use 'back' as that returns a direct reference and not an iterator

  //make an output vector of particles
  std::vector<TRKParticle> lostparticles;
  //reserve say 1000 particles for quick push_back
  int expectedNLost = (int)bunch->size()*0.01;
  lostparticles.reserve(expectedNLost);

#ifdef BDSDEBUG
  std::cout << __METHOD_NAME__ << " Bunch Population: " << bunch->size()
	    << "; Lost Particles Population " << lostparticles.size() << std::endl;
#endif

  while (iter != backactive+1) //a while loop gives over control of the increment of the iterator
    {
      if (OutsideAperture(*iter)) {
#ifdef TRKDEBUG
	std::cout << "Particle outside aperture! " << std::endl;
#endif
	  //note we're using the OutsideAperture virtual function in each derived class
	  lostparticles.push_back(*iter); //copy to output vector
#ifdef TRKDEBUG
	  std::cout << "BEFORE SWAP" << std::endl;
	  std::cout << *bunch <<std::endl;
#endif
	  std::swap(*iter,*backactive);   //swap the bad one with the last good one
#ifdef TRKDEBUG
	  std::cout << "AFTER SWAP" << std::endl;
	  std::cout << *bunch <<std::endl;
#endif
	  --backactive; //change mark of where last good one is
	  //don't increment the iterator so we check the unknown particle from the end
	}
      else {
#ifdef TRKDEBUG
	std::cout << "Particle inside aperture - continuing" << std::endl;
#endif
	++iter;
      }
    }
  //note this method only swaps bad particles to the end of the vector / container
  //std::remove_if for example does the same but causes a copy of all subsequent 
  //items in the vector along one slot -> time (N).  This is quicker and clearer
  //std::remove_if also would require a static object or plain funciton and hence
  //doesn't play nicely with polymorphism - we have to create a dummy functor class
  //for each type of aperture
  //note this algorithm is only good when typicaly vector size is over 1k or 10k and 
  //vector elements are moderately sized (better for larger and longer)
#ifdef TRKDEBUG
  std::cout << "BACK ACTIVE after testing loop" << std::endl;
  std::cout << *backactive << std::endl << std::endl;
  std::cout << "BUNCH BEFORE erasure" << std::endl;
  std::cout << *bunch;
#endif

  //erase all the bad particles at the back of the vector in one foul swoop
  bunch->Erase(backactive+1,bunch->end());
#ifdef TRKDEBUG
  std::cout << "BUNCH AFTER erasure" << std::endl;
  std::cout << *bunch;
#endif
  
  //now publish / shift all output particles to BDSIM here - TBC
  // for now lets just output them
#ifdef TRKDEBUG
  if (lostparticles.size() > 0)
    {
      std::cout << "Lost particles: " << std::endl;
      std::cout << TRKBunch(lostparticles);
      std::cout << "End of lost particles" << std::endl;
    }
#endif

#ifdef TRKDEBUG
  std::cout << __METHOD_NAME__ << "After aperture checking: " 
	    << "Bunch Population: " << bunch->size() 
	    << "; Lost Particles Population: " << lostparticles.size() << std::endl;
#endif
  
  TRKBunch* lostBunch = new TRKBunch(lostparticles);
  return lostBunch;
}

