#ifndef ZB_PID_PARAMETERS_H
#define ZB_PID_PARAMETERS_H

// ROOBARB
#include "XmlConfig.h"
#include "Logger.h"
using namespace jdb;

// ROOT
#include "TMath.h"

//Local
#include "PidPhaseSpace.h"
#include "PhaseSpaceRecentering.h"



class ZbPidParameters
{

protected:

	XmlConfig * cfg;
	string nodePath;
	PhaseSpaceRecentering * psr;
	Logger log;

	static double minP;
	static double refMass;

	// for the mean
	map< string, vector< double > > p0M;
	map< string, vector< double > > p1M;
	map< string, vector< double > > p2M;

	// for the sigma
	map< string, vector< double > > p0S;
	map< string, vector< double > > p1S;
	map< string, vector< double > > p2S;



public:
	ZbPidParameters( XmlConfig * cfg, string nodePath, PhaseSpaceRecentering * psr );
	~ZbPidParameters() {}

	static void setRefMass( double m );
	



	/**
	 * Calulates the mean of zb
	 * @param  p    Momentum in GeV/c
	 * @param  m    Mass of plc of interest in GeV/c
	 * @param  p0	Fit Parameter : timing constant  
	 * @param  p1 	Fit Parameter : momentum uncertainty
	 * @param  p2  	Fit Parameter : sim momentum uncertainty
	 * @return      The expected mean of zb
	 */
	static Double_t mean( 	Double_t p, 	
							Double_t m, 
							Double_t p0,
							Double_t p1,
							Double_t p2
							);

	
	/**
	 * Calulates the sigma of zb
	 * @param  p    Momentum in GeV/c
	 * @param  m    Mass of plc of interest in GeV/c
	 * @param  p0	Fit Parameter : timing constant  
	 * @param  p1 	Fit Parameter : momentum uncertainty
	 * @param  p2  	Fit Parameter : sim momentum uncertainty
	 * @return      The expected sigma of zb
	 */
	static Double_t sigma( 		Double_t p, 
								Double_t m,
								Double_t p0,
								Double_t p1,
								Double_t p2 );



	Double_t mean( string plc, Double_t p, Int_t iCen );
	Double_t sigma( string plc, Double_t p, Int_t iCen );

};


#endif