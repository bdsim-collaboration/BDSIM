#include "parameters.h"

#include <cstdio>
#include <iomanip>
#include <iostream>
#include <list>
#include <string>

#include "array.h"
#include "element.h"

using namespace GMAD;

Parameters::Parameters() {
  flush();
}

void Parameters::flush() {
  Element::flush();
  lset = false;
  ksset = false;
  k0set = false;
  k1set = false;
  k2set = false;
  k3set = false;
  k4set = false;
  angleset = false;
  beampipeThicknessset = false;
  aper1set = false;
  aper2set = false;
  aper3set = false;
  aper4set = false;
  apertureTypeset = false;
  beampipeMaterialset = false;
  magnetGeometryTypeset = false;
  outerMaterialset      = false;
  outerDiameterset      = false;
  tiltset = false;
  xsizeset = false;
  ysizeset = false;
  rset = false;
  Bset  = false;
  phiAngleInset = false;
  phiAngleOutset = false;
  offsetXset = false;
  offsetYset = false;
  tscintset = false;
  twindowset = false;
  bmapZOffsetset=false;
  xdirset = false;
  ydirset = false;
  zdirset = false; 
  waveLengthset = false; 
  gradientset = false;
  phiset = false;
  thetaset = false;
  psiset = false;
  knlset = false, kslset=false;
  blmLocZset = false;  blmLocThetaset = false;
  biasset = false;
  precisionRegionset = false;

  Aset = false;
  Zset = false;
  densityset = false;
  temperset = false;
  pressureset = false;
  stateset = false;
  symbolset = false;
  componentsset = false; componentsFractionsset = false; componentsWeightsset = false;
  
  geometryFileset = false;
  bmapFileset = false;
  materialset = false;
  scintmaterialset = false;
  windowmaterialset = false;
  airmaterialset = false;
  specset = false;
  
  numberWedgesset = false;
  wedgeLengthset= false;
  degraderHeightset = false;
  materialThicknessset = false;
  degraderOffsetset = false;
}

void Parameters::inherit_properties(struct Element& e)
{
  // copy parameters into temporary buffer params from element e
  // parameters already set in params have priority and are not overridden
  // this is used for the inheritance / newinstance mechanism
  
  if(!lset) { l = e.l; lset = true; }
  if(!ksset) { ks = e.ks; ksset = true; }
  if(!k0set) { k0 = e.k0; k0set = true; }
  if(!k1set) { k1 = e.k1; k1set = true; }
  if(!k2set) { k2 = e.k2; k2set = true; }
  if(!k3set) { k3 = e.k3; k3set = true; }
  if(!k4set) { k4 = e.k4; k4set = true; }
  if(!angleset) { angle = e.angle; angleset = true; }
  // aperture model
  if(!beampipeThicknessset) { beampipeThickness = e.beampipeThickness; beampipeThicknessset = true; }
  if(!aper1set) { aper1 = e.aper1; aper1set = true;}
  if(!aper2set) { aper2 = e.aper2; aper2set = true;}
  if(!aper3set) { aper3 = e.aper3; aper3set = true;}
  if(!aper4set) { aper4 = e.aper4; aper4set = true;}
  if(!apertureTypeset) { apertureType = e.apertureType; apertureTypeset = true;}
  if(!beampipeMaterialset) { beampipeMaterial = e.beampipeMaterial; beampipeMaterialset = true;}

  // magnet geometry
  if(!magnetGeometryTypeset) {magnetGeometryType = e.magnetGeometryType; magnetGeometryTypeset = true;}
  if(!outerMaterialset)      {outerMaterial      = e.outerMaterial;      outerMaterialset = true;}
  if(!outerDiameterset)      {outerDiameter = e.outerDiameter; outerDiameterset = true;}

  if(!tiltset) { tilt = e.tilt; tiltset = true; }
  if(!xsizeset) { xsize = e.xsize; xsizeset = true; }
  if(!ysizeset) { ysize = e.ysize; ysizeset = true; }
  if(!rset) { r = e.r; rset = true; }
  if(!Bset) { B = e.B; Bset = true; }
  if(!phiAngleInset) { phiAngleIn = e.phiAngleIn; phiAngleInset = true; }
  if(!phiAngleOutset) { phiAngleOut = e.phiAngleOut; phiAngleOutset = true; }
  if(!offsetXset) { offsetX = e.offsetX; offsetXset = true; }
  if(!offsetYset) { offsetY = e.offsetY; offsetYset = true; }
  if(!tscintset) { tscint = e.tscint; tscintset = true; }
  if(!twindowset) { twindow = e.twindow; twindowset = true; }
  if(!bmapZOffsetset) { bmapZOffset = e.bmapZOffset; bmapZOffsetset = true; }
  if(!xdirset) { xdir = e.xdir; xdirset = true; }
  if(!ydirset) { ydir = e.ydir; ydirset = true; }
  if(!zdirset) { zdir = e.zdir; zdirset = true; }
  if(!waveLength) { waveLength = e.waveLength; waveLengthset = true; }
  if(!gradientset) { gradient = e.gradient; gradientset = true; }
  if(!phiset) { phi = e.phi; phiset = true; }
  if(!psiset) { psi = e.psi; psiset = true; }
  if(!thetaset) { theta = e.theta; thetaset = true; }
  if(!numberWedgesset) { numberWedges = e.numberWedges; numberWedgesset = true; }
  if(!wedgeLengthset) { wedgeLength = e.wedgeLength; wedgeLengthset = true; }
  if(!degraderHeightset) {degraderHeight = e.degraderHeight; degraderHeightset = true; }
  if(!materialThicknessset) {materialThickness = e.materialThickness; materialThicknessset = true; }
  if(!degraderOffsetset) {degraderOffset = e.degraderOffset; degraderOffsetset = true; }

  if(!knlset) { knl = e.knl; knlset = true; }
  if(!kslset) { ksl = e.ksl; kslset = true; }
  //beam loss monitor locations
  if(!blmLocZset) { blmLocZ = e.blmLocZ; blmLocZset = true; }
  if(!blmLocThetaset) { blmLocTheta = e.blmLocTheta; blmLocThetaset = true; }

  // physics biasing
  if(!biasset) {bias = e.bias; biasset = true; }
  if(!precisionRegionset) { precisionRegion = e.precisionRegion; precisionRegionset = true; }
  //materials
  if(!Aset) { A = e.A; Aset = true; }
  if(!Zset) { Z = e.Z; Zset = true; }
  if(!densityset) { density = e.density; densityset = true; }
  if(!temperset) { temper = e.temper; temperset = true; }
  if(!pressureset) { pressure = e.pressure; pressureset = true; }
  if(!stateset) { state = e.state; stateset = true; }
  if(!symbolset) { symbol = e.symbol; symbolset = true; }
  if(!componentsset) 
    { components = e.components; componentsset = true; }
  if(!componentsFractionsset) 
    { componentsFractions = e.componentsFractions; componentsFractionsset = true; }
  if(!componentsWeightsset) 
    { componentsWeights = e.componentsWeights; componentsWeightsset = true; }

  if(!geometryFileset) { geometryFile = e.geometryFile; geometryFileset = true; }
  if(!bmapFileset) { bmapFile = e.bmapFile; bmapFileset = true; }
  if(!materialset) { material = e.material; materialset = true; }
  if(!windowmaterialset) { windowmaterial = e.windowmaterial; windowmaterialset = true; }
  if(!scintmaterialset) { scintmaterial = e.scintmaterial; scintmaterialset = true; }
  if(!airmaterialset) { airmaterial = e.airmaterial; airmaterialset = true; }
  if(!specset) { spec = e.spec; specset = true; }
}

void Parameters::set_value(std::string property, double value )
{
#ifdef BDSDEBUG
  std::cout << "parser> Setting value " << std::setw(25) << std::left << property << value << std::endl;
#endif

  if(property=="l") { l = value; lset = true; return;} // length
  if(property=="bmapZOffset") { bmapZOffset = value; bmapZOffsetset = true; return;} // field map z offset
  if(property=="B") { B = value; Bset = true; return;} // dipole field
  if(property=="ks") { ks = value; ksset = true; return;} // solenoid strength
  if(property=="k0") { k0 = value; k0set = true; return;} // dipole coef.
  if(property=="k1") { k1 = value; k1set = true; return;} // quadrupole coef. 
  if(property=="k2") { k2 = value; k2set = true; return;} // sextupole coef.
  if(property=="k3") { k3 = value; k3set = true; return;} // octupole coef.
  if(property=="k4") { k4 = value; k4set = true; return;} // decapole coef.
  if(property=="angle") { angle = value; angleset = true; return;} // dipole bending angle
  if(property=="phiAngleIn") { phiAngleIn = value; phiAngleInset = true; return;} // element incoming angle
  if(property=="phiAngleOut") { phiAngleOut = value; phiAngleOutset = true; return;} // element outgoing angle
  if(property=="beampipeThickness")
    { beampipeThickness = value; beampipeThicknessset = true; return;}
  
  if(property=="aper" ||property=="aperture")
    // for backwards compatibility
    { aper1 = value; aper1set = true; return;}
  if(property=="aper1" ||property=="aperture1")  // new aperture model 
    { aper1 = value; aper1set = true; return;}
  if(property=="aper2" ||property=="aperture2") 
    { aper2 = value; aper2set = true; return;}
  if(property=="aper3" ||property=="aperture3") 
    { aper3 = value; aper3set = true; return;}
  if(property=="aper4" ||property=="aperture4")
    { aper4 = value; aper4set = true; return;}
  if(property=="outerDiameter") 
    { outerDiameter = value; outerDiameterset = true; return;}
  if(property=="outR")
    // for backwards compatibility, boxSize = 2*outR
    { outerDiameter = 2 * value; outerDiameterset = true; return;}
  if(property=="xsize") { xsize = value; xsizeset = true; return;}
  if(property=="ysize") { ysize = value; ysizeset = true; return;}
  if(property=="tilt") { tilt = value; tiltset = true; return;}
  if(property=="offsetX") { offsetX = value; offsetXset = true; return;}
  if(property=="offsetY") { offsetX = value; offsetYset = true; return;}
  if(property=="x") {xdir = value; xdirset = true; return;} // x direction
  if(property=="y") {ydir = value; ydirset = true; return;} // y direction 
  if(property=="z") {zdir = value; zdirset = true; return;} // z direction 
  if(property=="phi") {phi = value; phiset = true; return;}  // polar angle
  if(property=="theta")  {theta = value; thetaset = true; return;} 
  // azimuthal angle
  if(property=="psi")  {psi = value; psiset = true; return;} // 3rd  angle
  if(property=="gradient")  {gradient = value; gradientset = true; return;} // rf voltage
  if(property=="precisionRegion") { precisionRegion = (int)value; precisionRegionset = true; return;}
  if(property=="A") {A = value; Aset = true; return;}  // mass number
  if(property=="Z") {Z = value; Zset = true; return;}  // atomic number
  if(property=="density") {density = value; densityset = true; return;}  // density
  if(property=="T") {temper = value; temperset = true; return;}  // temperature
  if(property=="P") {pressure = value; pressureset = true; return;}  // pressure
  if(property=="waveLength") {waveLength = value; waveLengthset = true; return;}
  if(property=="tscint") { tscint = value; tscintset = true; return;} // thickness for a scintillator screen 
  if(property=="twindow") { twindow = value; twindowset = true; return;} // thickness for a scintillator screen window 

  if(property=="numberWedges")      {numberWedges = (int)value; numberWedgesset = true; return;} // number of degrader wedges
  if(property=="wedgeLength")       {wedgeLength = value; wedgeLengthset = true; return;} // degrader wedge height
  if(property=="degraderHeight")    {degraderHeight = value; degraderHeightset = true; return;} // degrader element height
  if(property=="materialThickness") {materialThickness = value; materialThicknessset = true; return;} // degrader thickness
  if(property=="degraderOffset")    {degraderOffset = value; degraderOffsetset = true; return;} // degrader thickness
  // not implemented mad parameters will be ignored
  if(property=="e1") {return;}  // fringe field parameters
  if(property=="e2") {return;}
  if(property=="fint") {return;}
  if(property=="fintx") {return;}
  if(property=="hgap") {return;}
  if(property=="harmon") {return;} // rf cavity
  if(property=="lag") {return;}
  if(property=="volt") {return;}

  std::cerr << "Error: parser> unknown parameter option \"" << property << "\" with value " << value  << std::endl << std::endl;
  exit(1);
}

void Parameters::set_value(std::string property, std::string value )
{
#ifdef BDSDEBUG
  std::cout << "parser> Setting value " << std::setw(25) << std::left << property << value << std::endl;
#endif

  if(property=="geometry") 
    {
      geometryFileset = true;
      geometryFile = value;
      return;
    } 
  if(property=="bmap") 
    {
      bmapFileset = true;
      bmapFile = value;
      return;
    }
  if(property=="type") 
    {
      printf("Warning : type parameter is currently ignored\n");
      //ignore the "type attribute for the moment"
      return;
    }
  if(property=="outerMaterial") 
    {
      outerMaterialset = true;
      outerMaterial = value;
      return;
    }
  if(property=="material") 
    {	 
      materialset = true;
      material = value;
      return;
    }
  if(property=="apertureType")
    {
      apertureTypeset = true;
      apertureType = value;
      return;
    }
  if(property=="magnetGeometryType") 
    {
      magnetGeometryTypeset = true;
      magnetGeometryType = value;
      return;
    }
  if(property=="beampipeMaterial")
    {
      beampipeMaterialset = true;
      beampipeMaterial = value;
      return;
    }
  if(property=="scintmaterial") 
    {
      scintmaterialset = true;
      scintmaterial = value; 
      return;
    } // material for a scintillator screen 
  if(property=="windowmaterial") 
    {
      windowmaterialset = true;
      windowmaterial = value; 
      return;
    } // material for a scintillator screen window
  if(property=="airmaterial") 
    {
      airmaterialset = true;
      airmaterial = value; 
      return;
    } // material for air around scintillator screen 
  if(property=="spec") 
    {
      specset = true;
      spec = value;
      return;
    }
  if(property=="symbol")
    {
      symbolset = true;
      symbol = value;
      return;
    }
  if(property=="state")
    {
      stateset = true;
      state = value;
      return;
    }
  if(property=="bias")
    {
      biasset = true;
      bias = value;
      return;
    }
  
  std::cerr << "Error: parser> unknown parameter option \"" << property << "\" with value " << value  << std::endl;
  exit(1);
}

void Parameters::set_value(std::string property, Array* value)
{
#ifdef BDSDEBUG
  std::cout << "parser> Setting value " << std::setw(25) << std::left << property << std::endl;
#endif
  if(property=="knl") 
    {
      knlset = true;
      value->set_vector(knl);
      value->data.clear();
      return;
    } 
  if(property=="ksl") 
    {
      kslset = true;
      value->set_vector(ksl);
      value->data.clear();
      return;
    }
  if(property=="blmLocZ") 
    {
      blmLocZset = true;
      value->set_vector(blmLocZ);
      value->data.clear();
      return;
    }
  if(property=="blmLocTheta") 
    {
      blmLocThetaset = true;
      value->set_vector(blmLocTheta);
      value->data.clear();
      return;
    }
  if(property=="components")
    {
      componentsset = true;
      value->set_vector(components);
      value->symbols.clear();
      return;
    } 
  if(property=="componentsWeights")
    {
      componentsWeightsset = true;
      value->set_vector(componentsWeights);
      value->data.clear();
      return;
    }
  if(property=="componentsFractions")
    {
      componentsFractionsset = true;
      value->set_vector(componentsFractions);
      value->data.clear();
      return;
    }

  std::cerr << "Error: parser> unknown parameter option \"" << property << "\", or doesn't expect vector type" << std::endl;
  exit(1);
}

void Parameters::print()const{
  printf("printing parameters:\n");
  std::list<double>::const_iterator it;
  for(it = knl.begin();it!=knl.end();++it)
    printf(" %f ", (*it));
  printf("\n");
}
