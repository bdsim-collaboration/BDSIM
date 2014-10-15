#include "TRKThick.hh"

#include "TRKBunch.hh"
#include "TRKParticle.hh"
#include "TRKDrift.hh"
#include "TRKDipole.hh"
#include "TRKQuadrupole.hh"
#include "TRKSextupole.hh"
#include "TRKOctupole.hh"

TRKThick::TRKThick(int trackingStepsIn) : 
  TRKStrategy(trackingStepsIn) {
}

TRKThick::~TRKThick() { 
}

void TRKThick::Track(TRKDrift* el, TRKBunch* bunch) { 
  // this drift method can be done in one go, trackingsteps not needed;
  const double h = el->GetLength(); 
  TRKBunchIter iter = bunch->begin();
  TRKBunchIter end  = bunch->end();

  for (;iter!=end;++iter) {
    TRKParticle& part = *iter;
    vector3 dv = part.Mom().unit()*h;
    part.SetPosMom(dv+part.Pos(),part.Mom());    
  }
}

void TRKThick::Track(TRKDipole* el, TRKBunch* bunch) {
  std::cout << "WARNING - METHOD NOT IMPLEMENTED" << std::endl;
  return Track((TRKDrift*)el,bunch);
}

void TRKThick::Track(TRKBend* el, TRKBunch* bunch) { 
  std::cout << "WARNING - METHOD NOT IMPLEMENTED" << std::endl;
  return Track((TRKDrift*)el,bunch);
}

void TRKThick::Track(TRKQuadrupole* el, TRKBunch* bunch) { 
  double strength = el->GetStrength();
  if (std::abs(strength)<=1e-12) {
    return Track((TRKDrift*)el,bunch);
  }

  const double h = el->GetLength()/trackingSteps;

  TRKBunchIter iter = bunch->begin();
  TRKBunchIter end = bunch->end();
  
  for (;iter!=end;++iter) {
    TRKParticle& part = *iter;
    for (int i=0; i<trackingSteps; i++) {
      double x0 = part.X();
      double y0 = part.Y();
      double z0 = part.Z();
      double xp = part.Xp();
      double yp = part.Yp();
      double zp = part.Zp();

      vector6 vTemp(part.PosMom());
      // double charge, oh charge good point, should just add this to method signature
      double charge = 1 * TRK::e;

      double rigidity = std::abs(strength) * vTemp.Mom().mag() / charge; // to be checked
      double k = 1/std::sqrt(std::abs(rigidity));

      double c,s,ch,sh; 
      //  sincos(k*h,&s,&c);
      c = std::cos(k*h);
      s = std::sin(k*h);
      TRK::sincosh(k*h,sh,ch);

      double vOut[6];
      if (k>0) {
	vOut[0] = c * x0 + s * y0 / k;
	vOut[1] = c * y0 - s * x0 * k;
	vOut[3] = ch * xp + sh * yp / k;
	vOut[4] = ch * yp + sh * xp * k;
      } else {
	vOut[0] = ch * x0 + sh * y0 / k;
	vOut[1] = ch * y0 + sh * x0 * k;
	vOut[3] = c * xp + s * yp / k;
	vOut[4] = c * yp - s * xp * k;
      }
      vOut[2] = z0;
      vOut[5] = zp;
      part.SetPosMom(vector6(vOut));
    }
  }
}

void TRKThick::Track(TRKSextupole* el, TRKBunch* bunch) { 
  const double h = el->GetLength()/trackingSteps;

  TRKBunchIter iter = bunch->begin();
  TRKBunchIter end  = bunch->end();

  //TBC - justs copies coordinates on just now
  for (;iter!=end;++iter) {
    TRKParticle& part = *iter;
    for (int i=0; i<trackingSteps; i++) {
      part.SetPosMom(part.PosMom());
    }
  }
}

void TRKThick::Track(TRKOctupole* el, TRKBunch* bunch) {
  const double h = el->GetLength()/trackingSteps;

  TRKBunchIter iter = bunch->begin();
  TRKBunchIter end  = bunch->end();

  for (;iter!=end;++iter) {
    TRKParticle& part = *iter;
    for (int i=0; i<trackingSteps; i++) {
      part.SetPosMom(part.PosMom());
    }
  }
}
