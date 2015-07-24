#ifndef BDSBEAMLINE_H
#define BDSBEAMLINE_H

#include "globals.hh" // geant4 globals / types

#include "BDSBeamlineElement.hh"

#include <iterator>
#include <ostream>
#include <utility>    //for std::pair
#include <vector>

class BDSTiltOffset;

/**
 * @brief A vector of BDSBeamlineElement instances - a beamline.
 * 
 * This will calculate and construct a beamline as BDSAcceleratorComponents
 * are added in sequence - ie it calculates their placement positions and 
 * rotations with respect to the start of the beamline as well as their s
 * position in curvilinear coordinates.
 * 
 * Note, this is note a singleton as geometry hierarchy can be introduced
 * by placing beamline components inside parent volumes and therefore creating
 * a new beamline of parents.
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

/// Forward declaration for iterator so it can appear at the top
class BDSBeamline;

class BDSLine;
class BDSTransform3D;

/// Iterator for beamline
typedef std::vector<BDSBeamlineElement*>::const_iterator BDSBeamlineIterator;

class BDSBeamline{
public:
  /// Default constructor. Beamline started at 0,0,0 with beamline built along
  /// the z axis.
  BDSBeamline();

  /// Auxiliary constructor that allows a finite poition and rotation to be applied
  /// at the beginning of the beamline in global coordinates. Rembmer the maximum
  /// extents of the beamline will also be displaced.
  BDSBeamline(G4ThreeVector     initialGlobalPosition,
	      G4RotationMatrix* initialGlobalRotation);
  
  ~BDSBeamline();

  /// Add a component, but check to see if can be dynamically upcast to a line
  /// in which case, loop over it and apply
  /// AddSingleComponent(BDSAcceleratorComponent* component) to each component
  void AddComponent(BDSAcceleratorComponent* component, BDSTiltOffset* tiltOffset = NULL);

  /// Apply a Transform3D rotation and translation to the reference
  /// coordinates. Special method for the special case of unique component
  /// Transform3D. Modifies the end rotation and position of the last element
  /// in the beamline. Note this applies the offset dx,dy,dz first, then the rotation
  /// of coordinates
  void ApplyTransform3D(BDSTransform3D* component);

  /// Iterate over the beamline and print out the name, position, rotation
  /// and s position of each beamline element
  void PrintAllComponents(std::ostream& out) const;
  
  BDSBeamlineElement* GetFirstItem(); ///< Return a reference to the first element
  BDSBeamlineElement* GetLastItem();  ///< Return a reference to the last element

  /// Get an element by name. Returns null pointer if not found.
  BDSBeamlineElement* GetElement(G4String name);
  
  /// Get the total length of the beamline - the sum of the chord length of each element
  inline G4double     GetTotalChordLength() const;

  /// Get the total ARC length for the beamline - ie the maximum s position
  inline G4double     GetTotalArcLength() const;

  /// Get the number of elements
  std::vector<BDSBeamlineElement*>::size_type size() const;

  /// Get the maximum positive extent in all dimensions  
  G4ThreeVector GetMaximumExtentPositive() const;

  /// Get the maximum negative extent in all dimensions
  G4ThreeVector GetMaximumExtentNegative() const;

  /// Get the maximum extent absolute in each dimension
  G4ThreeVector GetMaximumExtentAbsolute() const;

  // Accessors in a similar style to std::vector
  /// Return a reference to the first element
  BDSBeamlineElement* front() const;
  /// Return a reference to the last element
  BDSBeamlineElement* back()  const;
  /// Return iterator to the beginning
  inline std::vector<BDSBeamlineElement*>::iterator begin();
  /// Return iterator to the end
  inline std::vector<BDSBeamlineElement*>::iterator end();
  /// Return iterator to the beginning
  inline std::vector<BDSBeamlineElement*>::const_iterator begin() const;
  /// Return iterator to the end
  inline std::vector<BDSBeamlineElement*>::const_iterator end()   const;
  /// Return whether the beamline is empty or not
  inline G4bool empty() const;

  /// output stream
  friend std::ostream& operator<< (std::ostream &out, BDSBeamline const &bl);

  /// Feedback about memory consumption for this beamline instance - container size,
  /// size of all BDSBeamlineElement() and size of all BDSAcceleratorComponent() stored.
  void PrintMemoryConsumption() const;
  
private: 
  /// Add a single component and calculate its position and rotation with respect
  /// to the beginning of the beamline
  void AddSingleComponent(BDSAcceleratorComponent* component, BDSTiltOffset* tiltOffset = NULL);

  /// Register the fully created element to a map of names vs element pointers. Used to
  /// look up transforms by name.
  void RegisterElement(BDSBeamlineElement* element);

  std::vector<BDSBeamlineElement*> beamline; ///< Beamline vector - the data

  G4double totalChordLength;
  G4double totalArcLength;

  G4ThreeVector maximumExtentPositive; ///< maximum extent in the positive coordinates in each dimension
  G4ThreeVector maximumExtentNegative; ///< maximum extent in the negative coordinates in each dimension

  ///@{ Current rotation axes
  ///  xARS = xAxisReferenceStart, M = Middle, E = End
  G4ThreeVector xARS, yARS, zARS, xARM, yARM, zARM, xARE, yARE, zARE;
  ///@}

  /// Current reference rotation at the end of the previous element
  G4RotationMatrix* previousReferenceRotationEnd;

  /// Current reference position at the end of the previous element
  G4ThreeVector previousReferencePositionEnd;

  /// Current s coordinate at the end of the previous element
  G4double previousSPositionEnd;

  /// Map of objects by name stored in this beam line. For now,
  /// only the base name (no suffix) will be used for the component
  /// and also not the names of the internal components ie the beam pipe
  /// name. This would result in a particularly large number of volumes
  /// and may not always be unique.
  std::map<G4String, BDSBeamlineElement*> components;
  
  /// assignment and copy constructor not implemented nor used
  BDSBeamline& operator=(const BDSBeamline&);
  BDSBeamline(BDSBeamline&);
};

inline BDSBeamlineElement* BDSBeamline::GetFirstItem()
{return front();}

inline BDSBeamlineElement* BDSBeamline::GetLastItem()
{return back();}

inline G4double BDSBeamline::GetTotalChordLength() const
{return totalChordLength;}

inline G4double BDSBeamline::GetTotalArcLength() const
{return totalArcLength;}

inline std::vector<BDSBeamlineElement*>::size_type BDSBeamline::size() const
{return beamline.size();}

inline G4ThreeVector BDSBeamline::GetMaximumExtentPositive() const
{return maximumExtentPositive;}

inline G4ThreeVector BDSBeamline::GetMaximumExtentNegative() const
{return maximumExtentNegative;}

inline BDSBeamlineElement* BDSBeamline::front() const
{return beamline.front();}

inline BDSBeamlineElement* BDSBeamline::back() const
{return beamline.back();}

inline std::vector<BDSBeamlineElement*>::iterator BDSBeamline::begin()
{return beamline.begin();}

inline std::vector<BDSBeamlineElement*>::iterator BDSBeamline::end()
{return beamline.end();}

inline std::vector<BDSBeamlineElement*>::const_iterator BDSBeamline::begin() const
{return beamline.begin();}

inline std::vector<BDSBeamlineElement*>::const_iterator BDSBeamline::end() const
{return beamline.end();}

inline G4bool BDSBeamline::empty() const
{return beamline.empty();}

#endif
