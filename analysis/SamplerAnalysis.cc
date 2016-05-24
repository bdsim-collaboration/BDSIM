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
  means.resize(6);
  optical.resize(2); //test with limited opt. funcs for now  ex, bx, ax, ey, by, ay
  for(int i=0;i<2;++i)
  {
    optical[i].resize(6);
    for(int j=0;j<6;++j)
    {
      optical[i][j]=0.0;
    }
  }
  

  powSums.resize(6);
  cenMoms.resize(6);
  covMats.resize(6);
  // (x,xp,y,yp,E,t) (x,xp,y,yp,E,t) v1pow, v2pow
  for(int i=0;i<6;++i)
  {
    powSums[i].resize(6);
    cenMoms[i].resize(6);
    covMats[i].resize(6);
    for(int j=0;j<6;++j)
    {
      powSums[i][j].resize(5);
      cenMoms[i][j].resize(5);
      covMats[i][j] = 0.0;
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

    // means
    for(int a = 0;a<6;++a)
    {
      means[a] += v[a];
    }

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
    std::cout << __METHOD_NAME__ << std::endl;
  }
  for(int i=0;i<6;++i)
  {
    means[i] = means[i]/npart;
  }

  for(int i=0;i<6;++i)
  {
    for(int j=0;j<6;++j)
    {
      covMats[i][j]=(npart*powSums[i][j][1][1] - powSums[i][i][1][0]*powSums[j][j][1][0])/(npart*(npart-1)); //beam sigma matrix
    }
  }

  for(int i=0;i<2;++i)
  {
    int j = 0;
    if(i== 1) j = 2;

    optical[i][0] = sqrt(covMats[j][j]*covMats[j+1][j+1]+pow(covMats[j][j+1],2));                   //emittance
    optical[i][1] = covMats[j+1][j+1]/sqrt(covMats[j][j]*covMats[j+1][j+1]+pow(covMats[j][j+1],2)); //alpha
    optical[i][2] = covMats[j][j]/sqrt(covMats[j][j]*covMats[j+1][j+1]+pow(covMats[j][j+1],2));     //beta
    optical[i][3] = 0.0;
    optical[i][4] = covMats[j][5]/covMats[5][5];                                                    //dispersion
    optical[i][6] = 0.0;
  }

  for(int i=0;i<2;++i)
  {
    std::cout<<"e = "<<optical[i][0]<<" b = "<<optical[i][1]<<" a = "<<optical[i][2]<<" d = "<< optical[i][3]<<std::endl;
  }

  // compute covariances
  // cov[][][][] = ;
}

double SamplerAnalysis::powSumToCentralMoment(fourDArray &powSums, int npart,  int a, int b, int m, int n)
{
  double moment = 0.0; 
  
  if((n == 4 && m == 0) || (n == 0 && m == 4))
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

