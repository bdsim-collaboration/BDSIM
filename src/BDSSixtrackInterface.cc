#include <algorithm>
#include <iostream>
#include <set>
#include <string>

std::string CleanFortranString(char* str, size_t count);

//std::vector<CollimationParticle> input_particles;
//std::vector<CollimationParticle> output_particles;
//std::vector<CollimationEnergyDeposition> EnergyDepositionConfiguration;
//std::set<int> keep_ids;

extern "C"
void g4_collimation_init(double* ReferenceE,
			 int*    seed,
			 double* recut,
			 double* aecut,
			 double* rcut,
			 double* rangecut_mm,
			 double* v0,
			 char*   PhysicsSelect,
			 bool*   g4_debug,
			 bool*   g4_keep_stable,
			 bool*   DoEnergyDeposition)
{;}

extern "C"
void g4_add_collimator(char*   name,
		       char*   material,
		       double* length,
		       double* aperture,
		       double* rotation,
		       double* x_offset,
		       double* y_offset)
{
  /*
  //  keep 48 value in sync with mNameLen in common_modules.f90
  std::string CollimatorName = CleanFortranString(name, 48);
  std::string CollimatorMaterialName = CleanFortranString(material, 4);
  std::cout << "GEANT4> Adding \"" << CollimatorName << "\" with material \"" << CollimatorMaterialName << "\" and rotation \"" << *rotation << "\" and offset x: \"" << *x_offset << "\ y: \"" << *y_offset << "\" and length \"";
  std::cout << *length << "\"" << std::endl;
  
  G4double length_in = *length *CLHEP::m;
  G4double aperture_in = *aperture *CLHEP::m;
  G4double rotation_in = *rotation *CLHEP::rad;
  G4double offset_in = *x_offset *CLHEP::m;
  
  geometry->AddCollimator(CollimatorName, length_in, aperture_in, rotation_in, offset_in, CollimatorMaterialName);
  */
}

extern "C"
void g4_terminate()
{;}

extern "C"
void g4_set_collimator(char* name)
{
  //  keep 48 value in sync with mNameLen in common_modules.f90
  std::string CollimatorName = CleanFortranString(name, 48);
  //geometry->SetCollimator(CollimatorName);
}

extern "C"
void g4_add_particle(double* x,
		     double* y,
		     double* xp,
		     double* yp,
		     double* e,
		     int32_t* pdgid,
		     int16_t* nzz,
		     int16_t* naa,
		     int16_t* nqq,
		     double* mass,
		     double* sigmv,
		     double* sx,
		     double* sy,
		     double* sz)
{
  //WARNING: at this stage in SixTrack the units have been converted to GeV, m, and rad!
  //The particle energy input is the TOTAL energy
  //mass (i.e. nucm) is already in MeV!
  /*
  double x_in = (*x) * CLHEP::m;
  double y_in = (*y) * CLHEP::m;
  
  //We want px and py, not the angle!
  //p_in is the TOTAL momentum of the particle
  
  double e_in = (*e) * CLHEP::GeV;
  double p_in = sqrt((e_in*e_in) - (*mass * *mass));
  
  // x' = p_x / p_in
  // -> p_x = p_in * x'
  //p_in will now be in MeV, xp, yp will be in rad -> units are good!
  double px_in = p_in * (*xp);
  double py_in = p_in * (*yp);
  
  // p_z^2 = p_in^2 - p_x^2 - p_y^2
  double p_z = sqrt( (p_in*p_in) - (px_in*px_in) - (py_in*py_in) );
  
  CollimationParticle in_particle;
  in_particle.x = x_in;
  in_particle.y = y_in;
  
  in_particle.px = px_in;
  in_particle.py = py_in;
  in_particle.pz = p_z;
  in_particle.p = p_in;
  
  in_particle.e = e_in;
  in_particle.pdgid = *pdgid;
  in_particle.z = *nzz;
  in_particle.a = *naa;
  in_particle.q = *nqq;
  in_particle.m = *mass;
  in_particle.id = input_particles.size();
  
  in_particle.sx = *sx;
  in_particle.sy = *sy;
  in_particle.sz = *sz;
  
  input_particles.push_back(in_particle);
  */
}

extern "C"
void g4_collimate()
{
  /*
  output_particles.clear();
  //Update the gun with this particle's details
  for(size_t n=0; n < input_particles.size(); n++)
    {
      part->SetParticleDetails(input_particles.at(n).x, input_particles.at(n).y, input_particles.at(n).px, input_particles.at(n).py, input_particles.at(n).pz, input_particles.at(n).e, input_particles.at(n).p, input_particles.at(n).pdgid, input_particles.at(n).q, input_particles.at(n).m, input_particles.at(n).sx, input_particles.at(n).sy, input_particles.at(n).sz);
      
      //Tell the "event" about the parent particle ID for tracking secondaries
      
      //Run!
      runManager->BeamOn(1);
    }
  input_particles.clear();
  std::cout << std::flush;
  */
}

extern "C"
void g4_collimate_return(int*     j,
			 double*  x,
			 double*  y,
			 double*  xp,
			 double*  yp,
			 double*  e,
			 int32_t* pdgid,
			 double*  m,
			 int16_t* z,
			 int16_t* a,
			 int16_t* q,
			 double*  sigmv,
			 int*     part_hit,
			 int*     part_abs,
			 double*  part_impact,
			 double*  part_indiv,
			 double*  part_linteract,
			 double*  sx,
			 double*  sy,
			 double*  sz)
{
  /*
    part_hit(j), part_abs(j), part_impact(j), part_indiv(j),
    & part_linteract(j))
    !++  Output information:
    !++
    !++  PART_HIT(MAX_NPART)     Hit flag for last hit (10000*element# + turn#)
    !++  PART_ABS(MAX_NPART)     Abs flag (10000*element# + turn#)
    !++  PART_IMPACT(MAX_NPART)  Impact parameter (0 for inner face)
    !++  PART_INDIV(MAX_NPART)   Divergence of impacting particles
  */
  
  //Here the units have been converted back to GeV and m (in the tracking action)
  /*
  *x  = output_particles.at(*j).x;
  *y  = output_particles.at(*j).y;
  double px = output_particles.at(*j).px;
  double py = output_particles.at(*j).py;
  
  //Remember, sixtrack xp, yp are p_x / p_total
  *xp = output_particles.at(*j).px / output_particles.at(*j).p;
  *yp = output_particles.at(*j).py / output_particles.at(*j).p;
  *e  = output_particles.at(*j).e;
  *pdgid  = output_particles.at(*j).pdgid;
  *z = output_particles.at(*j).z;
  *a  = output_particles.at(*j).a;
  *q  = output_particles.at(*j).q;
  
  //nucm is in MeV on both sides
  *m  = output_particles.at(*j).m;
  
  //Spin
  *sx = output_particles.at(*j).sx;
  *sy = output_particles.at(*j).sy;
  *sz = output_particles.at(*j).sz;
  
  //time, must be converted for using with sigmv
  *sigmv  = output_particles.at(*j).t;
  */
}

std::string CleanFortranString(char* str, size_t count)
{
  //Fortran string nightmares
  std::string Whitespace(" \t\n\r");
  std::string sstring(str,count);
  
  size_t lpos = sstring.find_last_not_of(Whitespace);
  if(lpos!=std::string::npos)
    {
	    sstring.erase(lpos+1);
	}

	std::transform(sstring.begin(), sstring.end(), sstring.begin(), ::toupper);
	//Fortran string happy place
	return sstring;
}

extern "C"
void g4_get_particle_count(int* g4_npart)
{
  //*g4_npart = output_particles.size();
}

extern "C"
void g4_collimation_clear()
{
  //input_particles.clear();
  //output_particles.clear();
}

extern "C"
void g4_keep_id(int* id)
{
  //keep_ids.insert(*id);
}

extern "C"
void g4_add_edep(char* name_in,
		 int* type,
		 double* xstep,
		 double* ystep,
		 double* zstep,
		 double* xmax,
		 double* ymax,
		 double* zmax,
		 double* xbigstep,
		 double* ybigstep,
		 double* zbigstep,
		 double* xbigmax,
		 double* ybigmax,
		 double* zbigmax)
{;}
