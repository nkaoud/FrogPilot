#include "car.h"

namespace {
#define DIM 9
#define EDIM 9
#define MEDIM 9
typedef void (*Hfun)(double *, double *, double *);

double mass;

void set_mass(double x){ mass = x;}

double rotational_inertia;

void set_rotational_inertia(double x){ rotational_inertia = x;}

double center_to_front;

void set_center_to_front(double x){ center_to_front = x;}

double center_to_rear;

void set_center_to_rear(double x){ center_to_rear = x;}

double stiffness_front;

void set_stiffness_front(double x){ stiffness_front = x;}

double stiffness_rear;

void set_stiffness_rear(double x){ stiffness_rear = x;}
const static double MAHA_THRESH_25 = 3.8414588206941227;
const static double MAHA_THRESH_24 = 5.991464547107981;
const static double MAHA_THRESH_30 = 3.8414588206941227;
const static double MAHA_THRESH_26 = 3.8414588206941227;
const static double MAHA_THRESH_27 = 3.8414588206941227;
const static double MAHA_THRESH_29 = 3.8414588206941227;
const static double MAHA_THRESH_28 = 3.8414588206941227;
const static double MAHA_THRESH_31 = 3.8414588206941227;

/******************************************************************************
 *                       Code generated with SymPy 1.12                       *
 *                                                                            *
 *              See http://www.sympy.org/ for more information.               *
 *                                                                            *
 *                         This file is part of 'ekf'                         *
 ******************************************************************************/
void err_fun(double *nom_x, double *delta_x, double *out_6859569633556049423) {
   out_6859569633556049423[0] = delta_x[0] + nom_x[0];
   out_6859569633556049423[1] = delta_x[1] + nom_x[1];
   out_6859569633556049423[2] = delta_x[2] + nom_x[2];
   out_6859569633556049423[3] = delta_x[3] + nom_x[3];
   out_6859569633556049423[4] = delta_x[4] + nom_x[4];
   out_6859569633556049423[5] = delta_x[5] + nom_x[5];
   out_6859569633556049423[6] = delta_x[6] + nom_x[6];
   out_6859569633556049423[7] = delta_x[7] + nom_x[7];
   out_6859569633556049423[8] = delta_x[8] + nom_x[8];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_6501774134728957274) {
   out_6501774134728957274[0] = -nom_x[0] + true_x[0];
   out_6501774134728957274[1] = -nom_x[1] + true_x[1];
   out_6501774134728957274[2] = -nom_x[2] + true_x[2];
   out_6501774134728957274[3] = -nom_x[3] + true_x[3];
   out_6501774134728957274[4] = -nom_x[4] + true_x[4];
   out_6501774134728957274[5] = -nom_x[5] + true_x[5];
   out_6501774134728957274[6] = -nom_x[6] + true_x[6];
   out_6501774134728957274[7] = -nom_x[7] + true_x[7];
   out_6501774134728957274[8] = -nom_x[8] + true_x[8];
}
void H_mod_fun(double *state, double *out_656847899355509699) {
   out_656847899355509699[0] = 1.0;
   out_656847899355509699[1] = 0;
   out_656847899355509699[2] = 0;
   out_656847899355509699[3] = 0;
   out_656847899355509699[4] = 0;
   out_656847899355509699[5] = 0;
   out_656847899355509699[6] = 0;
   out_656847899355509699[7] = 0;
   out_656847899355509699[8] = 0;
   out_656847899355509699[9] = 0;
   out_656847899355509699[10] = 1.0;
   out_656847899355509699[11] = 0;
   out_656847899355509699[12] = 0;
   out_656847899355509699[13] = 0;
   out_656847899355509699[14] = 0;
   out_656847899355509699[15] = 0;
   out_656847899355509699[16] = 0;
   out_656847899355509699[17] = 0;
   out_656847899355509699[18] = 0;
   out_656847899355509699[19] = 0;
   out_656847899355509699[20] = 1.0;
   out_656847899355509699[21] = 0;
   out_656847899355509699[22] = 0;
   out_656847899355509699[23] = 0;
   out_656847899355509699[24] = 0;
   out_656847899355509699[25] = 0;
   out_656847899355509699[26] = 0;
   out_656847899355509699[27] = 0;
   out_656847899355509699[28] = 0;
   out_656847899355509699[29] = 0;
   out_656847899355509699[30] = 1.0;
   out_656847899355509699[31] = 0;
   out_656847899355509699[32] = 0;
   out_656847899355509699[33] = 0;
   out_656847899355509699[34] = 0;
   out_656847899355509699[35] = 0;
   out_656847899355509699[36] = 0;
   out_656847899355509699[37] = 0;
   out_656847899355509699[38] = 0;
   out_656847899355509699[39] = 0;
   out_656847899355509699[40] = 1.0;
   out_656847899355509699[41] = 0;
   out_656847899355509699[42] = 0;
   out_656847899355509699[43] = 0;
   out_656847899355509699[44] = 0;
   out_656847899355509699[45] = 0;
   out_656847899355509699[46] = 0;
   out_656847899355509699[47] = 0;
   out_656847899355509699[48] = 0;
   out_656847899355509699[49] = 0;
   out_656847899355509699[50] = 1.0;
   out_656847899355509699[51] = 0;
   out_656847899355509699[52] = 0;
   out_656847899355509699[53] = 0;
   out_656847899355509699[54] = 0;
   out_656847899355509699[55] = 0;
   out_656847899355509699[56] = 0;
   out_656847899355509699[57] = 0;
   out_656847899355509699[58] = 0;
   out_656847899355509699[59] = 0;
   out_656847899355509699[60] = 1.0;
   out_656847899355509699[61] = 0;
   out_656847899355509699[62] = 0;
   out_656847899355509699[63] = 0;
   out_656847899355509699[64] = 0;
   out_656847899355509699[65] = 0;
   out_656847899355509699[66] = 0;
   out_656847899355509699[67] = 0;
   out_656847899355509699[68] = 0;
   out_656847899355509699[69] = 0;
   out_656847899355509699[70] = 1.0;
   out_656847899355509699[71] = 0;
   out_656847899355509699[72] = 0;
   out_656847899355509699[73] = 0;
   out_656847899355509699[74] = 0;
   out_656847899355509699[75] = 0;
   out_656847899355509699[76] = 0;
   out_656847899355509699[77] = 0;
   out_656847899355509699[78] = 0;
   out_656847899355509699[79] = 0;
   out_656847899355509699[80] = 1.0;
}
void f_fun(double *state, double dt, double *out_2927030515880606036) {
   out_2927030515880606036[0] = state[0];
   out_2927030515880606036[1] = state[1];
   out_2927030515880606036[2] = state[2];
   out_2927030515880606036[3] = state[3];
   out_2927030515880606036[4] = state[4];
   out_2927030515880606036[5] = dt*((-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]))*state[6] - 9.8000000000000007*state[8] + stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*state[1]) + (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*state[4])) + state[5];
   out_2927030515880606036[6] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*state[4])) + state[6];
   out_2927030515880606036[7] = state[7];
   out_2927030515880606036[8] = state[8];
}
void F_fun(double *state, double dt, double *out_3961897290624768689) {
   out_3961897290624768689[0] = 1;
   out_3961897290624768689[1] = 0;
   out_3961897290624768689[2] = 0;
   out_3961897290624768689[3] = 0;
   out_3961897290624768689[4] = 0;
   out_3961897290624768689[5] = 0;
   out_3961897290624768689[6] = 0;
   out_3961897290624768689[7] = 0;
   out_3961897290624768689[8] = 0;
   out_3961897290624768689[9] = 0;
   out_3961897290624768689[10] = 1;
   out_3961897290624768689[11] = 0;
   out_3961897290624768689[12] = 0;
   out_3961897290624768689[13] = 0;
   out_3961897290624768689[14] = 0;
   out_3961897290624768689[15] = 0;
   out_3961897290624768689[16] = 0;
   out_3961897290624768689[17] = 0;
   out_3961897290624768689[18] = 0;
   out_3961897290624768689[19] = 0;
   out_3961897290624768689[20] = 1;
   out_3961897290624768689[21] = 0;
   out_3961897290624768689[22] = 0;
   out_3961897290624768689[23] = 0;
   out_3961897290624768689[24] = 0;
   out_3961897290624768689[25] = 0;
   out_3961897290624768689[26] = 0;
   out_3961897290624768689[27] = 0;
   out_3961897290624768689[28] = 0;
   out_3961897290624768689[29] = 0;
   out_3961897290624768689[30] = 1;
   out_3961897290624768689[31] = 0;
   out_3961897290624768689[32] = 0;
   out_3961897290624768689[33] = 0;
   out_3961897290624768689[34] = 0;
   out_3961897290624768689[35] = 0;
   out_3961897290624768689[36] = 0;
   out_3961897290624768689[37] = 0;
   out_3961897290624768689[38] = 0;
   out_3961897290624768689[39] = 0;
   out_3961897290624768689[40] = 1;
   out_3961897290624768689[41] = 0;
   out_3961897290624768689[42] = 0;
   out_3961897290624768689[43] = 0;
   out_3961897290624768689[44] = 0;
   out_3961897290624768689[45] = dt*(stiffness_front*(-state[2] - state[3] + state[7])/(mass*state[1]) + (-stiffness_front - stiffness_rear)*state[5]/(mass*state[4]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[6]/(mass*state[4]));
   out_3961897290624768689[46] = -dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*pow(state[1], 2));
   out_3961897290624768689[47] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_3961897290624768689[48] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_3961897290624768689[49] = dt*((-1 - (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*pow(state[4], 2)))*state[6] - (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*pow(state[4], 2)));
   out_3961897290624768689[50] = dt*(-stiffness_front*state[0] - stiffness_rear*state[0])/(mass*state[4]) + 1;
   out_3961897290624768689[51] = dt*(-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]));
   out_3961897290624768689[52] = dt*stiffness_front*state[0]/(mass*state[1]);
   out_3961897290624768689[53] = -9.8000000000000007*dt;
   out_3961897290624768689[54] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front - pow(center_to_rear, 2)*stiffness_rear)*state[6]/(rotational_inertia*state[4]));
   out_3961897290624768689[55] = -center_to_front*dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*pow(state[1], 2));
   out_3961897290624768689[56] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_3961897290624768689[57] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_3961897290624768689[58] = dt*(-(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*pow(state[4], 2)) - (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*pow(state[4], 2)));
   out_3961897290624768689[59] = dt*(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(rotational_inertia*state[4]);
   out_3961897290624768689[60] = dt*(-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])/(rotational_inertia*state[4]) + 1;
   out_3961897290624768689[61] = center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_3961897290624768689[62] = 0;
   out_3961897290624768689[63] = 0;
   out_3961897290624768689[64] = 0;
   out_3961897290624768689[65] = 0;
   out_3961897290624768689[66] = 0;
   out_3961897290624768689[67] = 0;
   out_3961897290624768689[68] = 0;
   out_3961897290624768689[69] = 0;
   out_3961897290624768689[70] = 1;
   out_3961897290624768689[71] = 0;
   out_3961897290624768689[72] = 0;
   out_3961897290624768689[73] = 0;
   out_3961897290624768689[74] = 0;
   out_3961897290624768689[75] = 0;
   out_3961897290624768689[76] = 0;
   out_3961897290624768689[77] = 0;
   out_3961897290624768689[78] = 0;
   out_3961897290624768689[79] = 0;
   out_3961897290624768689[80] = 1;
}
void h_25(double *state, double *unused, double *out_4536610646621242271) {
   out_4536610646621242271[0] = state[6];
}
void H_25(double *state, double *unused, double *out_7591328919834093458) {
   out_7591328919834093458[0] = 0;
   out_7591328919834093458[1] = 0;
   out_7591328919834093458[2] = 0;
   out_7591328919834093458[3] = 0;
   out_7591328919834093458[4] = 0;
   out_7591328919834093458[5] = 0;
   out_7591328919834093458[6] = 1;
   out_7591328919834093458[7] = 0;
   out_7591328919834093458[8] = 0;
}
void h_24(double *state, double *unused, double *out_3575794141303901967) {
   out_3575794141303901967[0] = state[4];
   out_3575794141303901967[1] = state[5];
}
void H_24(double *state, double *unused, double *out_8682765554869958592) {
   out_8682765554869958592[0] = 0;
   out_8682765554869958592[1] = 0;
   out_8682765554869958592[2] = 0;
   out_8682765554869958592[3] = 0;
   out_8682765554869958592[4] = 1;
   out_8682765554869958592[5] = 0;
   out_8682765554869958592[6] = 0;
   out_8682765554869958592[7] = 0;
   out_8682765554869958592[8] = 0;
   out_8682765554869958592[9] = 0;
   out_8682765554869958592[10] = 0;
   out_8682765554869958592[11] = 0;
   out_8682765554869958592[12] = 0;
   out_8682765554869958592[13] = 0;
   out_8682765554869958592[14] = 1;
   out_8682765554869958592[15] = 0;
   out_8682765554869958592[16] = 0;
   out_8682765554869958592[17] = 0;
}
void h_30(double *state, double *unused, double *out_4261416584336736382) {
   out_4261416584336736382[0] = state[4];
}
void H_30(double *state, double *unused, double *out_5072995961326844831) {
   out_5072995961326844831[0] = 0;
   out_5072995961326844831[1] = 0;
   out_5072995961326844831[2] = 0;
   out_5072995961326844831[3] = 0;
   out_5072995961326844831[4] = 1;
   out_5072995961326844831[5] = 0;
   out_5072995961326844831[6] = 0;
   out_5072995961326844831[7] = 0;
   out_5072995961326844831[8] = 0;
}
void h_26(double *state, double *unused, double *out_8307042001814100146) {
   out_8307042001814100146[0] = state[7];
}
void H_26(double *state, double *unused, double *out_7113911835001401934) {
   out_7113911835001401934[0] = 0;
   out_7113911835001401934[1] = 0;
   out_7113911835001401934[2] = 0;
   out_7113911835001401934[3] = 0;
   out_7113911835001401934[4] = 0;
   out_7113911835001401934[5] = 0;
   out_7113911835001401934[6] = 0;
   out_7113911835001401934[7] = 1;
   out_7113911835001401934[8] = 0;
}
void h_27(double *state, double *unused, double *out_6227058481356923231) {
   out_6227058481356923231[0] = state[3];
}
void H_27(double *state, double *unused, double *out_2849401890142901614) {
   out_2849401890142901614[0] = 0;
   out_2849401890142901614[1] = 0;
   out_2849401890142901614[2] = 0;
   out_2849401890142901614[3] = 1;
   out_2849401890142901614[4] = 0;
   out_2849401890142901614[5] = 0;
   out_2849401890142901614[6] = 0;
   out_2849401890142901614[7] = 0;
   out_2849401890142901614[8] = 0;
}
void h_29(double *state, double *unused, double *out_740256683324109263) {
   out_740256683324109263[0] = state[1];
}
void H_29(double *state, double *unused, double *out_4562764617012452647) {
   out_4562764617012452647[0] = 0;
   out_4562764617012452647[1] = 1;
   out_4562764617012452647[2] = 0;
   out_4562764617012452647[3] = 0;
   out_4562764617012452647[4] = 0;
   out_4562764617012452647[5] = 0;
   out_4562764617012452647[6] = 0;
   out_4562764617012452647[7] = 0;
   out_4562764617012452647[8] = 0;
}
void h_28(double *state, double *unused, double *out_5820644074938779239) {
   out_5820644074938779239[0] = state[0];
}
void H_28(double *state, double *unused, double *out_8801580439627568395) {
   out_8801580439627568395[0] = 1;
   out_8801580439627568395[1] = 0;
   out_8801580439627568395[2] = 0;
   out_8801580439627568395[3] = 0;
   out_8801580439627568395[4] = 0;
   out_8801580439627568395[5] = 0;
   out_8801580439627568395[6] = 0;
   out_8801580439627568395[7] = 0;
   out_8801580439627568395[8] = 0;
}
void h_31(double *state, double *unused, double *out_7754686231247649216) {
   out_7754686231247649216[0] = state[8];
}
void H_31(double *state, double *unused, double *out_7560682957957133030) {
   out_7560682957957133030[0] = 0;
   out_7560682957957133030[1] = 0;
   out_7560682957957133030[2] = 0;
   out_7560682957957133030[3] = 0;
   out_7560682957957133030[4] = 0;
   out_7560682957957133030[5] = 0;
   out_7560682957957133030[6] = 0;
   out_7560682957957133030[7] = 0;
   out_7560682957957133030[8] = 1;
}
#include <eigen3/Eigen/Dense>
#include <iostream>

typedef Eigen::Matrix<double, DIM, DIM, Eigen::RowMajor> DDM;
typedef Eigen::Matrix<double, EDIM, EDIM, Eigen::RowMajor> EEM;
typedef Eigen::Matrix<double, DIM, EDIM, Eigen::RowMajor> DEM;

void predict(double *in_x, double *in_P, double *in_Q, double dt) {
  typedef Eigen::Matrix<double, MEDIM, MEDIM, Eigen::RowMajor> RRM;

  double nx[DIM] = {0};
  double in_F[EDIM*EDIM] = {0};

  // functions from sympy
  f_fun(in_x, dt, nx);
  F_fun(in_x, dt, in_F);


  EEM F(in_F);
  EEM P(in_P);
  EEM Q(in_Q);

  RRM F_main = F.topLeftCorner(MEDIM, MEDIM);
  P.topLeftCorner(MEDIM, MEDIM) = (F_main * P.topLeftCorner(MEDIM, MEDIM)) * F_main.transpose();
  P.topRightCorner(MEDIM, EDIM - MEDIM) = F_main * P.topRightCorner(MEDIM, EDIM - MEDIM);
  P.bottomLeftCorner(EDIM - MEDIM, MEDIM) = P.bottomLeftCorner(EDIM - MEDIM, MEDIM) * F_main.transpose();

  P = P + dt*Q;

  // copy out state
  memcpy(in_x, nx, DIM * sizeof(double));
  memcpy(in_P, P.data(), EDIM * EDIM * sizeof(double));
}

// note: extra_args dim only correct when null space projecting
// otherwise 1
template <int ZDIM, int EADIM, bool MAHA_TEST>
void update(double *in_x, double *in_P, Hfun h_fun, Hfun H_fun, Hfun Hea_fun, double *in_z, double *in_R, double *in_ea, double MAHA_THRESHOLD) {
  typedef Eigen::Matrix<double, ZDIM, ZDIM, Eigen::RowMajor> ZZM;
  typedef Eigen::Matrix<double, ZDIM, DIM, Eigen::RowMajor> ZDM;
  typedef Eigen::Matrix<double, Eigen::Dynamic, EDIM, Eigen::RowMajor> XEM;
  //typedef Eigen::Matrix<double, EDIM, ZDIM, Eigen::RowMajor> EZM;
  typedef Eigen::Matrix<double, Eigen::Dynamic, 1> X1M;
  typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> XXM;

  double in_hx[ZDIM] = {0};
  double in_H[ZDIM * DIM] = {0};
  double in_H_mod[EDIM * DIM] = {0};
  double delta_x[EDIM] = {0};
  double x_new[DIM] = {0};


  // state x, P
  Eigen::Matrix<double, ZDIM, 1> z(in_z);
  EEM P(in_P);
  ZZM pre_R(in_R);

  // functions from sympy
  h_fun(in_x, in_ea, in_hx);
  H_fun(in_x, in_ea, in_H);
  ZDM pre_H(in_H);

  // get y (y = z - hx)
  Eigen::Matrix<double, ZDIM, 1> pre_y(in_hx); pre_y = z - pre_y;
  X1M y; XXM H; XXM R;
  if (Hea_fun){
    typedef Eigen::Matrix<double, ZDIM, EADIM, Eigen::RowMajor> ZAM;
    double in_Hea[ZDIM * EADIM] = {0};
    Hea_fun(in_x, in_ea, in_Hea);
    ZAM Hea(in_Hea);
    XXM A = Hea.transpose().fullPivLu().kernel();


    y = A.transpose() * pre_y;
    H = A.transpose() * pre_H;
    R = A.transpose() * pre_R * A;
  } else {
    y = pre_y;
    H = pre_H;
    R = pre_R;
  }
  // get modified H
  H_mod_fun(in_x, in_H_mod);
  DEM H_mod(in_H_mod);
  XEM H_err = H * H_mod;

  // Do mahalobis distance test
  if (MAHA_TEST){
    XXM a = (H_err * P * H_err.transpose() + R).inverse();
    double maha_dist = y.transpose() * a * y;
    if (maha_dist > MAHA_THRESHOLD){
      R = 1.0e16 * R;
    }
  }

  // Outlier resilient weighting
  double weight = 1;//(1.5)/(1 + y.squaredNorm()/R.sum());

  // kalman gains and I_KH
  XXM S = ((H_err * P) * H_err.transpose()) + R/weight;
  XEM KT = S.fullPivLu().solve(H_err * P.transpose());
  //EZM K = KT.transpose(); TODO: WHY DOES THIS NOT COMPILE?
  //EZM K = S.fullPivLu().solve(H_err * P.transpose()).transpose();
  //std::cout << "Here is the matrix rot:\n" << K << std::endl;
  EEM I_KH = Eigen::Matrix<double, EDIM, EDIM>::Identity() - (KT.transpose() * H_err);

  // update state by injecting dx
  Eigen::Matrix<double, EDIM, 1> dx(delta_x);
  dx  = (KT.transpose() * y);
  memcpy(delta_x, dx.data(), EDIM * sizeof(double));
  err_fun(in_x, delta_x, x_new);
  Eigen::Matrix<double, DIM, 1> x(x_new);

  // update cov
  P = ((I_KH * P) * I_KH.transpose()) + ((KT.transpose() * R) * KT);

  // copy out state
  memcpy(in_x, x.data(), DIM * sizeof(double));
  memcpy(in_P, P.data(), EDIM * EDIM * sizeof(double));
  memcpy(in_z, y.data(), y.rows() * sizeof(double));
}




}
extern "C" {

void car_update_25(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_25, H_25, NULL, in_z, in_R, in_ea, MAHA_THRESH_25);
}
void car_update_24(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<2, 3, 0>(in_x, in_P, h_24, H_24, NULL, in_z, in_R, in_ea, MAHA_THRESH_24);
}
void car_update_30(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_30, H_30, NULL, in_z, in_R, in_ea, MAHA_THRESH_30);
}
void car_update_26(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_26, H_26, NULL, in_z, in_R, in_ea, MAHA_THRESH_26);
}
void car_update_27(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_27, H_27, NULL, in_z, in_R, in_ea, MAHA_THRESH_27);
}
void car_update_29(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_29, H_29, NULL, in_z, in_R, in_ea, MAHA_THRESH_29);
}
void car_update_28(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_28, H_28, NULL, in_z, in_R, in_ea, MAHA_THRESH_28);
}
void car_update_31(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_31, H_31, NULL, in_z, in_R, in_ea, MAHA_THRESH_31);
}
void car_err_fun(double *nom_x, double *delta_x, double *out_6859569633556049423) {
  err_fun(nom_x, delta_x, out_6859569633556049423);
}
void car_inv_err_fun(double *nom_x, double *true_x, double *out_6501774134728957274) {
  inv_err_fun(nom_x, true_x, out_6501774134728957274);
}
void car_H_mod_fun(double *state, double *out_656847899355509699) {
  H_mod_fun(state, out_656847899355509699);
}
void car_f_fun(double *state, double dt, double *out_2927030515880606036) {
  f_fun(state,  dt, out_2927030515880606036);
}
void car_F_fun(double *state, double dt, double *out_3961897290624768689) {
  F_fun(state,  dt, out_3961897290624768689);
}
void car_h_25(double *state, double *unused, double *out_4536610646621242271) {
  h_25(state, unused, out_4536610646621242271);
}
void car_H_25(double *state, double *unused, double *out_7591328919834093458) {
  H_25(state, unused, out_7591328919834093458);
}
void car_h_24(double *state, double *unused, double *out_3575794141303901967) {
  h_24(state, unused, out_3575794141303901967);
}
void car_H_24(double *state, double *unused, double *out_8682765554869958592) {
  H_24(state, unused, out_8682765554869958592);
}
void car_h_30(double *state, double *unused, double *out_4261416584336736382) {
  h_30(state, unused, out_4261416584336736382);
}
void car_H_30(double *state, double *unused, double *out_5072995961326844831) {
  H_30(state, unused, out_5072995961326844831);
}
void car_h_26(double *state, double *unused, double *out_8307042001814100146) {
  h_26(state, unused, out_8307042001814100146);
}
void car_H_26(double *state, double *unused, double *out_7113911835001401934) {
  H_26(state, unused, out_7113911835001401934);
}
void car_h_27(double *state, double *unused, double *out_6227058481356923231) {
  h_27(state, unused, out_6227058481356923231);
}
void car_H_27(double *state, double *unused, double *out_2849401890142901614) {
  H_27(state, unused, out_2849401890142901614);
}
void car_h_29(double *state, double *unused, double *out_740256683324109263) {
  h_29(state, unused, out_740256683324109263);
}
void car_H_29(double *state, double *unused, double *out_4562764617012452647) {
  H_29(state, unused, out_4562764617012452647);
}
void car_h_28(double *state, double *unused, double *out_5820644074938779239) {
  h_28(state, unused, out_5820644074938779239);
}
void car_H_28(double *state, double *unused, double *out_8801580439627568395) {
  H_28(state, unused, out_8801580439627568395);
}
void car_h_31(double *state, double *unused, double *out_7754686231247649216) {
  h_31(state, unused, out_7754686231247649216);
}
void car_H_31(double *state, double *unused, double *out_7560682957957133030) {
  H_31(state, unused, out_7560682957957133030);
}
void car_predict(double *in_x, double *in_P, double *in_Q, double dt) {
  predict(in_x, in_P, in_Q, dt);
}
void car_set_mass(double x) {
  set_mass(x);
}
void car_set_rotational_inertia(double x) {
  set_rotational_inertia(x);
}
void car_set_center_to_front(double x) {
  set_center_to_front(x);
}
void car_set_center_to_rear(double x) {
  set_center_to_rear(x);
}
void car_set_stiffness_front(double x) {
  set_stiffness_front(x);
}
void car_set_stiffness_rear(double x) {
  set_stiffness_rear(x);
}
}

const EKF car = {
  .name = "car",
  .kinds = { 25, 24, 30, 26, 27, 29, 28, 31 },
  .feature_kinds = {  },
  .f_fun = car_f_fun,
  .F_fun = car_F_fun,
  .err_fun = car_err_fun,
  .inv_err_fun = car_inv_err_fun,
  .H_mod_fun = car_H_mod_fun,
  .predict = car_predict,
  .hs = {
    { 25, car_h_25 },
    { 24, car_h_24 },
    { 30, car_h_30 },
    { 26, car_h_26 },
    { 27, car_h_27 },
    { 29, car_h_29 },
    { 28, car_h_28 },
    { 31, car_h_31 },
  },
  .Hs = {
    { 25, car_H_25 },
    { 24, car_H_24 },
    { 30, car_H_30 },
    { 26, car_H_26 },
    { 27, car_H_27 },
    { 29, car_H_29 },
    { 28, car_H_28 },
    { 31, car_H_31 },
  },
  .updates = {
    { 25, car_update_25 },
    { 24, car_update_24 },
    { 30, car_update_30 },
    { 26, car_update_26 },
    { 27, car_update_27 },
    { 29, car_update_29 },
    { 28, car_update_28 },
    { 31, car_update_31 },
  },
  .Hes = {
  },
  .sets = {
    { "mass", car_set_mass },
    { "rotational_inertia", car_set_rotational_inertia },
    { "center_to_front", car_set_center_to_front },
    { "center_to_rear", car_set_center_to_rear },
    { "stiffness_front", car_set_stiffness_front },
    { "stiffness_rear", car_set_stiffness_rear },
  },
  .extra_routines = {
  },
};

ekf_lib_init(car)
