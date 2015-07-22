#ifndef PID_PHASE_SPACE_H
#define PID_PHASE_SPACE_H

// Rcp Maker
#include "InclusiveSpectra.h"
#include "PhaseSpaceRecentering.h"
#include "Adapter/PicoDataStore.h"
#include "Params/EnergyLossParams.h"
#include "Params/FeedDownParams.h"
class SpectraCorrecter;

// STL
#include <algorithm> 
#include <vector>
#include <math.h>
using namespace std;


class PidPhaseSpace : public InclusiveSpectra
{
protected:


	// Configs for
	XmlConfig * cfgEnergyLoss;
	XmlConfig * cfgFeedDown;

	//Binning
	unique_ptr<HistoBins> binsTof;
	unique_ptr<HistoBins> binsDedx;
	unique_ptr<HistoBins> binsPt;
	unique_ptr<HistoBins> binsEta;
	vector<int> charges;
	// these are made on the fly
	double tofBinWidth, dedxBinWidth;


	
	// Plot Ranges 
	double tofPadding, dedxPadding, tofScalePadding, dedxScalePadding;

	
	// Phase Space Recentering 
	string centerSpecies;
	string psrMethod;
	double dedxSigmaIdeal, tofSigmaIdeal;
	PhaseSpaceRecentering * psr;
	

	double tofCut, dedxCut;

	bool make2D, makeEnhanced;

	
	double nSigBelow, nSigAbove;

	bool binByMomentum;
	bool makeCombinedCharge = false;

	unique_ptr<SpectraCorrecter> sc;
	float effWeight = 0;

	map< string, unique_ptr<EnergyLossParams> > elParams;

	map< string, vector< unique_ptr<FeedDownParams> > > fdParams;

public:
	PidPhaseSpace( XmlConfig* config, string np, string fl ="", string jp ="" );
	~PidPhaseSpace();

	/**
	 * Analyze a track in the current Event
	 * @iTrack 	- Track index 
	 */
	virtual void analyzeTrack( Int_t iTrack );
	virtual void analyzeTofTrack( Int_t iTrack );
	virtual void preEventLoop();
	virtual void postEventLoop();
	

	void enhanceDistributions( double avgP, int ptBin, int etaBin, int charge, double dedx, double tof );

	static vector<string> species;
	/* Get the string part of the name based on charge
	 * 
	 * @charge charge as -1, 0, +1
	 * @return        charge part of the name string
	 */
	static string chargeString( int charge = 0 ) {
		if ( -1 >= charge )	// negative
			return "n";
		else if ( 1 <= charge ) //positive
			return "p";
		return "a";	// all
	}

	/* Builds the string for a species histogram
	 * 
	 * @centerSpecies the centering species
	 * @charge        the charge, -1, 0, +1
	 * @return               string for histogram name
	 */
	static string speciesName( string centerSpecies, int charge = 0, int cenBin = 9 ){
		return "dedx_tof_" + chargeString(charge) + "_" + centerSpecies + "_" + ts(cenBin);
	}
	static string speciesName( string centerSpecies, int charge, int cenBin, int ptBin, int etaBin = 0 ){
		return "dedx_tof_" + chargeString(charge) + "_" + centerSpecies + "_" + ts(cenBin) + "_" + ts(ptBin) + "_" + ts(etaBin);
	}
	static string tofName( string centerSpecies, int charge, int cenBin, int ptBin, int etaBin = 0, string eSpecies = "" ){
		string ePart = "";
		if ( "" != eSpecies )
			ePart = "_" + eSpecies;
		return "tof_" + chargeString(charge) + "_" + centerSpecies + "_" + ts(cenBin) + "_" + ts(ptBin) + "_" + ts(etaBin) + ePart;
	}
	static string dedxName( string centerSpecies, int charge, int cenBin, int ptBin, int etaBin = 0, string eSpecies = "" ){
		string ePart = "";
		if ( "" != eSpecies )
			ePart = "_" + eSpecies;
		return "dedx_" + chargeString(charge) + "_" + centerSpecies + "_" + ts(cenBin) + "_" + ts(ptBin) + "_" + ts(etaBin) + ePart;
	}

	static double p( double pt, double eta ){
		return pt * cosh( eta );
	}
	double averagePt( int ptBin ){
		if ( ptBin < 0 || ptBin > binsPt->nBins() ){
			return 0;
		}
		return ((*binsPt)[ ptBin ] + (*binsPt)[ ptBin + 1 ]) / 2.0;
	}
	double averageEta( int etaBin ){
		if ( etaBin < 0 || etaBin > binsEta->nBins() ){
			return 0;
		} 
		return ((*binsEta)[ etaBin ] + (*binsEta)[ etaBin + 1 ]) / 2.0;
	}
	/**
	 * Calculates the average momentum for a pt, eta bin
	 * @ptBin  		the ptBin
	 * @etaBin 		the eta bin
	 * @return        the average p in GeV/c
	 */
	double averageP( int ptBin, int etaBin ){
		if ( ptBin < 0 || ptBin > binsPt->nBins() ){
			return 0;
		}
		if ( etaBin < 0 || etaBin > binsEta->nBins() ){
			return 0;
		} 

		double avgPt = ((*binsPt)[ ptBin ] + (*binsPt)[ ptBin + 1 ]) / 2.0;
		double avgEta = ((*binsEta)[ etaBin ] + (*binsEta)[ etaBin + 1 ]) / 2.0;

		return p( avgPt, avgEta );

	}

	/**
	 * Computes the upper and lower limits in tof and dedx phase space
	 * @pType             centering plc
	 * @p                 momentum transvers [GeV/c]
	 * @tofLow            double* receiving tofLow limit
	 * @tofHigh           double* receiving tofHigh limit
	 * @dedxLow           double* receiving dedxLow limit
	 * @dedxHigh          double* receiving dedxHigh limit
	 * @tofPadding        fixed value of padding applied to tof range
	 * @dedxPadding       fixed value of padding applied to dedx range
	 * @tofScaledPadding  percent padding added to tof range
	 * @dedxScaledPadding percent padding added to dedx range
	 */
	static void autoViewport( 	string pType, double p, PhaseSpaceRecentering * lpsr, double * tofLow, double* tofHigh, double * dedxLow, double * dedxHigh, 
								double tofPadding = 1, double dedxPadding = 1, double tofScaledPadding = 0, double dedxScaledPadding = 0 );


	void reportAll( string n );
	void reportAllTof(  );
	void reportAllDedx(  );

protected:

	void preparePhaseSpaceHistograms( string plc );

	double rapidity( double pt, double eta, double m ){
		double a = sqrt( m*m + pt*pt*cosh( eta )*cosh( eta ) ) + pt * sinh( eta );
		double b = sqrt( m*m + pt*pt );
		return log( a / b );
	}

	double feedDownWeight( int charge, double pt ){

		string name = centerSpecies + "_" + PidPhaseSpace::chargeString( charge );
		if ( fdParams.count( name ) ){

			for ( int i = 0; i < fdParams[ name ].size(); i++ ){
				if ( fdParams[ name ][ i ]->usable( refMult ) ){
					return fdParams[ name ][ i ]->weight( pt );
				}
			}
			ERROR( "Feed Down Corrections not found for refMult = " << refMult )	
		}
		ERROR( "Feed Down Corrections not found for " + name )
		return 1.0;

	}
	
	
};

#endif