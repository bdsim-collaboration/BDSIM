#ifndef __BDSACCELERATORCOMPONENT_H
#define __BDSACCELERATORCOMPONENT_H

#include "BDSGlobalConstants.hh" 

#include <cstring>
#include <list>
#include <vector> 
#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "globals.hh"
//#include "BDSBeamPipe.hh"
#include "BDSEnergyCounterSD.hh"

#include "G4MagneticField.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4FieldManager.hh"
#include "G4UserLimits.hh"
#include "G4CSGSolid.hh"
#include "G4Tubs.hh"


class BDSAcceleratorComponent 
{
public:
  //destructor
  virtual ~BDSAcceleratorComponent ();

  //name
  const G4String GetName () const;

  //type 
  const G4String GetType () const;

  /// 0 = no precision region, 1 = precision region 1, 2 = precision region 2.
  G4int GetPrecisionRegion() const;

  // angle - for bends etc.
  G4double GetAngle ();

  // geometry length of the component.

  virtual G4double GetYLength ();
  virtual G4double GetXLength ();
  virtual G4double GetArcLength ();   // note no z length - this is chord length
  virtual G4double GetChordLength (); // only chord OR arc makes it explicit
  
  G4double GetPhiAngleIn (); //polar angle in
  G4double GetPhiAngleOut (); //polar angle out

  G4double GetPhi (); //polar angle with respect to original frame
  G4double GetTheta (); //azimuthal angle with respect to original frame
  G4double GetPsi (); //azimuthal angle with respect to original frame

  G4double GetXOffset();  // frame offset 
  G4double GetYOffset();
  G4double GetZOffset();

  G4double GetTunnelRadius();
  G4double GetTunnelOffsetX();
  
  G4double GetAperX();
  G4double GetAperY();

  G4double GetK1();
  G4double GetK2();
  G4double GetK3();

  G4RotationMatrix* GetRotation();
  G4ThreeVector GetPosition();
  
  G4double GetTilt();  // component tilt 
  
  G4LogicalVolume* GetMarkerLogicalVolume() const;

  G4LogicalVolume* GetTunnelLogicalVolume() const;
  G4String GetTunnelCavityMaterial() const;
  BDSEnergyCounterSD* GetBDSEnergyCounter() const;
  
  void SetBDSEnergyCounter( BDSEnergyCounterSD* anBDSEnergyCounter);
  G4int GetCopyNumber() const;
  G4double GetSPos() const;
  void SetSPos(G4double spos);
  void AddSensitiveVolume(G4LogicalVolume* aLogVol);
  std::vector<G4LogicalVolume*> GetSensitiveVolumes();
  void SetGFlashVolumes(G4LogicalVolume* aLogVol);
  std::vector<G4LogicalVolume*> GetGFlashVolumes();
  void SetMultiplePhysicalVolumes(G4VPhysicalVolume* aPhysVol);
  std::vector<G4VPhysicalVolume*> GetMultiplePhysicalVolumes();
  void SetInnerMostLogicalVolume(G4LogicalVolume* aLogVol);
  G4LogicalVolume* GetInnerMostLogicalVolume() const;
  G4UserLimits* GetInnerBPUserLimits();
  G4UserLimits* GetUserLimits();

  //  void BuildOuterFieldManager();

  // in case a mapped field is provided creates a field mesh in global coordinates
  virtual void PrepareField(G4VPhysicalVolume *referenceVolume); 

  // in case a component requires specific alignment (e.g. SQL/BDSElement)
  /*
  virtual void AlignComponent(G4ThreeVector& TargetPos, 
			      G4RotationMatrix *TargetRot,
			      G4RotationMatrix& globalRotation,
			      G4ThreeVector& rtot,
			      G4ThreeVector& rlast,
			      G4ThreeVector& localX,
			      G4ThreeVector& localY,
			      G4ThreeVector& localZ); 
  */
  
  // get parameter value from the specification string

  G4double getParameterValue(G4String spec, G4String name) const;
  G4String getParameterValueString(G4String spec, G4String name) const;

  /// BDSComponentFactory creates BDSAcceleratorComponents
  friend class BDSComponentFactory;

private:
  /// private default constructor
  BDSAcceleratorComponent();
protected:
  /// initialise method
  /// checks if marker logical volume already exists and builds new one if not
  // can't be in constructor as calls virtual methods
  virtual void Initialise();

public:
  BDSAcceleratorComponent (
			  G4String& aName, 
			  G4double aLength,
			  G4double aBpRadius,
			  G4double aXAper,
			  G4double aYAper,
                          std::list<G4double> blmLocZ, 
			  std::list<G4double> blmLocTheta,
                          G4String aTunnelMaterial = "",
			  G4String aMaterial = "",
			  G4double phi=0.,  // polar angle (used in hor. bends)
			  G4double XOffset=0.,
			  G4double YOffset=0.,
			  G4double ZOffset=0.,
			  G4double tunnelRadius=0.,
			  G4double tunnelOffsetX=BDSGlobalConstants::Instance()->GetTunnelOffsetX(),
                          G4String aTunnelCavityMaterial = "Air");

  BDSAcceleratorComponent (
			  G4String& aName, 
			  G4double aLength,
			  G4double aBpRadius,
			  G4double aXAper,
			  G4double aYAper,
                          G4String aTunnelMaterial = "",
			  G4String aMaterial = "",
			  G4double phi=0.,  // polar angle (used in hor. bends)
			  G4double XOffset=0.,
			  G4double YOffset=0.,
			  G4double ZOffset=0.,
			  G4double tunnelRadius=0.,
			  G4double tunnelOffsetX=BDSGlobalConstants::Instance()->GetTunnelOffsetX(),
			  G4String aTunnelCavityMaterial = "Air");

  G4VisAttributes* GetVisAttributes()const; ///> get visual attributes
  G4LogicalVolume* itsOuterLogicalVolume;
  G4LogicalVolume* itsMarkerLogicalVolume;
  G4LogicalVolume* itsTunnelLogicalVolume;
  G4LogicalVolume* itsTunnelFloorLogicalVolume;


private:
  //
  //    Geometry building
  //

  /// build marker logical volume
  virtual void BuildMarkerLogicalVolume() = 0;
  /// set and return visual attributes
  virtual void SetVisAttributes(); 

protected:
  /// build logical volumes: marker, tunnel, field, blms etc.
  virtual void Build();
  /// build tunnel
  void BuildTunnel();
  /// build beam loss monitors
  virtual void BuildBLMs();

protected:
  /// set methods, protected
  void SetName(G4String aName);
  void SetType(G4String aType);

  void SetPhi (G4double val);
  void SetTheta(G4double val);
  void SetPsi(G4double val);

  void SetPrecisionRegion (G4int precisionRegionType);

  ///Set is only for Outline readout purposes - doesn't change magnet strengths
  void SetK1(G4double K1);
  void SetK2(G4double K2);
  void SetK3(G4double K3);

  //Values related to BLM placement and geometry
  G4double itsBlmLocationR;
  //  G4double itsBlmRadius;

  G4String itsName;
  G4double itsLength;
  G4double itsXLength;
  G4double itsYLength;
  G4double itsOuterR;
  G4double itsBpRadius;
  G4double itsXAper;
  G4double itsYAper;
  G4double itsAngle;   // the angle (rad) by which the reference coordinates are changed - for placement
  // sbends and rbends use this - h and vkicks this should be 0.
  G4String itsMaterial;
  G4VisAttributes* itsVisAttributes;
  std::list<G4double> itsBlmLocZ;
  std::list<G4double> itsBlmLocTheta;
  G4String itsTunnelMaterial;
  //Tunnel geom
  G4double itsXOffset;
  G4double itsYOffset;
  G4double itsZOffset;
  G4double itsTunnelRadius;
  G4double itsTunnelOffsetX;  
  /// component type, same as from typestr from enums.cc
  G4String itsType;

  G4double itsTilt;

  G4double itsPhiAngleIn;
  G4double itsPhiAngleOut;
  
  G4double itsPhi;
  G4double itsTheta;
  G4double itsPsi;
  G4double itsK1, itsK2, itsK3;
  //G4RotationMatrix* itsRotation; // rotation matrix (not used)
  //G4ThreeVector itsPosition;
  //  BDSBeamPipe* itsBeamPipe;
  G4MagIntegratorStepper*  itsOuterStepper;
  /// generic user limits
  G4UserLimits* itsUserLimits;
  /// specific user limits
  G4UserLimits* itsOuterUserLimits;
  G4UserLimits* itsMarkerUserLimits;
  G4UserLimits* itsInnerBeampipeUserLimits;
  G4LogicalVolume* itsInnerMostLogicalVolume;

  G4String itsTunnelCavityMaterial;
  G4int itsPrecisionRegion;

  /// Marker solid
  G4VSolid* itsMarkerSolidVolume;



  /// Solid shapes used in building tunnel
  G4VSolid* itsTunnelSolid;
  G4VSolid* itsSoilSolid;
  G4VSolid* itsInnerTunnelSolid;
  G4VSolid *itsTunnelCavity;
  G4VSolid *itsLargerTunnelCavity;
  G4VSolid *itsTunnelFloor;
  G4VSolid* itsLargerInnerTunnelSolid; 
  G4VSolid *itsTunnelMinusCavity;
  G4CSGSolid* itsTunnelSizedBlock;

  /// BLM logical volumes
  G4LogicalVolume* itsBLMLogicalVolume;
  G4LogicalVolume* itsBlmCaseLogicalVolume;
  /// BLM physical volumes
  std::vector<G4VPhysicalVolume*> itsBLMPhysiComp;
  /// Tunnel logical volumes
  G4LogicalVolume* itsSoilTunnelLogicalVolume;
  G4LogicalVolume* itsTunnelCavityLogicalVolume;
  G4LogicalVolume*  itsTunnelMinusCavityLogicalVolume;
  /// Tunnel physical volumes
  G4VPhysicalVolume* itsTunnelPhysiInner;
  G4VPhysicalVolume* itsTunnelPhysiComp;
  G4VPhysicalVolume* itsTunnelFloorPhysiComp;
  G4VPhysicalVolume* itsTunnelPhysiCompSoil;
  /// Tunnel user limits
  G4UserLimits* itsTunnelUserLimits;
  G4UserLimits* itsSoilTunnelUserLimits;
  G4UserLimits* itsInnerTunnelUserLimits;



private:
  /// assignment and copy constructor not implemented nor used
  BDSAcceleratorComponent& operator=(const BDSAcceleratorComponent&);
  BDSAcceleratorComponent(BDSAcceleratorComponent&);
  /// constructor initialisation
  void ConstructorInit();
  /// Calculate dimensions used for the marker volume etc.
  void CalculateLengths();

  G4RotationMatrix* nullRotationMatrix;
  G4RotationMatrix* tunnelRot;
  G4VisAttributes* VisAtt;
  G4VisAttributes* VisAtt1;
  G4VisAttributes* VisAtt2;
  G4Tubs* itsBLMSolid;
  G4Tubs* itsBlmOuterSolid;
  G4double itsSPos;
  /// count of logical volumes shared with other instances; start at 0
  G4int itsCopyNumber;
  //  G4int itsCollectionID;
  std::vector<G4LogicalVolume*> itsSensitiveVolumes;
  std::vector<G4LogicalVolume*> itsGFlashVolumes;
  //A vector containing the physical volumes in the accelerator component- to be used for geometric importance sampling etc.
  std::vector<G4VPhysicalVolume*> itsMultiplePhysicalVolumes;
  //  G4double itsZLower;
  //  G4double itsZUpper;
  //  G4double itsSynchEnergyLoss;

};

// Class BDSAcceleratorComponent 

inline G4double BDSAcceleratorComponent::GetChordLength ()
{return itsLength;}

inline G4double BDSAcceleratorComponent::GetXLength ()
{return itsXLength;}

inline G4double BDSAcceleratorComponent::GetYLength ()
{return itsYLength;}

inline G4double BDSAcceleratorComponent::GetArcLength ()
{return itsLength;}

inline G4double BDSAcceleratorComponent::GetAngle ()
{return itsAngle;}

inline G4double BDSAcceleratorComponent::GetPhiAngleIn ()
{return itsPhiAngleIn;}

inline G4double BDSAcceleratorComponent::GetPhiAngleOut ()
{return itsPhiAngleOut;}

inline G4double BDSAcceleratorComponent::GetPhi ()
{return itsPhi;}

inline void BDSAcceleratorComponent::SetPhi (G4double val)
{itsPhi = val;}

inline G4double BDSAcceleratorComponent::GetTheta ()
{return itsTheta;}

inline void BDSAcceleratorComponent::SetTheta (G4double val)
{itsTheta = val;}

inline G4double BDSAcceleratorComponent::GetPsi ()
{return itsPsi;}

inline void BDSAcceleratorComponent::SetPsi (G4double val)
{itsPsi = val;}

inline G4double BDSAcceleratorComponent::GetAperX()
{
  if(itsXAper==0) // i.e. it has not been set
    return itsBpRadius;
  else return itsXAper;
}

inline G4double BDSAcceleratorComponent::GetAperY()
{
  if(itsYAper==0) // i.e. it has not been set
    return itsBpRadius;
  else return itsYAper;
}

inline G4double BDSAcceleratorComponent::GetK1()
{ return itsK1; }

inline G4double BDSAcceleratorComponent::GetK2()
{ return itsK2; }

inline G4double BDSAcceleratorComponent::GetK3()
{ return itsK3; }

inline void BDSAcceleratorComponent::SetK1(G4double K1)
{ itsK1 = K1; }

inline void BDSAcceleratorComponent::SetK2(G4double K2)
{ itsK2 = K2; }

inline void BDSAcceleratorComponent::SetK3(G4double K3)
{ itsK3 = K3; }

//inline G4RotationMatrix* BDSAcceleratorComponent::GetRotation()
//{ return itsRotation;}

//inline G4ThreeVector BDSAcceleratorComponent::GetPosition()
//{ return itsPosition;}

inline const G4String BDSAcceleratorComponent::GetName () const
{return itsName;}

inline void BDSAcceleratorComponent::SetName (G4String aName)
{itsName=aName;}

inline const G4String BDSAcceleratorComponent::GetType () const
{return itsType;}

inline void BDSAcceleratorComponent::SetType (G4String aType)
{itsType=aType;}

inline G4int BDSAcceleratorComponent::GetPrecisionRegion () const
{return itsPrecisionRegion;}

inline void BDSAcceleratorComponent::SetPrecisionRegion (G4int precisionRegionType)
{itsPrecisionRegion = precisionRegionType;}

inline G4LogicalVolume* BDSAcceleratorComponent::GetMarkerLogicalVolume() const
{return itsMarkerLogicalVolume;}

inline G4LogicalVolume* BDSAcceleratorComponent::GetInnerMostLogicalVolume() const
{return itsInnerMostLogicalVolume;}

inline void BDSAcceleratorComponent::
SetInnerMostLogicalVolume(G4LogicalVolume* aLogVol)
{itsInnerMostLogicalVolume = aLogVol;}

inline G4VisAttributes* BDSAcceleratorComponent::GetVisAttributes() const
{return itsVisAttributes;}

inline void BDSAcceleratorComponent::SetVisAttributes()
{itsVisAttributes = new G4VisAttributes(true);
}

inline G4int BDSAcceleratorComponent::GetCopyNumber() const
{return itsCopyNumber;}

inline G4double BDSAcceleratorComponent::GetSPos() const
{return itsSPos;}

inline void BDSAcceleratorComponent::SetSPos(G4double spos)
{itsSPos=spos;}

inline  void BDSAcceleratorComponent::AddSensitiveVolume(G4LogicalVolume* aLogVol)
{ itsSensitiveVolumes.push_back(aLogVol);}

inline  std::vector<G4LogicalVolume*> BDSAcceleratorComponent::GetSensitiveVolumes()
{return itsSensitiveVolumes;}

inline void BDSAcceleratorComponent::SetGFlashVolumes(G4LogicalVolume* aLogVol)
{ itsGFlashVolumes.push_back(aLogVol);}

inline  std::vector<G4LogicalVolume*> BDSAcceleratorComponent::GetGFlashVolumes()
{return itsGFlashVolumes;}

inline void BDSAcceleratorComponent::SetMultiplePhysicalVolumes(G4VPhysicalVolume* aPhysVol)
{ itsMultiplePhysicalVolumes.push_back(aPhysVol);}

inline  std::vector<G4VPhysicalVolume*> BDSAcceleratorComponent::GetMultiplePhysicalVolumes()
{return itsMultiplePhysicalVolumes;}

inline  G4UserLimits* BDSAcceleratorComponent::GetUserLimits(){
  return itsUserLimits;
}

inline  G4UserLimits* BDSAcceleratorComponent::GetInnerBPUserLimits()
  {return itsInnerBeampipeUserLimits;}

inline  G4double BDSAcceleratorComponent::GetXOffset()
{return itsXOffset;}

inline G4double BDSAcceleratorComponent::GetYOffset() 
{return itsYOffset;}

inline G4double BDSAcceleratorComponent::GetZOffset()
{return itsZOffset;}

inline G4double BDSAcceleratorComponent::GetTunnelRadius()
{return itsTunnelRadius;}

inline G4double BDSAcceleratorComponent::GetTunnelOffsetX()
{return itsTunnelOffsetX;}

inline G4double BDSAcceleratorComponent::GetTilt()
{return itsTilt;}


inline  G4double BDSAcceleratorComponent::getParameterValue(G4String spec, G4String name) const
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

inline  G4String BDSAcceleratorComponent::getParameterValueString(G4String spec, G4String name) const
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
