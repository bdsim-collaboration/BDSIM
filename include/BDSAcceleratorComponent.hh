#ifndef __BDSACCELERATORCOMPONENT_H
#define __BDSACCELERATORCOMPONENT_H

#include "G4LogicalVolume.hh"
#include "globals.hh"          // geant4 globals / types

#include "BDSBeamPipeInfo.hh"
#include "BDSGeometryComponent.hh"
#include "BDSGlobalConstants.hh" 
#include "BDSTiltOffset.hh"

#include <vector>

class BDSAcceleratorComponent: public BDSGeometryComponent
{
public:
  /// Constructor - this is the minimum information needed to create a
  /// BDSAcceleratorComponent instance. Methods in the class will allow
  /// the derived class to associate the appropraite volumes to the members
  /// of BDSGeometryComponent - the base class.  The developer of a derived
  /// class should take care to set all members of BDSGeometryComponent in the
  /// derived class, including extents.
  /// Note, this class has arc length and chord length which are initially set
  /// to be the same, unless angle is != 0 in which case, the chord length is
  /// calculated from arc length
  BDSAcceleratorComponent(G4String         name,
			  G4double         arcLength,
			  G4double         angle,
			  G4String         type,
			  BDSTiltOffset    tiltOffset      = BDSTiltOffset(),
			  G4int            precisionRegion = 0,
			  BDSBeamPipeInfo* beamPipeInfo    = NULL);
  
  virtual ~BDSAcceleratorComponent();

  /// The name of the component without modification
  G4String GetName() const;

  /// Get a string describing the type of the component
  G4String GetType() const;

  /// 0 = no precision region, 1 = precision region 1, 2 = precision region 2.
  G4int GetPrecisionRegion() const;

  /// Get the angle the component induces in the reference trajector (rad). 
  /// Note, this is 0 for h and v kickers
  G4double GetAngle() const;

  /// Accessor for tilt offset information
  inline BDSTiltOffset GetTiltOffset() const;
  
  /// Access the length of the component.
  virtual G4double GetArcLength()   const; // note no z length - this is chord length
  virtual G4double GetChordLength() const; // only chord OR arc makes it explicit
  
  /// Access the read out geometry
  inline G4LogicalVolume* GetReadOutLogicalVolume() const;
  
  // in case a mapped field is provided creates a field mesh in global coordinates
  virtual void PrepareField(G4VPhysicalVolume *referenceVolume); 
  
  ///@{ Get parameter value from the specification string
  G4double GetParameterValue      (G4String spec, G4String name) const;
  G4String GetParameterValueString(G4String spec, G4String name) const;
  ///@}

  /// BDSComponentFactory creates BDSAcceleratorComponents
  friend class BDSComponentFactory;
  friend class BDSLine;
  
  // To be revisited
  G4double         GetSPos() const;
  void             SetSPos(G4double spos);
  void             SetGFlashVolumes(G4LogicalVolume* aLogVol);
  std::vector<G4LogicalVolume*> GetGFlashVolumes() const;
  void             SetMultiplePhysicalVolumes(G4VPhysicalVolume* aPhysVol);
  std::vector<G4VPhysicalVolume*> GetMultiplePhysicalVolumes() const;

protected:
  /// initialise method
  /// checks if marker logical volume already exists and builds new one if not
  // can't be in constructor as calls virtual methods
  virtual void Initialise();

  /// Attach marker solid and logical volume pointers to BDSGeometryComponent base class
  void RegisterMarkerWithBaseClass();
  
  /// build logical volumes: container, field, blms etc.
  virtual void Build();

  /// Build the container logical volume that all parts of the component will
  /// contained within - must be provided by derived class
  virtual void BuildContainerLogicalVolume() = 0;


  ///@{ Const protected member variable that may not be changed by derived classes
  const G4String   name;
  const G4double   arcLength;
  const G4String   type;
  ///@}
  
  ///@{ Protected member variable that can be modified by derived classes.
  G4double         chordLength;
  G4double         angle;
  BDSTiltOffset    tiltOffset;
  G4int            precisionRegion;
  BDSBeamPipeInfo* beamPipeInfo;
  ///@}

  /// Useful variables often used in construction
  static G4double    lengthSafety;
  static G4Material* emptyMaterial;
  
private:
  /// Private default constructor to force use of provided constructors, which
  /// ensure an object meets the requirements for the rest of the construction
  /// and placement machinery in BDSIM
  BDSAcceleratorComponent();

  /// Assignment and copy constructor not implemented nor used
  BDSAcceleratorComponent& operator=(const BDSAcceleratorComponent&);
  BDSAcceleratorComponent(BDSAcceleratorComponent&);
  
  G4LogicalVolume* readOutLV; ///< Read out geometry volume

  /// S coordinate - to be revisited / moved to BDSBeamlineElement
  G4double itsSPos;

  std::vector<G4LogicalVolume*> itsGFlashVolumes;
  //A vector containing the physical volumes in the accelerator component- to be used for geometric importance sampling etc.

  // to be moved to geometry component
  std::vector<G4VPhysicalVolume*> itsMultiplePhysicalVolumes;
};

inline G4String BDSAcceleratorComponent::GetName() const
{return name;}

inline G4double BDSAcceleratorComponent::GetChordLength() const
{return chordLength;}

inline G4double BDSAcceleratorComponent::GetArcLength() const
{return arcLength;}

inline G4double BDSAcceleratorComponent::GetAngle() const
{return angle;}

inline G4String BDSAcceleratorComponent::GetType() const
{return type;}

inline G4int BDSAcceleratorComponent::GetPrecisionRegion() const
{return precisionRegion;}

inline BDSTiltOffset BDSAcceleratorComponent::GetTiltOffset() const
{return tiltOffset;}

inline G4double BDSAcceleratorComponent::GetSPos() const
{return itsSPos;}

inline void BDSAcceleratorComponent::SetSPos(G4double spos)
{itsSPos=spos;}

inline void BDSAcceleratorComponent::SetGFlashVolumes(G4LogicalVolume* aLogVol)
{itsGFlashVolumes.push_back(aLogVol);}

inline std::vector<G4LogicalVolume*> BDSAcceleratorComponent::GetGFlashVolumes() const
{return itsGFlashVolumes;}

inline void BDSAcceleratorComponent::SetMultiplePhysicalVolumes(G4VPhysicalVolume* aPhysVol)
{itsMultiplePhysicalVolumes.push_back(aPhysVol);}

inline std::vector<G4VPhysicalVolume*> BDSAcceleratorComponent::GetMultiplePhysicalVolumes() const
{return itsMultiplePhysicalVolumes;}

inline G4LogicalVolume* BDSAcceleratorComponent::GetReadOutLogicalVolume() const
{return readOutLV;}

inline  G4double BDSAcceleratorComponent::GetParameterValue(G4String spec, G4String name) const
{
  G4double value = 0;

  std::string delimiters = "&";
  std::string param = name + "=";

  int pos = spec.find(param);
  if( pos >= 0 )
    {
      
      int pos2 = spec.find("&",pos);
      int pos3 = spec.length();
      int tend = pos2 < 0 ? pos3 : pos2; 
      int llen = tend - pos - param.length();
      
      std::string val = spec.substr(pos + param.length(), llen);
      
      value = atof(val.c_str());

  }
  return value;
}

inline  G4String BDSAcceleratorComponent::GetParameterValueString(G4String spec, G4String name) const
{
  G4String value = "";

  std::string delimiters = "&";
  std::string param = name + "=";

  int pos = spec.find(param);
  if( pos >= 0 )
    {
      int pos2 = spec.find("&",pos);
      int pos3 = spec.length();
      int tend = pos2 < 0 ? pos3 : pos2; 
      int llen = tend - pos - param.length();
      
      value = spec.substr(pos + param.length(), llen);
    }
  return value;
}

#endif
