#ifndef BDSBEAMLINE_H
#define BDSBEAMLINE_H

#include "globals.hh" // geant4 globals / types

#include "BDSBeamlineElement.hh"

#include <iterator>
#include <ostream>
#include <utility>    //for std::pair
#include <vector>

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
 * Note, the beamline will only calculate coordinates relative to its start
 * being 0,0,0. For placement from a different position, this should be applied
 * elsewhere when the placement is being performed and not in the calculated 
 * coordinates here.
 */

/// Forward declaration for iterator
class BDSBeamline;

/// Iterator for beamline
typedef std::vector<BDSBeamlineElement*>::const_iterator BDSBeamlineIterator;

class BDSBeamline{
public:
  BDSBeamline();
  ~BDSBeamline();

  /// Add a component and calculate its position and rotation with respect
  /// to the beginning of the beamline
  void AddComponent(BDSAcceleratorComponent* component);

  /// Iterate over the beamline and print out the name, position, rotation
  /// and s position of each beamline element
  void PrintAllComponents(std::ostream& out) const;

  BDSBeamlineElement* GetFirstItem();             ///< Return a reference to the first element
  BDSBeamlineElement* GetLastItem();              ///< Return a reference to the last element

  /// Get the total length of the beamline - the sum of the chord length of each element
  inline G4double     GetTotalChordLength() const;

  /// Get the total ARC length fo the beamline - ie the maximum s position
  inline G4double     GetTotalArcLength() const;

  /// Get the number of elements
  std::vector<BDSBeamlineElement*>::size_type size() const;
  
  G4ThreeVector       GetMaximumExtentPositive(); ///< Get the maximum positive extent in all dimensions  
  G4ThreeVector       GetMaximumExtentNegative(); ///< Get the maximum negative extent in all dimensions

  // Accessors in a similar style to std::vector
  /// Return a reference to the first element
  inline BDSBeamlineElement* front() const;
  /// Return a reference to the last element
  inline BDSBeamlineElement* back()  const;
  /// Return iterator to the beginning
  inline std::vector<BDSBeamlineElement*>::iterator begin();
  /// Return iterator to the end
  inline std::vector<BDSBeamlineElement*>::iterator end();
  /// Return reverse iterator to the reverse beginning
  inline std::vector<BDSBeamlineElement*>::reverse_iterator rbegin();
  /// Return reverse iterator to the reverse end
  inline std::vector<BDSBeamlineElement*>::reverse_iterator rend();
  /// Return iterator to the beginning
  inline std::vector<BDSBeamlineElement*>::const_iterator cbegin() const;
  /// Return iterator to the end
  inline std::vector<BDSBeamlineElement*>::const_iterator cend()   const;
  /// Return reverse iterator to the reverse beginning
  inline std::vector<BDSBeamlineElement*>::const_reverse_iterator crbegin() const;
  /// Return reverse iterator to the reverse end
  inline std::vector<BDSBeamlineElement*>::const_reverse_iterator crend()   const;

  /// output stream
  friend std::ostream& operator<< (std::ostream &out, BDSBeamline const &beamline);
  
private: 
  std::vector<BDSBeamlineElement*> beamline; ///< Beamline vector - the data

  G4double totalChordLength;
  G4double totalArcLength;

  std::pair<G4double, G4double> extentX;     ///< +ve and -ve extent in X
  std::pair<G4double, G4double> extentY;     ///< +ve and -ve extent in Y
  std::pair<G4double, G4double> extentZ;     ///< +ve and -ve extent in Z
  
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

inline G4ThreeVector BDSBeamline::GetMaximumExtentPositive()
{return G4ThreeVector(extentX.second, extentY.second, extentZ.second);}

inline G4ThreeVector BDSBeamline::GetMaximumExtentNegative()
{return G4ThreeVector(extentX.first, extentY.first, extentZ.first);}

inline BDSBeamlineElement* BDSBeamline::front() const
{return beamline.front();}

inline BDSBeamlineElement* BDSBeamline::back() const
{return beamline.back();}

inline std::vector<BDSBeamlineElement*>::iterator BDSBeamline::begin()
{return beamline.begin();}

inline std::vector<BDSBeamlineElement*>::iterator BDSBeamline::end()
{return beamline.end();}

inline std::vector<BDSBeamlineElement*>::reverse_iterator BDSBeamline::rbegin()
{return beamline.rbegin();}

inline std::vector<BDSBeamlineElement*>::reverse_iterator BDSBeamline::rend()
{return beamline.rend();}

inline std::vector<BDSBeamlineElement*>::const_iterator BDSBeamline::cbegin() const
{return beamline.begin();}

inline std::vector<BDSBeamlineElement*>::const_iterator BDSBeamline::cend() const
{return beamline.end();}

inline std::vector<BDSBeamlineElement*>::const_reverse_iterator BDSBeamline::crbegin() const
{return beamline.rbegin();}

inline std::vector<BDSBeamlineElement*>::const_reverse_iterator BDSBeamline::crend() const
{return beamline.rend();}

#endif
