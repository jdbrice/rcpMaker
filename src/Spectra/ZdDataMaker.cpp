 // RcpMaker
#include "Spectra/ZdDataMaker.h"
#include "Correction/SpectraCorrecter.h"

// ROOT
#include "TLine.h"

// STL
#define _USE_MATH_DEFINES
#include <cmath> // for M_1_PI etc.


void ZdDataMaker::initialize() {

	InclusiveSpectra::initialize();

	/**
	 * Initialize the Phase Space Recentering Object
	 */
	tofSigmaIdeal 	= config.getDouble( nodePath+".ZRecentering.sigma:tof", 0.011);
	dedxSigmaIdeal 	= config.getDouble( nodePath+".ZRecentering.sigma:dedx", 0.033);
	zr 				= new ZRecentering( dedxSigmaIdeal,
									 	tofSigmaIdeal,
									 	config.getString( nodePath+".Bichsel.table", "dedxBichsel.root"),
									 	config.getInt( nodePath+".Bichsel.method", 0) );

		// method for phase space recentering
	zrMethod 		= config.getXString( nodePath + ".ZRecentering.method", "traditional" );
		// alias the centered species for ease of use
	centerSpecies 	= config.getXString( nodePath + ".ZRecentering.centerSpecies", "K" );

	//Make the momentum transverse binning
	binsPt = unique_ptr<HistoBins>(new HistoBins( config, "binning.pt" ));

	// Get the list of charges we are looking at
	charges = config.getIntVector( "binning.charges" );

	// make the energy loss params
	vector<int> charges = { -1, 1 };
	if ( config.exists( nodePath + ".EnergyLossParams:path" ) ){
		string path = config.getXString( nodePath + ".EnergyLossParams:path" );

		for ( int c : charges ){

			string cfgName = path + centerSpecies + "_" + Common::chargeString( c ) + ".xml";
			XmlConfig cfgEL( cfgName );

			for ( int cb : config.getIntVector( nodePath + ".CentralityBins" ) ){
				
				// Name like 'Pi_p_0' ... 'Pi_n_6' to be used for quick lookup
				string name = centerSpecies + "_" + Common::chargeString( c ) +"_" + ts( cb );
				elParams[ name ] = unique_ptr<EnergyLossParams>( new EnergyLossParams(&cfgEL, "EnergyLossParams[" + ts(cb) + "]") );
			}

		}	
	}
	

}

ZdDataMaker::~ZdDataMaker(){

}

void ZdDataMaker::preEventLoop() {
	INFO( classname(), "");
	
	InclusiveSpectra::preEventLoop();

	book->cd();

	// make the energy loss histograms
	book->cd( "energyLoss" );
	for ( int c : charges ){
		for ( int cb : centralityBins ) {
			book->clone( "/", "corrPt", "energyLoss", "corrPt_" + Common::chargeString(c) + "_" + ts(cb) );
		}
	}	

	book->cd();
	for ( int c : charges ) {
		// for ( string plc : Common::species ){
			for ( int cb : centralityBins ) {

				string nname = Common::speciesName( centerSpecies, c, cb );
				book->clone( "zd_vs_", "zd_vs_pT_" + nname );
				book->clone( "zd_vs_", "zd_vs_mT_" + nname );
				book->clone( "zd_vs_", "zd_vs_mTm0_" + nname );

				book->clone( "zb_vs_", "zb_vs_pT_" + nname );
				book->clone( "zb_vs_", "zb_vs_mT_" + nname );
				book->clone( "zb_vs_", "zb_vs_mTm0_" + nname );
			}
		// }
	}
}

void ZdDataMaker::postEventLoop() {
	INFO( classname(), "");
	book->cd();

}

void ZdDataMaker::analyzeTrack( int iTrack ){
	InclusiveSpectra::analyzeTrack( iTrack );
	
	book->cd();

	// Alias Track Traits
	int charge 		= pico->trackCharge( iTrack );
	double pt 		= pico->trackPt( iTrack );
	double p 		= pico->trackP( iTrack );
	double eta 		= pico->trackEta( iTrack );
	double phi 		= pico->trackPhi( iTrack );
	double mass 	= Common::mass( centerSpecies );
	trackPt 		= pt; // saved for whole track calculations

	/************ Energy Loss Corrections **********/
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
		ERROR( classname(), "No Energy Loss Params Given - These must be applied here" )
	} 
	/************ Energy Loss Corrections **********/

	double mT = Common::transverse_mass( pt, mass );
	double mTm0 = mT - mass;

	// Must be done after corrections
	int ptBin 	  = binsPt->findBin( pt );
	corrTrackPt = pt;
	double avgP = binAverageP( ptBin );

	book->cd();

	// Traditionally Recentered values
	double raw_dEdx = pico->trackDedx(iTrack);
	double tof 		= zr->rTof(centerSpecies, pico->trackBeta(iTrack), p );
	double dedx 	= zr->rDedx(centerSpecies, pico->trackDedx(iTrack), p );
	
	// Non-Linearly Recentered values
	double tofNL 	= zr->nlTof(centerSpecies, pico->trackBeta(iTrack), p, avgP );
	double dedxNL 	= zr->nlDedx(centerSpecies, pico->trackDedx(iTrack), p, avgP );

	double z_d      = dedx;
	double z_b      = tof;
	// Choose recentered values
	if ( "nonlinear" == zrMethod ){
		z_b = tofNL;
		z_d = dedxNL;
	} 

	// event weight from RefMult correction
	double trackWeight = eventWeight;

	book->fill( "zd_vs_pT_"   + Common::speciesName( centerSpecies, charge, cBin ), pt, z_d, trackWeight );
	book->fill( "zd_vs_mT_"   + Common::speciesName( centerSpecies, charge, cBin ), mT, z_d, trackWeight );
	book->fill( "zd_vs_mTm0_" + Common::speciesName( centerSpecies, charge, cBin ), mTm0, z_d, trackWeight );

	book->fill( "zb_vs_pT_"   + Common::speciesName( centerSpecies, charge, cBin ), pt, z_b, trackWeight );
	book->fill( "zb_vs_mT_"   + Common::speciesName( centerSpecies, charge, cBin ), mT, z_b, trackWeight );
	book->fill( "zb_vs_mTm0_" + Common::speciesName( centerSpecies, charge, cBin ), mTm0, z_b, trackWeight );

	book->cd();
}
