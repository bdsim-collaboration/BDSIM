#include "parameters.h"

#include <cstdio>
#include <list>
#include <string>

#include "element.h"

Parameters::Parameters() {
  flush();
}

void Parameters::flush() {
  l=0; lset = 0;
  r = 0; rset = 0;
  tscint = 0.0003; tscintset = 0;
  twindow = 0.0; twindowset = 0;

  // materials' parameters
  A = 0; Aset = 0; //g*mol^-1
  Z = 0; Zset = 0; 
  density = 0; densityset = 0; //g*cm-3
  temper = 300; temperset = 0; //kelvin
  pressure = 1; pressureset = 0; //atm
  state = "solid"; stateset = 0; // "solid", "liquid", or "gas"
  symbol = ""; symbolset = 0;
  componentsset = 0; componentsFractionsset = 0; componentsWeightsset = 0;
  components.erase(components.begin(),components.end());
  componentsFractions.erase(componentsFractions.begin(),componentsFractions.end());
  componentsWeights.erase(componentsWeights.begin(),componentsWeights.end());

  flatlength = 0; flatlengthset = 0;
  taperlength = 0; taperlengthset = 0;
  angle = 0; angleset = 0;
  phiAngleIn = 0; phiAngleInset = 0;
  phiAngleOut = 0; phiAngleOutset = 0;
  xsize = 0; xsizeset = 0;
  ysize = 0; ysizeset = 0;
  hgap = 0; hgapset = 0;    
  xdir = 0; xdirset = 0;
  ydir = 0; ydirset = 0;
  zdir = 0; zdirset = 0; 
  waveLength = 0; waveLengthset = 0; 

  phi = 0; phiset = 0;
  theta = 0; thetaset = 0;
  psi = 0; psiset = 0;

  tunnelRadius = 0; tunnelRadiusset = 0;
  tunnelOffsetX = 0; tunnelOffsetXset = 0;

  precisionRegion = 0; precisionRegionset = 0;

  beampipeThickness = 0; beampipeThicknessset = 0;
  aper = 0; aperset = 0;
  outR = 0; outRset = 0;
  inR = 0; inRset = 0;
  bpRad = 0; bpRadset = 0;
  tilt = 0; tiltset = 0;

  aperX = 0; aperXset = 0;
  aperY = 0; aperYset = 0;
  aperYUp = 0; aperYUpset = 0;
  aperYDown = 0; aperYDownset = 0;
  aperDy=0; aperDyset = 0;

  B  = 0; Bset  = 0;
  k0 = 0; k0set = 0;
  k1 = 0; k1set = 0;
  k2 = 0; k2set = 0;
  k3 = 0; k3set = 0;
  ks = 0; ksset = 0;

  gradient = 0; gradientset = 0;
    
  knlset = 0; kslset=0;

  knl.erase(knl.begin(),knl.end());
  ksl.erase(ksl.begin(),ksl.end());

  //Beam loss monitor locations
  blmLocZset = 0;  blmLocThetaset = 0;
  blmLocZ.erase(blmLocZ.begin(), blmLocZ.end());
  blmLocTheta.erase(blmLocTheta.begin(), blmLocTheta.end());

  //precisionRegion
  precisionRegion = 0; precisionRegionset=0;

  geometry = "";  geomset = 0;

  bmap = ""; bmapset = 0;

  material = ""; materialset = 0;
  scintmaterial = ""; scintmaterialset = 0;
  windowmaterial = ""; windowmaterialset = 0;
  airmaterial = ""; airmaterialset = 0;
  tunnelMaterial = ""; tunnelmaterialset = 0;
  tunnelCavityMaterial = ""; tunnelcavitymaterialset = 0;
  spec = ""; specset = 0;
}

void Parameters::inherit_properties(struct Element& e)
{
  // copy parameters into temporary buffer params from element e
  // parameters already set in params have priority and are not overridden
  
  if(!lset) { l = e.l; lset = 1; }
  if(!Bset) { B = e.B; Bset = 1; }
  if(!ksset) { ks = e.ks; ksset = 1; }
  if(!k0set) { k0 = e.k0; k0set = 1; }
  if(!k1set) { k1 = e.k1; k1set = 1; }
  if(!k2set) { k2 = e.k2; k2set = 1; }
  if(!k3set) { k3 = e.k3; k3set = 1; }
  if(!angleset) { angle = e.angle; angleset = 1; }
  if(!phiAngleInset) { phiAngleIn = e.phiAngleIn; phiAngleInset = 1; }
  if(!phiAngleOutset) { phiAngleOut = e.phiAngleOut; phiAngleOutset = 1; }
  if(!xsizeset) { xsize = e.xsize; xsizeset = 1; }
  if(!ysizeset) { ysize = e.ysize; ysizeset = 1; }
 
  if(!xdirset) { xdir = e.xdir; xdirset = 1; }
  if(!ydirset) { ydir = e.ydir; ydirset = 1; }
  if(!zdirset) { zdir = e.zdir; zdirset = 1; }
  if(!waveLength) { waveLength = e.waveLength; waveLengthset = 1; }
  if(!phiset) { phi = e.phi; phiset = 1; }
  if(!psiset) { psi = e.psi; psiset = 1; }
  if(!thetaset) { theta = e.theta; thetaset = 1; }
  if(!hgapset) { hgap = e.hgap; hgapset = 1; }
  if(!flatlengthset) { flatlength = e.flatlength; flatlengthset = 1; }
  if(!taperlengthset) { taperlength = e.taperlength; taperlengthset = 1; }

  if(!tscintset) { tscint = e.tscint; tscintset = 1; }
  if(!twindowset) { twindow = e.twindow; twindowset = 1; }

  //materials
  if(!Aset) { A = e.A; Aset = 1; }
  if(!Zset) { Z = e.Z; Zset = 1; }
  if(!densityset) { density = e.density; densityset = 1; }
  if(!temperset) { temper = e.temper; temperset = 1; }
  if(!pressureset) { pressure = e.pressure; pressureset = 1; }
  if(!stateset) { state = e.state; stateset = 1; }
  if(!symbolset) { symbol = e.symbol; symbolset = 1; }
  if(!componentsset) 
    { components = e.components; componentsset = 1; }
  if(!componentsWeightsset) 
    { componentsWeights = e.componentsWeights; componentsWeightsset = 1; }
  if(!componentsFractionsset) 
    { componentsFractions = e.componentsFractions; componentsFractionsset = 1; }

  if(!beampipeThicknessset) { beampipeThickness = e.beampipeThickness; beampipeThicknessset = 1; }
  if(!aperset) { aper = e.aper; aperset = 1; }
  if(!aperXset) { aperX = e.aperX; aperXset = 1; }
  if(!aperYset) { aperY = e.aperY; aperYset = 1; }
  if(!aperYUpset) { aperYUp = e.aperYUp; aperYUpset = 1; }
  if(!aperYDownset) { aperYDown = e.aperYDown; aperYDownset = 1; }
  if(!aperDyset) { aperDy = e.aperDy; aperDyset = 1; }
  if(!inRset) { inR = e.inR; inRset = 1; }
  if(!bpRadset) { bpRad = e.bpRad; bpRadset = 1; }
  if(!outRset) { outR = e.outR; outRset = 1; }

  if(!gradientset) { gradient = e.gradient; gradientset = 1; }

  if(!tiltset) { tilt = e.tilt; tiltset = 1; }
  if(!knlset) { knl = e.knl; knlset = 1; }
  if(!kslset) { ksl = e.ksl; kslset = 1; }
  //beam loss monitor locations
  if(!blmLocZset) { blmLocZ = e.blmLocZ; blmLocZset = 1; }
  if(!blmLocThetaset) { blmLocTheta = e.blmLocTheta; blmLocThetaset = 1; }

  if(!specset) { spec = e.spec; specset = 1; }
  if(!materialset) { material = e.spec; materialset = 1; }
  if(!scintmaterialset) { scintmaterial = e.spec; scintmaterialset = 1; }
  if(!windowmaterialset) { windowmaterial = e.spec; windowmaterialset = 1; }
  if(!airmaterialset) { airmaterial = e.spec; airmaterialset = 1; }
  if(!tunnelmaterialset) { tunnelMaterial = e.spec; tunnelmaterialset = 1; }
  if(!tunnelcavitymaterialset) { tunnelCavityMaterial = e.spec; tunnelcavitymaterialset = 1; }
  if(!tunnelRadiusset) { tunnelRadius = e.tunnelRadius; tunnelRadiusset = 1; }
  if(!tunnelOffsetXset) { tunnelOffsetX = e.tunnelOffsetX; tunnelOffsetXset = 1; }
  if(!precisionRegionset) { precisionRegion = e.precisionRegion; precisionRegionset = 1; }

}

void Parameters::print()const{
  printf("printing parameters:\n");
  std::list<double>::const_iterator it;
  for(it = knl.begin();it!=knl.end();++it)
    printf(" %f ", (*it));
  printf("\n");
}
