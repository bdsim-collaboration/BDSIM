// This code implementation is the intellectual property of
// the GEANT4 collaboration.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//
// $Id: BDSVisManager.cc,v 1.3 2007/10/05 11:40:26 malton Exp $
// GEANT4 tag $Name:  $
//
// 
// John Allison 24th January 1998.

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#ifdef G4VIS_USE

#include "BDSVisManager.hh"
#include "G4HepRepFile.hh"

// Supported drivers...

#ifdef G4VIS_USE_DAWN
#include "G4FukuiRenderer.hh"
#endif

#ifdef G4VIS_USE_DAWNFILE
#include "G4DAWNFILE.hh"
#endif

#ifdef G4VIS_USE_OPACS
#include "G4Wo.hh"
#include "G4Xo.hh"
#endif

#ifdef G4VIS_USE_OPENGLX
#include "G4OpenGLImmediateX.hh"
#include "G4OpenGLStoredX.hh"
#endif

#ifdef G4VIS_USE_OPENGLWIN32
#include "G4OpenGLImmediateWin32.hh"
#include "G4OpenGLStoredWin32.hh"
#endif

#ifdef G4VIS_USE_OPENGLQT
#include "G4OpenGLImmediateQt.hh"
#include "G4OpenGLStoredQt.hh"
#endif

#ifdef G4VIS_USE_OPENGLXM
#include "G4OpenGLImmediateXm.hh"
#include "G4OpenGLStoredXm.hh"
#endif

#ifdef G4VIS_USE_OIX
#include "G4OpenInventorX.hh"
#endif

#ifdef G4VIS_USE_OIWIN32
#include "G4OpenInventorWin32.hh"
#endif

#ifdef G4VIS_USE_VRML
#include "G4VRML1.hh"
#include "G4VRML2.hh"
#endif

#ifdef G4VIS_USE_VRMLFILE
#include "G4VRML1File.hh"
#include "G4VRML2File.hh"
#endif

#ifdef G4VIS_USE_RAYTRACER
#include "G4RayTracer.hh"
#endif

#ifdef G4VIS_USE_RAYTRACERX
#include "G4RayTracerX.hh"
#endif


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

BDSVisManager::BDSVisManager () {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void BDSVisManager::RegisterGraphicsSystems () {

  RegisterGraphicsSystem (new G4HepRepFile);


#ifdef G4VIS_USE_DAWN
  RegisterGraphicsSystem (new G4FukuiRenderer);
#endif

#ifdef G4VIS_USE_DAWNFILE
  RegisterGraphicsSystem (new G4DAWNFILE);
#endif

#ifdef G4VIS_USE_OPACS
  RegisterGraphicsSystem (new G4Wo);
  RegisterGraphicsSystem (new G4Xo);
#endif

 
#ifdef G4VIS_USE_OPENGLX
  RegisterGraphicsSystem (new G4OpenGLImmediateX);
  RegisterGraphicsSystem (new G4OpenGLStoredX);
#endif
 

#ifdef G4VIS_USE_OPENGLWIN32
  RegisterGraphicsSystem (new G4OpenGLImmediateWin32);
  RegisterGraphicsSystem (new G4OpenGLStoredWin32);
#endif

#ifdef G4VIS_USE_OPENGLQT
  RegisterGraphicsSystem (new G4OpenGLImmediateQt);
  RegisterGraphicsSystem (new G4OpenGLStoredQt);
#endif

#ifdef G4VIS_USE_OPENGLXM
  RegisterGraphicsSystem (new G4OpenGLImmediateXm);
  RegisterGraphicsSystem (new G4OpenGLStoredXm);
#endif

#ifdef G4VIS_USE_OIX
  RegisterGraphicsSystem (new G4OpenInventorX);
#endif

#ifdef G4VIS_USE_OIWIN32
  RegisterGraphicsSystem (new G4OpenInventorWin32);
#endif

#ifdef G4VIS_USE_VRML
  RegisterGraphicsSystem (new G4VRML1);
  RegisterGraphicsSystem (new G4VRML2);
#endif

#ifdef G4VIS_USE_VRMLFILE
  RegisterGraphicsSystem (new G4VRML1File);
  RegisterGraphicsSystem (new G4VRML2File);
#endif

#ifdef G4VIS_USE_RAYTRACER
  RegisterGraphicsSystem (new G4RayTracer);
#endif

#ifdef G4VIS_USE_RAYTRACERX
  RegisterGraphicsSystem (new G4RayTracerX);
#endif


// Duplicate printout
//  if (fVerbose > 0) {
//    G4cout <<
//      "\nYou have successfully chosen to use the following graphics systems."
//	 << G4endl;
//    PrintAvailableGraphicsSystems ();
//  }
}

#endif

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void
BDSVisManager::RegisterModelFactories() {}

BDSVisManager::~BDSVisManager(){}
