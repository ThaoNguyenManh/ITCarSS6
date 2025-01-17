/*This file has been prepared for Doxygen automatic documentation generation.*/   
/*! \file *********************************************************************  
 *  
 * \brief General PID implementation for AVR.  
 *  
 * Discrete PID controller implementation. Set up by giving P/I/D terms  
 * to Init_PID(), and uses a struct PID_DATA to store internal values.  
 *  
 * - File:               pid.c  
 * - Compiler:           ATMEL STUDIO  
 * - Supported devices:  All AVR devices can be used.  
 * - AppNote:            AVR221 - Discrete PID controller  
 *  
 * \author               Atmel Corporation: http://www.atmel.com \n  
 *                       Support email: avr@atmel.com  
 *  
 * $Name: RELEASE_1_0 $  
 * $Revision: 1.1 $  
 * $RCSfile: pid.c,v $  
 * $Date: 2006/02/16 11:46:13 $  
 *****************************************************************************/ 

#include "stdint.h" 

#define SCALING_FACTOR		128
#define TIME_INTERVAL_PID   100	//ms

typedef struct PID_DATA{ 
  //! Last process value, used to find derivative of process value. 
  int16_t lastProcessValue; 
  //! Summation of errors, used for integrate calculations 
  int32_t sumError; 
  //! The Proportional tuning constant, multiplied with SCALING_FACTOR 
  int16_t P_Factor; 
  //! The Integral tuning constant, multiplied with SCALING_FACTOR 
  int16_t I_Factor; 
  //! The Derivative tuning constant, multiplied with SCALING_FACTOR 
  int16_t D_Factor; 
  //! Maximum allowed error, avoid overflow 
  int16_t maxError; 
  //! Maximum allowed sumerror, avoid overflow 
  int32_t maxSumError; 
} pidData_t; 
 
// Maximum value of variables 
#define MAX_INT         INT16_MAX 
#define MAX_LONG        INT32_MAX 
#define MAX_I_TERM      (MAX_LONG / 2) 
 
// Boolean values 
#define FALSE           0 
#define TRUE            1 

/*! \brief Initialisation of PID controller parameters.  
 *  
 *  Initialise the variables used by the PID algorithm.  
 *  
 *  \param p_factor  Proportional term.  
 *  \param i_factor  Integral term.  
 *  \param d_factor  Derivate term.  
 *  \param pid  Struct with PID status.  
 */ 
void pid_Init(int16_t p_factor, int16_t i_factor, int16_t d_factor, struct PID_DATA *pid)
{
	 // Start values for PID controller
	 pid->sumError = 0;
	 pid->lastProcessValue = 0;
	 // Tuning constants for PID loop
	 pid->P_Factor = p_factor;
	 pid->I_Factor = i_factor;
	 pid->D_Factor = d_factor;
	 // Limits to avoid overflow
	 pid->maxError = MAX_INT / (pid->P_Factor + 1);
	 pid->maxSumError = MAX_I_TERM / (pid->I_Factor + 1);
}

/*! \brief PID control algorithm.  
 *  
 *  Calculates output from setpoint, process value and PID status.  
 *  
 *  \param setPoint  Desired value.  
 *  \param processValue  Measured value.  
 *  \param pid_st  PID status struct.  
 */ 
int16_t pid_Controller(int16_t setPoint, int16_t processValue, struct PID_DATA *pid_st)
{
	int16_t error, p_term, d_term;
	int32_t i_term, ret, temp;
	
	error = setPoint - processValue;
	// Calculate Pterm and limit error overflow
	if (error > pid_st->maxError){
		p_term = MAX_INT;
	}
	else if (error < -pid_st->maxError){
		p_term = -MAX_INT;
	}
	else{
		p_term = pid_st->P_Factor * error;
	}
	
	// Calculate Iterm and limit integral runaway
	temp = pid_st->sumError + error;
	if(temp > pid_st->maxSumError){
		i_term = MAX_I_TERM;
		pid_st->sumError = pid_st->maxSumError;
	}
	else if(temp < -pid_st->maxSumError){
		i_term = -MAX_I_TERM;
		pid_st->sumError = -pid_st->maxSumError;
	}
	else{
		pid_st->sumError = temp;
		i_term = pid_st->I_Factor * pid_st->sumError;
	}
	
	// Calculate Dterm
	d_term = pid_st->D_Factor * (pid_st->lastProcessValue - processValue);
	
	pid_st->lastProcessValue = processValue;
	
	ret = (p_term + (i_term) + d_term) / SCALING_FACTOR;
	if(ret > MAX_INT){
		ret = MAX_INT;
	}
	else if(ret < -MAX_INT){
		ret = -MAX_INT;
	}
	
	return((int16_t)ret);
}

/*! \brief Resets the integrator.  
 *  
 *  Calling this function will reset the integrator in the PID regulator.  
 */  
void pid_Reset_Integrator(pidData_t *pid_st)
{
	pid_st->sumError = 0;   
}