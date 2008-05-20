/* BDSIM, v0.4   

19 May 2008 by Marchiori G.
11 Oct 2007 by Steve Malton

*/

const int DEBUG = 0;

//==============================================================



#include "BDSGlobalConstants.hh" // must be first in include list

#include "BDSMultipoleMagField.hh"

#include "G4Navigator.hh"
#include "G4TransportationManager.hh"

ofstream testf;


BDSMultipoleMagField::BDSMultipoleMagField(list<G4double> kn, list<G4double> ks)
{

  if (DEBUG) {
    G4cout<<"Creating BDSMultipoleMagField"<<G4endl;
    G4cout<<"size="<<kn.size()<<G4endl;
  }

  // compute magnetic rigidity brho
  // formula: B(Tesla)*rho(m) = p(GeV)/(0.299792458 * |charge(e)|)
  //
  // charge (in |e| units)
  G4double charge = BDSGlobals->GetParticleDefinition()->GetPDGCharge();
  // momentum (in GeV/c)
  G4double momentum = (BDSGlobals->GetBeamMomentum())/GeV;
  // rigidity (in T*m)
  G4double brho = ( momentum / (0.299792458 * fabs(charge)));
  // rigidity (in Geant4 units)
  brho *= (tesla*m);
  if (DEBUG) {
    G4cout<<"beam charge="<<charge<<"e"<<G4endl;
    G4cout<<"beam momentum="<<momentum<<"GeV/c"<<G4endl;
    G4cout<<"rigidity="<<brho/(tesla*m)<<"T*m"<<G4endl;
  }

  // convert strengths Kn from BDSIM units (m^-n) to Geant4 internal units
  // and compute coefficients of field expansion
  // Bn = Brho * Kn 
  // in Geant4 units
  bn = kn;
  bs = ks;
  list<G4double>::iterator it;
  list<G4double>::iterator its;
  int n(0);
  for(it=bn.begin(), its=bs.begin();it!=bn.end();it++, its++)
    {
      n++;
      if (DEBUG) {
	G4cout<<2*n<<"-pole strengths:"<<G4endl;
	G4cout<<"Kn : "<<(*it )<<"m^-"<<n<<G4endl;
	G4cout<<"Ks : "<<(*its)<<"m^-"<<n<<G4endl;
      }
      (*it) *= brho/pow(m,n);
      (*its) *= brho/pow(m,n);
      if (DEBUG) {
	G4cout<<2*n<<"-pole field coefficients:"<<G4endl;
	G4cout<<"Bn : "<<(*it )<<"T/m^"<<n-1<<G4endl;
	G4cout<<"Bs : "<<(*its)<<"T/m^"<<n-1<<G4endl;
      }
    }

  // write field map to debug file
  /*
  if (DEBUG) {
    G4cout<<"Writing field map to file field.txt"<<G4endl;

    testf.open("field.txt");

    G4double pt[4];
    G4double b[6];

    testf<<"x(cm) y(cm) Bx(T) By(T) Bz(T) "<<G4endl;

    for(G4double x=-1*cm;x<1*cm;x+=0.1*mm)
      for(G4double y=-1*cm;y<1*cm;y+=0.1*mm){
	pt[0]= x;
	pt[1]= y;
	pt[2]= pt[3]=0;
	GetFieldValue(pt,b);
	testf<<x/cm<<" "<<y/cm<<" "
	     <<b[0]/tesla<<" "<<b[1]/tesla<<" "<<b[2]/tesla<<G4endl;
      }
  }
  */
}

BDSMultipoleMagField::~BDSMultipoleMagField(){}

void BDSMultipoleMagField::GetFieldValue( const G4double *Point,
		       G4double *Bfield ) const
{
  if (DEBUG)
    G4cout<<"Called GetFieldValue at position (in global coordinates): ("
	  <<Point[0]/cm<<" "<<Point[1]/cm<<" "<<Point[2]/cm
	  <<")cm"<<G4endl;

  //
  // convert global to local coordinates
  //
  G4Navigator* MulNavigator=
    G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
  
  G4ThreeVector LocalR, GlobalR, LocalBField;
  
  GlobalR.setX(Point[0]);
  GlobalR.setY(Point[1]);
  GlobalR.setZ(Point[2]);
  G4AffineTransform GlobalAffine=MulNavigator->GetGlobalToLocalTransform();
  LocalR=GlobalAffine.TransformPoint(GlobalR); 

  if (DEBUG)
    G4cout<<"Current position in local coordinates: ("
	  <<LocalR[0]/cm<<" "<<LocalR[1]/cm<<" "<<LocalR[2]/cm
	  <<") cm"<<G4endl;
 
  //
  // compute B field in local coordinates
  // Bx = Br * cos(phi) - Bphi * sin(phi)
  // By = Br * sin(phi) + Bphi * cos(phi)
  // where, if n=1 is for dipole:
  // Br  (n) (normal) = +Bn/(n-1)! * r^(n-1) * sin(n*phi)
  // Bphi(n) (normal) = +Bn/(n-1)! * r^(n-1) * cos(n*phi)
  // Br  (n) (skewed) = +Bn/(n-1)! * r^(n-1) * cos(n*phi)
  // Bphi(n) (skewed) = -Bn/(n-1)! * r^(n-1) * sin(n*phi)

  G4double br = 0;
  G4double bphi = 0;

  list<G4double>::const_iterator it;
  list<G4double>::const_iterator its;

  G4double r = sqrt(LocalR[0]*LocalR[0] + LocalR[1]*LocalR[1]);
  G4double phi;
  if(fabs(r)>1.e-11*m) phi = atan2(LocalR[1],LocalR[0]);
  else phi = 0; // don't care

  if (DEBUG) 
    G4cout<<"In local coordinates, r= "<<r/m<<"m, phi="<<phi<<"rad"<<G4endl;


  G4int order=0;
  G4double fact = -1;


  // I want to use the strange convention of dipole coeff. with opposite sign -
  // then it is the same sign as angle 
  for(it=bn.begin(), its=bs.begin();it!=bn.end();it++, its++)
    {
      order++;

      br   += (*it ) * pow(r,order-1) * sin(order*phi) / fact; //normal
      br   -= (*its) * pow(r,order-1) * cos(order*phi) / fact; //skewed

      bphi += (*it ) * pow(r,order-1) * cos(order*phi) / fact; //normal
      bphi += (*its) * pow(r,order-1) * sin(order*phi) / fact; //skewed

      if(order==1) {br *= -1; bphi *=-1; }

      fact *= order;
    }
  
  LocalBField[0]=( br*cos(phi)-bphi*sin(phi) );
  LocalBField[1]=( br*sin(phi)+bphi*cos(phi) );
  LocalBField[2]=0;

  if (DEBUG) {
    G4cout<<"order="<<order<<G4endl;
    G4cout<<"In local coordinates:"<<G4endl
	  <<"Br="<<br/tesla<<"T, Bphi="<<bphi/tesla<<"T, "
	  <<"Bx="<<LocalBField[0]/tesla<<"T, By="<<LocalBField[1]/tesla<<"T"
	  <<G4endl;
  }

  //
  // convert B field to global coordinate system
  //
  G4AffineTransform LocalAffine = MulNavigator->GetLocalToGlobalTransform();
  G4ThreeVector GlobalBField = LocalAffine.TransformAxis(LocalBField);

  // currently BDSRK4Stepper - unlike the other steppers like BDSSextStepper -
  // wants B field in global coordinates (and in Geant4 units)
  Bfield[0]=GlobalBField.x();
  Bfield[1]=GlobalBField.y();
  Bfield[2]=GlobalBField.z();

  Bfield[3]=0.;
  Bfield[4]=0.;
  Bfield[5]=0.;
}



