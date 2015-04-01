#ifndef BDSBEAMPIPEFACTORYRECTELLIPSE_H
#define BDSBEAMPIPEFACTORYRECTELLIPSE_H

#include "BDSBeamPipeFactoryBase.hh"
#include "BDSBeamPipe.hh"

/**
 * @brief Factory for rectellipse aperture model beampipes.
 * 
 * This is the overlap of (boolean AND) a rectangle and an ellipse
 * transversely.  This factory is similar to the simple LHC factory
 * and uses the intersection of an elliptical tube and a box. This
 * method produces a solid shape, so to achieve a pipe, a smaller
 * duplicate intersection solid is made for subtraction from the outer
 * one. To achieve the angled faces, G4CutTubs is used and the rectangular
 * box is made longer so the first intersection gives the angled face
 * from the easier G4CutTubs primitive.
 * 
 * This factory requires four aperture parameters. Aper1,2 are the horizontal
 * and vertical half widths of the rectangle respectively, and aper3,4 are the
 * horizontal and vertical semi-axes of the ellipse. Note, will aper1,2 should be
 * < aper1,2 otherwise an elliptical tube will be more efficient.
 * 
 * Singleton pattern
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

class BDSBeamPipeFactoryRectEllipse: public BDSBeamPipeFactoryBase
{
public:
  static BDSBeamPipeFactoryRectEllipse* Instance(); ///< singleton accessor
  
  virtual ~BDSBeamPipeFactoryRectEllipse();

  virtual BDSBeamPipe* CreateBeamPipe(G4String    nameIn,                     // name
				      G4double    lengthIn,                   // length [mm]
				      G4double    aper1 = 0,                  // aperture parameter 1
				      G4double    aper2 = 0,                  // aperture parameter 2
				      G4double    aper3 = 0,                  // aperture parameter 3
				      G4double    aper4 = 0,                  // aperture parameter 4
				      G4Material* vacuumMaterialIn = NULL,    // vacuum material
				      G4double    beamPipeThicknessIn = 0,    // beampipe thickness [mm]
				      G4Material* beamPipeMaterialIn = NULL   // beampipe material
				      );
  
  virtual BDSBeamPipe* CreateBeamPipeAngledIn(G4String    nameIn,
					      G4double    lengthIn,
					      G4double    angleInIn,   // the normal angle of the input face
					      G4double    aper1 = 0,
					      G4double    aper2 = 0,
					      G4double    aper3 = 0,
					      G4double    aper4 = 0,
					      G4Material* vacuumMaterialIn = NULL,
					      G4double    beamPipeThicknessIn = 0,
					      G4Material* beamPipeMaterialIn = NULL
					      );
  
  virtual BDSBeamPipe* CreateBeamPipeAngledOut(G4String    nameIn,
					       G4double    lengthIn,
					       G4double    angleOutIn, // the normal angle of the output face
					       G4double    aper1 = 0,
					       G4double    aper2 = 0,
					       G4double    aper3 = 0,
					       G4double    aper4 = 0,
					       G4Material* vacuumMaterialIn = NULL,
					       G4double    beamPipeThicknessIn = 0,
					       G4Material* beamPipeMaterialIn = NULL
					       );
  
  virtual BDSBeamPipe* CreateBeamPipeAngledInOut(G4String    nameIn,
						 G4double    lengthIn,
						 G4double    angleInIn,  // the normal angle of the input face
						 G4double    angleOutIn, // the normal angle of the output face
						 G4double    aper1 = 0,
						 G4double    aper2 = 0,
						 G4double    aper3 = 0,
						 G4double    aper4 = 0,
						 G4Material* vacuumMaterialIn = NULL,
						 G4double    beamPipeThicknessIn = 0,
						 G4Material* beamPipeMaterialIn = NULL
						 );

private:
  BDSBeamPipeFactoryRectEllipse(); ///< private default constructor - singleton pattern
  static BDSBeamPipeFactoryRectEllipse* _instance;

  void TestInputParameters(G4Material*& vacuumMaterialIn,
			   G4double&    beamPipeThicknessIn,
			   G4Material*& beamPipeMaterialIn,
			   G4double&    aper1In,
			   G4double&    aper2In,
			   G4double&    aper3In,
			   G4double&    aper4In);

  //abstract common build features to one function
  //use member variables unique to this factory to pass them around

  BDSBeamPipe* CommonFinalConstruction(G4String    nameIn,
				       G4Material* vacuumMaterialIn,
				       G4Material* beamPipeMaterialIn,
				       G4double    lengthIn,
				       G4double    widthIn,
				       G4double    heightIn);
  void CreateGeneralAngledSolids(G4String      nameIn,
				 G4double      lengthIn,
				 G4double      aper1In,
				 G4double      aper2In,
				 G4double      aper3In,
				 G4double      aper4In,
				 G4double      beamPipeThicknessIn,
				 G4ThreeVector inputfaceIn,
				 G4ThreeVector outputfaceIn);

  void CreateContainerSubtractionSolid(G4String& nameIn,
				       G4double& lengthIn,
				       G4double& beamPipeThicknessIn,
				       G4double& aper1In,
				       G4double& aper2In,
				       G4double& aper3In,
				       G4double& aper4In);

};
  
#endif
