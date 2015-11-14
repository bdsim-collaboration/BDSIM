#ifndef BDSACCELERATORCOMPONENT_H
#define BDSACCELERATORCOMPONENT_H

#include "G4LogicalVolume.hh"
#include "globals.hh"          // geant4 globals / types

#include "BDSBeamPipeInfo.hh"
#include "BDSGeometryComponent.hh"
#include "BDSGlobalConstants.hh" 

#include <list>
#include <string>
#include <vector>

/**
 * @brief Abstract class that represents a component of an accelerator.
 *
 * It must be constructed with a name, length (arc), angle it
 * induces (x,z plane in the local coordinates of the component) in 
 * the reference trajectory and a string
 * representing its type. The class has no concept of its position
 * in the beamline or in global coordinates. This information is contained
 * in an instance of BDSBeamlineElement.
 * 
 * This is an abstract class as the derived class must provide the 
 * implementation of BuildContainerLogicalVolume() that constructs
 * the basic container. This is the minimum required so that an instance
 * of the derived class will operate with the rest of the placement machinery in
 * BDSIM. Typically, a derived class overrides the Build() function as well.
 * 
 * The class provides deferred construction through the Initialise() function
 * to allow two stage construction if it's required.
 * 
 * Note, the geometry of any derived component should be nominally constructed
 * along local z axis (beam direction) and x,y are transverse dimensions in a 
 * right-handed coordinate system.
 * 
 * This was significantly reworked in version 0.7 from the original. The indicator
 * author is the maintainer of the new version.
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

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
			  G4bool           precisionRegion = 0,
			  BDSBeamPipeInfo* beamPipeInfo    = nullptr);
  
  virtual ~BDSAcceleratorComponent();

  /// The name of the component without modification
  G4String GetName() const;

  /// Get a string describing the type of the component
  G4String GetType() const;

  /// Whether precision output is to be recorded for this component
  G4bool GetPrecisionRegion() const;

  /// Set whether precision output should be recorded for this component
  void   SetPrecisionRegion(G4bool precisionRegionIn);

  /// Access beam pipe information
  BDSBeamPipeInfo* GetBeamPipeInfo() const;

  /// Get the angle the component induces in the reference trajector (rad). 
  /// Note, this is 0 for h and v kickers
  G4double GetAngle() const;
  
  /// Access the length of the component.
  virtual G4double GetArcLength()   const; // note no z length - this is chord length
  virtual G4double GetChordLength() const; // only chord OR arc makes it explicit
  
  /// Access the read out geometry
  inline G4LogicalVolume* GetReadOutLogicalVolume() const;

  /// Access the vacuum volume the main beam goes through in this component if any. Default is
  /// nullptr.
  inline G4LogicalVolume* GetAcceleratorVacuumLogicalVolume() const {return acceleratorVacuumLV;}
  
  // in case a mapped field is provided creates a field mesh in global coordinates
  virtual void PrepareField(G4VPhysicalVolume *referenceVolume); 
  
  ///@{ Get parameter value from the specification ('spec') string
  G4double GetParameterValue      (G4String spec, G4String name) const;
  G4String GetParameterValueString(G4String spec, G4String name) const;
  ///@}

  ///@{ This function should be revisited given recent changes (v0.7)
  void             SetGFlashVolumes(G4LogicalVolume* aLogVol);
  std::vector<G4LogicalVolume*> GetGFlashVolumes() const;
  ///@}

  /// Increment (+1) the number of times this component has been copied.
  void  IncrementCopyNumber();

  /// Get the number of times this component has been copied.
  G4int GetCopyNumber()const;

  /// initialise method
  /// checks if marker logical volume already exists and builds new one if not
  /// can't be in constructor as calls virtual methods
  virtual void Initialise();

  /// Copy the bias list to this element
  void SetBiasVacuumList(std::list<std::string> biasVacuumListIn);
  void SetBiasMaterialList(std::list<std::string> biasMaterialListIn);

  /// Access the bias list copied from parser
  std::list<std::string> GetBiasVacuumList();
  std::list<std::string> GetBiasMaterialList();
  
protected:
  /// Build the container only. Should be overridden by derived class to add more geometry
  /// apart from the container volume. The overridden Build() function can however, call
  /// make use of this function to call BuildContainerLogicalVolume() by calling
  /// BDSAcceleratorComponent::Build() at the beginning.
  virtual void Build();

  /// Build the container solid and logical volume that all parts of the component will
  /// contained within - must be provided by derived class.
  virtual void BuildContainerLogicalVolume() = 0;

  /// Assign the accelerator tracking volume - only callable by derived classes - ie not public.
  /// This is just setting a reference to the accelerator volume and it is not deleted by
  /// this class (BDSAcceleratorComponent) - therefore, the derived class should also deal with
  /// memory management of this volume - whether this is by using the inherited (from BDSGeometryComponent)
  /// RegisterLogicalVolume() or by manually deleting itself.
  inline void SetAcceleratorVacuumLogicalVolume(G4LogicalVolume* accVacLVIn) {acceleratorVacuumLV = accVacLVIn;}
  
  ///@{ Const protected member variable that may not be changed by derived classes
  const G4String   name;
  const G4double   arcLength;
  const G4String   type;
  ///@}
  
  ///@{ Protected member variable that can be modified by derived classes.
  G4double         chordLength;
  G4double         angle;
  G4bool           precisionRegion;
  BDSBeamPipeInfo* beamPipeInfo;
  ///@}

  /// Useful variables often used in construction
  static G4double    lengthSafety;
  static G4Material* emptyMaterial;
  static G4bool      checkOverlaps;

  /// Read out geometry volume. Protected so derived classes can fiddle if they require.
  /// This is a possibility as derived classes can override Initialise which calls the
  /// BuildReadOutVolume construction.
  G4LogicalVolume* readOutLV;

  /// The logical volume in this component that is the volume the beam passes through that
  /// is typically vacuum. Discretised in this way for cuts / physics process to be assigned
  /// differently from general component material.
  G4LogicalVolume* acceleratorVacuumLV;

  /// A larger length safety that can be used where tracking accuracy isn't required
  /// or more tolerant geometry is required (1um).
  static G4double const lengthSafetyLarge;
  
private:
  /// Private default constructor to force use of provided constructors, which
  /// ensure an object meets the requirements for the rest of the construction
  /// and placement machinery in BDSIM
  BDSAcceleratorComponent();

  /// Assignment and copy constructor not implemented nor used
  BDSAcceleratorComponent& operator=(const BDSAcceleratorComponent&);
  BDSAcceleratorComponent(BDSAcceleratorComponent&);

  /// Build readout geometry volume
  G4LogicalVolume* BuildReadOutVolume(G4String name,
				      G4double chordLength,
				      G4double angle);

 

  std::vector<G4LogicalVolume*> itsGFlashVolumes;
  //A vector containing the physical volumes in the accelerator component- to be used for geometric importance sampling etc.

  /// Boolean record of whether this component has been already initialised.
  /// This check protects against duplicate initialisation and therefore the potential
  /// memory leaks that would ensue.
  G4bool initialised;
  /// Record of how many times this component has been copied.
  G4int copyNumber;

  /// Copy of bias list from parser for this particlar element
  std::list<std::string> biasVacuumList;
  std::list<std::string> biasMaterialList;
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

inline G4bool BDSAcceleratorComponent::GetPrecisionRegion() const
{return precisionRegion;}

inline void   BDSAcceleratorComponent::SetPrecisionRegion(G4bool precisionRegionIn)
{precisionRegion = precisionRegionIn;}

inline BDSBeamPipeInfo* BDSAcceleratorComponent::GetBeamPipeInfo() const
{return beamPipeInfo;}

inline void BDSAcceleratorComponent::SetGFlashVolumes(G4LogicalVolume* aLogVol)
{itsGFlashVolumes.push_back(aLogVol);}

inline std::vector<G4LogicalVolume*> BDSAcceleratorComponent::GetGFlashVolumes() const
{return itsGFlashVolumes;}

inline void BDSAcceleratorComponent::IncrementCopyNumber()
{copyNumber++;}

inline G4int BDSAcceleratorComponent::GetCopyNumber()const
{return copyNumber;}

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

inline void BDSAcceleratorComponent::SetBiasVacuumList(std::list<std::string> biasVacuumListIn)
{biasVacuumList = biasVacuumListIn;}

inline std::list<std::string> BDSAcceleratorComponent::GetBiasVacuumList()
{return biasVacuumList;}

inline void BDSAcceleratorComponent::SetBiasMaterialList(std::list<std::string> biasMaterialListIn)
{biasMaterialList = biasMaterialListIn;}

inline std::list<std::string> BDSAcceleratorComponent::GetBiasMaterialList()
{return biasMaterialList;}

#endif
