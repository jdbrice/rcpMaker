#include "McMaker/TpcEffMaker.h"

TpcEffMaker::TpcEffMaker( XmlConfig * config, string nodePath, string fileList, string jobPrefix ) 
	: InclusiveSpectra( config, nodePath, fileList, jobPrefix ){

}

TpcEffMaker::~TpcEffMaker(){

}

void TpcEffMaker::analyzeTrack( Int_t iTrack ){

	if ( !makeSpectra )
		return;

	// use the ptMc instead of primary pT
	double pt = pico->mcPt( iTrack );
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
	int charge = pico->trackCharge( iTrack );
	 
	book->cd( "inclusiveTof" );
	
	if ( cBin >= 0 ){
		string cName = "pt_" + ts( cBin ) + "_" + Common::chargeString( charge );
		book->fill( cName, pt, eventWeight );		
	}
	book->cd( );
	
}