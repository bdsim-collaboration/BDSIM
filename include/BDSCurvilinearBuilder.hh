#include "BDSBeamlineElement.hh"
#include "BDSUtilities.hh"

#include "globals.hh"

class BDSBeamline;
class BDSCurvilinearFactory;
class BDSSimpleComponent;

/**
 * @brief Factory for simple parallel geometry for curvilinear coordinates.
 *
 * This builds a beam line of geometry w.r.t. a beam line that can be used
 * for curvilinear coordinates.
 *
 * @author Laurie Nevay
 */

class BDSCurvilinearBuilder
{
public:
  BDSCurvilinearBuilder();
  ~BDSCurvilinearBuilder();

  /// Build a beam line of curvilinear geometry based on another beam line.
  BDSBeamline* BuildCurvilinearBeamLine1To1(BDSBeamline const* const beamline,
					    const G4bool circular);

private:
  BDSCurvilinearBuilder(const BDSCurvilinearBuilder&) = delete;
  BDSCurvilinearBuilder& operator=(const BDSCurvilinearBuilder&) = delete;

  /// Create a curvilinear element for a beam line that represents the curvilinear
  /// coordinates between startElement and finishElement. This creates a BDSSimpleComponent
  /// first then wraps it in a premade BDSBeamlineElement. This must ONLY be used for a range
  /// of elements with the same tilt for the correct coordinate frame to be produced. The tilt
  /// is taken from the first element and assumed to be the same for all. The index is the
  /// index it'l have in the curvilinear beam line.
  BDSBeamlineElement* CreateCurvilinearElement(G4String                    elementName,
					       BDSBeamline::const_iterator startElement,
					       BDSBeamline::const_iterator finishElement,
					       G4int                       index);

  /// Create the BDSBeamlineElement by wrapping a BDSSimpleComponent.
  BDSBeamlineElement* CreateElementFromComponent(BDSSimpleComponent* component,
						 BDSBeamline::const_iterator startElement,
						 BDSBeamline::const_iterator finishElement,
						 G4int                       index);

  
  BDSBeamlineElement* CreateBonusSectionStart(BDSBeamline const* const beamline);
  BDSBeamlineElement* CreateBonusSectionEnd(BDSBeamline const* const beamline);

  /// Simple interrogation function to determine if an element has a finite angle or not.
  inline G4bool Angled(BDSBeamlineElement const* const element) const;

  /// Whether an element is too short for its own curvilinear volume.
  inline G4bool TooShort(BDSBeamlineElement const* const element) const;

  /// Length that was used for padding on the beam line we're building with respesct to.
  G4double paddingLength;

  G4double curvilinearRadius; ///< Radius for curvilinear geometry.
  G4double bonusChordLength;  ///< Length of any possible bonus section added to beginning and end.

  /// Factory to build curvilinear geometry.
  BDSCurvilinearFactory* factory;
};

inline G4bool BDSCurvilinearBuilder::Angled(BDSBeamlineElement const* const element) const
{return BDS::IsFinite(element->GetAngle());}
