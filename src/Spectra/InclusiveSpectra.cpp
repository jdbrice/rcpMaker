
#include "Spectra/InclusiveSpectra.h"
#include "ChainLoader.h"
#include "Adapter/ProdPicoDst.h"
#include "Adapter/RcpPicoDst.h"

#include <limits.h>
#include "Spectra/PidHistoMaker.h"

InclusiveSpectra::InclusiveSpectra( XmlConfig * config, string np, string fileList, string prefix ) : TreeAnalyzer( config, np, fileList, prefix  ){

	logger->setClassSpace( "InclusiveSpectra" );
	logger->info(__FUNCTION__) << endl;
	/**
	 * Make the desired PicoDataStore Interface
	 */
	logger->debug(__FUNCTION__) << ds << endl;
	if ( ds && ds->getTreeName() == "PicoDst" ){
		logger->info(__FUNCTION__) << "DataDtore" << endl;
		pico = unique_ptr<PicoDataStore>( new ProdPicoDst( ds->getChain() ) );
	} else if ( chain && "PicoDst" == cfg->getString( np + "input.dst:treeName" ) ){
		logger->info(__FUNCTION__) << "ProdPico" << endl;
		pico = unique_ptr<PicoDataStore>( new ProdPicoDst( chain ) );
	} else if ( chain && "rcpPicoDst" == cfg->getString( np + "input.dst:treeName" ) ){
		logger->info(__FUNCTION__) << "Rcp Pico" << endl;
		pico = unique_ptr<PicoDataStore>( new RcpPicoDst( chain ) );
	}

	logger->info(__FUNCTION__) << "Got a valid Data Adapter" << endl;


    // Tracks cuts
    cut_nHitsFit				= unique_ptr<ConfigRange>(new ConfigRange( cfg, "TrackCuts.nHitsFit", 				0, 		INT_MAX ) );
    cut_dca 					= unique_ptr<ConfigRange>(new ConfigRange( cfg, "TrackCuts.dca", 					0, 		INT_MAX ) );
	cut_nHitsFitOverPossible 	= unique_ptr<ConfigRange>(new ConfigRange( cfg, "TrackCuts.nHitsFitOverPossible", 	0, 		INT_MAX ) );
    cut_nHitsDedx 				= unique_ptr<ConfigRange>(new ConfigRange( cfg, "TrackCuts.nHitsDedx", 				0, 		INT_MAX ) );
    cut_pt 						= unique_ptr<ConfigRange>(new ConfigRange( cfg, "TrackCuts.pt", 					0, 		INT_MAX ) );
    cut_ptGlobalOverPrimary 	= unique_ptr<ConfigRange>(new ConfigRange( cfg, "TrackCuts.ptGlobalOverPrimary", 	0.7, 	1.42 ) );
    cut_yLocal 					= unique_ptr<ConfigRange>(new ConfigRange( cfg, "TrackCuts.yLocal", 				-1.6, 	1.6 ) );
    cut_zLocal 					= unique_ptr<ConfigRange>(new ConfigRange( cfg, "TrackCuts.zLocal", 				-2.8, 	2.8 ) );
    cut_matchFlag				= unique_ptr<ConfigRange>(new ConfigRange( cfg, "TrackCuts.matchFlag", 				1, 		3 ) );
    cut_rapidity				= unique_ptr<ConfigRange>(new ConfigRange( cfg, "TrackCuts.rapidity", 				-0.25, 	0.25 ) );




   	// Setup the centrality bins
   	logger->info( __FUNCTION__ ) << "Loading Centrality Map" << endl; 
    centralityBinMap = cfg->getIntMap( np + "CentralityMap" );
    logger->info( __FUNCTION__ ) << "c[ 0 ] = " << centralityBinMap[ 0 ] << endl;
    

    
    // Setup the options
	makeTrackQA = cfg->getBool( np + "MakeQA:track", false );
	if( makeTrackQA )
	    logger->info( __FUNCTION__ ) << "Making Track QA" << endl;

	makeEventQA = cfg->getBool( np + "MakeQA:event", false );
	if( makeEventQA )
	    logger->info( __FUNCTION__ ) << "Making Event QA" << endl;



	plc = cfg->getString( nodePath + "input:plc", "" );
	mass = Common::mass( plc );
	if ( mass < 0 )
		ERROR( "Invalid Particle Species " << plc )


	cfg->report();

}

/* Destructor
 *
 */
InclusiveSpectra::~InclusiveSpectra(){
	
}


void InclusiveSpectra::makeCentralityHistos() {
	
	/**
	 * Make centrality ptHistos
	 */
	book->cd();
	logger->info(__FUNCTION__) << "Making " << nCentralityBins() << " centralities" << endl; 

	book->clone( "pt", "pt_p" );
	book->clone( "pt", "pt_n" );
	book->clone( "pt", "pt_tof" );
	book->clone( "pt", "pt_tof_p" );
	book->clone( "pt", "pt_tof_n" );

	for ( int iC = 0; iC < nCentralityBins(); iC ++ ){
		string hName = "pt_" + ts(iC);
		logger->info( __FUNCTION__ ) << hName << endl;
		book->clone( "pt", hName );
		book->clone( "pt", hName + "_p" );
		book->clone( "pt", hName + "_n" );

		hName = "pt_tof_" + ts(iC);
		logger->info( __FUNCTION__ ) << hName << endl;
		book->clone( "pt", hName + "_p" );
		book->clone( "pt", hName + "_n" );
	}
	for ( int iB = 0; iB < 11; iB++ ){
		if( centralityBinMap.find( iB ) != centralityBinMap.end() )
			logger->info( __FUNCTION__ ) << "[" << iB << "] = " << centralityBinMap[ iB ] << endl;
	}
}


void InclusiveSpectra::preEventLoop(){
	
	TreeAnalyzer::preEventLoop();

	book->cd();
	makeCentralityHistos();

	if ( makeEventQA ){
		book->cd( "EventQA" );
		logger->info(__FUNCTION__) << "Making Event QA histograms " << endl;
		book->makeAll( cfg, "QAHistograms.Event" );
		book->setBin( "mass", 1, mass, 0 );
	}
	if ( makeTrackQA ){
		book->cd( "TrackQA" );
		logger->info(__FUNCTION__) << "Making track QA histograms " << endl;
		book->makeAll( cfg, "QAHistograms.Track" );
	}




}

void InclusiveSpectra::postEventLoop(){

}


void InclusiveSpectra::analyzeEvent(){

	// Number of tracks
	Int_t nTracks = pico->numTracks();

	for ( Int_t iTrack = 0; iTrack < nTracks; iTrack ++ ){

		if ( !keepTrack( iTrack ) )
			continue;
		
		analyzeTrack( iTrack );	

		if ( !keepTofTrack( iTrack ) )
			continue;
		
		analyzeTofTrack( iTrack );			
	}

}


void InclusiveSpectra::analyzeTrack( Int_t iTrack ){

	double pt = pico->trackPt( iTrack );
	int charge = pico->trackCharge( iTrack );
	 
	book->cd();
	book->fill( "pt", pt, eventWeight );
	
	book->fill( "pt_" + Common::chargeString( charge ) , pt, eventWeight );
	
	if ( cBin >= 0 ){
		string cName = "pt_" + ts( cBin ) ;
		book->fill( cName, pt, eventWeight );
		
		cName = "pt_" + ts( cBin ) + "_" + Common::chargeString( charge );
		book->fill( cName, pt, eventWeight );
	}
	
}

void InclusiveSpectra::analyzeTofTrack( Int_t iTrack ){

	double pt = pico->trackPt( iTrack );
	int charge = pico->trackCharge( iTrack );
	 
	book->cd();
	book->fill( "pt_tof", pt, eventWeight );
	book->fill( "pt_tof_" + Common::chargeString( charge ), pt, eventWeight );
	
	if ( cBin >= 0 ){
		string cName = "pt_tof_" + ts( cBin ) + "_" + Common::chargeString( charge );
		book->fill( cName, pt, eventWeight );		
	}
	
}


bool InclusiveSpectra::keepEvent(){

	if ( "rcpPicoDst" == cfg->getString( nodePath + "input.dst:treeName" ) ){

		eventWeight = pico->eventWeight();
		refMult 	= pico->refMult();
		cBin 		= centralityBin();	// gets the mapped centrality bin

		// Report event counts for normalization
		if ( makeEventQA ){
			book->cd( "EventQA" );
			
			book->fill( "cuts", 1 );
			book->fill( "corrRefMult", 			refMult, 		eventWeight );
			book->fill( "mappedRefMultBins", 	cBin, 			eventWeight );
			book->fill( "refMultBin9", 			pico->b9(), 	eventWeight );
			book->fill( "refMultBin16", 		pico->b16(), 	eventWeight );

			
		}

		return true;
	}

	ERROR( "RcpPicoDsts are required" )
	return false;
}

bool InclusiveSpectra::keepTrack( Int_t iTrack ){

	if ( makeTrackQA ){
		book->cd( "TrackQA" );
		book->get( "cuts" )->Fill( "All", 1 );
	}
	
	
	// alias
	double 	dca 			= pico->trackDca( iTrack );
	int 	nHitsDedx 		= pico->trackNHitsDedx( iTrack );
	int 	nHitsFit 		= pico->trackNHitsFit( iTrack );
	int 	nHitsPossible 	= pico->trackNHitsPossible( iTrack );
	double 	fitRatio 		= ( (double)nHitsFit /  (double)nHitsPossible);
	double 	ptPrimary 		= pico->trackPt( iTrack );
	double 	ptGlobal 		= pico->globalPt( iTrack );
	double 	eta 			= pico->trackEta( iTrack );
	double 	rapidity 		= Common::rapidity( ptPrimary, eta, mass );
	

	/**
	 * Pre Track Cut QA
	 */
	if ( makeTrackQA ){

		book->fill( "pre_dca", 					dca );
		book->fill( "pre_nHitsDedx", 			nHitsDedx );
		book->fill( "pre_nHitsFit", 			nHitsFit );
		book->fill( "pre_nHitsPossible", 		nHitsPossible );
		book->fill( "pre_nHitsFitOverPossible", fitRatio );
		book->fill( "pre_nHitsFitVsPossible", 	nHitsPossible, nHitsFit );
		book->fill( "pre_ptPrimary", 			ptPrimary );
		book->fill( "pre_ptGlobal", 			ptGlobal );
		book->fill( "pre_ptGlobalOverPrimary", 	ptGlobal / ptPrimary );
		book->fill( "pre_ptGlobalVsPrimary", 	ptPrimary, ptGlobal );
		book->fill( "pre_eta", 					eta );
		book->fill( "pre_rapidity", 			rapidity );
		

	}
	


	if ( ptPrimary < cut_pt->min || ptPrimary > cut_pt->max )
		return false;
	if ( makeTrackQA )
		book->get( "cuts" )->Fill( "ptPrimary", 1 );
	
	if ( (ptGlobal / ptPrimary) < cut_ptGlobalOverPrimary->min || (ptGlobal / ptPrimary) > cut_ptGlobalOverPrimary->max )
		return false;
	if ( makeTrackQA )
		book->get( "cuts" )->Fill( "ptRatio", 1 );
	
	if ( dca < cut_dca->min || dca > cut_dca->max )
		return false;
	if ( makeTrackQA )
		book->get( "cuts" )->Fill( "dca", 1 );
	
	if ( nHitsDedx < cut_nHitsDedx->min || nHitsDedx > cut_nHitsDedx->max )
		return false;
	if ( makeTrackQA )
		book->get( "cuts" )->Fill( "nHitsDedx", 1 );
	
	if ( nHitsFit < cut_nHitsFit->min || nHitsFit > cut_nHitsFit->max )
		return false;
	if ( makeTrackQA )
		book->get( "cuts" )->Fill( "nHitsFit", 1 );
	
	if ( fitRatio < cut_nHitsFitOverPossible->min || fitRatio > cut_nHitsFitOverPossible->max )
		return false; 
	if ( makeTrackQA )
		book->get( "cuts" )->Fill( "nHitsRatio", 1 );

	if ( rapidity < cut_rapidity->min || rapidity > cut_rapidity->max )
		return false;
	if ( makeTrackQA )
		book->get( "cuts" )->Fill( "rapidity", 1 );


	
	// Post Track Cut QA
	if ( makeTrackQA ){

		book->fill( "dca", 					dca );
		book->fill( "nHitsDedx", 			nHitsDedx );
		book->fill( "nHitsFit", 			nHitsFit );
		book->fill( "nHitsPossible", 		nHitsPossible );
		book->fill( "nHitsFitOverPossible", fitRatio );
		book->fill( "nHitsFitVsPossible", 	nHitsPossible, nHitsFit );
		book->fill( "ptPrimary", 			ptPrimary );
		book->fill( "ptGlobal", 			ptGlobal );
		book->fill( "ptGlobalOverPrimary", 	ptGlobal / ptPrimary );
		book->fill( "ptGlobalVsPrimary", 	ptPrimary, ptGlobal );
		book->fill( "eta", 					eta );
		book->fill( "rapidity", 			rapidity );
	}

	return true;
}


bool InclusiveSpectra::keepTofTrack( Int_t iTrack ){

	// alias
	double 	yLocal 			= pico->trackYLocal( iTrack );
	double 	zLocal 			= pico->trackZLocal( iTrack );
	int 	matchFlag		= pico->trackTofMatch( iTrack );
	

	/**
	 * Pre Track Cut QA
	 */
	if ( makeTrackQA ){
		book->cd( "TrackQA" );
		book->fill( "pre_yLocal", 				yLocal );
		book->fill( "pre_zLocal", 				zLocal );
		book->fill( "pre_matchFlag", 			matchFlag );
	}

	// TOF track cuts
	if ( matchFlag < cut_matchFlag->min || matchFlag > cut_matchFlag->max )
		return false;
	if ( makeTrackQA )
		book->fill( "cuts", "tofMatch" );
	
	if ( yLocal < cut_yLocal->min || yLocal > cut_yLocal->max )
		return false;
	if ( makeTrackQA )
		book->fill( "cuts", "yLocal" );
	
	if ( zLocal < cut_zLocal->min || zLocal > cut_zLocal->max )
		return false;
	if ( makeTrackQA )
		book->fill( "cuts", "zLocal" );


	// Post Track Cut QA
	if ( makeTrackQA ){

		book->fill( "yLocal", 				yLocal );
		book->fill( "zLocal", 				zLocal );
		book->fill( "matchFlag", 			matchFlag );
	}

	return true;
}






