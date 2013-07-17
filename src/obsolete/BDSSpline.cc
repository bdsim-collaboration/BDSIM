/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/

#include "globals.hh"
#include "BDSSpline.hh"
//---------------
BDSSpline::BDSSpline(G4int nIn):n(nIn),xscal(0),yscal(0)
{
  G4int i;
  u=vDbl(n);
  tab=vTab(n);
  for(i=0;i<n;i++) tab[i]=new BDSSpline_tab_entry();

}

void BDSSpline::initialise(vDbl* xIn,G4int xscalIn, 
			   vDbl* yIn,G4int yscalIn)
{
  G4int i;
  G4double sig,p;  
  
  xscal=xscalIn;
  yscal=yscalIn;
  
  BDSSpline_tab_entry* tabEntry=tab[0];

  tabEntry->y2=0.0;

 
  for (i=0;i<n;i++)
    {
      BDSSpline_tab_entry* tabEntry=tab[i];
      switch (xscal){
      case 0:
	tabEntry->x=(*xIn)[i];
	break;
      case 1:
	tabEntry->x=log((*xIn)[i]);
	break;
      }
      switch (yscal){
      case 0:
	tabEntry->y=(*yIn)[i];
	break;
      case 1:
	tabEntry->y=log((*yIn)[i]);
	break;
      }	    
    }
 
  u[0]=0.0;
  for (i=1;i<n-1;i++)
    {
      BDSSpline_tab_entry* tab_i=tab[i];
      BDSSpline_tab_entry* tab_im1=tab[i-1];
      BDSSpline_tab_entry* tab_ip1=tab[i+1];

      sig=(tab_i->x-tab_im1->x)/(tab_ip1->x-tab_im1->x);
      
      p=1.0/(sig*tab_im1->y2+2.0);

      tab_i->y2=(sig-1.0)*p;    
      u[i]=
	(6.0*((tab_ip1->y-tab_i->y)/(tab_ip1->x-tab_i->x) 
	      -(tab_i->y-tab_im1->y)/(tab_i->x-tab_im1->x)) 
	 /(tab_ip1->x -tab_im1->x)  - sig*u[i-1])*p;
    }

  BDSSpline_tab_entry* tab_nm1=tab[n-1];

  tab_nm1->y2=0.0;
  for (i=n-2;i>=0;i--)
    {
      BDSSpline_tab_entry* tab_i=tab[i];
      BDSSpline_tab_entry* tab_ip1=tab[i+1];
      
      tab_i->y2=tab_i->y2*tab_ip1->y2+u[i];
    }
}

G4double BDSSpline::integrate(G4double xIn)const
{
  int kmin,kmax,kpoint;
  double a,b,w;

  kmin=0;
  kmax=n-1;

  BDSSpline_tab_entry* tab_kmx=tab[kmax];
  BDSSpline_tab_entry* tab_kmn=tab[kmin];
  BDSSpline_tab_entry* tab_0=tab[0];

  switch(xscal){
  case 0:	
    break;
  case 1:
    xIn=log(xIn);
    break;
}
 
  if (xIn>tab_kmx->x)
    {
      if (yscal)return exp(tab_kmx->y);
      else return tab_kmx->y; 
    }
  if (xIn<tab_0->x)
    {
      if (yscal) return exp(tab_0->y);
      else return tab_0->y;
    }
  while (kmax-kmin>1){
    kpoint=(kmax+kmin)/2;
    BDSSpline_tab_entry* tab_kpt=tab[kpoint];
    
    if (tab_kpt->x>xIn) kmax=kpoint;
    else kmin=kpoint;
  }

  w=tab_kmx->x - tab_kmn->x;

  a=(tab_kmx->x-xIn)/w;

  b=(xIn-tab_kmn->x)/w;
  G4double x=a*tab_kmn->y+b*tab_kmx->y+
    (a*(a*a-1.0)*tab_kmn->y2
     +b*(b*b-1.0)*tab_kmx->y2)*w*w/6.0;

  switch (yscal){
  case 0:
    return x;
  case 1:
    return exp(x);
  }
  return x;
}


BDSSpline::~BDSSpline()
{
  size_t i;
  for(i=0;i<tab.size();i++)
    delete tab[i];
}

