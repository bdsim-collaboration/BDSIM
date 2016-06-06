#include "BDSOutputROOTEventModel.hh"

ClassImp(BDSOutputROOTEventModel)

BDSOutputROOTEventModel::BDSOutputROOTEventModel()
{
}

BDSOutputROOTEventModel::~BDSOutputROOTEventModel()
{
}

#ifndef __ROOTBUILD__
void BDSOutputROOTEventModel::Fill()
{
  // get accelerator model
  BDSBeamline *beamline = BDSAcceleratorModel::Instance()->GetFlatBeamline();

  double angle;
  CLHEP::Hep3Vector axis;
  // iterate over flat beamline
  for(auto i = beamline->begin(); i != beamline->end(); ++i)
  {
    // Name
    this->componentName.push_back((*i)->GetName());
    this->placementName.push_back((*i)->GetPlacementName());

    // Length
    this->length.push_back((float &&) (*i)->GetAcceleratorComponent()->GetArcLength() / CLHEP::m);

    // Positions
    G4ThreeVector p;
    p = (*i)->GetPositionStart();
    this->staPos.push_back(TVector3(p.getX() / CLHEP::m, p.getY() / CLHEP::m, p.getZ() / CLHEP::m));
    p = (*i)->GetPositionMiddle();
    this->midPos.push_back(TVector3(p.getX() / CLHEP::m, p.getY() / CLHEP::m, p.getZ() / CLHEP::m));
    p = (*i)->GetPositionEnd();
    this->endPos.push_back(TVector3(p.getX() / CLHEP::m, p.getY() / CLHEP::m, p.getZ() / CLHEP::m));

    // Rotations
    G4RotationMatrix *gr;
    gr = (*i)->GetRotationStart();
    TRotation rr = TRotation();
    rr.SetToIdentity();
    gr->getAngleAxis(angle,axis);
    rr.Rotate(angle,TVector3(axis.x(),axis.y(),axis.z()));
    this->staRot.push_back(rr);

    gr = (*i)->GetRotationMiddle();
    gr->getAngleAxis(angle,axis);
    rr.SetToIdentity();
    rr.Rotate(angle,TVector3(axis.x(),axis.y(),axis.z()));
    //G4cout << (*i)->GetName() << " " << angle << " " << axis.x() << " " << axis.y() << " " << axis.z() << G4endl;
    this->midRot.push_back(rr);

    gr = (*i)->GetRotationEnd();
    gr->getAngleAxis(angle,axis);
    rr.SetToIdentity();
    rr.Rotate(angle,TVector3(axis.x(),axis.y(),axis.z()));
    this->endRot.push_back(rr);

    // Reference orbit positions
    p = (*i)->GetReferencePositionStart();
    this->staRefPos.push_back(TVector3(p.getX() / CLHEP::m, p.getY() / CLHEP::m, p.getZ() / CLHEP::m));
    p = (*i)->GetReferencePositionMiddle();
    this->midRefPos.push_back(TVector3(p.getX() / CLHEP::m, p.getY() / CLHEP::m, p.getZ() / CLHEP::m));
    p = (*i)->GetReferencePositionEnd();
    this->endRefPos.push_back(TVector3(p.getX() / CLHEP::m, p.getY() / CLHEP::m, p.getZ() / CLHEP::m));

    // Reference orbit rotations
    gr = (*i)->GetReferenceRotationStart();
    gr->getAngleAxis(angle,axis);
    rr.SetToIdentity();
    rr.Rotate(angle,TVector3(axis.x(),axis.y(),axis.z()));
    this->staRefRot.push_back(rr);

    gr = (*i)->GetReferenceRotationMiddle();
    gr->getAngleAxis(angle,axis);
    rr.SetToIdentity();
    rr.Rotate(angle,TVector3(axis.x(),axis.y(),axis.z()));
    this->midRefRot.push_back(rr);

    gr = (*i)->GetReferenceRotationEnd();
    gr->getAngleAxis(angle,axis);
    rr.SetToIdentity();
    rr.Rotate(angle,TVector3(axis.x(),axis.y(),axis.z()));
    this->endRefRot.push_back(rr);

    // S positions
    this->staS.push_back((float &&) (*i)->GetSPositionStart()  / CLHEP::m);
    this->midS.push_back((float &&) (*i)->GetSPositionMiddle() / CLHEP::m);
    this->endS.push_back((float &&) (*i)->GetSPositionEnd()    / CLHEP::m);
  }
}
#endif
