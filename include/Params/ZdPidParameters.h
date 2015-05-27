#ifndef ZD_PID_PARAMETERS_H
#define ZD_PID_PARAMETERS_H

// ROOBARB
#include "XmlConfig.h"
#include "Logger.h"
using namespace jdb;

// ROOT
#include "TMath.h"

//Local
#include "PidPhaseSpace.h"



class ZdPidParameters
{

protected:

	XmlConfig * cfg;
	string nodePath;
	Logger log;

	// for the sigma
	map< string, vector< double > > p0;
	map< string, vector< double > > p1;
	map< string, vector< double > > p2;
	map< string, vector< double > > p3;



public:
	ZdPidParameters( XmlConfig * cfg, string nodePath);
	~ZdPidParameters() {}
	
	/**
	 * Calulates the sigma of Zd
	 * @param  p    Momentum in GeV/c
	 * @param  m    Mass of plc of interest in GeV/c
	 * @param  p0	Fit Parameter : timing constant  
	 * @param  p1 	Fit Parameter : momentum uncertainty
	 * @param  p2  	Fit Parameter : sim momentum uncertainty
	 * @return      The expected sigma of Zd
	 */
	static Double_t sigma( 		Double_t p, 
								Double_t p0,
								Double_t p1,
								Double_t p2,
								Double_t p3 );


	Double_t sigma( string plc, Double_t p, Int_t iCen );

};


#endif