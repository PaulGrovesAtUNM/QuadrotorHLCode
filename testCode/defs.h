#ifndef DEFS_H
#define DEFS_H

#include <cmath>
#include <string>


using namespace std;

/*** GLOBAL VARIABLE DEFINITIONS ***/
/* Rather than scattering lots of parameters that will be kept constant (such as quadrotor mass, acceleration
   due to gravity, etc.) all over the program, all constants are kept here (with the exception of a few used
   in the stateEstimation function; as different stateEstimation algorithms might be used, constants for 
   one algorithm would be unnecessary for another one). This becomes extremely convenient for functions 
   that use arrays, as functions with two-dimensional arrays require any dimensions after the first to be 
   constants. See below for the inclusion of the actual functions in the program. */

// Variable and Conversion Constants
const double PI= 3.141592653589793;
const double RPM_TO_RADS = 2*PI/60.0;
const double RADS_TO_RPM =60.0/(2*PI);

// Quadrotor Parameters
const double G = 9.81; // acceleration due to gravity (m/s^2)
const double M_QUAD = 0.502; // mass of the quadrotor (kg)
const double J_QUAD_XY = 0.0117, J_QUAD_Z = 0.0234; // rotational inertia of quadrotor around x, y, z axes (SI units)
const double L_QUAD = 0.18; // length of quadrotor arm (m)
const double R_PROP = 0.1; // radius of rotor (m)
const double T_FACTOR = 6.11*pow(10,-8); // Propeller thrust factor (N/(rpm)^2); given parameter
const double D_FACTOR = 1.5*pow(10,-9); //  Propeller drag factor (N*m/(rpm)^2); given parameter
const double B_PROP = T_FACTOR/pow(RPM_TO_RADS,2); // thrust factor (N/(rad)^2); actually used
const double K_PROP = D_FACTOR/pow(RPM_TO_RADS,2); // drag factor (N*m/(rad)^2); actually used
const double OMEGA_MAX = 8600*RPM_TO_RADS, OMEGA_MIN = 1200*RPM_TO_RADS; //max and min motor speeds (rad/s)
const double OMEGA_SQ_MAX = pow(OMEGA_MAX,2), OMEGA_SQ_MIN = pow(OMEGA_MIN,2); // squared max and min motor speeds

// Simulation Parameters
const double TS=.01; // time step size of program and Vicon
const double TS_MICRO = TS * 1000000; // time step in microseconds
const double OMEGA_IDLE = 1300*RPM_TO_RADS;  // defines the idle motor speed (rad/s)
const double OMEGA_RAMP = 4000*RPM_TO_RADS;  // defines the final motor speed after ramping up (rad/s)
const int STATE_NUM = 12;	// number of states
const int LATENCY_TIME_STEP_NUM = 0; // the loop latency of the system, expressed in time steps
const int TIME_STEP_NUM = 2 + LATENCY_TIME_STEP_NUM; // the total number of time steps tracked by the program.
// This is used in controlling how many state vectors and input vectors are tracked while the program runs.
// For arrays that use TIME_STEP_NUM to determine their width, [0] in this dimension references info from the 
// last time step, [1] references info from the current time step, and anything from [2] to [1+LATENCY_TIME_STEP_NUM]
// references future time steps. These future time steps are used for state prediction to compensate for the latency.

// Position Reference Parameters
const double REF_Z = 0.70; // sets the reference height of the quadrotor to a non-zero value

// Safety Parameters
const double X_ERROR_KILL = 0.4;
const double Y_ERROR_KILL = 0.4; // these variables set the X, Y, and Z limits for where the quadrotor should be killed.
const double Z_ERROR_KILL = 1.0;

// Vicon-related stuff
const std::string QUAD_NAME = "quadrotor_1b"; // specifies the name of the Vicon model that will be tracked
const int COORD_NUM = 6; 				// specifies how many coordinates the Vicon system captures

// State and Input Definitions (used for array indexing; also makes it easy to change which state is which index
// for most, but not all, aspects of the program.)
const int X = 0, Y = 1, Z = 2, DX = 3, DY = 4, DZ = 5;
const int OX = 6, OY = 7, OZ = 8, DOX = 9, DOY = 10, DOZ = 11;
const int THRUST = 0, TORQUE_X = 1, TORQUE_Y = 2, TORQUE_Z = 3;

#endif
