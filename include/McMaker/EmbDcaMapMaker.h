#ifndef EMB_DCA_MAP_MAKER_H
#define EMB_DCA_MAP_MAKER_H

#include "Spectra/InclusiveSpectra.h"

class EmbDcaMapMaker : public InclusiveSpectra
{
public:
	EmbDcaMapMaker() {};
	~EmbDcaMapMaker() {};

	virtual void initialize(){
		InclusiveSpectra::initialize();
		cut_dca->max = 1000;
	}


	void makeCentralityHistos(){
		InclusiveSpectra::makeCentralityHistos();

		string sCharge = config.getString( nodePath + ".input:charge" );
		string plc     = config.getString( nodePath + ".input:plc" );
		book->cd();
		for ( int iC : centralityBins ){
			INFO( classname(), "dca_vs_pt_" + plc + "_" + ts( iC ) + "_" + sCharge );
			book->clone( "dca_vs_pt", "emb_dca_vs_pt_" + plc + "_" + ts( iC ) + "_" + sCharge );
		}

		book->clone( "dca_vs_pt", "emb_dca_vs_pt_" + plc + "_" + sCharge );

	}

	virtual void analyzeTrack( Int_t iTrack ){

		if ( !makeSpectra )
			return;

		book->cd();
		// use the ptMc instead of primary pT
		double pt = pico->mcPt( iTrack );
		double dca = pico->trackDca( iTrack );
		int charge = pico->trackCharge( iTrack );
		string plc     = config.getString( nodePath + ".input:plc" );

		if ( cBin >= 0 ){
			string cName = "emb_dca_vs_pt_" + plc + "_" + ts( cBin ) + "_" + Common::chargeString( charge );
			book->fill( cName, pt, dca, eventWeight );
		}


		string cName = "emb_dca_vs_pt_" + plc + "_" + Common::chargeString( charge );
		book->fill( cName, pt, dca, eventWeight );

		book->cd();
		
	}


protected:
	
};


#endif