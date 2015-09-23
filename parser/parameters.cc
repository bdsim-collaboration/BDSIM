#include "parameters.h"

#include <cstdio>
#include <iomanip>
#include <iostream>
#include <list>
#include <string>

#include "array.h"
#include "element.h"

using namespace GMAD;

Parameters::Parameters():Element() {
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
}

void Parameters::inherit_properties(struct Element& e)
{
  // copy parameters into temporary buffer params from element e
  // parameters already set in params have priority and are not overridden
  // this is used for the inheritance / newinstance mechanism
  
  if(!lset) { l = e.l; lset = 1; }
  if(!bmapZOffsetset) { bmapZOffset = e.bmapZOffset; bmapZOffsetset = 1; }
  if(!Bset) { B = e.B; Bset = 1; }
  if(!ksset) { ks = e.ks; ksset = 1; }
  if(!k0set) { k0 = e.k0; k0set = 1; }
  if(!k1set) { k1 = e.k1; k1set = 1; }
  if(!k2set) { k2 = e.k2; k2set = 1; }
  if(!k3set) { k3 = e.k3; k3set = 1; }
  if(!k4set) { k4 = e.k4; k4set = 1; }
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

  // aperture model
  if(!beampipeThicknessset) { beampipeThickness = e.beampipeThickness; beampipeThicknessset = 1; }
  if(!aper1set) { aper1 = e.aper1; aper1set = 1;}
  if(!aper2set) { aper2 = e.aper2; aper2set = 1;}
  if(!aper3set) { aper3 = e.aper3; aper3set = 1;}
  if(!aper4set) { aper4 = e.aper4; aper4set = 1;}
  if(!apertureTypeset) { apertureType = e.apertureType; apertureTypeset = 1;}
  if(!beampipeMaterialset) { beampipeMaterial = e.beampipeMaterial; beampipeMaterialset = 1;}

  // magnet geometry
  if(!magnetGeometryTypeset) {magnetGeometryType = e.magnetGeometryType; magnetGeometryTypeset = 1;}
  if(!outerMaterialset)      {outerMaterial      = e.outerMaterial;      outerMaterialset = 1;}
  if(!outerDiameterset)      {outerDiameter = e.outerDiameter; outerDiameterset = 1;}

  if(!gradientset) { gradient = e.gradient; gradientset = 1; }

  if(!tiltset) { tilt = e.tilt; tiltset = 1; }
  if(!offsetXset) { offsetX = e.offsetX; offsetXset = 1; }
  if(!offsetYset) { offsetY = e.offsetY; offsetYset = 1; }
  if(!knlset) { knl = e.knl; knlset = 1; }
  if(!kslset) { ksl = e.ksl; kslset = 1; }
  // physics biasing
  if(!biasset) {bias = e.bias; biasset = 1; }
  //beam loss monitor locations
  if(!blmLocZset) { blmLocZ = e.blmLocZ; blmLocZset = 1; }
  if(!blmLocThetaset) { blmLocTheta = e.blmLocTheta; blmLocThetaset = 1; }

  if(!specset) { spec = e.spec; specset = 1; }
  if(!materialset) { material = e.spec; materialset = 1; }
  if(!scintmaterialset) { scintmaterial = e.spec; scintmaterialset = 1; }
  if(!windowmaterialset) { windowmaterial = e.spec; windowmaterialset = 1; }
  if(!airmaterialset) { airmaterial = e.spec; airmaterialset = 1; }
  if(!precisionRegionset) { precisionRegion = e.precisionRegion; precisionRegionset = 1; }

}

void Parameters::set_value(std::string property, double value )
{
#ifdef BDSDEBUG
  std::cout << "parser> Setting value " << std::setw(25) << std::left << property << value << std::endl;
#endif

  if(property=="l") { l = value; lset = 1; return;} // length
  if(property=="bmapZOffset") { bmapZOffset = value; bmapZOffsetset = 1; return;} // field map z offset
  if(property=="B") { B = value; Bset = 1; return;} // dipole field
  if(property=="ks") { ks = value; ksset = 1; return;} // solenoid strength
  if(property=="k0") { k0 = value; k0set = 1; return;} // dipole coef.
  if(property=="k1") { k1 = value; k1set = 1; return;} // quadrupole coef. 
  if(property=="k2") { k2 = value; k2set = 1; return;} // sextupole coef.
  if(property=="k3") { k3 = value; k3set = 1; return;} // octupole coef.
  if(property=="k4") { k4 = value; k4set = 1; return;} // decapole coef.
  if(property=="angle") { angle = value; angleset = 1; return;} // dipole bending angle
  if(property=="phiAngleIn") { phiAngleIn = value; phiAngleInset = 1; return;} // element incoming angle
  if(property=="phiAngleOut") { phiAngleOut = value; phiAngleOutset = 1; return;} // element outgoing angle
  if(property=="beampipeThickness")
    { beampipeThickness = value; beampipeThicknessset = 1; return;}
  
  if(property=="aper" ||property=="aperture")
    // for backwards compatibility
    { aper1 = value; aper1set = 1; return;}
  if(property=="aper1" ||property=="aperture1")  // new aperture model 
    { aper1 = value; aper1set = 1; return;}
  if(property=="aper2" ||property=="aperture2") 
    { aper2 = value; aper2set = 1; return;}
  if(property=="aper3" ||property=="aperture3") 
    { aper3 = value; aper3set = 1; return;}
  if(property=="aper4" ||property=="aperture4")
    { aper4 = value; aper4set = 1; return;}
  if(property=="outerDiameter") 
    { outerDiameter = value; outerDiameterset = 1; return;}
  if(property=="outR")
    // for backwards compatibility, boxSize = 2*outR
    { outerDiameter = 2 * value; outerDiameterset = 1; return;}
  if(property=="xsize") { xsize = value; xsizeset = 1; return;}
  if(property=="ysize") { ysize = value; ysizeset = 1; return;}
  if(property=="tilt") { tilt = value; tiltset = 1; return;}
  if(property=="offsetX") { offsetX = value; offsetXset = 1; return;}
  if(property=="offsetY") { offsetX = value; offsetYset = 1; return;}
  if(property=="x") {xdir = value; xdirset = 1; return;} // x direction
  if(property=="y") {ydir = value; ydirset = 1; return;} // y direction 
  if(property=="z") {zdir = value; zdirset = 1; return;} // z direction 
  if(property=="phi") {phi = value; phiset = 1; return;}  // polar angle
  if(property=="theta")  {theta = value; thetaset = 1; return;} 
  // azimuthal angle
  if(property=="psi")  {psi = value; psiset = 1; return;} // 3rd  angle
  if(property=="gradient")  {gradient = value; gradientset = 1; return;} // rf voltage
  if(property=="precisionRegion") { precisionRegion = (int)value; precisionRegionset = 1; return;}
  if(property=="A") {A = value; Aset = 1; return;}  // mass number
  if(property=="Z") {Z = value; Zset = 1; return;}  // atomic number
  if(property=="density") {density = value; densityset = 1; return;}  // density
  if(property=="T") {temper = value; temperset = 1; return;}  // temperature
  if(property=="P") {pressure = value; pressureset = 1; return;}  // pressure
  if(property=="waveLength") {waveLength = value; waveLengthset = 1; return;}
  if(property=="tscint") { tscint = value; tscintset = 1; return;} // thickness for a scintillator screen 
  if(property=="twindow") { twindow = value; twindowset = 1; return;} // thickness for a scintillator screen window 
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
      geometryFileset = 1;
      geometryFile = value;
      return;
    } 
  if(property=="bmap") 
    {
      bmapFileset = 1;
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
      outerMaterialset = 1;
      outerMaterial = value;
      return;
    }
  if(property=="material") 
    {	 
      materialset = 1;
      material = value;
      return;
    }
  if(property=="apertureType")
    {
      apertureTypeset = 1;
      apertureType = value;
      return;
    }
  if(property=="magnetGeometryType") 
    {
      magnetGeometryTypeset = 1;
      magnetGeometryType = value;
      return;
    }
  if(property=="beampipeMaterial")
    {
      beampipeMaterialset = 1;
      beampipeMaterial = value;
      return;
    }
  if(property=="scintmaterial") 
    {
      scintmaterialset = 1;
      scintmaterial = value; 
      return;
    } // material for a scintillator screen 
  if(property=="windowmaterial") 
    {
      windowmaterialset = 1;
      windowmaterial = value; 
      return;
    } // material for a scintillator screen window
  if(property=="airmaterial") 
    {
      airmaterialset = 1;
      airmaterial = value; 
      return;
    } // material for air around scintillator screen 
  if(property=="spec") 
    {
      specset = 1;
      spec = value;
      return;
    }
  if(property=="symbol")
    {
      symbolset = 1;
      symbol = value;
      return;
    }
  if(property=="state")
    {
      stateset = 1;
      state = value;
      return;
    }
  if(property=="bias")
    {
      biasset = 1;
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
      knlset = 1;
      value->set_vector(knl);
      value->data.clear();
      return;
    } 
  if(property=="ksl") 
    {
      kslset = 1;
      value->set_vector(ksl);
      value->data.clear();
      return;
    }
  if(property=="blmLocZ") 
    {
      blmLocZset = 1;
      value->set_vector(blmLocZ);
      value->data.clear();
      return;
    }
  if(property=="blmLocTheta") 
    {
      blmLocThetaset = 1;
      value->set_vector(blmLocTheta);
      value->data.clear();
      return;
    }
  if(property=="components")
    {
      componentsset = 1;
      value->set_vector(components);
      value->symbols.clear();
      return;
    } 
  if(property=="componentsWeights")
    {
      componentsWeightsset = 1;
      value->set_vector(componentsWeights);
      value->data.clear();
      return;
    }
  if(property=="componentsFractions")
    {
      componentsFractionsset = 1;
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
