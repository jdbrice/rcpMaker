#ifndef PHASE_SPACE_RECENTERING_H
#define PHASE_SPACE_RECENTERING_H

#include "Bichsel.h"
#include "TofGenerator.h"

using namespace TMath;

class PhaseSpaceRecentering
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
	PhaseSpaceRecentering( double dedxSigma, double tofSigma, string bTable = "dedxBichsel.root", int bMethod = 0 ){
		this->dedxSigma = dedxSigma;
		this->tofSigma = tofSigma;

		dedxGen = new Bichsel( bTable, bMethod);
		tofGen = new TofGenerator();

		species = { "Pi", "K", "P" };
	}
	~PhaseSpaceRecentering(){
		delete dedxGen;
		delete tofGen;
	}

	/**
	 * Static usage
	 */
	static constexpr double piMass = 0.1395702;		// [GeV]
	static constexpr double kaonMass = 0.493667;	// [GeV]
	static constexpr double protonMass = 0.9382721;	// [GeV]
	
	 /**
	 * Get the expected mass from the string identifier
	 * @param  	pType 	- string identifier for species
	 * @return	mass of the given particle species
	 */
	
	TofGenerator * tofGenerator() { return tofGen; }
	Bichsel * dedxGenerator() { return dedxGen; }
	double mass( string pType ){
		if ( "P" == pType )
			return protonMass;
		if ( "K" == pType )
			return kaonMass;
		if ( "Pi" == pType )
			return piMass;
		return -10.0;	
	}
	vector<string> otherSpecies( string center ){
		vector<string> res;
		for ( int i = 0; i < species.size(); i++ ){
			if ( species[ i ] != center )
				res.push_back( species[ i ] );
		}
		return res;
	}
	vector<string> allSpecies(){
		return species;
	}
	vector<double> centeredTofMeans( string center, double p, vector<string> others ){

		double cMean = tofGen->mean( p, mass( center ) );
		
		vector<double> res;
		for ( int i = 0; i < others.size(); i++ ){
			double m = (tofGen->mean( p, mass( others[ i ] ) ) - cMean);
			res.push_back( m );
		}

		return res;
	}
	vector<double> centeredTofMeans( string center, double p ){

		double cMean = tofGen->mean( p, mass( center ) );
		
		vector<double> res;
		for ( int i = 0; i < species.size(); i++ ){
			double m = (tofGen->mean( p, mass( species[ i ] ) ) - cMean);
			res.push_back( m );
		}

		return res;
	}
	vector<double> centeredDedxMeans( string center, double p, vector<string> others ){
		
		const double cMean = dedxGen->mean10( p, mass( center ), -1, 1000 );

		vector<double> res;
		for ( int i = 0; i < others.size(); i++ ){
			double m = dedxGen->mean10( p, mass( others[ i ] ), -1, 1000 ) - cMean;
			res.push_back( m );
		}

		return res;
	}
	vector<double> centeredDedxMeans( string center, double p ){
		
		const double cMean = dedxGen->mean10( p, mass( center ), -1, 1000 );
		
		vector<double> res;
		for ( int i = 0; i < species.size(); i++ ){
			double m = dedxGen->mean10( p, mass( species[ i ] ), -1, 1000 ) - cMean;
			res.push_back( m );
		}
		return res;
	}

	/**
	 * Calculates the difference from the expected value of log10( dedx )
	 * @param  	pType 	- particle type
	 * @param	dedx 	- measured dEdx value
	 * @param 	p 		- measured momentum value
	 * @return	The recentered dEdx value
	 */
	double rDedx( string centerSpecies, double dedx, double p ){ 

		double mean = dedxGen->mean10( p, mass( centerSpecies ), -1, 1000 );
		double nDedx = Log10( dedx);
		double nSig = nDedx - mean ;

		return nSig;

		return -999.0;
	}

	/**
	 * Calculates the difference from the expected value of log10( dedx )
	 * Uses the non-linear recentering scheme
	 * @param  pType particle type
	 * @param  iHit  hit index in the nTuple
	 * @return       n Sigma from expectation
	 */
	double nlDedx( string centerSpecies, double dedx, double p, double avgP ){

		double dedx10 = Log10( dedx );

		// mean for this species
		//double mu = dedxGen->mean10( p, mass( centerSpecies ), -1, 1000 );
		const double muAvg = dedxGen->mean10( avgP, mass( centerSpecies ), -1, 1000 );

		double n1 = 0;
		double d1 = 0;

		for ( int i = 0; i < species.size(); i++ ){

			const double iMu = dedxGen->mean10( p, mass( species[ i ] ), -1, 1000 );
			const double iMuAvg = dedxGen->mean10( avgP, mass( species[ i ] ), -1, 1000 );
			
			// may change to a functional dependance 
			double sigma = dedxSigma; 

			double iL = lh( dedx10, iMu, sigma );
			double w = dedx10 + iMuAvg - iMu;
			

			n1 += (iL * w);
			d1 += iL;

		}

		if ( 0 == n1 || 0 == d1){
			return dedx10 - muAvg;
		}

		double nDedx = (n1/d1) - (muAvg);

		return nDedx;

	}


	/**
	 * Calculates the difference from the expected value of 1/beta
	 * @param  	centerSpecies - particle type
	 * @param  	tof 	- the beta measured
	 * @param	p 	- momentum
	 * @return	recentered around centerSpecies
	 */
	double rTof( string centerSpecies, double beta, double p  ){

		double deltaInvBeta = ( 1.0 / beta ) - tofGen->mean( p, mass( centerSpecies ) );

		return deltaInvBeta;
	}

	/**
	 * Calculates the difference from the expected value of 1/beta
	 * Uses the nonlinear recentering scheme
	 * @param  pType particle type
	 * @param  iHit  hit index in the nTuple
	 * @return       n Sigma from expectation
	 */
	double nlTof( string centerSpecies, double beta, double p, double avgP ){

		const double sigma = tofSigma; 
		const double tof = 1.0 / beta;
		
		// mean for this species
		//const double mu =  tofGen->mean( p, mass( centerSpecies ) );
		const double muAvg =  tofGen->mean( avgP, mass( centerSpecies ) );

		const vector< string > species = { "K", "P", "Pi" };

		double n1 = 0;
		double d1 = 0;

		for ( int i = 0; i < species.size(); i++ ){

			double iMu =  tofGen->mean( p, mass( species[ i ] ) ) ;
			double iMuAvg =  tofGen->mean( avgP, mass( species[ i ] ) ) ;
			
			double iL = lh( tof, iMu, sigma );
			
			double w = tof + iMuAvg - iMu;
			
			n1 += (iL * w);
			d1 += iL;

		}

		if ( 0 == n1 || 0 == d1){
			return tof - muAvg;
		}
		
		double nTof = (n1/d1) - ( muAvg );

		return nTof;

	}

	/**
	 * Likelihood function
	 * A gauss around the expected value with expected sigma
	 * @param  x     measured value
	 * @param  mu    expected mean
	 * @param  sigma expected sigma
	 * @return       the unnormalized likelihood
	 */
	static double lh( double x, double mu, double sigma ){

		double a = sigma * TMath::Sqrt( 2 * TMath::Pi() );
		double b = ( x - mu );
		double c = 2 * sigma*sigma;
		double d = (1/a) * TMath::Exp( -b*b / c );

		return d;
	}
	
};







#endif