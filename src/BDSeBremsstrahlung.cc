//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * The following disclaimer summarizes all the specific disclaimers *
// * of contributors to this software. The specific disclaimers,which *
// * govern, are listed with their locations in:                      *
// *   http://cern.ch/geant4/license                                  *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//
//
// $Id: BDSeBremsstrahlung.cc,v 1.3 2005/08/18 16:49:41 agapov Exp $
// GEANT4 tag $Name:  $
//
//
//      ------------ BDSeBremsstrahlung physics process --------
//                     by Michel Maire, 24 July 1996
//
// 26-09-96 extension of the total crosssection above 100 GeV, M.Maire
//  1-10-96 new type G4OrderedTable; ComputePartialSumSigma(), M.Maire
// 16-10-96 DoIt() call to the non static GetEnergyCuts(), L.Urban
// 13-12-96 Sign corrected in grejmax and greject
//          error definition of screenvar, L.Urban
// 20-03-97 new energy loss+ionisation+brems scheme, L.Urban
// 07-04-98 remove 'tracking cut' of the diffracted particle, MMa
// 13-08-98 new methods SetBining() PrintInfo()
// 03-03-99 Bug fixed in LPM effect, L.Urban
// 10-02-00 modifications , new e.m. structure, L.Urban
// 07-08-00 new cross section/en.loss parametrisation, LPM flag , L.Urban
// 21-09-00 corrections in the LPM implementation, L.Urban
// 28-05-01 V.Ivanchenko minor changes to provide ANSI -wall compilation
// 09-08-01 new methods Store/Retrieve PhysicsTable (mma)
// 17-09-01 migration of Materials to pure STL (mma)
// 21-09-01 completion of RetrievePhysicsTable() (mma)
// 29-10-01 all static functions no more inlined (mma)
// 08-11-01 particleMass becomes a local variable
// 11-11-02 fix of division by 0 (VI)
// 16-01-03 Migrade to cut per region (V.Ivanchenko)
// 26-04-03 fix problems of retrieve tables (V.Ivanchenko)
//
// --------------------------------------------------------------

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#include "BDSeBremsstrahlung.hh"
#include "G4EnergyLossTables.hh"
#include "G4ios.hh"
#include "G4UnitsTable.hh"
#include "G4ProductionCutsTable.hh"

// gab>>
G4bool BDSeBremsFiredThisEvent;
G4double BDSeBremFireDist;
G4double BDSeBremZMin,BDSeBremZMax;
//<< gab


G4double BDSeBremsstrahlung::LowerBoundLambda = 1.*keV;
G4double BDSeBremsstrahlung::UpperBoundLambda = 100.*TeV;
G4int	 BDSeBremsstrahlung::NbinLambda = 100;
G4double BDSeBremsstrahlung::probsup = 1.00;
G4bool   BDSeBremsstrahlung::LPMflag = true;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
 
// constructor
 
BDSeBremsstrahlung::BDSeBremsstrahlung(const G4String& processName)
  : G4VeEnergyLoss(processName),      // initialization
    theMeanFreePathTable(NULL)
{ // MinThreshold = 10*keV;
 MinThreshold = 1*keV; }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

// destructor

BDSeBremsstrahlung::~BDSeBremsstrahlung()
{
  if (theMeanFreePathTable) {
    theMeanFreePathTable->clearAndDestroy();
    delete theMeanFreePathTable;
  }

  PartialSumSigma.clearAndDestroy();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BDSeBremsstrahlung::SetLowerBoundLambda(G4double val) 
     {LowerBoundLambda = val;}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
    
void BDSeBremsstrahlung::SetUpperBoundLambda(G4double val) 
     {UpperBoundLambda = val;}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
    
void BDSeBremsstrahlung::SetNbinLambda(G4int n)            
     {NbinLambda = n;}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
    
G4double BDSeBremsstrahlung::GetLowerBoundLambda()         
         {return LowerBoundLambda;}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double BDSeBremsstrahlung::GetUpperBoundLambda()
         {return UpperBoundLambda;}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
    
G4int BDSeBremsstrahlung::GetNbinLambda()                  
      {return NbinLambda;}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BDSeBremsstrahlung::SetLPMflag(G4bool val)
     {LPMflag = val;}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
    
G4bool BDSeBremsstrahlung::GetLPMflag()
       {return LPMflag;}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BDSeBremsstrahlung::BuildPhysicsTable(const G4ParticleDefinition& aParticleType)
{
  if( !CutsWhereModified() && theLossTable) return;

  LowestKineticEnergy  = GetLowerBoundEloss();
  HighestKineticEnergy = GetUpperBoundEloss();
  TotBin               = GetNbinEloss();

  BuildLossTable(aParticleType);

  if (&aParticleType==G4Electron::Electron())
   {
    RecorderOfElectronProcess[CounterOfElectronProcess] = (*this).theLossTable;
    CounterOfElectronProcess++;
   }
  else
   {
    RecorderOfPositronProcess[CounterOfPositronProcess] = (*this).theLossTable;
    CounterOfPositronProcess++;
   }

  BuildLambdaTable(aParticleType);

  BuildDEDXTable  (aParticleType);

  if (&aParticleType==G4Electron::Electron()) PrintInfoDefinition();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void BDSeBremsstrahlung::BuildLossTable(const G4ParticleDefinition& aParticleType)
{
  G4double KineticEnergy,TotalEnergy,bremloss,Z,x,
           losslim,loss,rate,natom,Cut;

  const G4double MinKinEnergy = 1.*keV;
  const G4double Thigh = 100.*GeV;
  const G4double Cuthigh = 50.*GeV;
  const G4double Factorhigh = 36./(1450.*GeV);
  const G4double coef1 = -0.5, coef2 = 2./9.;

  G4double particleMass = aParticleType.GetPDGMass() ;

  const G4ProductionCutsTable* theCoupleTable=
        G4ProductionCutsTable::GetProductionCutsTable();
  size_t numOfCouples = theCoupleTable->GetTableSize();

  if (theLossTable) {theLossTable->clearAndDestroy(); delete theLossTable;}
  theLossTable = new G4PhysicsTable(numOfCouples);

  secondaryEnergyCuts = theCoupleTable->GetEnergyCutsVector(0);

  //  loop for materials
  //
  for (size_t J=0; J<numOfCouples; J++)
   {

     G4PhysicsLogVector* aVector = new G4PhysicsLogVector(
                               LowestKineticEnergy,HighestKineticEnergy,TotBin);

     // get elements in the material
     const G4MaterialCutsCouple* couple = theCoupleTable->GetMaterialCutsCouple(J);
     const G4Material* material= couple->GetMaterial();

     const G4ElementVector* theElementVector = material->GetElementVector();
     const G4double* theAtomicNumDensityVector = material->GetAtomicNumDensityVector();
     const G4int NumberOfElements = material->GetNumberOfElements();

       //  loop for the kinetic energy values
       for (G4int i=0; i<TotBin; i++)
         {
          KineticEnergy = aVector->GetLowEdgeEnergy(i) ;
          TotalEnergy = KineticEnergy+particleMass ;
          Cut = SecondaryEnergyThreshold(J);
          if (Cut < MinThreshold)  Cut = MinThreshold;
          if (Cut > KineticEnergy) Cut = KineticEnergy;

          bremloss = 0.;

          if (KineticEnergy>MinKinEnergy)
            {
             //  loop for elements in the material
             for (G4int iel=0; iel<NumberOfElements; iel++)
               {
                Z=(*theElementVector)[iel]->GetZ();
                natom = theAtomicNumDensityVector[iel] ;
                if (KineticEnergy <= Thigh)
                  {
                   //loss for MinKinEnergy<KineticEnergy<=100 GeV
                   x=log(TotalEnergy/particleMass);
                   loss = ComputeBremLoss(Z,natom,KineticEnergy,Cut,x) ;
                   if (&aParticleType==G4Positron::Positron())
                      loss *= ComputePositronCorrFactorLoss(Z,KineticEnergy,Cut) ;   
                  }
                else
                  {
                   // extrapolation for KineticEnergy>100 GeV
                   x=log(Thigh/particleMass) ;
                   if (Cut<Thigh)
                     {
                      losslim = ComputeBremLoss(Z,natom,Thigh,Cut,x) ;
                      if (&aParticleType==G4Positron::Positron())
                         loss *= ComputePositronCorrFactorLoss(Z,Thigh,Cut) ;
                      rate = Cut/TotalEnergy ;
                      loss = losslim*(1.+coef1*rate+coef2*rate*rate) ;
                      rate = Cut/Thigh ;
                      loss /= (1.+coef1*rate+coef2*rate*rate) ;
                     }
                   else
                     {
                      losslim = ComputeBremLoss(Z,natom,Thigh,Cuthigh,x) ;  
                      if (&aParticleType==G4Positron::Positron())
                         loss *= ComputePositronCorrFactorLoss(Z,Thigh,Cuthigh) ;   
                      rate = Cut/TotalEnergy ;
                      loss = losslim*(1.+coef1*rate+coef2*rate*rate) ;
                      loss *= Factorhigh*Cut ;
                     }

                  }
                bremloss += natom*loss;
               }

            }

	   static const G4double MigdalConstant = classic_electr_radius
                                                 *electron_Compton_length
                                                 *electron_Compton_length/pi;
	   G4double TotalEnergy = KineticEnergy+electron_mass_c2 ;
	   G4double kp2 = MigdalConstant*TotalEnergy*TotalEnergy*
                                     (material->GetElectronDensity()) ;

           // now compute the correction due to the supression(s)
           G4double kmin = 1.*eV ;
           G4double kmax = Cut ;

           if(kmax > kmin)
           {

             G4double floss = 0. ;
             G4int nmax = 100 ;

             G4double vmin=log(kmin);
             G4double vmax=log(kmax) ;
             G4int nn = (G4int)(nmax*(vmax-vmin)/(log(HighestKineticEnergy)-vmin)) ;
             G4double u,fac,c,v,dv ;
            if(nn > 0)
            {
             dv = (vmax-vmin)/nn ;
             v = vmin-dv ;
             for(G4int n=0; n<=nn; n++)
             {
               v += dv;  u = exp(v);               
               fac = u*SupressionFunction(material,KineticEnergy,u);
               probsup = 1.;
	       fac *= probsup*(u*u/(u*u+kp2))+1.-probsup;
               if ((n==0)||(n==nn)) c=0.5;
               else                 c=1. ;
               fac   *= c ;
               floss += fac ;
             }
             floss *=dv/(kmax-kmin); 
            }
            else floss = 1.;
            if(floss > 1.) floss = 1.;
            // correct the loss
            bremloss *= floss;
          }
          
          if(bremloss < 0.) bremloss = 0.;
          aVector->PutValue(i,bremloss);  
        }

       theLossTable->insert(aVector);
    }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4double BDSeBremsstrahlung::ComputeBremLoss(G4double Z,G4double,
                         G4double T,G4double Cut,G4double)

// compute loss due to soft brems
{
  static const G4double beta=1.00,ksi=2.00  ;
  static const G4double clossh = 0.254 , closslow = 1./3. , alosslow = 1. ;
  static const G4double Tlim= 10.*MeV ;

  static const G4double xlim = 1.2 ;
  static const G4int NZ = 8 ;
  static const G4int Nloss = 11 ;
  static const G4double ZZ[NZ] =
        {2.,4.,6.,14.,26.,50.,82.,92.};
  static const G4double coefloss[NZ][Nloss] = {
  // Z=2
 { 0.98916,        0.47564,        -0.2505,       -0.45186,        0.14462,
   0.21307,      -0.013738,      -0.045689,     -0.0042914,      0.0034429,
   0.00064189},

  // Z=4
 { 1.0626,        0.37662,       -0.23646,       -0.45188,        0.14295,
   0.22906,      -0.011041,      -0.051398,     -0.0055123,      0.0039919,
   0.00078003},
  // Z=6
 { 1.0954,          0.315,       -0.24011,       -0.43849,        0.15017,
   0.23001,      -0.012846,      -0.052555,     -0.0055114,      0.0041283,
   0.00080318},

  // Z=14
 { 1.1649,        0.18976,       -0.24972,       -0.30124,         0.1555,
   0.13565,      -0.024765,      -0.027047,    -0.00059821,      0.0019373,
   0.00027647},

  // Z=26
 { 1.2261,        0.14272,       -0.25672,       -0.28407,        0.13874,
   0.13586,      -0.020562,      -0.026722,    -0.00089557,      0.0018665,
   0.00026981},

  // Z=50
 { 1.3147,       0.020049,       -0.35543,       -0.13927,        0.17666,
   0.073746,      -0.036076,      -0.013407,      0.0025727,     0.00084005,
  -1.4082e-05},

  // Z=82
 { 1.3986,       -0.10586,       -0.49187,     -0.0048846,        0.23621,
   0.031652,      -0.052938,     -0.0076639,      0.0048181,     0.00056486,
  -0.00011995},

  // Z=92
 { 1.4217,         -0.116,       -0.55497,      -0.044075,        0.27506,
   0.081364,      -0.058143,      -0.023402,      0.0031322,      0.0020201,
   0.00017519}

    } ;
 static G4double aaa=0.414 ;
 static G4double bbb=0.345 ;
 static G4double ccc=0.460 ;

  G4int iz = 0;
  G4double delz = 1.e6;
  for (G4int ii=0; ii<NZ; ii++)
    {
      if(fabs(Z-ZZ[ii]) < delz)  { iz = ii; delz = fabs(Z-ZZ[ii]);}
    }

  G4double xx = log10(T);
  G4double fl = 1.;

  if (xx <= xlim)
    {
      fl = coefloss[iz][Nloss-1];
      for (G4int j=Nloss-2; j>=0; j--) fl = fl*xx+coefloss[iz][j];
      if (fl < 0.) fl = 0.;
    }

  G4double loss;
  G4double E = T+electron_mass_c2 ;

  loss = Z*(Z+ksi)*E*E/(T+E)*exp(beta*log(Cut/T))*(2.-clossh*exp(log(Z)/4.));
  if (T <= Tlim) loss /= exp(closslow*log(Tlim/T));
  if( T <= Cut)  loss *= exp(alosslow*log(T/Cut));

  //  correction ................................
  loss *= (aaa+bbb*T/Tlim)/(1.+ccc*T/Tlim);
  loss *= fl;
  loss /= Avogadro;

  return loss;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4double BDSeBremsstrahlung::ComputePositronCorrFactorLoss(
                            G4double Z,G4double KineticEnergy,G4double GammaCut)

//calculates the correction factor for the energy loss due to bremsstrahlung for positrons
//the same correction is in the (discrete) bremsstrahlung

{
  static const G4double K = 132.9416*eV ;
  static const G4double a1=4.15e-1, a3=2.10e-3, a5=54.0e-5 ;

  G4double x   = log(KineticEnergy/(K*Z*Z)), x2 = x*x, x3 = x2*x;
  G4double eta = 0.5+atan(a1*x+a3*x3+a5*x3*x2)/pi;
  G4double e0  = GammaCut/KineticEnergy;

  G4double factor(0.);
  if (e0!=1.0) { factor=log(1.-e0)/eta; factor=exp(factor);}
  factor = eta*(1.-factor)/e0;

  return factor;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void BDSeBremsstrahlung::BuildLambdaTable(
                  const G4ParticleDefinition& ParticleType)
{
   G4double LowEdgeEnergy , Value;
   G4double FixedEnergy = (LowerBoundLambda + UpperBoundLambda)/2.;

   //create table
   //
   const G4ProductionCutsTable* theCoupleTable=
         G4ProductionCutsTable::GetProductionCutsTable();
   size_t numOfCouples = theCoupleTable->GetTableSize();

   //create table
   if (theMeanFreePathTable) {theMeanFreePathTable->clearAndDestroy();
                              delete theMeanFreePathTable;
                             }
   theMeanFreePathTable = new G4PhysicsTable(numOfCouples);

   PartialSumSigma.clearAndDestroy();
   PartialSumSigma.resize(numOfCouples);

   G4PhysicsLogVector* ptrVector;
   for ( size_t J=0; J<numOfCouples; J++ )
       {
        //create physics vector then fill it ....
        ptrVector = new G4PhysicsLogVector(LowerBoundLambda, UpperBoundLambda,
                                           NbinLambda ) ;

        const G4MaterialCutsCouple* couple = theCoupleTable->GetMaterialCutsCouple(J);

        for ( G4int i = 0 ; i < NbinLambda ; i++ )
           {
             LowEdgeEnergy = ptrVector->GetLowEdgeEnergy( i ) ;
             Value = ComputeMeanFreePath( &ParticleType, LowEdgeEnergy,couple);
             ptrVector->PutValue( i , Value ) ;
           }

        theMeanFreePathTable->insertAt( J , ptrVector );

        // Compute the PartialSumSigma table at a given fixed energy
        ComputePartialSumSigma( &ParticleType, FixedEnergy, couple) ;
   }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4double BDSeBremsstrahlung::ComputeMeanFreePath(
                                  const G4ParticleDefinition* ParticleType,
                                  G4double KineticEnergy,
                                  const G4MaterialCutsCouple* couple)
{
  const G4Material* aMaterial = couple->GetMaterial();
  const G4ElementVector* theElementVector = aMaterial->GetElementVector() ;
  const G4double* theAtomNumDensityVector = aMaterial->GetAtomicNumDensityVector();
  G4double GammaEnergyCut = SecondaryEnergyThreshold(couple->GetIndex());
  if (GammaEnergyCut < MinThreshold) GammaEnergyCut = MinThreshold;

  G4double SIGMA = 0;

  for ( size_t i=0 ; i < aMaterial->GetNumberOfElements() ; i++ )
     {
       SIGMA += theAtomNumDensityVector[i] *
                ComputeCrossSectionPerAtom( ParticleType, KineticEnergy,
                                           (*theElementVector)[i]->GetZ(),
                                           GammaEnergyCut );
     }

           // now compute the correction due to the supression(s)

           G4double kmax = KineticEnergy ;
           G4double kmin = GammaEnergyCut ;

           static const G4double MigdalConstant = classic_electr_radius
                                                 *electron_Compton_length
                                                 *electron_Compton_length/pi;
           G4double TotalEnergy = KineticEnergy+electron_mass_c2 ;
           G4double kp2 = MigdalConstant*TotalEnergy*TotalEnergy*
                                     (aMaterial->GetElectronDensity()) ;

           if(kmax > kmin)
           {
            G4double fsig = 0.;
            G4int nmax = 100 ;
            G4double vmin=log(kmin);
            G4double vmax=log(kmax) ;
            G4int nn = (G4int)(nmax*(vmax-vmin)/(log(HighestKineticEnergy)-vmin));
            G4double u,fac,c,v,dv,y ;
            if(nn > 0)
            {
              dv = (vmax-vmin)/nn ;
              v = vmin-dv ;

             for(G4int n=0; n<=nn; n++)
             {
               v += dv;  u = exp(v);
               fac = SupressionFunction(aMaterial,KineticEnergy,u);
               y = u/kmax;
               fac *= (4.-4.*y+3.*y*y)/3.;
               fac *= probsup*(u*u/(u*u+kp2))+1.-probsup;
               if ((n==0)||(n==nn)) c=0.5;
               else                 c=1. ;
               fac  *= c;
               fsig += fac;
             }
             y = kmin/kmax ;
             fsig *=dv/(-4.*log(y)/3.-4.*(1.-y)/3.+0.5*(1.-y*y));
            }
            else fsig = 1.;
            if (fsig > 1.) fsig = 1.;
            // correct the cross section
            SIGMA *= fsig;
          }

  return SIGMA > DBL_MIN ? 1./SIGMA : DBL_MAX;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4double BDSeBremsstrahlung::ComputeCrossSectionPerAtom(
                                     const G4ParticleDefinition* ParticleType,
                                           G4double KineticEnergy, G4double AtomicNumber,
                                           G4double GammaEnergyCut)

// Calculates the cross section per atom in GEANT4 internal units.
//

{
  G4double CrossSection = 0.0 ;
  if ( KineticEnergy < 1*keV ) return CrossSection;
  if ( KineticEnergy <= GammaEnergyCut ) return CrossSection;

  static const G4double ksi=2.0, alfa=1.00;
  static const G4double csigh = 0.127, csiglow = 0.25, asiglow = 0.020*MeV ;
  static const G4double Tlim = 10.*MeV ;

  static const G4double xlim = 1.2 ;
  static const G4int NZ = 8 ;
  static const G4int Nsig = 11 ;
  static const G4double ZZ[NZ] =
        {2.,4.,6.,14.,26.,50.,82.,92.} ;
  static const G4double coefsig[NZ][Nsig] = {
  // Z=2
  { 0.4638,        0.37748,        0.32249,      -0.060362,      -0.065004,
   -0.033457,      -0.004583,       0.011954,      0.0030404,     -0.0010077,
   -0.00028131},

  // Z=4
  { 0.50008,        0.33483,        0.34364,      -0.086262,      -0.055361,
   -0.028168,     -0.0056172,       0.011129,      0.0027528,    -0.00092265,
   -0.00024348},

  // Z=6
  { 0.51587,        0.31095,        0.34996,       -0.11623,      -0.056167,
   -0.0087154,     0.00053943,      0.0054092,     0.00077685,    -0.00039635,
   -6.7818e-05},

  // Z=14
  { 0.55058,        0.25629,        0.35854,      -0.080656,      -0.054308,
   -0.049933,    -0.00064246,       0.016597,      0.0021789,      -0.001327,
   -0.00025983},

  // Z=26
  { 0.5791,        0.26152,        0.38953,       -0.17104,      -0.099172,
    0.024596,       0.023718,     -0.0039205,     -0.0036658,     0.00041749,
    0.00023408},

  // Z=50
  { 0.62085,        0.27045,        0.39073,       -0.37916,       -0.18878,
    0.23905,       0.095028,      -0.068744,      -0.023809,      0.0062408,
    0.0020407},

  // Z=82
  { 0.66053,        0.24513,        0.35404,       -0.47275,       -0.22837,
    0.35647,        0.13203,        -0.1049,      -0.034851,      0.0095046,
    0.0030535},

  // Z=92
  { 0.67143,        0.23079,        0.32256,       -0.46248,       -0.20013,
    0.3506,        0.11779,        -0.1024,      -0.032013,      0.0092279,
    0.0028592}

    } ;

  G4int iz = 0 ;
  G4double delz = 1.e6 ;
  for (G4int ii=0; ii<NZ; ii++)
  {
    if(fabs(AtomicNumber-ZZ[ii]) < delz)
    {
      iz = ii ;
      delz = fabs(AtomicNumber-ZZ[ii]) ;
    }
  }

  G4double xx = log10(KineticEnergy) ;
  G4double fs = 1. ;

  if(xx <= xlim)
  {
	  fs = coefsig[iz][Nsig-1] ;
		for (G4int j=Nsig-2; j>=0; j--)
          {
		  fs = fs*xx+coefsig[iz][j] ;
          }
		if(fs < 0.) fs = 0. ;
  }


 CrossSection = AtomicNumber*(AtomicNumber+ksi)*
                (1.-csigh*exp(log(AtomicNumber)/4.))*
                 pow(log(KineticEnergy/GammaEnergyCut),alfa) ;

 if(KineticEnergy <= Tlim)
     CrossSection *= exp(csiglow*log(Tlim/KineticEnergy))*
                     (1.+asiglow/(sqrt(AtomicNumber)*KineticEnergy)) ;

 if (ParticleType == G4Positron::Positron())
     CrossSection *= ComputePositronCorrFactorSigma(AtomicNumber, KineticEnergy,
                                                             GammaEnergyCut);
 CrossSection *= fs ;
 CrossSection /= Avogadro ;

 if (CrossSection < 0.) CrossSection = 0.;
 return CrossSection;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4double BDSeBremsstrahlung::ComputePositronCorrFactorSigma( G4double AtomicNumber,
                                           G4double KineticEnergy, G4double GammaEnergyCut)

// Calculates the correction factor for the total cross section of the positron bremsstrahl.
// Eta is the ratio of positron to electron energy loss by bremstrahlung.
// A parametrized formula from L. Urban is used to estimate eta. It is a fit to the results
// of L. Kim & al: Phys Rev. A33,3002 (1986)

{
 static const G4double K = 132.9416*eV;
 static const G4double a1 = 4.15e-1, a3 = 2.10e-3, a5 = 54.0e-5;

 G4double x = log(KineticEnergy/(K*AtomicNumber*AtomicNumber));
 G4double eta = 0.5 + atan(a1*x + a3*x*x*x + a5*x*x*x*x*x)/pi ;
 G4double alfa = (1. - eta)/eta;
 return eta*pow((1. - GammaEnergyCut/KineticEnergy) , alfa);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void BDSeBremsstrahlung::ComputePartialSumSigma(const G4ParticleDefinition* ParticleType,
                                               G4double KineticEnergy,
                                               const G4MaterialCutsCouple* couple)

// Build the table of cross section per element. The table is built for MATERIALS.
// This table is used by DoIt to select randomly an element in the material.
{
   const G4Material* aMaterial= couple->GetMaterial();
   G4int NbOfElements = aMaterial->GetNumberOfElements();
   const G4ElementVector* theElementVector = aMaterial->GetElementVector();
   const G4double* theAtomNumDensityVector = aMaterial->GetAtomicNumDensityVector();
   size_t index = couple->GetIndex();
   G4double GammaEnergyCut = SecondaryEnergyThreshold(index);

   PartialSumSigma[index] = new G4DataVector();

   G4double SIGMA = 0. ;

   for ( G4int Ielem=0 ; Ielem < NbOfElements ; Ielem++ )
      {
        SIGMA += theAtomNumDensityVector[Ielem] *
                 ComputeCrossSectionPerAtom( ParticleType, KineticEnergy,
                                            (*theElementVector)[Ielem]->GetZ(),
                                             GammaEnergyCut );
        PartialSumSigma[index]->push_back(SIGMA);
   }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4VParticleChange* BDSeBremsstrahlung::PostStepDoIt(const G4Track& trackData,
                                                   const G4Step& stepData)
//
// The emitted gamma energy is sampled using a parametrized formula from L. Urban.
// This parametrization is derived from :
//    cross-section values of Seltzer and Berger for electron energies 1 keV - 10 GeV,
//    screened Bethe Heilter differential cross section above 10 GeV,
//    Migdal corrections in both case.
//  Seltzer & Berger: Nim B 12:95 (1985)
//  Nelson, Hirayama & Rogers: Technical report 265 SLAC (1985)
//  Migdal: Phys Rev 103:1811 (1956); Messel & Crawford: Pergamon Press (1970)
//
// A modified version of the random number techniques of Butcher & Messel is used
//    (Nuc Phys 20(1960),15).
//
// GEANT4 internal units.
//
{
  static const G4double
     ah10 = 4.67733E+00, ah11 =-6.19012E-01, ah12 = 2.02225E-02,
     ah20 =-7.34101E+00, ah21 = 1.00462E+00, ah22 =-3.20985E-02,
     ah30 = 2.93119E+00, ah31 =-4.03761E-01, ah32 = 1.25153E-02;

  static const G4double
     bh10 = 4.23071E+00, bh11 =-6.10995E-01, bh12 = 1.95531E-02,
     bh20 =-7.12527E+00, bh21 = 9.69160E-01, bh22 =-2.74255E-02,
     bh30 = 2.69925E+00, bh31 =-3.63283E-01, bh32 = 9.55316E-03;

  static const G4double
     al00 =-2.05398E+00, al01 = 2.38815E-02, al02 = 5.25483E-04,
     al10 =-7.69748E-02, al11 =-6.91499E-02, al12 = 2.22453E-03,
     al20 = 4.06463E-02, al21 =-1.01281E-02, al22 = 3.40919E-04;

  static const G4double
     bl00 = 1.04133E+00, bl01 =-9.43291E-03, bl02 =-4.54758E-04,
     bl10 = 1.19253E-01, bl11 = 4.07467E-02, bl12 =-1.30718E-03,
     bl20 =-1.59391E-02, bl21 = 7.27752E-03, bl22 =-1.94405E-04;

  static const G4double MigdalConstant = classic_electr_radius
                                        *electron_Compton_length
                                        *electron_Compton_length/pi;
  // const G4double LPMconstant = fine_structure_const*electron_mass_c2*
  //                              electron_mass_c2/(8.*pi*hbarc) ;

  // gab>>: make sure only one brem per event
   BDSeBremsFiredThisEvent=true;
   
   G4double GammaEnergy ;
   G4bool LPMOK = false ;

   aParticleChange.Initialize(trackData);

   //G4double LPMEnergy = LPMconstant*(aMaterial->GetRadlen()) ;

   const G4MaterialCutsCouple* couple = trackData.GetMaterialCutsCouple();
   const G4Material* aMaterial        = couple->GetMaterial();
   const G4DynamicParticle* aDynamicParticle=trackData.GetDynamicParticle();
   G4double charge = aDynamicParticle->GetDefinition()->GetPDGCharge();

   G4double           KineticEnergy     = aDynamicParticle->GetKineticEnergy();
   G4ParticleMomentum ParticleDirection = aDynamicParticle->GetMomentumDirection();

   // Gamma production cut in this material

   G4double GammaEnergyCut = SecondaryEnergyThreshold(couple->GetIndex());
   if (GammaEnergyCut < MinThreshold) GammaEnergyCut = MinThreshold;

   // check against insufficient energy
    if (KineticEnergy < GammaEnergyCut)
       {
         
#if G4VERSION > 6
	 aParticleChange.ProposeMomentumDirection( ParticleDirection );
         aParticleChange.ProposeEnergy( KineticEnergy );
         aParticleChange.ProposeLocalEnergyDeposit (0.);
         aParticleChange.SetNumberOfSecondaries(0);
#else
	 aParticleChange.SetMomentumChange( ParticleDirection );
         aParticleChange.SetEnergyChange( KineticEnergy );
         aParticleChange.SetLocalEnergyDeposit (0.);
         aParticleChange.SetNumberOfSecondaries(0);
#endif 

	 return G4VContinuousDiscreteProcess::PostStepDoIt(trackData,stepData);
       }

   // select randomly one element constituing the material
   G4Element* anElement = SelectRandomAtom(couple);

   // Extract Z factors for this Element
   G4double lnZ = 3.*(anElement->GetIonisation()->GetlogZ3());
   G4double FZ = lnZ* (4.- 0.55*lnZ);
   G4double ZZ = anElement->GetIonisation()->GetZZ3();

   // limits of the energy sampling
   G4double TotalEnergy = KineticEnergy + electron_mass_c2;
   G4double xmin     = GammaEnergyCut/KineticEnergy;
   G4double epsilmin = GammaEnergyCut/TotalEnergy;
   G4double epsilmax = KineticEnergy/TotalEnergy;

   // Migdal factor
   G4double
     MigdalFactor = (aMaterial->GetElectronDensity())*MigdalConstant
                          /(epsilmax*epsilmax);

   //
   G4double x, epsil, greject, migdal, grejmax;
   G4double U = log(KineticEnergy/electron_mass_c2), U2 = U*U;

   //
   //  sample the energy rate of the emitted gamma for electron kinetic energy > 1 MeV
   //

  do {
   if (KineticEnergy > 1.*MeV)
     {
       // parameters
       G4double ah1 = ah10 + ZZ* (ah11 + ZZ* ah12),
                ah2 = ah20 + ZZ* (ah21 + ZZ* ah22),
                ah3 = ah30 + ZZ* (ah31 + ZZ* ah32);

       G4double bh1 = bh10 + ZZ* (bh11 + ZZ* bh12),
                bh2 = bh20 + ZZ* (bh21 + ZZ* bh22),
                bh3 = bh30 + ZZ* (bh31 + ZZ* bh32);

       G4double ah = 1.   + (ah1*U2 + ah2*U + ah3) / (U2*U);
       G4double bh = 0.75 + (bh1*U2 + bh2*U + bh3) / (U2*U);

       // limit of the screening variable
       G4double screenfac =
       136.*electron_mass_c2/((anElement->GetIonisation()->GetZ3())*TotalEnergy);
       G4double screenmin = screenfac*epsilmin/(1.-epsilmin);

       // Compute the maximum of the rejection function
       G4double F1 = std::max(ScreenFunction1(screenmin) - FZ ,0.);
       G4double F2 = std::max(ScreenFunction2(screenmin) - FZ ,0.);
       grejmax = (F1 - epsilmin* (F1*ah - bh*epsilmin*F2))/(42.392 - FZ);

       // sample the energy rate of the emitted Gamma
       G4double screenvar;


       do {

             x = pow(xmin, G4UniformRand());
             epsil = x*KineticEnergy/TotalEnergy;
             screenvar = screenfac*epsil/(1-epsil);
             F1 = std::max(ScreenFunction1(screenvar) - FZ ,0.);
             F2 = std::max(ScreenFunction2(screenvar) - FZ ,0.);
             migdal = (1. + MigdalFactor)/(1. + MigdalFactor/(x*x));
             greject = migdal*(F1 - epsil* (ah*F1 - bh*epsil*F2))/(42.392 - FZ);
        }  while( greject < G4UniformRand()*grejmax );

    }

   else
     {
       // sample the energy rate of the emitted gamma for electron kinetic energy < 1 MeV
       //
       // parameters
       G4double al0 = al00 + ZZ* (al01 + ZZ* al02),
                al1 = al10 + ZZ* (al11 + ZZ* al12),
                al2 = al20 + ZZ* (al21 + ZZ* al22);

       G4double bl0 = bl00 + ZZ* (bl01 + ZZ* bl02),
                bl1 = bl10 + ZZ* (bl11 + ZZ* bl12),
                bl2 = bl20 + ZZ* (bl21 + ZZ* bl22);

       G4double al = al0 + al1*U + al2*U2;
       G4double bl = bl0 + bl1*U + bl2*U2;

       // Compute the maximum of the rejection function
       grejmax = std::max(1. + xmin* (al + bl*xmin), 1.+al+bl);
       G4double xm = -al/(2.*bl);
       if ((xmin < xm)&&(xm < 1.)) grejmax = std::max(grejmax, 1.+ xm* (al + bl*xm));

       // sample the energy rate of the emitted Gamma

       do {  x = pow(xmin, G4UniformRand());
             migdal = (1. + MigdalFactor)/(1. + MigdalFactor/(x*x));
             greject = migdal*(1. + x* (al + bl*x));
        }  while( greject < G4UniformRand()*grejmax );
   }

   GammaEnergy = x*KineticEnergy;

   if(LPMflag)
   {
     // take into account the supression due to the LPM effect
     if (G4UniformRand() <= SupressionFunction(aMaterial,KineticEnergy,GammaEnergy))
     LPMOK = true ;
   }
   else
     LPMOK = true ;

  } while (!LPMOK) ;


   //protection: DO NOT PRODUCE a gamma with energy 0. !
   if (GammaEnergy <= 0.)
       return G4VContinuousDiscreteProcess::PostStepDoIt(trackData,stepData);

   //
   //  angles of the emitted gamma. ( Z - axis along the parent particle)
   //
   //  universal distribution suggested by L. Urban (Geant3 manual (1993) Phys211),
   //  derived from Tsai distribution (Rev Mod Phys 49,421(1977))

   G4double u;
   const G4double a1 = 0.625 , a2 = 3.*a1 , d = 27. ;

   if (9./(9.+d) > G4UniformRand()) u = - log(G4UniformRand()*G4UniformRand())/a1 ;
      else                          u = - log(G4UniformRand()*G4UniformRand())/a2 ;

   G4double Teta = u*electron_mass_c2/TotalEnergy ;
   G4double Phi  = twopi * G4UniformRand() ;
   G4double dirx = sin(Teta)*cos(Phi) , diry = sin(Teta)*sin(Phi) , dirz = cos(Teta) ;

   G4ThreeVector GammaDirection ( dirx, diry, dirz);
   GammaDirection.rotateUz(ParticleDirection);

   // create G4DynamicParticle object for the Gamma
   G4DynamicParticle* aGamma= new G4DynamicParticle (G4Gamma::Gamma(),
                                                  GammaDirection, GammaEnergy);

   aParticleChange.SetNumberOfSecondaries(1);
   aParticleChange.AddSecondary(aGamma);

   //
   // Update the incident particle
   //

   G4double NewKinEnergy = KineticEnergy - GammaEnergy;
  
#if G4VERSION > 6
   if (NewKinEnergy > 0.)
     {
      aParticleChange.ProposeMomentumDirection( ParticleDirection );
      aParticleChange.ProposeEnergy( NewKinEnergy );
      aParticleChange.ProposeLocalEnergyDeposit (0.);
     }
   else
     {
      aParticleChange.ProposeEnergy( 0. );
      aParticleChange.ProposeLocalEnergyDeposit (0.);
      if (charge<0.) aParticleChange.ProposeTrackStatus(fStopAndKill);
          else       aParticleChange.ProposeTrackStatus(fStopButAlive);
     }
#else
   if (NewKinEnergy > 0.)
     {
       aParticleChange.SetMomentumChange( ParticleDirection );
       aParticleChange.SetEnergyChange( NewKinEnergy );
       aParticleChange.SetLocalEnergyDeposit (0.);
     }
   else
     {
       aParticleChange.SetEnergyChange( 0. );
       aParticleChange.SetLocalEnergyDeposit (0.);
       if (charge<0.) aParticleChange.SetStatusChange(fStopAndKill);
       else       aParticleChange.SetStatusChange(fStopButAlive);
     }
#endif


   return G4VContinuousDiscreteProcess::PostStepDoIt(trackData,stepData);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4Element* BDSeBremsstrahlung::SelectRandomAtom(const G4MaterialCutsCouple* couple) const
{
  // select randomly 1 element within the material

  size_t index = couple->GetIndex();
  const G4Material* aMaterial  = couple->GetMaterial();
  const G4int NumberOfElements = aMaterial->GetNumberOfElements();
  const G4ElementVector* theElementVector = aMaterial->GetElementVector();

  G4double rval = G4UniformRand()*((*PartialSumSigma[index])[NumberOfElements-1]);
  for ( G4int i=0; i < NumberOfElements; i++ )
    {
      if (rval <= (*PartialSumSigma[index])[i]) return ((*theElementVector)[i]);
    }
  G4cout << " WARNING !!! - The Material '"<< aMaterial->GetName()
         << "' has no elements, NULL pointer returned." << G4endl;
  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....


G4double BDSeBremsstrahlung::SupressionFunction(const G4Material* aMaterial,
                              G4double KineticEnergy,G4double GammaEnergy)
{
  // supression due to the LPM effect+polarisation of the medium/
  //   supression due to the polarisation alone
  const G4double MigdalConstant = classic_electr_radius*
                                  electron_Compton_length*
                                  electron_Compton_length/pi ;

  const G4double LPMconstant = fine_structure_const*electron_mass_c2*
                               electron_mass_c2/(8.*pi*hbarc) ;
  G4double TotalEnergy,TotalEnergySquare,LPMEnergy,LPMGammaEnergyLimit,
           LPMGammaEnergyLimit2,GammaEnergySquare,sp,s2lpm,supr,w,splim,Cnorm ;

  TotalEnergy = KineticEnergy+electron_mass_c2 ;
  TotalEnergySquare = TotalEnergy*TotalEnergy ;

  LPMEnergy = LPMconstant*(aMaterial->GetRadlen()) ;
  LPMGammaEnergyLimit = TotalEnergySquare/LPMEnergy ;
  GammaEnergySquare = GammaEnergy*GammaEnergy ;

  LPMGammaEnergyLimit2 = LPMGammaEnergyLimit*LPMGammaEnergyLimit ;
  splim = LPMGammaEnergyLimit2/(LPMGammaEnergyLimit2+MigdalConstant*TotalEnergySquare*
                                     (aMaterial->GetElectronDensity())) ;
  w = 1.+1./splim ;

  sp = GammaEnergySquare/(GammaEnergySquare+MigdalConstant*TotalEnergySquare*
                                     (aMaterial->GetElectronDensity())) ;
  if (LPMflag)
    {
     s2lpm = LPMEnergy*GammaEnergy/TotalEnergySquare;
     if (s2lpm < 1.)
       {
        if ((1.-sp) < 1.e-6) w = s2lpm*(3.-sp);
        else                 w = s2lpm*(1.+1./sp);

        Cnorm = 2./(sqrt(w*w+4.)-w) ;
        supr = Cnorm*(sqrt(w*w+4.*s2lpm)-w)/2. ;
       }
     else supr = sp;
    }
  else supr = sp;

  supr /= sp;
  return supr;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool BDSeBremsstrahlung::StorePhysicsTable(const G4ParticleDefinition* particle,
				              const G4String& directory,
				              G4bool          ascii)
{
  G4String filename;

  // store stopping power table
  filename = GetPhysicsTableFileName(particle,directory,"StoppingPower",ascii);
  if ( !theLossTable->StorePhysicsTable(filename, ascii) ){
    G4cout << " FAIL theLossTable->StorePhysicsTable in " << filename
           << G4endl;
    return false;
  }

  // store mean free path table
  filename = GetPhysicsTableFileName(particle,directory,"MeanFreePath",ascii);
  if ( !theMeanFreePathTable->StorePhysicsTable(filename, ascii) ){
    G4cout << " FAIL theMeanFreePathTable->StorePhysicsTable in " << filename
           << G4endl;
    return false;
  }

  // store PartialSumSigma table (G4OrderedTable)
  filename = GetPhysicsTableFileName(particle,directory,"PartSumSigma",ascii);
  if ( !PartialSumSigma.Store(filename, ascii) ){
    G4cout << " FAIL PartialSumSigma.store in " << filename
           << G4endl;
    return false;
  }

  G4cout << GetProcessName() << " for " << particle->GetParticleName()
         << ": Success to store the PhysicsTables in "
         << directory << G4endl;
  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool BDSeBremsstrahlung::RetrievePhysicsTable(const G4ParticleDefinition* particle,
					         const G4String& directory,
				                 G4bool          ascii)
{
  // delete theLossTable and theMeanFreePathTable
  if (theLossTable != 0) {
    theLossTable->clearAndDestroy();
    delete theLossTable;
  }
  if (theMeanFreePathTable != 0) {
    theMeanFreePathTable->clearAndDestroy();
    delete theMeanFreePathTable;
  }


    // get bining from EnergyLoss
  LowestKineticEnergy  = GetLowerBoundEloss();
  HighestKineticEnergy = GetUpperBoundEloss();
  TotBin               = GetNbinEloss();

  G4String filename;
  const G4ProductionCutsTable* theCoupleTable=
        G4ProductionCutsTable::GetProductionCutsTable();
  size_t numOfCouples = theCoupleTable->GetTableSize();

  secondaryEnergyCuts = theCoupleTable->GetEnergyCutsVector(0);

  // retreive stopping power table
  filename = GetPhysicsTableFileName(particle,directory,"StoppingPower",ascii);
  theLossTable = new G4PhysicsTable(numOfCouples);
  if ( !theLossTable->RetrievePhysicsTable(filename, ascii) ){
    G4cout << " FAIL theLossTable0->RetrievePhysicsTable in " << filename
           << G4endl;
    return false;
  }

  // retreive mean free path table
  filename = GetPhysicsTableFileName(particle,directory,"MeanFreePath",ascii);
  theMeanFreePathTable = new G4PhysicsTable(numOfCouples);
  if ( !theMeanFreePathTable->RetrievePhysicsTable(filename, ascii) ){
    G4cout << " FAIL theMeanFreePathTable->RetrievePhysicsTable in " << filename
           << G4endl;
    return false;
  }

  // retrieve PartialSumSigma table (G4OrderedTable)
  PartialSumSigma.clearAndDestroy();
  PartialSumSigma.reserve(numOfCouples);
  filename = GetPhysicsTableFileName(particle,directory,"PartSumSigma",ascii);
  if ( !PartialSumSigma.Retrieve(filename, ascii) ){
    G4cout << " FAIL PartialSumSigma.retrieve in " << filename
           << G4endl;
    return false;
  }

  G4cout << GetProcessName() << " for " << particle->GetParticleName()
         << ": Success to retrieve the PhysicsTables from "
         << directory << G4endl;

  if (particle==G4Electron::Electron())
   {
    RecorderOfElectronProcess[CounterOfElectronProcess] = (*this).theLossTable;
    CounterOfElectronProcess++;
   }
  else
   {
    RecorderOfPositronProcess[CounterOfPositronProcess] = (*this).theLossTable;
    CounterOfPositronProcess++;
   }

  BuildDEDXTable (*particle);

  if (particle==G4Electron::Electron()) PrintInfoDefinition();
  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BDSeBremsstrahlung::PrintInfoDefinition()
{
  G4String comments = "Total cross sections from a NEW parametrisation"
            " based on the EEDL data library. "
            "\n Good description from 1 KeV to 100 GeV.\n"
            "        log scale extrapolation above 100 GeV \n"
            "        Gamma energy sampled from a parametrised formula.";

  G4cout << G4endl << GetProcessName() << ":  " << comments
         << "\n        PhysicsTables from "
	 << G4BestUnit(LowerBoundLambda,"Energy")
         << " to " << G4BestUnit(UpperBoundLambda,"Energy")
         << " in " << NbinLambda << " bins. \n";
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
