 // RcpMaker
#include "Spectra/DcaMapMaker.h"
#include "Correction/SpectraCorrecter.h"

// ROOT
#include "TLine.h"

// STL
#define _USE_MATH_DEFINES
#include <cmath> // for M_1_PI etc.


void DcaMapMaker::initialize() {

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
	zrMethod 		= config.getString( nodePath + ".ZRecentering.method", "traditional" );
		// alias the centered species for ease of use
	centerSpecies 	= config.getString( nodePath + ".ZRecentering.centerSpecies", "K" );

	//Make the momentum transverse binning
	binsPt = unique_ptr<HistoBins>(new HistoBins( config, "binning.pt" ));

	// Get the list of charges we are looking at
	charges = config.getIntVector( "binning.charges" );
	

	// make the energy loss params
	vector<int> charges = { -1, 1 };
	if ( config.exists( nodePath + ".EnergyLossParams:path" ) ){
		string path = config.getString( nodePath + ".EnergyLossParams:path" );

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

	xVar = config.getString( nodePath + ":xVar", "pT" );

	nSigmaZdCut = config.getDouble( nodePath + ":nSigmaZdCut", 2.0 );
	nSigmaZbCut = config.getDouble( nodePath + ":nSigmaZbCut", 2.0 );
	tofMinPt    = config.getDouble( nodePath + ":tofMinPt", 1.0 );

	// Disable the DCA cut!
	cut_dca->max = 1000;
	

}

DcaMapMaker::~DcaMapMaker(){

}

void DcaMapMaker::preEventLoop() {
	INFO( classname(), "");
	
	InclusiveSpectra::preEventLoop();

	// make the energy loss histograms
	book->cd( "energyLoss" );
	for ( int c : charges ){
		for ( int cb : centralityBins ) {
			book->clone( "/", "corrPt", "energyLoss", "corrPt_" + Common::chargeString(c) + "_" + ts(cb) );
		}
	}	
	
	book->cd();
	for ( string c : { "p", "n" } ){
		for ( int iC : centralityBins ){
			INFO( classname(), "dca_vs_pt_" + centerSpecies + "_" + ts( iC ) + "_" + c );
			book->clone( "dca_vs_pt", "dca_vs_pt_" + centerSpecies + "_" + ts( iC ) + "_" + c );
		}

		book->clone( "dca_vs_pt", "dca_vs_pt_" + centerSpecies + "_" + c );
	}
}

void DcaMapMaker::postEventLoop() {
	INFO( classname(), "");
	book->cd();

}

void DcaMapMaker::analyzeTrack( int iTrack ){
	InclusiveSpectra::analyzeTrack( iTrack );
	
	book->cd();

	// Alias Track Traits
	int charge 		= pico->trackCharge( iTrack );
	double pt 		= pico->trackPt( iTrack );
	double p 		= pico->trackP( iTrack );
	double eta 		= pico->trackEta( iTrack );
	double phi 		= pico->trackPhi( iTrack );
	double mass 	= Common::mass( centerSpecies );
	double dca 		= pico->trackDca( iTrack );
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

	double pX = pt * cos( phi );
	double pY = pt * sin( phi );
	double mT = Common::transverse_mass( pX, pY, mass );
	double mTm0 = mT - mass;

	// Must be done after corrections
	int ptBin 	= binsPt->findBin( pt );
	corrTrackPt = pt;
	double avgP = binAverageP( ptBin );

	double x = pt;
	
	if ( "mT" == xVar ){
		ptBin 	= binsPt->findBin( mT );
		x = mT;
	}
	else if ( "mTm0" == xVar ){
		ptBin 	= binsPt->findBin( mTm0 );
		x = mT; // not a typo
	}

	// Require valid p bin
	if ( ptBin < 0 ){	// only caused by pT outside of range we are interested in
		return;
	}

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

	// fill the tree
	if ( fabs(dedx / dedxSigmaIdeal)  < nSigmaZdCut ){

		double tofFactor = 5;
		if ( pt > tofMinPt )
			tofFactor = 1;

		if ( pt < tofMinPt || fabs(tof / tofSigmaIdeal) < nSigmaZbCut * tofFactor ){
			if ( cBin >= 0 ){
				string cName = "dca_vs_pt_" + centerSpecies + "_" + ts( cBin ) + "_" + Common::chargeString( charge );
				book->fill( cName, pt, dca, eventWeight );
			}

			string cName = "dca_vs_pt_" + centerSpecies + "_" + Common::chargeString( charge );
			book->fill( cName, pt, dca, eventWeight );
		}
	
	}
}
