#ifndef PID_PHASE_SPACE
#define PID_PHASE_SPACE

#include "InclusiveSpectra.h"
#include "Reporter.h"
#include "PhaseSpaceRecentering.h"

class PidPhaseSpace : public InclusiveSpectra
{
protected:

	/**
	 * Binning
	 */
	HistoBins* binsPt;
	HistoBins* binsEta;
	HistoBins* binsCharge;
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
	PhaseSpaceRecentering * psr;
	vector<string> species;

	double tofCut, dedxCut;

public:
	PidPhaseSpace( XmlConfig* config, string np );
	~PidPhaseSpace(){}

	/**
	 * Analyze a track in the current Event
	 * @param	iTrack 	- Track index 
	 */
	virtual void analyzeTrack( Int_t iTrack );
	virtual void preLoop();
	virtual void postLoop();
	

	void enhanceDistributions( int ptBin, int etaBin, double dedx, double tof );

	/**
	 * Get the string part of the name based on charge
	 * @param  charge charge as -1, 0, +1
	 * @return        charge part of the name string
	 */
	static string chargeString( int charge = 0 ) {
		if ( -1 >= charge )	// negative
			return "n";
		else if ( 1 <= charge ) //positive
			return "p";
		return "a";	// all
	}

	/**
	 * Builds the string for a species histogram
	 * @param  centerSpecies the centering species
	 * @param  charge        the charge, -1, 0, +1
	 * @return               string for histogram name
	 */
	static string speciesName( string centerSpecies, int charge = 0 ){
		return "dedx_tof_" + chargeString(charge) + "_" + centerSpecies;
	}
	static string speciesName( string centerSpecies, int charge, int ptBin, int etaBin = 0 ){
		return "dedx_tof_" + chargeString(charge) + "_" + centerSpecies + "_" + ts(ptBin) + "_" + ts(etaBin);
	}
	static string tofName( string centerSpecies, int charge, int ptBin, int etaBin = 0, string eSpecies = "" ){
		if ( "" == eSpecies )
			return "tof_" + chargeString(charge) + "_" + centerSpecies + "_" + ts(ptBin) + "_" + ts(etaBin);
		else
			return "tof_" + chargeString(charge) + "_" + centerSpecies + "_" + ts(ptBin) + "_" + ts(etaBin) + "_" + eSpecies;
	}
	static string dedxName( string centerSpecies, int charge, int ptBin, int etaBin = 0, string eSpecies = "" ){
		if ( "" == eSpecies )
			return "dedx_" + chargeString(charge) + "_" + centerSpecies + "_" + ts(ptBin) + "_" + ts(etaBin);
		else
			return "dedx_" + chargeString(charge) + "_" + centerSpecies + "_" + ts(ptBin) + "_" + ts(etaBin) + "_" + eSpecies;
	}

protected:

	void preparePhaseSpaceHistograms( string plc );

	/**
	 * Computes the upper and lower limits in tof and dedx phase space
	 * @param pType             centering plc
	 * @param p                 momentum transvers [GeV/c]
	 * @param tofLow            double* receiving tofLow limit
	 * @param tofHigh           double* receiving tofHigh limit
	 * @param dedxLow           double* receiving dedxLow limit
	 * @param dedxHigh          double* receiving dedxHigh limit
	 * @param tofPadding        fixed value of padding applied to tof range
	 * @param dedxPadding       fixed value of padding applied to dedx range
	 * @param tofScaledPadding  percent padding added to tof range
	 * @param dedxScaledPadding percent padding added to dedx range
	 */
	void autoViewport( 	string pType, double p, double * tofLow, double* tofHigh, double * dedxLow, double * dedxHigh, 
								double tofPadding = 1, double dedxPadding = 1, double tofScaledPadding = 0, double dedxScaledPadding = 0 );

	
};

#endif