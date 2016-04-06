#include "BDSOutputROOTEventModel.hh"

ClassImp(BDSOutputROOTEventModel)

BDSOutputROOTEventModel::BDSOutputROOTEventModel()
{
}

BDSOutputROOTEventModel::~BDSOutputROOTEventModel()
{
}

#ifndef __MAKECINT__
void BDSOutputROOTEventModel::Fill()
{
  // get accelerator model
  BDSBeamline *beamline = BDSAcceleratorModel::Instance()->GetFlatBeamline();

  // iterate over flat beamline
  for(auto i = beamline->begin(); i != beamline->end(); ++i)
  {
    // Name
    this->componentName.push_back((*i)->GetName());
    this->placementName.push_back((*i)->GetPlacementName());

    // Length
    this->length.push_back((float &&) (*i)->GetAcceleratorComponent()->GetArcLength());

    // Positions
    G4ThreeVector p;
    p = (*i)->GetPositionStart();
    this->staPos.push_back(TVector3(p.getX(),p.getY(),p.getZ()));
    p = (*i)->GetPositionMiddle();
    this->midPos.push_back(TVector3(p.getX(),p.getY(),p.getZ()));
    p = (*i)->GetPositionEnd();
    this->endPos.push_back(TVector3(p.getX(),p.getY(),p.getZ()));

    // Rotations
    G4RotationMatrix *gr;
    gr = (*i)->GetRotationStart();
    TRotation rr = TRotation();
    rr.SetXEulerAngles(gr->getTheta(),gr->getPhi(),gr->getPsi());
    this->staRot.push_back(rr);
    gr = (*i)->GetRotationMiddle();
    rr.SetXEulerAngles(gr->getTheta(),gr->getPhi(),gr->getPsi());
    this->midRot.push_back(rr);
    gr = (*i)->GetRotationEnd();
    rr.SetXEulerAngles(gr->getTheta(),gr->getPhi(),gr->getPsi());
    this->endRot.push_back(rr);

    // Reference orbit positions
    p = (*i)->GetReferencePositionStart();
    this->staRefPos.push_back(TVector3(p.getX(),p.getY(),p.getZ()));
    p = (*i)->GetReferencePositionMiddle();
    this->midRefPos.push_back(TVector3(p.getX(),p.getY(),p.getZ()));
    p = (*i)->GetReferencePositionEnd();
    this->endRefPos.push_back(TVector3(p.getX(),p.getY(),p.getZ()));

    // Reference orbit rotations
    gr = (*i)->GetReferenceRotationStart();
    rr.SetXEulerAngles(gr->getTheta(),gr->getPhi(),gr->getPsi());
    this->staRefRot.push_back(rr);
    gr = (*i)->GetReferenceRotationMiddle();
    rr.SetXEulerAngles(gr->getTheta(),gr->getPhi(),gr->getPsi());
    this->midRefRot.push_back(rr);
    gr = (*i)->GetReferenceRotationEnd();
    rr.SetXEulerAngles(gr->getTheta(),gr->getPhi(),gr->getPsi());
    this->endRefRot.push_back(rr);

    // S positions
    this->staS.push_back((float &&) (*i)->GetSPositionStart());
    this->midS.push_back((float &&) (*i)->GetSPositionMiddle());
    this->endS.push_back((float &&) (*i)->GetSPositionEnd());
  }
}
#endif