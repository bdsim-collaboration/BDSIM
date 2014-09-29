#include "TRKThin.hh"

#include <cmath>
#include <complex>
#include <cstdlib>

#include "TRKBunch.hh"
#include "TRKHybrid.hh"
#include "TRKParticle.hh"
#include "TRKDipole.hh"
#include "TRKQuadrupole.hh"
#include "TRKSextupole.hh"
#include "TRKOctupole.hh"

TRKThin::TRKThin(int trackingStepsIn) : 
  TRKStrategy(trackingStepsIn) {
}

TRKThin::~TRKThin() { 
}

void TRKThin::Track(TRKDrift* el, TRKBunch* bunch) { 
  const double h = el->GetLength()/trackingSteps;

  TRKBunchIter iter = bunch->begin();
  TRKBunchIter end = bunch->end();

  for (;iter!=end;++iter) {
    TRKParticle* part = *iter;
    for (int i=0; i<trackingSteps; i++) {
      vector3 dv = part->mom().unit()*h;
      part->pluspos(dv);
    }
  }
}

void TRKThin::Track(TRKDipole* el, TRKBunch* bunch) { 
  /// from Sixtrack Physics Manual 3.2.2 Thin Dipole expanded Hamiltonian

  const double strength = el->GetStrength();
  if (std::abs(strength)<=1e-12) {
    return Track((TRKDrift*)el,bunch);
  }

  const double h = el->GetLength()/trackingSteps;

  for (int i=0; i<trackingSteps; i++) {
      
    TRKBunchIter iter = bunch->begin();
    TRKBunchIter end = bunch->end();
    
    // TODO how to use half? add option?
    // half drift
    Track((TRKDrift*)el,bunch);


    for (;iter!=end;++iter) {


    }
    // TODO how to use half? add option?
    // half drift
    Track((TRKDrift*)el,bunch);
  }
}

void TRKThin::Track(TRKQuadrupole* el, TRKBunch* bunch) { 
  const double strength = el->GetStrength();
  if (std::abs(strength)<=1e-12) {
    return Track((TRKDrift*)el,bunch);
  }

  const double h = el->GetLength()/trackingSteps;

  TRKBunchIter iter = bunch->begin();
  TRKBunchIter end = bunch->end();
  
  // initialise kick // only needed once
  const double Kn = strength;

  for (;iter!=end;++iter) {
    TRKParticle* part = *iter;
    // paraxial approximation ONLY!!
    if(iter== bunch->begin() && (std::abs(part->Zp())>0.99)&&(std::abs(strength)<1.e-6)) {
      TRKHybrid hybrid(trackingSteps);
      return hybrid.Track(el,bunch);
    }
    
    for (int i=0; i<trackingSteps; i++) {

      double x0 = part->X();
      double y0 = part->Y();
      double z0 = part->Z();
      double xp = part->Xp();
      double yp = part->Yp();
      double zp = part->Zp();

      // calculate particle kick
      std::complex<double> Kick = Kn * std::complex<double>(x0, y0);
      double kickxp = -real(Kick);
      double kickyp = +imag(Kick);

      // apply kick, 2 steps
      double xp_ = xp + kickxp * 0.5;
      double yp_ = yp + kickyp * 0.5;
    
      double vOut[6];

      vOut[0] = x0 + xp_ * h;
      vOut[1] = y0 + yp_ * h;
      vOut[2] = z0 + (0.5e-6 * (xp_*xp_ + yp_*yp_) * h); // to be checked

      // calculate particle kick
      Kick = Kn * std::complex<double>(vOut[0], vOut[1]);
      kickxp = -real(Kick);
      kickyp = +imag(Kick);

      vOut[3] = xp_ + kickxp * 0.5;
      vOut[4] = yp_ + kickyp * 0.5;
      vOut[5] = zp;
      *part=vector6(vOut);
    }
  }
}

void TRKThin::Track(TRKSextupole* el, TRKBunch* bunch) { 
  const double strength = el->GetStrength();
  if (std::abs(strength)<=1e-12) {
    return Track((TRKDrift*)el,bunch);
  }

  const double h = el->GetLength()/trackingSteps;

  TRKBunchIter iter = bunch->begin();
  TRKBunchIter end = bunch->end();
  
  // initialise kick // only needed once
  double Kn = strength * 0.5;

  for (;iter!=end;++iter) {
    TRKParticle* part = *iter;
    // paraxial approximation ONLY!!
    if(iter== bunch->begin() && (std::abs(part->Zp())>0.99)&&(std::abs(strength)<1.e-6)) {
      TRKHybrid hybrid(trackingSteps);
      return hybrid.Track(el,bunch);
    }
    
    for (int i=0; i<trackingSteps; i++) {

      double x0 = part->X();
      double y0 = part->Y();
      double z0 = part->Z();
      double xp = part->Xp();
      double yp = part->Yp();
      double zp = part->Zp();

      // adapted from PLACET, element_thin_lens

      // calculate particle kick
      std::complex<double> Kick = Kn * std::pow(std::complex<double>(x0, y0),2);
      double kickxp = -real(Kick);
      double kickyp = +imag(Kick);

      // apply kick, 2 steps
      double xp_ = xp + kickxp * 0.5;
      double yp_ = yp + kickyp * 0.5;

      double vOut[6];
      vOut[0] = x0 + xp_ * h;
      vOut[1] = y0 + yp_ * h;
      vOut[2] = z0 + (0.5e-6 * (xp_*xp_ + yp_*yp_) * h); // to be checked

      // calculate particle kick
      Kick = Kn * std::pow(std::complex<double>(vOut[0], vOut[1]),2);
      kickxp = -real(Kick);
      kickyp = +imag(Kick);

      vOut[3] = xp_ + kickxp * 0.5;
      vOut[4] = yp_ + kickyp * 0.5;
      vOut[5] = zp;
      *part=vector6(vOut);
    }
  }
}

void TRKThin::Track(TRKOctupole* el, TRKBunch* bunch) { 
  const double strength = el->GetStrength();
  if (std::abs(strength)<=1e-12) {
    return Track((TRKDrift*)el,bunch);
  }

  const double h = el->GetLength()/trackingSteps;

  TRKBunchIter iter = bunch->begin();
  TRKBunchIter end = bunch->end();
  
  // initialise kick // only needed once
  double Kn = strength / 6;

  for (;iter!=end;++iter) {
    TRKParticle* part = *iter;
    // paraxial approximation ONLY!!
    if(iter== bunch->begin() && (std::abs(part->Zp())>0.99)&&(std::abs(strength)<1.e-6)) {
      TRKHybrid hybrid(trackingSteps);
      return hybrid.Track(el,bunch);
    }
    
    for (int i=0; i<trackingSteps; i++) {

      double x0 = part->X();
      double y0 = part->Y();
      double z0 = part->Z();
      double xp = part->Xp();
      double yp = part->Yp();
      double zp = part->Zp();

      // adapted from PLACET, element_thin_lens

      // calculate particle kick
      std::complex<double> Kick = Kn * std::pow(std::complex<double>(x0, y0),3);
      double kickxp = -real(Kick);
      double kickyp = +imag(Kick);

      // apply kick, 2 steps
      double xp_ = xp + kickxp * 0.5;
      double yp_ = yp + kickyp * 0.5;
      
      double vOut[6];
      vOut[0] = x0 + xp_ * h;
      vOut[1] = y0 + yp_ * h;
      vOut[2] = z0 + (0.5e-6 * (xp_*xp_ + yp_*yp_) * h); // to be checked

      // calculate particle kick
      Kick = Kn * std::pow(std::complex<double>(vOut[0], vOut[1]),3);
      kickxp = -real(Kick);
      kickyp = +imag(Kick);

      vOut[3] = xp_ + kickxp * 0.5;
      vOut[4] = yp_ + kickyp * 0.5;
      vOut[5] = zp;
      *part=vector6(vOut);
    }
  }
}
