 // RcpMaker
#include "Spectra/PidDataMaker.h"
#include "Correction/SpectraCorrecter.h"

// ROOT
#include "TLine.h"

// STL
#define _USE_MATH_DEFINES
#include <cmath> // for M_1_PI etc.


PidDataMaker::PidDataMaker( XmlConfig* config, string np, string fl, string jp ) : InclusiveSpectra( config, np, fl, jp ) {

	/**
	 * Initialize the Phase Space Recentering Object
	 */
	tofSigmaIdeal 	= cfg->getDouble( np+"ZRecentering.sigma:tof", 0.011);
	dedxSigmaIdeal 	= cfg->getDouble( np+"ZRecentering.sigma:dedx", 0.033);
	zr 				= new ZRecentering( dedxSigmaIdeal,
									 	tofSigmaIdeal,
									 	cfg->getString( np+"Bichsel.table", "dedxBichsel.root"),
									 	cfg->getInt( np+"Bichsel.method", 0) );

		// method for phase space recentering
	zrMethod 		= config->getString( np + "ZRecentering.method", "traditional" );
		// alias the centered species for ease of use
	centerSpecies 	= cfg->getString( np + "ZRecentering.centerSpecies", "K" );

	//Make the momentum transverse binning
	binsPt 	= unique_ptr<HistoBins>(new HistoBins( cfg, "binning.pt" ));

	// Get the list of charges we are looking at
	charges = cfg->getIntVector( "binning.charges" );

	// Efficiency corrector
	sc = unique_ptr<SpectraCorrecter>( new SpectraCorrecter( cfg, nodePath ) ); 

	// make the energy loss params
	vector<int> charges = { -1, 1 };
	if ( cfg->exists( np + "EnergyLossParams:path" ) ){
		string path = cfg->getString( np + "EnergyLossParams:path" );

		for ( int c : charges ){

			string cfgName = path + centerSpecies + "_" + Common::chargeString( c ) + ".xml";
			XmlConfig cfgEL( cfgName );

			for ( int cb : cfg->getIntVector( nodePath + "CentralityBins" ) ){
				
				// Name like 'Pi_p_0' ... 'Pi_n_6' to be used for quick lookup
				string name = centerSpecies + "_" + Common::chargeString( c ) +"_" + ts( cb );
				elParams[ name ] = unique_ptr<EnergyLossParams>( new EnergyLossParams(&cfgEL, "EnergyLossParams[" + ts(cb) + "]") );
			}

		}	
	}

}

PidDataMaker::~PidDataMaker(){

}

void PidDataMaker::preEventLoop() {
	logger->info(__FUNCTION__) << endl;
	
	InclusiveSpectra::preEventLoop();

	book->cd();

	// make the energy loss histograms
	book->cd( "energyLoss" );
	for ( int c : charges ){
		for ( int cb : centralityBins ) {
			book->clone( "/", "corrPt", "energyLoss", "corrPt_" + Common::chargeString(c) + "_" + ts(cb) );
		}
	}	


	for ( int c : charges ) {
		for ( string plc : Common::species ){
			for ( int cb : centralityBins ) {
				for ( int iPt = 0; iPt < binsPt->nBins(); iPt++ ){
				
					// create a PidPoint
					string nname = Common::speciesName( centerSpecies, c, cb, iPt );
					pidPoints[ nname ] = unique_ptr<TNtuple>( new TNtuple( nname.c_str(), "PidData", "zb:zd:w" ) );
				}
			}
		}
	}
}

void PidDataMaker::postEventLoop() {
	logger->info(__FUNCTION__) << endl;
	book->cd();

	// write the PidData trees to the file
	book->cd( "PidPoints" );
	for ( auto &k : pidPoints ){
		k.second->Write( k.first.c_str() );
	}

}

void PidDataMaker::analyzeTofTrack( int iTrack ){
	InclusiveSpectra::analyzeTofTrack( iTrack );
	
	book->cd();

	// Alias Track Traits
	int charge 		= pico->trackCharge( iTrack );
	double pt 		= pico->trackPt( iTrack );
	double p 		= pico->trackP( iTrack );
	double eta 		= pico->trackEta( iTrack );
	trackPt 		= pt; // saved for whole track calculations

	/************ Corrections **********/
	// Apply Energy Loss Corrections if given
	string elName = centerSpecies + "_" + Common::chargeString( charge ) +"_" + ts( cBin );
	if ( elParams.count( elName ) ){

		double corrPt = pt - elParams[ elName ]->eval( pt );
		// QA
		book->cd( "energyLoss" );
		book->fill( "corrPt_" + Common::chargeString( charge ) + "_" + ts(cBin), pt, pt - corrPt );
		book->cd(  );
		
		// recalc the p from corrPt and eta
		p = Common::p( corrPt, eta );
		pt = corrPt;
	} else {
		ERROR( "No Energy Loss Params Given - These must be applied here" )
	} 
	/************ Corrections **********/

	// Must be done after corrections
	int ptBin 	= binsPt->findBin( pt );
	double ptBinWidth = binsPt->binWidth( ptBin );
	corrTrackPt = pt;
	double avgP = binAverageP( ptBin );

	// Require valid p bin
	if ( ptBin < 0 )
		return;

	book->cd();

	// Traditionally Recentered values
	double tof 		= zr->rTof(centerSpecies, pico->trackBeta(iTrack), p );
	double dedx 	= zr->rDedx(centerSpecies, pico->trackDedx(iTrack), p );
	// Non-Linearly Recentered values
	double tofNL 	= zr->nlTof(centerSpecies, pico->trackBeta(iTrack), p, avgP );
	double dedxNL 	= zr->nlDedx(centerSpecies, pico->trackDedx(iTrack), p, avgP );

	// Choose recentered values
	if ( "nonlinear" == zrMethod ){
		tof = tofNL;
		dedx = dedxNL;
	} 

	// event weight from RefMult correction
	double trackWeight = eventWeight;
	
	// standard dN/dpT normalization
	trackWeight = trackWeight * M_1_PI * 0.5; 			// 1.0 / ( 2 pi )
	trackWeight = trackWeight * ( 1.0 / corrTrackPt ); 	// 1.0 / pT
	trackWeight = trackWeight * ( 1.0 / ptBinWidth ); 	// 1.0 / ( bin width )
	trackWeight = trackWeight * ( 1.0 / ( cut_rapidity->max - cut_rapidity->min ) ); 	// 1.0 / dy

	// correct for TPC matching efficiency
	trackWeight = trackWeight * sc->tpcEffWeight( centerSpecies, trackPt, cBin, charge );
	
	// fill the tree
	string name = Common::speciesName( centerSpecies, charge, cBin, ptBin );
	pidPoints[ name ]->Fill( tof, dedx, trackWeight );
	
	book->cd();
}
