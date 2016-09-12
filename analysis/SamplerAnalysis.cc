#include "SamplerAnalysis.hh"
#include "Config.hh"
#include "rebdsim.hh"

#include <cmath>

SamplerAnalysis::SamplerAnalysis():
  s(nullptr),
  S(0)
{
  CommonCtor();
}

#ifndef __ROOTDOUBLE__
SamplerAnalysis::SamplerAnalysis(BDSOutputROOTEventSampler<float> *samplerIn):
#else 
  SamplerAnalysis::SamplerAnalysis(BDSOutputROOTEventSampler<double> *samplerIn):
#endif
  s(samplerIn),
  S(0)
{
  CommonCtor();
}

void SamplerAnalysis::CommonCtor()
{
  if(Config::Instance()->Debug())
    {std::cout << __METHOD_NAME__ << std::endl;}
  npart = 0;
  
  optical.resize(3); // resize to 3 entries initialised to 0
  varOptical.resize(3);
  for(int i=0;i<3;++i)
    {
      optical[i].resize(24, 0);   //12 for central values and 12 for errors
      varOptical[i].resize(12, 0);
    }

  covMats.resize(3);
  for(int i=0;i<3;++i)
  {
    covMats[i].resize(3);
    for(int j=0;j<3;++j)
      {covMats[i][j].resize(3, 0);}
  }

  derivMats.resize(3);
  for(int i=0;i<3;++i)
  {
    derivMats[i].resize(12);
    for(int j=0;j<12;++j)
      {derivMats[i][j].resize(3, 0);}
  }
  
  powSums.resize(6);
  cenMoms.resize(6);

  // (x,xp,y,yp,E,t) (x,xp,y,yp,E,t) v1pow, v2pow
  for(int i=0;i<6;++i)
  {
    powSums[i].resize(6);
    cenMoms[i].resize(6);

    for(int j=0;j<6;++j)
    {
      powSums[i][j].resize(5);
      cenMoms[i][j].resize(5);
      for(int k=0;k<=4;++k)
      {
        powSums[i][j][k].resize(5, 0);
        cenMoms[i][j][k].resize(5, 0);
      }
    }
  }
}

SamplerAnalysis::~SamplerAnalysis()
{;}

void SamplerAnalysis::Initialise()
{
  if(Config::Instance()->Debug())
    {std::cout << __METHOD_NAME__ << std::endl;}
  npart = 0;
}

void SamplerAnalysis::Process()
{
  if(Config::Instance()->Debug())
    {std::cout << __METHOD_NAME__ << std::endl;}

  std::vector<double> v;
  v.resize(6);

  this->S = this->s->S;
  
  // loop over all entries
  for(int i=0;i<this->s->n;++i)
  {
    if (s->parentID[i] != 0)
      {continue;}              //select only primary particles
    v[0] = s->x[i];
    v[1] = s->xp[i];
    v[2] = s->y[i];
    v[3] = s->yp[i];
    v[4] = s->energy[i];
    v[5] = s->t[i];

    // power sums
    for(int a=0;a<6;++a)
      {
	for(int b=0;b<6;++b)
	  {
	    for (int j = 0; j <= 4; ++j)
	      {
		for (int k = 0; k <= 4; ++k)
		  {
		    powSums[a][b][j][k] += pow(v[a],j)*pow(v[b],k);
		  }
	      }
	  }
      }
  npart++;  
  }
}

void SamplerAnalysis::Terminate()
{
  if(Config::Instance()->Debug())
    {std::cout << " " << __METHOD_NAME__ << this->s->modelID << " " << npart << std::flush;}

  // central moments
  for(int a=0;a<6;++a)
  {
    for(int b=0;b<6;++b)
      {
	for (int j = 0; j <= 4; ++j)
	  {
	    for (int k = 0; k <= 4; ++k)
	      {
		cenMoms[a][b][j][k] = powSumToCentralMoment(powSums, (int)npart, a, b, j, k);
	      }
	  }
      }
  }

  //optical function calculation  
  for(int i=0;i<3;++i)
  {
    int j = 0;
    if(i == 1) j = 2;
    if(i == 2) j = 4;

    //note: optical functions vector not populated in sequential order in order to apply dispersion correction to lattice funcs.

    optical[i][6]  = cenMoms[j][j+1][1][0];                                                                                                // mean spatial (transv.)/ mean E (longit.)
    optical[i][7]  = cenMoms[j][j+1][0][1];                                                                                                // mean divergence (transv.)/ mean t (longit.)
    optical[i][8]  = sqrt(cenMoms[j][j+1][2][0]);                                                                                          // sigma spatial   (transv.)/ sigma E (longit.)
    optical[i][9]  = sqrt(cenMoms[j][j+1][0][2]);                                                                                          // sigma divergence (transv.)/ sigma t (longit.)

    if (i==2) continue;    //tranverse optical function calculation skipped for longitudinal plane, only mean and sigma of longit. coordinates recorded

    optical[i][4]  = (cenMoms[4][4][1][0]*cenMoms[j][4][1][1])/cenMoms[4][4][2][0];                                                        // eta
    optical[i][5]  = (cenMoms[4][4][1][0]*cenMoms[j+1][4][1][1])/cenMoms[4][4][2][0];                                                      // eta prime

    // check for nans (expected if dE=0) and set disp=0 if found
    if (std::isnan(optical[i][4]))
      {optical[i][4] = 0;}
    if (std::isnan(optical[i][5]))
      {optical[i][5] = 0;}

    double corrCentMom_2_0 = 0.0, corrCentMom_0_2 = 0.0, corrCentMom_1_1 = 0.0; //temp variables to store dispersion corrected moments
    
    corrCentMom_2_0 = cenMoms[j][j+1][2][0] + (pow(optical[i][4],2)*cenMoms[4][4][2][0])/pow(cenMoms[4][4][1][0],2)
            - 2*(optical[i][4]*cenMoms[j][4][1][1])/cenMoms[4][4][1][0];
    
    corrCentMom_0_2 = cenMoms[j][j+1][0][2] + (pow(optical[i][5],2)*cenMoms[4][4][2][0])/pow(cenMoms[4][4][1][0],2)
            - 2*(optical[i][5]*cenMoms[j+1][4][1][1])/cenMoms[4][4][1][0];
    
    corrCentMom_1_1 = cenMoms[j][j+1][1][1] + (optical[i][4]*optical[i][5]*cenMoms[4][4][2][0])/pow(cenMoms[4][4][1][0],2)
            - (optical[i][5]*cenMoms[j][4][1][1])/cenMoms[4][4][1][0] - (optical[i][4]*cenMoms[j+1][4][1][1])/cenMoms[4][4][1][0];

    
    optical[i][0]  = sqrt(corrCentMom_2_0*corrCentMom_0_2-pow(corrCentMom_1_1,2));                                                         // emittance
    optical[i][1]  = -corrCentMom_1_1/optical[i][0];                                                                                       // alpha
    optical[i][2]  = corrCentMom_2_0/optical[i][0];                                                                                        // beta
    optical[i][3]  = (1+pow(optical[i][1],2))/optical[i][2];                                                                               // gamma

    optical[i][10] = this->S;
    optical[i][11] = npart;
  }

  //statistical error calculation
  
  //covariance matrix of central moments for optical functions.
  for(int i=0;i<3;++i)
    {
      for(int j=0;j<3;++j)
	{
	  for(int k=0;k<3;++k)
	    {
	      covMats[i][j][k]=centMomToCovariance(cenMoms, (int)npart, i, j, k);
	    }
	}
    }
  
  //derivative matrix of parameters for optical functions. Each entry is a first order derivative w.r.t central moments.  
  for(int i=0;i<3;++i)
    {
      for(int j=0;j<12;++j) //loop over optical functions.
	{
	  for(int k=0;k<3;++k) //loop over derivative indices
	    {
	      derivMats[i][j][k]=centMomToDerivative(cenMoms, i, j, k);
	    }
	}
    }
  
  //compute variances of optical functions
  for(int i=0;i<3;++i)      
    {
      for(int j=0;j<12;++j)
	{
	  for(int k=0;k<3;++k)
	    {
	      for(int l=0;l<3;++l)
		{
		  varOptical[i][j] += derivMats[i][j][k]*derivMats[i][j][l]*covMats[i][k][l];
		}
	    }
	}
    }

  //compute the sigmas of optical functions
  for(int i=0;i<3;++i)      
    {
      for(int j=0;j<12;++j)
	{
	  if(j==6 || j==7)
	    {optical[i][j+12]=optical[i][j+2]/sqrt(npart);} //errors of the means 
	  else if(j == 10 || j == 11)
	    {optical[i][j+12]= 0.0;}                        //no errors on S and Npart
	  else
	    {optical[i][j+12]=sqrt(varOptical[i][j]);}
	}
    }
  
}



double SamplerAnalysis::powSumToCentralMoment(fourDArray &powSums,
					      int         npart,
					      int a,
					      int b,
					      int m,
					      int n)
//Returns a central moment calculated from the corresponding coordinate power sums.
//Arguments:
//    powSums: array contatining the coordinate power sums
//    a, b:  integer identifier for the coordinate (0->x, 1->xp, 2->y, 3->yp, 4->E, 5->t)
//    m, n:  order of the moment wrt to the coordinate
//    note:  total order of the mixed moment is given by k = m + n
  
{
  double moment = 0.0;

  if((m == 1 && n == 0) || (m == 0 && n == 1))
    {
      double s_1_0 = 0.0;
      
      s_1_0 = powSums[a][b][m][n];

      moment = s_1_0/npart;
    }

  else if((n == 2 && m == 0) || (n == 0 && m == 2))
    {
      double s_1_0 = 0.0, s_2_0 = 0.0;
      if(m == 2)
      {
	      s_1_0 = powSums[a][b][m-1][n];
	      s_2_0 = powSums[a][b][m][n];
      }
      else if(n == 2)
      {
	      s_1_0 = powSums[a][b][m][n-1];
	      s_2_0 = powSums[a][b][m][n];
      }

      moment =  (npart*s_2_0 - pow(s_1_0,2))/(npart*(npart-1));
    }

  else if(n == 1 && m == 1)
    {
      double s_1_0 = 0.0, s_0_1 = 0.0, s_1_1 = 0.0;

      s_1_0 = powSums[a][b][m][n-1];
      s_0_1 = powSums[a][b][m-1][n];
      s_1_1 = powSums[a][b][m][n];

      moment =  (npart*s_1_1 - s_0_1*s_1_0)/(npart*(npart-1));
    }
  
  else if((n == 4 && m == 0) || (n == 0 && m == 4))
    {
      double s_1_0 = 0.0, s_2_0 = 0.0, s_3_0 = 0.0, s_4_0 = 0.0;
      if(m == 4)
      {
	      s_1_0 = powSums[a][b][m-3][n];
	      s_2_0 = powSums[a][b][m-2][n];
	      s_3_0 = powSums[a][b][m-1][n];
	      s_4_0 = powSums[a][b][m][n];
      }
      else if( n == 4)
      {
	      s_1_0 = powSums[a][b][m][n-3];
	      s_2_0 = powSums[a][b][m][n-2];
	      s_3_0 = powSums[a][b][m][n-1];
	      s_4_0 = powSums[a][b][m][n];
      }

      moment = - (3*pow(s_1_0,4))/pow(npart,4) + (6*pow(s_1_0,2)*s_2_0)/pow(npart,3)
	       - (4*s_1_0*s_3_0)/pow(npart,2) + s_4_0/npart;
    }

  else if((m == 3 && n == 1) || (m == 1 && n ==3))
    {
      double s_1_0 = 0.0, s_0_1 = 0.0, s_1_1 = 0.0, s_2_0 = 0.0, s_2_1 = 0.0, s_3_0 = 0.0, s_3_1 = 0.0;
      
      if(m == 3)
      {
	      s_1_0 = powSums[a][b][m-2][n-1];
	      s_0_1 = powSums[a][b][m-3][n];
	      s_1_1 = powSums[a][b][m-2][n];
	      s_2_0 = powSums[a][b][m-1][n-1];
	      s_2_1 = powSums[a][b][m-1][n];
	      s_3_0 = powSums[a][b][m][n-1];
	      s_3_1 = powSums[a][b][m][n];
      }
      else if(n == 3)
      {
	      s_1_0 = powSums[a][b][m-1][n-2];
	      s_0_1 = powSums[a][b][m][n-3];
	      s_1_1 = powSums[a][b][m][n-2];
	      s_2_0 = powSums[a][b][m-1][n-1];
	      s_2_1 = powSums[a][b][m][n-1];
	      s_3_0 = powSums[a][b][m-1][n];
	      s_3_1 = powSums[a][b][m][n];
      }

      moment = - (3*s_0_1*pow(s_1_0,3))/pow(npart,4) + (3*s_1_0*s_1_0*s_1_1)/pow(npart,3)
               + (3*s_0_1*s_1_0*s_2_0)/pow(npart,3) - (3*s_1_0*s_2_1)/(npart*npart)
               - (s_0_1*s_3_0)/(npart*npart) + s_3_1/npart;
    }

   else if(m == 2 && n == 2)
    {
      double s_1_0 = 0.0, s_0_1 = 0.0, s_1_1 = 0.0, s_2_0 = 0.0, s_0_2 = 0.0, s_1_2 = 0.0, s_2_1 = 0.0, s_2_2 = 0.0;

      s_1_0 = powSums[a][b][m-1][n-2];
      s_0_1 = powSums[a][b][m-2][n-1];
      s_1_1 = powSums[a][b][m-1][n-1];
      s_2_0 = powSums[a][b][m][n-2];
      s_0_2 = powSums[a][b][m-2][n];
      s_1_2 = powSums[a][b][m-1][n];
      s_2_1 = powSums[a][b][m][n-1];
      s_2_2 = powSums[a][b][m][n];

      moment = - (3*pow(s_0_1,2)*pow(s_1_0,2))/pow(npart,4) + (s_0_2*pow(s_1_0,2))/pow(npart,3)
	       + (4*s_0_1*s_1_0*s_1_1)/pow(npart,3) - (2*s_1_0*s_1_2)/pow(npart,2)
	       + (pow(s_0_1,2)*s_2_0)/pow(npart,3) - (2*s_0_1*s_2_1)/pow(npart,2) + s_2_2/npart; 
    }

    return moment;
}

double SamplerAnalysis::centMomToCovariance(fourDArray &centMoms, int npart,  int k, int i, int j)
{
  // Returns a matrix element of the parameter covariance matrix which is a 3x3 symmetric matrix in each plane (coupling is ignored). 
  // Arguments:
  //     int k:   plane specifier (k=0: horizontal, k=1: vertical, k=2: longitudinal)
  //     int i,j: indices of matrix elements (i,j=0: <uu>, i,j=1: <u'u'>, i,j=2: <uu'>)
  //     e.g. covMat[0][1][2] = cov[<x'x'>,<xx'>], covMat[1][0][0] = cov[<yy>,<yy>]
  
  double cov = 0.0;

  int a = 0;
  if(k == 1) {a=2;}
  if(k == 2) {a=4;}

  if((i == 0 && j == 0) || (i == 1 && j == 1))
    {
      double m_4_0 = 0.0, m_2_0 = 0.0;
      
      if(i == 0)
      {	
	      m_4_0 = centMoms[a][a+1][4][0];
	      m_2_0 = centMoms[a][a+1][2][0];
      }
      
      else if(i == 1)
      {
	      m_4_0 = centMoms[a][a+1][0][4];
	      m_2_0 = centMoms[a][a+1][0][2];
      }

      cov = -((npart-3)*pow(m_2_0,2))/(npart*(npart-1)) + m_4_0/npart;
    }
  
  else if(i == 2 && j == 2)
    {
      double m_1_1 = 0.0, m_2_0 = 0.0, m_0_2 = 0.0, m_2_2 = 0.0;

      m_1_1 = centMoms[a][a+1][1][1];
      m_2_0 = centMoms[a][a+1][2][0];
      m_0_2 = centMoms[a][a+1][0][2];
      m_2_2 = centMoms[a][a+1][2][2];

      cov = -((npart-2)*pow(m_1_1,2))/(npart*(npart-1)) + (m_0_2*m_2_0)/(npart*(npart-1)) + m_2_2/npart;
    }

  else if((i == 0 && j == 2) || (i == 1 && j == 2) || (i == 2 && j == 0) || (i == 2 && j == 1))
  {
    double m_1_1 = 0.0, m_2_0 = 0.0, m_3_1 = 0.0;

    if((i == 0 && j == 2) || (i == 2 && j == 0 ))
    {
      m_1_1 = centMoms[a][a+1][1][1];
      m_2_0 = centMoms[a][a+1][2][0];
      m_3_1 = centMoms[a][a+1][3][1];
    }
    else if((i == 1 && j == 2) || (i == 2 && j == 1))
    {
      m_1_1 = centMoms[a][a+1][1][1];
      m_2_0 = centMoms[a][a+1][0][2];
      m_3_1 = centMoms[a][a+1][1][3];
    }

    cov = -((npart-3)*m_1_1*m_2_0)/(npart*(npart-1)) + m_3_1/npart;
  }
  else if((i == 0 && j == 1) || (i == 1 && j == 0) )
  {
    double m_1_1 = 0.0, m_2_0 = 0.0, m_0_2 = 0.0,  m_2_2 = 0.0;
      
    m_1_1 = centMoms[a][a+1][1][1];
    m_2_0 = centMoms[a][a+1][0][2];
    m_0_2 = centMoms[a][a+1][2][0];
    m_2_2 = centMoms[a][a+1][2][2];

    cov = 2*pow(m_1_1,2)/(npart*(npart-1)) - m_2_0*m_0_2/npart + m_2_2/npart;
  }

  return cov;
}


double SamplerAnalysis::centMomToDerivative(fourDArray &centMoms, int k, int t, int i)
  // Returns the derivative of an optical function w.r.t. central moments. 
  // Arguments:
  //     int t: function specifier, corresponds to index of the function in the optical function vector. 
  //     int k: plane specifier (k=0: horizontal, k=1: vertical, k=2: longitudinal)
  //     int i: central moment to diffrentiate w.r.t, i=0: <uu>, i=1: <u'u'>, i=2: <uu'>
  //     e.g. derivMat[t=2][k=0][i=0]: d(beta)/d<xx> , derivMat[t=0][k=1][i=1]: d(emittance)/d<yy'>
{
  double deriv = 0.0;

  int a = 0;
  if(k == 1) {a=2;}
  if(k == 2) {a=4;}
  
  switch(t)   
    {
    case 0:
      //emittance
      if(i == 0 && k < 2)  // k<2 check selects transverse planes, longitudinal parameters are not calculated
      {
	deriv = centMoms[a][a+1][0][2]/(2*sqrt(centMoms[a][a+1][2][0]*centMoms[a][a+1][0][2]-pow(centMoms[a][a+1][1][1],2)));
      }
      else if(i == 1 && k < 2)
      {
	deriv = centMoms[a][a+1][2][0]/(2*sqrt(centMoms[a][a+1][2][0]*centMoms[a][a+1][0][2]-pow(centMoms[a][a+1][1][1],2)));
      }
      else if(i == 2 && k < 2)
      {
	deriv = -centMoms[a][a+1][1][1]/(sqrt(centMoms[a][a+1][2][0]*centMoms[a][a+1][0][2]-pow(centMoms[a][a+1][1][1],2)));
      }
      else {deriv=0;}
      
      return deriv;
      break;
      
    case 1:
      //alpha
      if(i == 0 && k < 2) 
      {
	deriv = centMoms[a][a+1][0][2]*centMoms[a][a+1][1][1]/(2*pow(centMoms[a][a+1][2][0]*centMoms[a][a+1][0][2]-pow(centMoms[a][a+1][1][1],2),3./2.));
      }
      else if(i == 1 && k < 2)
      {
	deriv = centMoms[a][a+1][2][0]*centMoms[a][a+1][1][1]/(2*pow(centMoms[a][a+1][2][0]*centMoms[a][a+1][0][2]-pow(centMoms[a][a+1][1][1],2),3./2.)); 
      }
      else if(i == 2 && k < 2)
      {
	deriv = - centMoms[a][a+1][2][0]*centMoms[a][a+1][0][2]/pow(centMoms[a][a+1][2][0]*centMoms[a][a+1][0][2]-pow(centMoms[a][a+1][1][1],2),3./2.);
      }
      else {deriv=0;}
      
      return deriv;
      break;

    case 2:
      //beta
      if(i == 0 && k < 2) 
      {
	deriv = (centMoms[a][a+1][2][0]*centMoms[a][a+1][0][2]-2*pow(centMoms[a][a+1][1][1],2))/(2*pow(centMoms[a][a+1][2][0]*centMoms[a][a+1][0][2]-pow(centMoms[a][a+1][1][1],2),3./2.)); 
      }
      else if(i == 1 && k < 2)
      {
	deriv = - pow(centMoms[a][a+1][2][0],2)/(2*pow(centMoms[a][a+1][2][0]*centMoms[a][a+1][0][2]-pow(centMoms[a][a+1][1][1],2),3./2.));
      }
      else if(i == 2 && k < 2)
      {
	deriv = centMoms[a][a+1][2][0]*centMoms[a][a+1][1][1]/pow(centMoms[a][a+1][2][0]*centMoms[a][a+1][0][2]-pow(centMoms[a][a+1][1][1],2),3./2.);
      }
      else {deriv=0;}
      
      return deriv;
      break;


    case 3:
      //gamma
      return 0; //tbc
      break;

    case 4:
      //eta
      if(i == 0 && k < 2)
      {
	deriv = 0;
      }
      else if(i == 1 && k < 2)
      {
	deriv = -centMoms[a][4][1][1]/pow(centMoms[4][4][2][0],2);
      }
      else if(i == 2 && k < 2)
      {
	deriv = 1/centMoms[4][4][2][0];;
      }
      else {deriv=0;}
      
      return deriv;
      break;

    case 5:
      //eta prime
      if(i == 0 && k < 2)
      {
	deriv = 0;
      }
      else if(i == 1 && k < 2)
      {
	deriv = -centMoms[a+1][4][1][1]/pow(centMoms[4][4][2][0],2);
      }
      else if(i == 2 && k < 2)
      {
	deriv = 1/centMoms[4][4][2][0];;
      }
      else {deriv=0;}
      
      return deriv;
      break;

    case 6:                // mean derivatives are all 0 as they don't depend on second order moments
      //mean spatial    
      return 0;              
      break;

    case 7:
      //mean divergence
      return 0;
      break;

    case 8:
      //sigma spatial
      if(i == 0)
      {
	deriv = 1/(2*sqrt(centMoms[a][a+1][2][0]));
      }
      
      else {deriv=0;}
      
      return deriv;
      break;

    case 9:
      //sigma divergence
      if(i == 1)
      {
	deriv = 1/(2*sqrt(centMoms[a][a+1][0][2]));
      }
      
      else {deriv=0;}
      
      return deriv;
      break;
      
    default:
      return 0;
      break;
    }
}
