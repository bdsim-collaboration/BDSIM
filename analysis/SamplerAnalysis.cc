#include "SamplerAnalysis.hh"
#include "Config.hh"
#include "rebdsim.hh"


SamplerAnalysis::SamplerAnalysis()
{
  this->CommonCtor();
}

#ifndef __ROOTDOUBLE__
SamplerAnalysis::SamplerAnalysis(BDSOutputROOTEventSampler<float> *samplerIn)
#else 
SamplerAnalysis::SamplerAnalysis(BDSOutputROOTEventSampler<double> *samplerIn)
#endif
{
  this->s = samplerIn;
  this->CommonCtor();
}

void SamplerAnalysis::CommonCtor()
{
  if(Config::Instance()->Debug())
  {
    std::cout << __METHOD_NAME__ << std::endl;
  }
  npart = 0;
  
  optical.resize(2); 
  for(int i=0;i<2;++i)
  {
    optical[i].resize(10);
    for(int j=0;j<10;++j)
    {
      optical[i][j]=0.0;
    }
  }

  covMats.resize(3);
  for(int i=0;i<3;++i)
  {
    covMats[i].resize(3);
    for(int j=0;j<3;++j)
    {
      covMats[i][j].resize(3);
      for(int k=0;k<3;++k)
      {
        covMats[i][j][k] = 0.0;
      }
    }
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
        powSums[i][j][k].resize(5);
        cenMoms[i][j][k].resize(5);
        for(int l=0;l<=4;++l)
        {
          powSums[i][j][k][l] = 0.0;
          cenMoms[i][j][k][l] = 0.0;
        }
      }
    }
  }
}


SamplerAnalysis::~SamplerAnalysis()
{ }

void SamplerAnalysis::Initialise()
{
  if(Config::Instance()->Debug())
  {
    std::cout << __METHOD_NAME__ << std::endl;
  }
  npart = 0;
}

void SamplerAnalysis::Process()
{
  if(Config::Instance()->Debug())
  {
    std::cout << __METHOD_NAME__ << std::endl;
  }

  std::vector<double> v;
  v.resize(6);

  // loop over all entries
  for(int i=0;i<this->s->n;++i)
  {
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
  }
  npart++;
}

void SamplerAnalysis::Terminate()
{
  if(Config::Instance()->Debug())
  {
    std::cout << __METHOD_NAME__ << this->s->modelID << " " << npart << std::endl;
  }

  // central moments
  for(int a=0;a<6;++a)
  {
    for(int b=0;b<6;++b)
	  {
	    for (int j = 0; j <= 4; ++j)
	      {
	      for (int k = 0; k <= 4; ++k)
		    {
		      cenMoms[a][b][j][k] = powSumToCentralMoment(powSums, npart, a, b, j, k);
		    }
	      }
	  }
  }

  //optical function calculation  
  for(int i=0;i<2;++i)
  {
    int j = 0;
    if(i== 1) j = 2;

    optical[i][0]  = sqrt(cenMoms[j][j+1][2][0]*cenMoms[j][j+1][0][2]-pow(cenMoms[j][j+1][1][1],2));                        // emittance
    optical[i][1]  = -cenMoms[j][j+1][1][1]/sqrt(cenMoms[j][j+1][2][0]*cenMoms[j][j+1][0][2]-pow(cenMoms[j][j+1][1][1],2)); // alpha
    optical[i][2]  = cenMoms[j][j+1][2][0]/sqrt(cenMoms[j][j+1][2][0]*cenMoms[j][j+1][0][2]-pow(cenMoms[j][j+1][1][1],2));  // beta
    optical[i][3]  = (1+pow(optical[i][1],2))/optical[i][2];                                                                // gamma
    optical[i][4]  = cenMoms[j][4][1][1]/cenMoms[4][4][2][0];                                                               // eta
    optical[i][5]  = cenMoms[j+1][4][1][1]/cenMoms[4][4][2][0];                                                             // eta prime
    optical[i][6]  = cenMoms[j][j+1][1][0];                                                                                 // mean spatial
    optical[i][7]  = cenMoms[j][j+1][0][1];                                                                                 // mean divergence
    optical[i][8]  = sqrt(cenMoms[j][j+1][2][0]);                                                                           // sigma spatial
    optical[i][9]  = sqrt(cenMoms[j][j+1][0][2]);                                                                           // sigma divergence 
  }

  for(int i=0;i<2;++i)
  {
    std::cout<<i<<" e = "<<optical[i][0]<<" b = "<<optical[i][2]<<" a = "<<optical[i][1]<<" d = "<< optical[i][4]<<std::endl;
    std::cout<<i<<" sigma = "<< sqrt(cenMoms[i][i+1][2][0])<<std::endl;
    std::cout<<i<<" mean = "<<cenMoms[i][i+1][1][0]<<std::endl;
  }

  //statistical error calculation
  
  //covariance matrix of parameters for optical functions. no coupling considered.  
  for(int i=0;i<3;++i)
    {
      for(int j=0;j<3;++j)
	{
	  for(int k=0;k<3;++k)
	    {
	      covMats[i][j][k]=centMomToCovariance(cenMoms, npart, i, j, k);
	    }
	}
    }

  
}

std::vector<std::vector<double>> SamplerAnalysis::GetOpticalFunctions()
{
  return optical;
}

double SamplerAnalysis::powSumToCentralMoment(fourDArray &powSums, int npart,  int a, int b, int m, int n)
{
  double moment = 0.0;

  if((m == 1 && n == 0) || (m == 0 && n == 1))
    {
      double s_1_0 = 0.0;
      
      s_1_0 = powSums[a][b][m][n];

      moment = s_1_0/npart;

      return moment;
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
      
      return moment;
    }

  else if(n == 1 && m == 1)
    {
      double s_1_0 = 0.0, s_0_1 = 0.0, s_1_1 = 0.0;

      s_1_0 = powSums[a][b][m][n-1];
      s_0_1 = powSums[a][b][m-1][n];
      s_1_1 = powSums[a][b][m][n];

      moment =  (npart*s_1_1 - s_0_1*s_1_0)/(npart*(npart-1));
      
      return moment;
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

      moment =  (-3*pow(s_1_0,4))/pow(npart,4) + (6*pow(s_1_0,2)*s_2_0)/pow(npart,3)
               +(-4*s_1_0*s_3_0)/pow(npart,2) + s_4_0/npart;
      
      return moment;
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

      moment =   (-3*s_0_1*pow(s_1_0,3))/pow(npart,4) + (3*pow(s_1_0,2)*s_1_1)/pow(npart,3)
                 + (3*s_0_1*s_1_0*s_2_0)/pow(npart,3) + (-3*s_1_0*s_2_1)/pow(npart,2)
                 + (-s_0_1*s_3_0)/pow(npart,2) + s_3_1/npart;
      
      return moment;
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

      moment =  (-3*pow(s_0_1,2)*pow(s_1_0,2))/pow(npart,4) + (s_0_2*pow(s_1_0,2))/pow(npart,3)
	      + (4*s_0_1*s_1_0*s_1_1)/pow(npart,3) + (2*s_1_0*s_1_2)/pow(npart,2)
	      + (pow(s_0_1,2)*s_2_0)/pow(npart,3) + (-2*s_0_1*s_2_1)/pow(npart,2) + s_2_2/npart;
      
      return moment;
      
    }

    return 0;
}

double SamplerAnalysis::centMomToCovariance(fourDArray &centMoms, int npart,  int k, int i, int j)
{
  // Calculates the matrix elements of the parameter covariance matrix which is a 3x3 symmetric matrix in each plane (coupling is ignored). 
  // Inputs:
  // int k: plane specifier (k=0: horizontal, k=1: vertical, k=2: longitudinal)
  // int i,j: indices of matrix elements (i,j=0: <uu>, i,j=1: <u'u'>, i,j=2: <uu'>)
  // e.g. covMat[0][1][2] = cov[<x'x'>,<xx'>], covMat[1][0][0] = cov[<yy>,<yy>]
  
  double cov = 0.0;

  int a = 0;
  if(k == 1) {a=2;}
  if(k == 2) {a=4;}

  if((i == 0 && j == 0) || (i == 1 && j == 1))
    {
      double m_4_0 = 0.0, m_2_0 = 0.0;
      
      if(i == 1)
      {	
	      m_4_0 = centMoms[a][a+1][4][0];
	      m_2_0 = centMoms[a][a+1][2][0];
      }
      
      else if(i == 2)
      {
	      m_4_0 = centMoms[a][a+1][0][4];
	      m_2_0 = centMoms[a][a+1][0][2];
      }

      cov = ((npart-3)*pow(m_2_0,2))/(npart*(npart-1)) + m_4_0/npart;
      
    }
  
  else if(i == 2 && j == 2)
    {
      double m_1_1 = 0.0, m_2_0 = 0.0, m_0_2 = 0.0, m_2_2 = 0.0;

      m_1_1 = centMoms[a][a+1][1][1];
      m_2_0 = centMoms[a][a+1][2][0];
      m_0_2 = centMoms[a][a+1][0][2];
      m_2_2 = centMoms[a][a+1][2][2];

      cov = ((npart-2)*pow(m_1_1,2))/(npart*(npart-1)) + (m_0_2*m_2_0)/(npart*(npart-1)) + m_2_2/npart;
    }

  else if((i == 0 && j == 1) || (i == 1 && j == 2))
  {
    double m_1_1 = 0.0, m_2_0 = 0.0, m_3_1 = 0.0;

    if(i == 1)
    {
      m_1_1 = centMoms[a][a+1][1][1];
      m_2_0 = centMoms[a][a+1][2][0];
      m_3_1 = centMoms[a][a+1][3][1];
    }
    else if(i == 2)
    {
      m_1_1 = centMoms[a][a+1][1][1];
      m_2_0 = centMoms[a][a+1][0][2];
      m_3_1 = centMoms[a][a+1][1][3];
    }

    cov = ((npart-3)*m_1_1*m_2_0)/(npart*(npart-1)) + m_3_1/npart;
  }
  else if(i == 0 && j == 2)
  {
    double m_1_1 = 0.0, m_2_0 = 0.0, m_0_2 = 0.0,  m_2_2 = 0.0;
      
    m_1_1 = centMoms[a][a+1][1][1];
    m_2_0 = centMoms[a][a+1][0][2];
    m_0_2 = centMoms[a][a+1][2][0];
    m_2_2 = centMoms[a][a+1][2][2];

    cov = 2*pow(m_1_1,2)/(npart*(npart-1)) + (-m_0_2*m_2_0)/npart + m_2_2/npart;
  }

  return 0;
}

