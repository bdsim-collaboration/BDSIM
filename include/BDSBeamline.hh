#ifndef BDSBEAMLINE_H
#define BDSBEAMLINE_H

#include "globals.hh" // geant4 globals / types
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"

#include "BDSBeamlineElement.hh"

#include <iterator>
#include <ostream>
#include <utility>    //for std::pair
#include <vector>

class BDSGeometryComponent;
class BDSTiltOffset;
class BDSTransform3D; ///< Forward declaration for iterator so it can appear at the top

/**
 * @brief A vector of BDSBeamlineElement instances - a beamline.
 * 
 * This will calculate and construct a beamline as BDSAcceleratorComponents
 * are added in sequence - ie it calculates their placement positions and 
 * rotations with respect to the start of the beamline as well as their s
 * position in curvilinear coordinates.
 * 
 * Note, this is not a singleton as geometry hierarchy can be introduced
 * by placing beamline components inside parent volumes and therefore creating
 * a new beamline of parents. It can also be used to create multiple beam lines.
 * 
 * @author Laurie Nevay
 */

class BDSBeamline
{
private:
  /// Typedefs up first so we can declare public iterators.
  typedef std::vector<BDSBeamlineElement*> BeamlineVector;

  /// Vector of beam line elements - the data.
  BeamlineVector beamline;
  
public:
  /// Versatile basic constructor that allows a finite poition and rotation to be applied
  /// at the beginning of the beamline in global coordinates. Rembmer the maximum
  /// extents of the beamline will also be displaced. The default constructor is in effect
  /// achieved via defaults
  BDSBeamline(G4ThreeVector     initialGlobalPosition = G4ThreeVector(0,0,0),
	      G4RotationMatrix* initialGlobalRotation = nullptr);
  
  ~BDSBeamline();

  /// Add a component, but check to see if it can be dynamically upcast to a line
  /// in which case, loop over it and apply
  /// AddSingleComponent(BDSAcceleratorComponent* component) to each component
  /// Returns vector of components added
  std::vector<BDSBeamlineElement*> AddComponent(BDSAcceleratorComponent* component,
						BDSTiltOffset* tiltOffset  = nullptr,
						BDSSamplerType samplerType = BDSSamplerType::none,
						G4String       samplerNameIn = "");

  /// Apply a Transform3D rotation and translation to the reference
  /// coordinates. Special method for the special case of unique component
  /// Transform3D. Modifies the end rotation and position of the last element
  /// in the beamline. Note this applies the offset dx,dy,dz first, then the rotation
  /// of coordinates
  void ApplyTransform3D(BDSTransform3D* component);

  /// Add a preassembled beam line element. In this case, the coordinates will have been
  /// calculated external to this class and as such, it's the responsibility of the
  /// developer to make sure the coordinates are correct and do not cause overlaps. This
  /// will be useful for tunnel construction for example or for a non-contiguous beamline.
  /// Subsequent components added via the AddComponent() method will be appended in the usual
  /// way to the end cooridinates of this element.
  void AddBeamlineElement(BDSBeamlineElement* element);

  /// Iterate over the beamline and print out the name, position, rotation
  /// and s position of each beamline element
  void PrintAllComponents(std::ostream& out) const;

  /// Once the beamline element has been constructed and all positions and rotations
  /// use these to update the world extent of this beam line.
  void UpdateExtents(BDSBeamlineElement* element);
  
  BDSBeamlineElement* GetFirstItem(); ///< Return a reference to the first element
  BDSBeamlineElement* GetLastItem();  ///< Return a reference to the last element

  /// Get an element by name. Returns null pointer if not found.
  BDSBeamlineElement* GetElement(G4String name);
  
  /// Get the total length of the beamline - the sum of the chord length of each element
  inline G4double     GetTotalChordLength() const;

  /// Get the total ARC length for the beamline - ie the maximum s position
  inline G4double     GetTotalArcLength() const;

  /// Get the number of elements
  BeamlineVector::size_type size() const {return beamline.size();}

  /// Get the maximum positive extent in all dimensions  
  G4ThreeVector GetMaximumExtentPositive() const;

  /// Get the maximum negative extent in all dimensions
  G4ThreeVector GetMaximumExtentNegative() const;

  /// Get the maximum extent absolute in each dimension
  G4ThreeVector GetMaximumExtentAbsolute() const;

  /// Get the local to global transform for curvilinear coordinates
  /// to global coordinates. 0,0 transverse position by default.
  G4Transform3D GetGlobalEuclideanTransform(G4double s,
					    G4double x = 0,
					    G4double y = 0);

  ///@{ Iterator mechanics
  typedef BeamlineVector::iterator       iterator;
  typedef BeamlineVector::const_iterator const_iterator;
  iterator       begin()       {return beamline.begin();}
  iterator       end()         {return beamline.end();}
  const_iterator begin() const {return beamline.begin();}
  const_iterator end()   const {return beamline.end();}
  G4bool         empty() const {return beamline.empty();}
  ///@}

  /// Return a pointer to the previous element. First this beamline is
  /// searched for the vector. If there is no such element or no previous
  /// element because it's the beginning, then a nullptr is returned. The
  /// caller should test on this.
  BDSBeamlineElement* GetPrevious(BDSBeamlineElement* element);
  BDSBeamlineElement* GetNext(BDSBeamlineElement* element);

  BDSBeamlineElement* GetPrevious(G4int index);
  BDSBeamlineElement* GetNext(G4int index);
  
  // Accessors in a similar style to std::vector
  /// Return a reference to the first element
  BDSBeamlineElement* front() const;
  /// Return a reference to the last element
  BDSBeamlineElement* back()  const;
  
  /// output stream
  friend std::ostream& operator<< (std::ostream &out, BDSBeamline const &bl);

  /// Feedback about memory consumption for this beamline instance - container size,
  /// size of all BDSBeamlineElement() and size of all BDSAcceleratorComponent() stored.
  void PrintMemoryConsumption() const;

  BDSBeamlineElement* ProvideEndPieceElementBefore(BDSSimpleComponent* endPiece,
						   G4int    index,
						   G4double endPieceLength) const;
  BDSBeamlineElement* ProvideEndPieceElementAfter(BDSSimpleComponent* endPiece,
						  G4int    index,
						  G4double endPieceLength) const;

  /// Whether the supplied index will lie within the beam line vector.
  G4bool IndexOK(G4int index) const;
  
private:
  /// Add a single component and calculate its position and rotation with respect
  /// to the beginning of the beamline
  /// Returns pointer to component added
  BDSBeamlineElement* AddSingleComponent(BDSAcceleratorComponent* component,
					 BDSTiltOffset* tiltOffset  = nullptr,
					 BDSSamplerType samplerType = BDSSamplerType::none,
					 G4String       samplerNameIn = "");

  /// Register the fully created element to a map of names vs element pointers. Used to
  /// look up transforms by name.
  void RegisterElement(BDSBeamlineElement* element);

  G4double totalChordLength;
  G4double totalArcLength;

  G4ThreeVector maximumExtentPositive; ///< maximum extent in the positive coordinates in each dimension
  G4ThreeVector maximumExtentNegative; ///< maximum extent in the negative coordinates in each dimension

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

  /// Vector of s coordinates at the end of each element. This is intended
  /// so that an iterator pointing to the s position will be the correct
  /// index for the beamline element in the main BDSBeamlineVector element.
  /// This is filled in order so it's sorted by design.
  std::vector<G4double> sEnd;
  
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

inline G4ThreeVector BDSBeamline::GetMaximumExtentPositive() const
{return maximumExtentPositive;}

inline G4ThreeVector BDSBeamline::GetMaximumExtentNegative() const
{return maximumExtentNegative;}

inline BDSBeamlineElement* BDSBeamline::front() const
{return beamline.front();}

inline BDSBeamlineElement* BDSBeamline::back() const
{return beamline.back();}

#endif
