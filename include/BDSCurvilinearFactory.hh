class BDSAcceleratorComponent;
class BDSBeamline;
class BDSSimpleComponent;

/**
 * @brief Factory for simple parallel geometry for curvilinear coordinates.
 *
 * This builds a beam line of geometry based on an existing beam line. It makes
 * simple shapes with the correct length and angle of each component. In the case
 * of a straight component (no angle) it's a box and in the case of an angled component
 * or one with face rotations, it's a G4CutTubs.
 *
 * @author Laurie Nevay
 */

class BDSCurvilinearFactory
{
public:
  static BDSCurvilinearFactory* Instance(); ///< Singleton accessor.

  ~BDSCurvilinearFactory();

  /// Build a beam line of curvilinear geometry based on another beam line.
  BDSBeamline* BuildCurvilinearBeamLine(BDSBeamline const* const beamline);

private:
  BDSCurvilinearFactory(); ///< Private constructor as singleton pattern.
  static BDSCurvilinearFactory* instance; ///< Singleton instance.

  /// Build a single component.
  BDSSimpleComponent* BuildCurvilinearComponent(BDSBeamlineElement const* const element);

  G4double curvilinearRadius; ///< Radius for curvilinear geometry.
  G4bool   checkOverlaps;     ///< Whether to check overlaps
};
