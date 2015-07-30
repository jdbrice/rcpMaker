#include "McMaker/TofEffMaker.h"
#include "TLine.h"

#include "Spectra/PidHistoMaker.h"

TofEffMaker::TofEffMaker( XmlConfig* config, string np, string fl, string jp ) : InclusiveSpectra( config, np, fl, jp ) {

	DEBUG("")
	/**
	 * Initialize the Phase Space Recentering Object
	 */
	tofSigmaIdeal = cfg->getDouble( np+"ZRecentering.sigma:tof", 0.011);
	dedxSigmaIdeal = cfg->getDouble( np+"ZRecentering.sigma:dedx", 0.033);
	psr = new ZRecentering( dedxSigmaIdeal,
									 tofSigmaIdeal,
									 cfg->getString( np+"Bichsel.table", "dedxBichsel.root"),
									 cfg->getInt( np+"Bichsel.method", 0) );

		// method for phase space recentering
	psrMethod = config->getString( np+"ZRecentering.method", "traditional" );
		// alias the centered species for ease of use
	centerSpecies = cfg->getString( np+"ZRecentering.centerSpecies", "K" );
		// enhanced distro options

	
	// Make the momentum transverse binning
	binsPt = unique_ptr<HistoBins>(new HistoBins( cfg, "bin.pt" ));
	
	// Make the eta binning
	binsEta = unique_ptr<HistoBins>(new HistoBins( cfg, "bin.eta" ));

	// Make charge bins
	charges = cfg->getIntVector( "bin.charges" );

 }

 TofEffMaker::~TofEffMaker(){

 }

void TofEffMaker::preEventLoop() {
	logger->info(__FUNCTION__) << endl;
	
	InclusiveSpectra::preEventLoop();
}

void TofEffMaker::postEventLoop() {
	DEBUG("")
}

void TofEffMaker::analyzeTofTrack( int iTrack ){

	book->cd();

	double pt 		= pico->trackPt( iTrack );
	double eta 		= pico->trackEta( iTrack );
	double y 		= rapidity( pt, eta, psr->mass( centerSpecies ) );

	int ptBin 	= binsPt->findBin( pt );
	int charge 	= pico->trackCharge( iTrack );

	if ( ptBin < 0 || cBin < 0 )
		return;

	// cut on rapidity
	if ( makeTrackQA ){
		book->cd( "TrackQA" );
		book->fill( "pre_rapidity", y );
	}

	// Rapidity Cut
	if ( y < cutRapidity->min || y > cutRapidity->max )
		return;

	if ( makeTrackQA )
		book->fill( "rapidity", 	y );

	// fill the dN/dPt plots
	book->cd();
	if ( cBin >= 0 ){
		string cName = "pt_tof_" + ts( cBin ) + "_" + Common::chargeString(charge);
		book->fill( cName, pt, eventWeight );		
	}
}

void TofEffMaker::analyzeTrack( int iTrack ){

	book->cd();


	double pt 		= pico->trackPt( iTrack );
	double eta 		= pico->trackEta( iTrack );
	double y 		= rapidity( pt, eta, psr->mass( centerSpecies ) );

	int ptBin 	= binsPt->findBin( pt );
	int charge 	= pico->trackCharge( iTrack );

	if ( ptBin < 0 || cBin < 0 )
		return;

	// cut on rapidity
	if ( makeTrackQA ){
		book->cd( "TrackQA" );
		book->fill( "pre_rapidity", y );
	}

	// Rapidity Cut
	if ( y < cutRapidity->min || y > cutRapidity->max )
		return;

	if ( makeTrackQA )
		book->fill( "rapidity", 	y );

	// fill the dN/dPt plots
	book->cd();
	if ( cBin >= 0 ){
		string cName = "pt_" + ts( cBin ) + "_" + Common::chargeString(charge);
		book->fill( cName, pt, eventWeight );		
	}

}







