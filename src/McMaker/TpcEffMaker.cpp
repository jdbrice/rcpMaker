#include "McMaker/TpcEffMaker.h"

void TpcEffMaker::initialize(){
	InclusiveSpectra::initialize();

	trackType = config.getString( nodePath + ".input:type" );
	if ( "mc" == trackType ){
		cut_nHitsDedx->min = 0;
		cut_nHitsFit->min = 0;
		cut_nHitsFitOverPossible->min = 0;
	}
}

TpcEffMaker::~TpcEffMaker(){

}

void TpcEffMaker::analyzeTrack( Int_t iTrack ){

	if ( !makeSpectra )
		return;

	// use the ptMc instead of primary pT
	double pt = pico->mcPt( iTrack );
	if ( "mc" == trackType ){
		pt = pico->trackPt( iTrack );
	}
	int charge = pico->trackCharge( iTrack );
	 
	book->cd( "inclusive" );
	
	if ( cBin >= 0 ){
		string cName = "pt_" + ts( cBin ) + "_" + Common::chargeString( charge );
		book->fill( cName, pt, eventWeight );
	}
	book->cd();
	
}

void TpcEffMaker::analyzeTofTrack( Int_t iTrack ){

	if ( !makeTofSpectra )
		return;

	double pt = pico->mcPt( iTrack );
	if ( "mc" == trackType ){
		pt = pico->trackPt( iTrack );
	}
	int charge = pico->trackCharge( iTrack );
	 
	book->cd( "inclusiveTof" );
	
	if ( cBin >= 0 ){
		string cName = "pt_" + ts( cBin ) + "_" + Common::chargeString( charge );
		book->fill( cName, pt, eventWeight );		
	}
	book->cd( );
	
}