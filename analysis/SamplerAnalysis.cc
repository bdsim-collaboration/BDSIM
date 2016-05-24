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
    std::cout << __METHOD_NAME__ << this->s->modelID << " " << npart << std::endl;
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

  std::cout << covMats[2][2] << " " << covMats[3][3] << " " << covMats[2][3] << " " << covMats[3][2] << " " << covMats[3][5] << " " <<covMats[5][5] << std::endl;

  for(int i=0;i<2;++i)
  {
    int j = 0;
    if(i==1) j = 2;

    optical[i][0] = sqrt(covMats[j][j]*covMats[j+1][j+1]+pow(covMats[j][j+1],2));                   // emittance
    optical[i][1] = covMats[j+1][j+1]/sqrt(covMats[j][j]*covMats[j+1][j+1]+pow(covMats[j][j+1],2)); // alpha
    optical[i][2] = covMats[j][j]/sqrt(covMats[j][j]*covMats[j+1][j+1]+pow(covMats[j][j+1],2));     // beta
    optical[i][3] = 0.0;                                                                            // gamma
    optical[i][4] = covMats[j][5]/covMats[5][5];                                                    // eta
    optical[i][6] = 0.0;                                                                            // eta prime
  }

  for(int i=0;i<2;++i)
  {
    std::cout<<"e = "<<optical[i][0]<<" b = "<<optical[i][1]<<" a = "<<optical[i][2]<<" d = "<< optical[i][3]<<std::endl;
  }

  // compute covariances
  // cov[][][][] = ;
}

std::vector<std::vector<double>> SamplerAnalysis::GetOpticalFunctions()
{
  return optical;
}

double SamplerAnalysis::powSumToCentralMoment(fourDArray &powSums, int npart,  int a, int b, int m, int n)
{
  double moment = 0.0;             //store the value of the moment
  
  if((n == 4 && m == 0) || (n == 0 && m == 4))
    {
      double s1,s2,s3,s4;
      if(m == 4)
      {
	s1 = 0.0;
	s2 = 0.0;
	s3 = 0.0;
	s4 = 0.0;
      }
      else if( n == 4)
      {
	s1 = 0.0;
	s2 = 0.0;
	s3 = 0.0;
	s4 = 0.0;
      }

      moment =  (-3*pow(powSums[a][b][m-3][n],4))/pow(npart,4) + (6*pow(powSums[a][b][m-3][n],2)*powSums[a][b][m-2][n])/pow(npart,3)
               +(-4*powSums[a][b][m-3][n]*powSums[a][b][m-1][n])/pow(npart,2) + powSums[a][b][m-3][n]/npart;
      return moment;
    }

    if((m == 1 || n == 1) && m + n == 4)
    {
      if(m > n)
      {
	moment =  (-3*pow(powSums[a][b][m-2][n],3)*powSums[a][b][m-3][n])/pow(npart,4); //icomplete
      }
      else
      {
	moment =   0.0; //incomplete
      }
    }

    return 0;
}

