#include "Spectra/TofEffSpectra.h"

void TofEffSpectra::initialize(  ){

	InclusiveSpectra::initialize();

	tofSigmaIdeal 	= config.getDouble( nodePath+".ZRecentering.sigma:tof", 0.012 );
	dedxSigmaIdeal 	= config.getDouble( nodePath+".ZRecentering.sigma:dedx", 0.07 );
	zr 				= new ZRecentering( dedxSigmaIdeal,
									 	tofSigmaIdeal,
									 	config.getString( nodePath+".Bichsel.table", "dedxBichsel.root"),
									 	config.getInt( nodePath+".Bichsel.method", 0) );

		// method for phase space recentering
	zrMethod 		= config.getString( nodePath + ".ZRecentering.method", "traditional" );
		// alias the centered species for ease of use
	centerSpecies 	= config.getString( nodePath + ".ZRecentering.centerSpecies", "K" );


	nSigmaDedxCut = config.getDouble( nodePath + ".nSigmaDedx:cut", -1 );

}


TofEffSpectra::~TofEffSpectra(){

}

void TofEffSpectra::makeCentralityHistos(){

	InclusiveSpectra::makeCentralityHistos();

	/**
	 * Make centrality ptHistos
	 */
	book->cd();
	
	for ( int iC : centralityBins ){
		string hName = "pt_" + ts(iC);
		INFO( classname(), hName );
		book->cd( "inclusive_vs_dedx" );
		book->clone( "", "pt_vs_dedx", "inclusive_vs_dedx", hName + "_p" );
		book->clone( "", "pt_vs_dedx", "inclusive_vs_dedx", hName + "_n" );	
		
		INFO( classname(), hName );
		book->cd( "inclusiveTof_vs_dedx" );
		book->clone( "", "pt_vs_dedx", "inclusiveTof_vs_dedx", hName + "_p" );
		book->clone( "", "pt_vs_dedx", "inclusiveTof_vs_dedx", hName + "_n" );
		
	}
}


void TofEffSpectra::analyzeTrack( int iTrack ){

	double pt = pico->trackPt( iTrack );
	int charge = pico->trackCharge( iTrack );
	double dedx = pico->trackDedx( iTrack );

	double p 		= pico->trackP( iTrack );
	double rdedx 	= zr->rDedx(centerSpecies, pico->trackDedx(iTrack), p );
	double nSigma 	= rdedx / dedxSigmaIdeal;


	book->cd( "inclusive_vs_dedx" );

	if ( cBin >= 0 ){
		string cName = "pt_" + ts( cBin ) + "_" + Common::chargeString( charge );
		book->get2D( cName )->Fill( pt, rdedx, eventWeight );
	}
	book->cd();

	if ( nSigmaDedxCut > 0 && abs(nSigma) > nSigmaDedxCut )
		return;
	 
	

	book->cd( "inclusive" );	
	if ( cBin >= 0 ){
		string cName = "pt_" + ts( cBin ) + "_" + Common::chargeString( charge );
		book->fill( cName, pt, eventWeight );
	}
	book->cd();

}


void TofEffSpectra::analyzeTofTrack( int iTrack ){

	double pt = pico->trackPt( iTrack );
	int charge = pico->trackCharge( iTrack );
	double dedx = pico->trackDedx( iTrack );

	double p 		= pico->trackP( iTrack );
	double rdedx 	= zr->rDedx(centerSpecies, pico->trackDedx(iTrack), p );
	double nSigma 	= rdedx / dedxSigmaIdeal;

	
	book->cd( "inclusiveTof_vs_dedx" );
	
	if ( cBin >= 0 ){
		string cName = "pt_" + ts( cBin ) + "_" + Common::chargeString( charge );
		book->get2D( cName )->Fill( pt, rdedx, eventWeight );
	}
	book->cd();


	if ( nSigmaDedxCut > 0 && abs(nSigma) > nSigmaDedxCut )
		return;
	 
	book->cd( "inclusiveTof" );	
	if ( cBin >= 0 ){
		string cName = "pt_" + ts( cBin ) + "_" + Common::chargeString( charge );
		book->fill( cName, pt, eventWeight );
	}
	book->cd();

}