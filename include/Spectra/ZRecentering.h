#ifndef PHASE_SPACE_RECENTERING_H
#define PHASE_SPACE_RECENTERING_H

// RcpMaker
#include "Params/Bichsel.h"
#include "Params/TofGenerator.h"



// STL
#include <map>
using namespace std;


class ZRecentering
{

protected:
	/**
	 * The naive sigma values for calculating the likelihood
	 */
	double dedxSigma;
	double tofSigma;

	Bichsel * dedxGen;
	TofGenerator * tofGen;

	vector<string> species;

public:
	ZRecentering( double dedxSigma, double tofSigma, string bTable = "dedxBichsel.root", int bMethod = 0 );
	~ZRecentering();

	// Masses
	double eMass;		// [GeV]
	double piMass;		// [GeV]
	double kaonMass;	// [GeV]
	double protonMass;	// [GeV]
	double deuteronMass;// [GeV]
	
	 /**
	 * Get the expected mass from the string identifier
	 * @param  	pType 	- string identifier for species
	 * @return	mass of the given particle species
	 */
	double mass( string pType );
	vector<string> otherSpecies( string center );
	vector<string> allSpecies();
	vector<double> centeredTofMeans( string center, double p, vector<string> others );
	vector<double> centeredTofMeans( string center, double p );
	map<string, double> centeredTofMap( string center, double p );

	TofGenerator * tofGenerator();
	Bichsel * dedxGenerator();

	vector<double> centeredDedxMeans( string center, double p, vector<string> others );
	vector<double> centeredDedxMeans( string center, double p );

	map<string, double> centeredDedxMap( string center, double p );

	/**
	 * Calculates the difference from the expected value of log10( dedx )
	 * @param  	pType 	- particle type
	 * @param	dedx 	- measured dEdx value
	 * @param 	p 		- measured momentum value
	 * @return	The recentered dEdx value
	 */
	double rDedx( string centerSpecies, double dedx, double p );

	/**
	 * Calculates the difference from the expected value of log10( dedx )
	 * Uses the non-linear recentering scheme
	 * @param  pType particle type
	 * @param  iHit  hit index in the nTuple
	 * @return       n Sigma from expectation
	 */
	double nlDedx( string centerSpecies, double dedx, double p, double avgP );


	/**
	 * Calculates the difference from the expected value of 1/beta
	 * @param  	centerSpecies - particle type
	 * @param  	tof 	- the beta measured
	 * @param	p 	- momentum
	 * @return	recentered around centerSpecies
	 */
	double rTof( string centerSpecies, double beta, double p  );

	/**
	 * Calculates the difference from the expected value of 1/beta
	 * Uses the nonlinear recentering scheme
	 * @param  pType particle type
	 * @param  iHit  hit index in the nTuple
	 * @return       n Sigma from expectation
	 */
	double nlTof( string centerSpecies, double beta, double p, double avgP );
	double nl2Tof( string centerSpecies, double beta, double p, double avgP );

	/* Likelihood function
	 * 
	 * A gauss around the expected value with expected sigma
	 * @x     measured value
	 * @mu    expected mean
	 * @sigma expected sigma
	 * @return       the unnormalized likelihood
	 */
	static double lh( double x, double mu, double sigma );
	static double lh2( double x, double mu, double sigma );
	
};







#endif