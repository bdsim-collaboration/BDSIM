#include "BDSGlobalConstants.hh" 

#include "BDSMultipoleMagField.hh"

#include "G4Navigator.hh"
#include "G4ParticleDefinition.hh"
#include "G4TransportationManager.hh"
#include "BDSDebug.hh"

BDSMultipoleMagField::BDSMultipoleMagField(std::list<G4double> kn,
					   std::list<G4double> ks)
{

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
  G4cout<<"Creating BDSMultipoleMagField"<<G4endl;
  G4cout<<"size="<<kn.size()<<G4endl;
#endif

  // compute magnetic rigidity brho
  // formula: B(Tesla)*rho(m) = p(GeV)/(0.299792458 * |charge(e)|)
  //
  // charge (in |e| units)
  G4double charge = BDSGlobalConstants::Instance()->GetParticleDefinition()->GetPDGCharge();
  // momentum (in GeV/c)
  G4double momentum = (BDSGlobalConstants::Instance()->GetBeamMomentum())/CLHEP::GeV;
  // rigidity (in T*m)
  G4double brho = ( momentum / (0.299792458 * fabs(charge)));
  // rigidity (in Geant4 units)
  brho *= (CLHEP::tesla*CLHEP::m);
#ifdef BDSDEBUG 
    G4cout<<"beam charge="<<charge<<"e"<<G4endl;
    G4cout<<"beam momentum="<<momentum<<"GeV/c"<<G4endl;
    G4cout<<"rigidity="<<brho/(CLHEP::tesla*CLHEP::m)<<"T*m"<<G4endl;
#endif

  // convert strengths Kn from BDSIM units (m^-n) to Geant4 internal units
  // and compute coefficients of field expansion
  // Bn = Brho * Kn 
  // in Geant4 units
  bn = kn;
  bs = ks;
  std::list<G4double>::iterator it;
  std::list<G4double>::iterator its;
  int n(0);
  for(it=bn.begin(), its=bs.begin();it!=bn.end();it++, its++)
    {
      n++;
#ifdef BDSDEBUG 
      G4cout<<2*n<<"-pole strengths:"<<G4endl;
      G4cout<<"Kn : "<<(*it )<<"m^-"<<n<<G4endl;
      G4cout<<"Ks : "<<(*its)<<"m^-"<<n<<G4endl;
#endif
      (*it) *= brho/pow(CLHEP::m,n);
      (*its) *= brho/pow(CLHEP::m,n);
#ifdef BDSDEBUG 
      G4cout<<2*n<<"-pole field coefficients:"<<G4endl;
      G4cout<<"Bn : "<<(*it )<<"T/m^"<<n-1<<G4endl;
      G4cout<<"Bs : "<<(*its)<<"T/m^"<<n-1<<G4endl;
#endif
    }

  // write field map to debug file
  /*
    #ifdef BDSDEBUG 
    G4cout<<"Writing field map to file field.txt"<<G4endl;
    
    testf.open("field.txt");
    
    G4double pt[4];
    G4double b[6];
    
    testf<<"x(cm) y(cm) Bx(T) By(T) Bz(T) "<<G4endl;
    
    for(G4double x=-1*CLHEP::cm;x<1*CLHEP::cm;x+=0.1*CLHEP::mm)
    for(G4double y=-1*CLHEP::cm;y<1*CLHEP::cm;y+=0.1*CLHEP::mm){
    pt[0]= x;
    pt[1]= y;
    pt[2]= pt[3]=0;
	GetFieldValue(pt,b);
	testf<<x/CLHEP::cm<<" "<<y/CLHEP::cm<<" "
        <<b[0]/CLHEP::tesla<<" "<<b[1]/CLHEP::tesla<<" "<<b[2]/CLHEP::tesla<<G4endl;
        }
        #endif
  */
}

BDSMultipoleMagField::~BDSMultipoleMagField(){}

void BDSMultipoleMagField::GetFieldValue( const G4double *Point,
					  G4double *Bfield ) const
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
  G4cout<<"Called GetFieldValue at position (in global coordinates): ("
        <<Point[0]/CLHEP::cm<<" "<<Point[1]/CLHEP::cm<<" "<<Point[2]/CLHEP::cm
        <<")cm"<<G4endl;
#endif

  // convert global to local coordinates
  G4Navigator* MulNavigator=
    G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
  
  G4ThreeVector LocalR, GlobalR, LocalBField;
  
  GlobalR.setX(Point[0]);
  GlobalR.setY(Point[1]);
  GlobalR.setZ(Point[2]);
  G4AffineTransform GlobalAffine=MulNavigator->GetGlobalToLocalTransform();
  LocalR=GlobalAffine.TransformPoint(GlobalR); 

#ifdef BDSDEBUG
  G4cout<<"Current position in local coordinates: ("
        <<LocalR[0]/CLHEP::cm<<" "<<LocalR[1]/CLHEP::cm<<" "<<LocalR[2]/CLHEP::cm
        <<") cm"<<G4endl;
#endif
 
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

  std::list<G4double>::const_iterator it;
  std::list<G4double>::const_iterator its;

  G4double r = sqrt(LocalR[0]*LocalR[0] + LocalR[1]*LocalR[1]);
  G4double phi;
  if(fabs(r)>1.e-11*CLHEP::m) phi = atan2(LocalR[1],LocalR[0]);
  else phi = 0; // don't care

#ifdef BDSDEBUG 
  G4cout<<"In local coordinates, r= "<<r/CLHEP::m<<"m, phi="<<phi<<"rad"<<G4endl;
#endif

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

#ifdef BDSDEBUG 
  G4cout<<"order="<<order<<G4endl;
  G4cout<<"In local coordinates:"<<G4endl
        <<"Br="<<br/CLHEP::tesla<<"T, Bphi="<<bphi/CLHEP::tesla<<"T, "
        <<"Bx="<<LocalBField[0]/CLHEP::tesla<<"T, By="<<LocalBField[1]/CLHEP::tesla<<"T"
        <<G4endl;
#endif

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



