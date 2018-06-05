#ifndef ELEMENT_H
#define ELEMENT_H

#include <iomanip>
#include <iostream>
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
    double scaling; ///< Overall scaling of field strength.
    double ks; ///< solenoid
    // double k0; // for dipole field B or angle is used
    double k1; ///< quadrupole
    double k2; ///< sextupole
    double k3; ///< octupole
    double k4; ///< decapole
    double angle; ///< bending angle
    double B; ///< magnetic field
    double e1; ///< input pole face rotation for bends
    double e2; ///< output pole face rotation for bends
    double fint; ///< fringe field integral at the dipole entrance
    double fintx;  ///< fringe field integral at the dipole exit
    double fintK2; ///< second fringe field integral at the dipole entrance - for TRANSPORT matching
    double fintxK2;///< second fringe field integral at the dipole exit - for TRANSPORT matching
    double hgap;   ///< half distance of pole separation for purposes of fringe fields - 'half gap'
    double h1; ///< input pole face curvature for bends
    double h2; ///< output pole face curvature for bends
    double kick;   ///< fractional delta p for either h or v kicker
    double hkick;  ///< fractional delta px for hkicker
    double vkick;  ///< fractional delta py for vkicker
    std::list<double> knl; ///< multipole expansion coefficients
    std::list<double> ksl; ///< skew multipole expansion
    double gradient;  ///< for rf cavities in V / m
    double E;         ///< electric field amplitude for rf cavities in V
    double frequency; ///< frequency for rf cavity in Hz
    double phase;     ///< phase of rf cavity (rad)
    double tOffset;   ///< time offset used for phase calculation (ns)

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
    int    hStyle;  ///< -1 = unset; 0 = false (ie c style); 1 = true, use hstyle
    double vhRatio; ///< ratio of vertial to horizontal for some magnets
    double coilWidthFraction; ///< Fraction of available h space the coil will take up.
    double coilHeightFraction; ///< Fraction of availalbe v space the coil will take up.
    double tilt;    ///< tilt
    double xsize, ysize; ///< collimator aperture or laser spotsize for laser
    double xsizeOut, ysizeOut; ///< collimator aperture or laser spotsize for laser
    double offsetX; ///< offset X
    double offsetY; ///< offset Y

    // screen parameters
    double tscint; ///<thickness of scintillating part of screen
    double twindow; ///<thickness of window
    double tmount; ///<thickness of the screen mount
    double windowScreenGap; ///< air gap between window and screen
    double screenXSize, screenYSize; ///< for screen
    ///@{ for screen
    std::list<double>      layerThicknesses;
    std::list<std::string> layerMaterials;
    std::list<int>         layerIsSampler;
    ///@}
    
    ///@{ for AWAKE spectrometer
    double screenPSize; ///<Phosphor particle size in screen
    double screenEndZ;
    double poleStartZ;
    double screenWidth;
    double awakeMagnetOffsetX;
    ///@}

    ///@{ for 3d transform and laser
    double xdir;
    double ydir;
    double zdir;
    ///@}
    double waveLength; ///< for laser wire and 3d transforms
    
    double phi, theta, psi; ///< for 3d transforms

    ///@{ for degrader
    int numberWedges;
    double wedgeLength;
    double degraderHeight;
    double materialThickness;
    double degraderOffset;
    ///@}

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
    
    std::string geometryFile; ///< for Element, file for external geometry
    std::string material;
    std::string windowmaterial; ///< for AWAKE
    std::string scintmaterial;  ///< for AWAKE
    std::string mountmaterial;  ///< for AWAKE spectrometer
    std::string spec;  ///< arbitrary specification to pass to beamline builder
    std::string cavityModel; ///< model for rf cavities

    /// Override colour for certain items
    std::string colour;
  
    /// Whether the angle was set. Unique as we may technically have 0 angle but a finite
    /// field. This allows us to distinguish later on.
    /// NOTE: this is not used in Params.
    bool   angleSet;

    /// in case the element is a list itself (line)
    std::list <Element> *lst;

    /// print method
    void print(int ident=0)const;

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
    /// Set methods by property name and value
    template <typename T>
      void set_value(std::string property, T value);
 
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

  template <typename T>
    void Element::set_value(std::string property, T value)
    {
#ifdef BDSDEBUG
      std::cout << "element> Setting value " << std::setw(25) << std::left << property << value << std::endl;
#endif
      // member method can throw runtime_error, catch and exit gracefully
      try {
        Published<Element>::set(this,property,value);
      }
      catch(std::runtime_error) {
        std::cerr << "Error: element> unknown option \"" << property << "\" with value " << value  << std::endl;
        exit(1);
      }
    }
}
 
#endif
