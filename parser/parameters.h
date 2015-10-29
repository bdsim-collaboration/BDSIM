#ifndef __PARAMETERS_H
#define __PARAMETERS_H

#include <list>
#include <string>
#include "element.h"

namespace GMAD {

  struct Array;

  /**
   * @brief Parameters - Element class with booleans
   * 
   * This is a temporary storage class of the parameters before the actual Element is created. 
   * The booleans are needed for inheritance and extension of already created Elements.
   * The class is a singleton.
   *
   * @author Jochem Snuverink <Jochem.Snuverink@rhul.ac.uk> (based on I. Agapov)
   */

  struct Parameters : public Element {

    ///@{ booleans for every Element member, except name and type, which can't be altered
    bool lset;
    bool ksset,k0set,k1set,k2set,k3set,k4set,angleset;
    bool beampipeThicknessset;
    bool aper1set, aper2set, aper3set, aper4set;
    bool apertureTypeset;
    bool beampipeMaterialset;
    bool magnetGeometryTypeset;
    bool outerMaterialset;
    bool outerDiameterset;
    bool tiltset,xsizeset,ysizeset,xsizeOutset,ysizeOutset,rset,Bset, phiAngleInset, phiAngleOutset;
    bool offsetXset, offsetYset;
    bool tscintset, twindowset, bmapZOffsetset; 
    bool xdirset, ydirset, zdirset, waveLengthset;
    bool gradientset;
    bool phiset, thetaset, psiset;
    bool knlset;
    bool kslset;
    bool blmLocZset;
    bool blmLocThetaset;
    bool biasset,biasMaterialset,biasVacuumset;
    bool precisionRegionset;
    
    bool Aset; 
    bool Zset; 
    bool densityset; 
    bool temperset;
    bool pressureset;
    bool stateset;
    bool symbolset;
    bool componentsset;
    bool componentsFractionsset;
    bool componentsWeightsset;

    bool geometryFileset;
    bool bmapFileset;
    bool materialset;
    bool windowmaterialset;
    bool scintmaterialset;
    bool airmaterialset;
    bool specset;

    bool numberWedgesset;
    bool wedgeLengthset;
    bool degraderHeightset;
    bool materialThicknessset;
    bool degraderOffsetset;

    //// reset the parameters to defaults
    void flush();

    /// copy parameters into temporary buffer params from element e
    /// parameters already set in params have priority and are not overridden
    void inherit_properties(struct Element& e);

    /// print multipole expansion array
    void print()const;

    /// set methods by property name, numeric values
    void set_value(std::string property, double value);
    /// set methods by property name, string values
    void set_value(std::string property, std::string value);
    /// set methods by property name, Array values
    void set_value(std::string property, Array* value);

    /// constructor
    Parameters();
  };
}

#endif
