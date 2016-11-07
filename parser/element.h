#ifndef ELEMENT_H
#define ELEMENT_H

#include <list>
#include <map>
#include <string>

#include "published.h"

namespace GMAD
{
  enum class ElementType;

  struct Parameters;
 
  /**
   * @brief Element class
   * 
   * @author I. Agapov
   */

  struct Element : public Published<Element>{
    ElementType type; ///< element enum
    std::string name;

    double l; ///< length in metres
    double ks; ///< solenoid
    double k0; ///< dipole
    double k1; ///< quadrupole
    double k2; ///< sextupole
    double k3; ///< octupole
    double k4; ///< decapole
    double angle; ///< bending angle

    ///@{ beampipe information, new aperture model
    double beampipeThickness;
    double aper1;
    double aper2;
    double aper3;
    double aper4;
    std::string apertureType;
    std::string beampipeMaterial;
    std::string vacuumMaterial;
    ///@}
  
    // magnet geometry
    std::string magnetGeometryType;
    std::string outerMaterial;
    double outerDiameter;
    bool   yokeOnInside;

    double tilt; ///< tilt
    double xsize, ysize; ///< collimator aperture or laser spotsize for laser
    double xsizeOut, ysizeOut; ///< collimator aperture or laser spotsize for laser
    double B; ///< magnetic field
    double e1; ///< input pole face rotation for bends
    double e2; ///< output pole face rotation for bends
    double fint; ///< fringe field integral at the dipole entrance
    double fintx;  ///< fringe field integral at the dipole exit
    double offsetX; ///< offset X
    double offsetY; ///< offset Y
    double tscint; ///<thickness of scintillating part of screen
    double twindow; ///<thickness of window
    double tmount; ///<thickness of the screen mount
    double windowScreenGap; ///< air gap between window and screen
    double screenXSize, screenYSize;
    double screenPSize; //Phosphor particle size in screen
    double screenEndZ;
    double poleStartZ;
    double screenWidth;
    std::list<double> layerThicknesses;
    std::list<std::string> layerMaterials;
    std::list<int> layerIsSampler;
    double xdir;
    double ydir;
    double zdir;
    double waveLength; ///< for laser wire and 3d transforms
    double gradient; ///< for rf cavities
    double phi, theta, psi; ///< for 3d transforms
    int numberWedges; ///< for degrader
    double wedgeLength; ///< for degrader
    double degraderHeight; ///< for degrader
    double materialThickness; ///< for degrader
    double degraderOffset; ///< for degrader

    std::list<double> knl; ///< multipole expansion coefficients
    std::list<double> ksl; ///< skew multipole expansion

    ///@{List of beam loss monitor locations
    std::list<double> blmLocZ;
    std::list<double> blmLocTheta;
    ///@}
  
    ///@{ temporary string for bias setting
    std::string bias;
    std::string biasMaterial;
    std::string biasVacuum;
    ///@}
    /// physics biasing list for the material
    std::list<std::string> biasMaterialList;
    /// physics biasing list for the vacuum
    std::list<std::string> biasVacuumList;

    std::string samplerName; ///< name of sampler (default empty)
    std::string samplerType; ///< element has a sampler of this type (default "none")
    double samplerRadius; ///< radius for cylindrical sampler
    
    std::string region;      ///< region with range cuts
    std::string fieldOuter;  ///< Outer field.
    std::string fieldVacuum; ///< Vacuum field.
    std::string fieldAll;    ///< Field for everything.
    
    std::string geometryFile;
    std::string material;
    std::string windowmaterial;
    std::string scintmaterial;
    std::string mountmaterial;
    std::string airmaterial;
    std::string spec;  ///< arbitrary specification to pass to beamline builder
    std::string cavityModel; ///< model for rf cavities
  
    /// in case the element is a list itself (line)
    std::list <Element> *lst;

    /// print method
    void print(int &ident)const;

    /// flush method
    void flush();

    /// check if element is of a special type
    bool isSpecial()const;
    /// property lookup by name (slow method)
    /// only for properties with type int/double!
    double property_lookup(std::string property_name)const;

    /// set sampler info
    void setSamplerInfo(std::string samplerType, std::string samplerName, double samplerRadius);

    ///@{ set method from Parameters structure
    void set(const Parameters& params);
    void set(const Parameters& params,std::string nameIn, ElementType typeIn);
    ///@}

    /// Override colour for certain items
    std::string colour;
  
    /// constructor
    Element();

  private:
    /// publish members so these can be looked up from parser
    void PublishMembers();
    /// map that translates between alternative parser names for members, could be made static
    std::map<std::string,std::string> alternativeNames;

  protected:
    /// returns 'official' member name for property
    std::string getPublishedName(std::string name)const;
  };
}
 
#endif
