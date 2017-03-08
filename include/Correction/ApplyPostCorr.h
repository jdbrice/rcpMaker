#ifndef CORRECTION_APPLY_POST_CORR_H
#define CORRECTION_APPLY_POST_CORR_H

#include "Correction/SpectraCorrecter.h"

//Roobarb
#include "XmlConfig.h"
#include "XmlFunction.h"
#include "XmlGraph.h"
#include "HistoAnalyzer.h"
using namespace jdb;

// STL
#include <string>
#include <map>
using namespace std;

class ApplyPostCorr : public HistoAnalyzer
{
protected:

	// map<string, unique_ptr<XmlFunction> > tpcEff;
	// map<string, unique_ptr<XmlGraph> > tofEff;
	// map<string, unique_ptr<XmlFunction> > feedDown;

	unique_ptr<SpectraCorrecter> sc;

	map<string, TH1*> tofEff;

	string plc = "UNK";

	bool apply_feeddown;
	bool apply_tofEff;
	bool apply_tofSpeciesEff;
	bool apply_tpcEff;
	bool apply_pTFactor;
	bool apply_dy;
	bool apply_binWidth;
	bool apply_twopi;

public:
	virtual const char* classname() const {return "ApplyPostCorr"; }
	ApplyPostCorr( );
	~ApplyPostCorr() {}

	virtual void initialize();

	void setupCorrections();
	void setupCloneArmy( TH1* h_origin, string cyn, string plc );

	virtual void make();

	float tofEffCorr( string name, float bCenter ){
		if ( false == apply_tofEff ) return 1.0;
		if ( tofEff.count( name ) <= 0 ){
			ERRORC( "Cannot find TofEff for " << name );
			return 1;
		}

		TH1 * h = tofEff[ name ];
		int bin = h->GetXaxis()->FindBin( bCenter );
		float eff = h->GetBinContent( bin );
		INFOC( "TofEff : pT=" << bCenter << " [bin " << bin << "] = " << eff );
		return 1.0 / eff;
	}


	/* Species dep tof Eff
	 * This is the factor determined from comparing TPC only yields 
	 * versus TPC+TOF yields **AFTER** TOF efficiency corrections.
	 * The remaining difference in yield is attributed to species 
	 * dependent inefficiency in the TOF mtching / fitting.
	 *
	 * This factor is extracted for pi+ and Proton since the TPC only 
	 * yield can be cleanly extracted. 
	 * For Kaons we can directly extract this, so we use the average of 
	 * pion & proton in the corresponding centrality bin with a larger
	 * systematic uncertainty
	 */
	float tofSpeciesEff( string plc, int cen){
		string name = plc+"_p_" + ts(cen);
		// INFOC( "children : " << vts( config.childrenOf( nodePath + ".TofSpeciesEff", "TofSpeciesEff" ) ) );
		// get average of pion and proton
		if ( "K" == plc ){
			return ( tofSpeciesEff( "Pi", cen ) + tofSpeciesEff( "P", cen ) ) / 2.0;
		}

		INFOC( "TSE [" << name <<"]: " << config.q( nodePath + ".TofSpeciesEff.TofSpeciesEff{name=="+name+"}" ) );
		double v = config.getDouble( config.q( nodePath + ".TofSpeciesEff.TofSpeciesEff{name=="+name+"}" ) + ":value", 1.0 );
		INFOC( name << " = " <<  v );
		return v;
	}
	
};



#endif