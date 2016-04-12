#include "BDSOutputROOTEventSampler.hh"

class SamplerAnalysis
{
public:
  SamplerAnalysis();
  SamplerAnalysis(BDSOutputROOTEventSampler<float>*);
  void CommonCtor();
  virtual ~SamplerAnalysis();

  void Initialise();
  void Process();
  void Terminate();

  BDSOutputROOTEventSampler<float> *s;

protected:
  // sums - initialised to zero as that's what they start at
  double npart;
  double s_s; //so we can get the mean s from not cut data
  double E_s; //sum of energy needed to calculate mean energy
  double E_m; //mean energy, needs a new variable because of re-initailisation
  //after second loop
  double dE; //denotes relative energy spread

  //power sums
  //s_i_k denotes a sum of horizontal co-ordinates, e.g. for horizontal: sh_i_k= sum(x^i*xp^k)
  //energy-coordinate power sums
  double se_1;
  double se_2;
  double sxpe_1_1;
  double sxe_1_1;
  double sype_1_1;
  double sye_1_1;

  //coordinate power sums
  double sh_0_1, sh_1_0, sv_0_1, sv_1_0;
  double sh_0_2, sh_2_0, sv_0_2, sv_2_0;
  double sh_1_1, sv_1_1;
  double sh_0_3, sh_3_0, sv_0_3, sv_3_0;
  double sh_2_1, sh_1_2, sv_2_1, sv_1_2;
  double sh_4_0, sh_0_4, sv_4_0, sv_0_4;
  double sh_3_1, sh_1_3, sv_3_1, sv_1_3;
  double sh_2_2, sv_2_2;

  double se_3,se_4;
  double sxe_2_1, sxe_1_2, sxe_2_2, sxe_3_1, sxe_1_3;
  double sye_2_1, sye_1_2, sye_2_2, sye_3_1, sye_1_3;
  double sxpe_2_1, sxpe_1_2, sxpe_2_2, sxpe_3_1, sxpe_1_3;
  double sype_2_1, sype_1_2, sype_2_2, sype_3_1, sype_1_3;

  //central moments
  //m_i_k denotes a central moment of order i in first coordinate and j in the second coordinate, h-horizontal, v-vertical
  double mh_1_0, mh_0_1, mv_1_0, mv_0_1, me_1; //these are means
  double mh_2_0, mh_0_2, mv_2_0, mv_0_2, me_2; //these are variances
  double mh_1_1, mv_1_1, mex_1_1, mey_1_1, mexp_1_1, meyp_1_1; //these are covariances
  double mh_4_0, mh_0_4, mv_4_0, mv_0_4;
  double mh_3_1, mh_1_3, mv_3_1, mv_1_3;
  double mh_2_2, mv_2_2;

  double me_4;
  double mex_3_1, mex_1_3, mex_2_2;
  double mey_3_1, mey_1_3, mey_2_2;
  double mexp_3_1, mexp_1_3, mexp_2_2;
  double meyp_3_1, meyp_1_3, meyp_2_2;

  //covariances of central moments for error calculations. cov_vc_x_xxp denotes the covariance of a variance (v) and a covariance (c) of the moments that follow
  double cov_vv_x, cov_vv_y;
  double cov_vv_xp, cov_vv_yp;
  double cov_cc_xxp_xxp, cov_cc_yyp_yyp;
  double cov_vc_x_xxp, cov_vc_y_yyp;
  double cov_vc_xp_xxp, cov_vc_yp_yyp;
  double cov_vv_x_xp, cov_vv_y_yp;

  double cov_vv_E;
  double cov_cc_Ex_Ex, cov_cc_Ey_Ey, cov_cc_Exp_Exp, cov_cc_Eyp_Eyp;
  double cov_vc_E_Ex, cov_vc_E_Ey, cov_vc_E_Exp, cov_vc_E_Eyp;

  double emitt_x, emitt_y;
  double beta_x, beta_y, alph_x, alph_y, disp_x, disp_xp, disp_y, disp_yp;
  double sigma_x, sigma_y;
  double sigma_xp, sigma_yp;

  //d_fun_d_v denotes a derivative of fun w.r.t variable v
  //var_func_x denotes the variance of the corresponding function
  double d_emx_d_xx, d_emx_d_xxp, d_emx_d_xpxp; //for opt func uncertatnies
  double d_emy_d_yy, d_emy_d_yyp, d_emy_d_ypyp;
  double var_emitt_x, var_emitt_y;
  double sigma_emitt_x, sigma_emitt_y;
  double d_btx_d_xx, d_btx_d_xxp, d_btx_d_xpxp;
  double d_bty_d_yy, d_bty_d_yyp, d_bty_d_ypyp;
  double var_beta_x, var_beta_y;
  double sigma_beta_x, sigma_beta_y;
  double d_alx_d_xx, d_alx_d_xxp, d_alx_d_xpxp;
  double d_aly_d_yy, d_aly_d_yyp, d_aly_d_ypyp;
  double var_alph_x, var_alph_y;
  double sigma_alph_x, sigma_alph_y;
  double var_var_x, var_var_y, var_var_xp, var_var_yp;
  double sigma_sigm_x, sigma_sigm_y, sigma_sigm_xp, sigma_sigm_yp;

  double d_disp_d_cov;
  double d_dispx_d_varE, d_dispy_d_varE, d_dispxp_d_varE, d_dispyp_d_varE;
  double var_disp_x, var_disp_y, var_disp_xp, var_disp_yp;
  double sigma_disp_x, sigma_disp_y, sigma_disp_xp, sigma_disp_yp;
};
