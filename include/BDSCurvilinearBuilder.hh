#include "BDSUtilities.hh"

class BDSAcceleratorComponent;
class BDSBeamline;
class BDSBeamlineElement;
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
  BDSBeamline* BuildCurvilinearBeamLine1To1(BDSBeamline const* const beamline);

  BDSBeamline* BuildCurvilinearBeamLine(BDSBeamline const* const beamline);

private:
  BDSBeamlineElement* BuildCurvilinearElement(BDSBeamlineElement const* const startElement,
					      BDSBeamlineElement const* const finishElement) const;

  /// Simple interrogation function to determine if an element has a finite angle or not.
  inline G4bool Angled(BDSBeamlineElement const* const element) const;

  /// Whether an element is too short for its own curvilinear volume
  inline G4bool TooShort(BDSBeamlineElement const* const element) const;

  /// Accumulate an element's properties onto a set of variables which are passed by
  /// reference.
  void Accumulate(BDSBeamlineElement const* const element,
		  G4double& accumualtedArcLength,
		  G4double& accumulatedAngle,
		  G4bool&   straightSoFar) const;

  G4double curvilinearRadius; ///< Radius for curvilinear geometry.
  G4bool   checkOverlaps;     ///< Whether to check overlaps.
  G4double lengthSafety;      ///< Length safety.
  G4double minimumLength;     ///< Minimum length of a curvilinear section.

  /// Factory to build curvilinear geometry.
  BDSCurvilinearFactory* factory;



    /// OLD
  BDSBeamlineElement* BuildBeamLineElement(BDSSimpleComponent* component,
					   BDSBeamlineElement const* const element);

  /// OLD
  /// Build a single component.
  BDSSimpleComponent* BuildCurvilinearComponent(BDSBeamlineElement const* const element);
};

inline G4bool BDSCurvilinearBuilder::Angled(BDSBeamlineElement const* const element) const
{
  return BDS::IsFinite(element->GetAngle());
}

inline G4bool BDSCurvilinearBuilder::TooShort(BDSBeamlineElement const* const element) const
{
  return element->GetArcLength() < minimumLength;
}
