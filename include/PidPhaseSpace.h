#ifndef PID_PHASE_SPACE_H
#define PID_PHASE_SPACE_H

#include "InclusiveSpectra.h"
#include "PhaseSpaceRecentering.h"
#include "PicoDataStore.h"
#include "TofPidParams.h"
#include "DedxPidParams.h"
#include <algorithm>    // std::find
#include <vector>
using namespace std;

#include <math.h>

class PidPhaseSpace : public InclusiveSpectra
{
protected:

	/**
	 * Binning
	 */
	unique_ptr<HistoBins> binsTof;
	unique_ptr<HistoBins> binsDedx;
	unique_ptr<HistoBins> binsPt;
	unique_ptr<HistoBins> binsEta;
	unique_ptr<HistoBins> binsCharge;
	// these are made on the fly
	double tofBinWidth, dedxBinWidth;


	/**
	 * Plot Ranges
	 */
	double tofPadding, dedxPadding, tofScalePadding, dedxScalePadding;

	/**
	 * Phase Space Recentering
	 */
	string centerSpecies;
	string psrMethod;
	double dedxSigmaIdeal, tofSigmaIdeal;
	PhaseSpaceRecentering * psr;
	

	double tofCut, dedxCut;

	bool make2D, makeEnhanced;

	/**
	 * After first pass
	 */
	vector<TofPidParams*> tofParams;
	vector<DedxPidParams*> dedxParams;
	bool useTofParams = false;
	bool useDedxParams = false;

public:
	PidPhaseSpace( XmlConfig* config, string np, string fl ="", string jp ="" );
	~PidPhaseSpace();

	/**
	 * Analyze a track in the current Event
	 * @iTrack 	- Track index 
	 */
	virtual void analyzeTrack( Int_t iTrack );
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


	void reportAll(  );

protected:

	void preparePhaseSpaceHistograms( string plc );

	double getTofMean( string plc, double p );
	double getTofSigma( string plc, double p );

	double getDedxMean( string plc, double p );
	double getDedxSigma( string plc, double p );

	
	
};

#endif