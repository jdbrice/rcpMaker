
#include "InclusiveSpectra.h"
#include "ChainLoader.h"
#include "Adapter/ProdPicoDst.h"
#include "Adapter/RcpPicoDst.h"

#include <limits.h>
#include "PidPhaseSpace.h"

/**
 * Constructor
 */
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
	/**
	 * Load in the common configs
	 */
	if ( cfg->exists( np + "MakeQA:config" ) )
		qaCfg = unique_ptr<XmlConfig>(new XmlConfig( cfg->getString( np + "MakeQA:config" ) ) );
	if ( cfg->exists( np + "Cuts:config" ) )
		cutsCfg = unique_ptr<XmlConfig>(new XmlConfig( cfg->getString( np + "Cuts:config" ) ) );
	if ( cfg->exists( np + "RMCParams:config" ) )
		rmc = unique_ptr<RefMultCorrection> (new RefMultCorrection( config->getString( np + "RMCParams:config" ) ) );




	/**
	 * Event and Track Cuts
	 */
    // Events
    triggerMask 			= cutsCfg->getInt( "EventCuts.triggerMask" );
    cutVertexZ 				= unique_ptr<ConfigRange>(new ConfigRange( cutsCfg.get(), "EventCuts.vertexZ", 				-200, 	200 ) );
    cutVertexR 				= unique_ptr<ConfigRange>(new ConfigRange( cutsCfg.get(), "EventCuts.vertexR", 				0, 		10 ) );
    cutVertexROffset 		= unique_ptr<ConfigPoint>(new ConfigPoint( cutsCfg.get(), "EventCuts.vertexROffset", 		0.0, 	0.0 )  );
    cutNTofMatchedTracks 	= unique_ptr<ConfigRange>(new ConfigRange( cutsCfg.get(), "EventCuts.nTofMatchedTracks", 	2, 		1000 ) );
    // Tracks
    cutNHitsFit				= unique_ptr<ConfigRange>(new ConfigRange( cutsCfg.get(), "TrackCuts.nHitsFit", 			0, 		INT_MAX ) );
    cutDca 					= unique_ptr<ConfigRange>(new ConfigRange( cutsCfg.get(), "TrackCuts.dca", 					0, 		INT_MAX ) );
	cutNHitsFitOverPossible = unique_ptr<ConfigRange>(new ConfigRange( cutsCfg.get(), "TrackCuts.nHitsFitOverPossible", 0, 		INT_MAX ) );
    cutNHitsDedx 			= unique_ptr<ConfigRange>(new ConfigRange( cutsCfg.get(), "TrackCuts.nHitsDedx", 			0, 		INT_MAX ) );
    cutPt 					= unique_ptr<ConfigRange>(new ConfigRange( cutsCfg.get(), "TrackCuts.pt", 					0, 		INT_MAX ) );
    cutPtGlobalOverPrimary 	= unique_ptr<ConfigRange>(new ConfigRange( cutsCfg.get(), "TrackCuts.ptGlobalOverPrimary", 	0.7, 	1.42 ) );
    cutYLocal 				= unique_ptr<ConfigRange>(new ConfigRange( cutsCfg.get(), "TrackCuts.yLocal", 				-1.6, 	1.6 ) );
    cutZLocal 				= unique_ptr<ConfigRange>(new ConfigRange( cutsCfg.get(), "TrackCuts.zLocal", 				-2.8, 	2.8 ) );
    cutRapidity				= unique_ptr<ConfigRange>(new ConfigRange( cutsCfg.get(), "TrackCuts.rapidity", 			-0.25, 	0.25 ) );




   	/**
   	 * Setup the centrality bins
   	 */
   	logger->info( __FUNCTION__ ) << "Loading Centrality Map" << endl; 
    centralityBinMap = cfg->getIntMap( np + "CentralityMap" );
    

    
    /**
     * Setup the options
     */
    if ( qaCfg ){
    	logger->info(__FUNCTION__) << "GOT QA CONFIG " << endl;
	    makeEventQA = cfg->getBool( np + "MakeQA:event", false );
	    makeTrackQA = cfg->getBool( np + "MakeQA:track", false );
    } else {
    	makeEventQA = false;
    	makeTrackQA = false;
    }
    if ( makeEventQA )
	    logger->info( __FUNCTION__ ) << "Making Event QA" << endl;
	if( makeTrackQA )
	    logger->info( __FUNCTION__ ) << "Making Track QA" << endl;



}
/**
 * Destructor
 */
InclusiveSpectra::~InclusiveSpectra(){
	
}


void InclusiveSpectra::makeCentralityHistos() {
	
	/**
	 * Make centrality ptHistos
	 */
	book->cd();
	logger->info(__FUNCTION__) << "Making " << nCentralityBins() << " centralities" << endl; 
	for ( int iC = 0; iC < nCentralityBins(); iC ++ ){
		string hName = "pt_" + ts(iC);
		logger->info( __FUNCTION__ ) << hName << endl;
		book->clone( "ptBase", hName + "_p" );
		book->clone( "ptBase", hName + "_n" );

		hName = "pt_tof_" + ts(iC);
		logger->info( __FUNCTION__ ) << hName << endl;
		book->clone( "ptBase", hName + "_p" );
		book->clone( "ptBase", hName + "_n" );
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

	
	if ( qaCfg->exists( "Event" ) && makeEventQA ){
		book->cd("EventQA");
		logger->info(__FUNCTION__) << " Making event QA histograms " << endl;
		book->makeAll( qaCfg.get(), "Event" );
	} 
	if ( qaCfg->exists( "Track" ) && makeTrackQA ){
		book->cd("TrackQA");
		logger->info(__FUNCTION__) << " Making track QA histograms " << endl;
		book->makeAll( qaCfg.get(), "Track" );
	}



}

void InclusiveSpectra::postEventLoop(){

}


void InclusiveSpectra::analyzeEvent(){

	nTofMatchedTracks = 0;
	Int_t nTracks = pico->numTracks();

	for ( Int_t iTrack = 0; iTrack < nTracks; iTrack ++ ){

		if ( !keepTrack( iTrack ) )
			continue;
		
		analyzeTrack( iTrack );	

		if ( !keepTofTrack( iTrack ) )
			continue;
		
		analyzeTofTrack( iTrack );			
	}

	// if ( pico->b9() != rmc->bin9( refMult )  )
	// 	cout << "ERROR in RMC" << endl;
}


void InclusiveSpectra::analyzeTrack( Int_t iTrack ){

	double pt = pico->trackPt( iTrack );
	int charge = pico->trackCharge( iTrack );
	 
	book->cd();
	book->fill( "ptAll", pt, eventWeight );
	if ( 1 == charge )
		book->fill( "ptPos", pt, eventWeight );
	else if ( -1 == charge  )
		book->fill( "ptNeg", pt, eventWeight );
	
	if ( cBin >= 0 ){
		string cName = "pt_" + ts( cBin ) + "_" + PidPhaseSpace::chargeString( charge );
		book->fill( cName, pt, eventWeight );		
	}
	
}

void InclusiveSpectra::analyzeTofTrack( Int_t iTrack ){

	double pt = pico->trackPt( iTrack );
	int charge = pico->trackCharge( iTrack );
	 
	book->cd();
	book->fill( "ptAll", pt, eventWeight );
	if ( 1 == charge )
		book->fill( "ptPos", pt, eventWeight );
	else if ( -1 == charge  )
		book->fill( "ptNeg", pt, eventWeight );
	
	if ( cBin >= 0 ){
		string cName = "pt_tof_" + ts( cBin ) + "_" + PidPhaseSpace::chargeString( charge );
		book->fill( cName, pt, eventWeight );		
	}
	
}


bool InclusiveSpectra::keepEvent(){

	if ( "rcpPicoDst" == cfg->getString( nodePath + "input.dst:treeName" ) ){

		eventWeight = pico->eventWeight();
		refMult 	= pico->refMult();
		cBin 		= centralityBin( refMult );
		
		
		//Bad Run Rejection
		if ( rmc->isBad( pico->runId() ) ){
			logger->debug( __FUNCTION__ ) << "Rejecting Run : " << pico->runId() << endl;
			return false;
		} 

		// Report event counts for normalization
		if ( makeEventQA ){
			book->cd( "EventQA" );
			
			book->fill( "corrRefMult", refMult, eventWeight );
			book->fill( "mappedRefMultBins", cBin, eventWeight );
			book->fill( "refMultBin9", pico->b9(), eventWeight );
			book->fill( "refMultBin16", pico->b16(), eventWeight );
		}

		return true;
	}

	if ( makeEventQA ){
		book->cd( "EventQA" );
		book->get( "eventCuts" )->Fill( "All", 1 );
	}

	/**
	 * Bad Run Rejection
	 */
	if ( rmc->isBad( pico->runId() ) ){
		logger->debug( __FUNCTION__ ) << "Rejecting Run : " << pico->runId() << endl;
		return false;
	} 

	if ( makeEventQA )
		book->get( "eventCuts" )->Fill( "GoodRuns", 1 );
	
	/**
	 * Trigger Selection
	 */
	if ( !(pico->triggerWord() & triggerMask) )
		return false;
	
	if ( makeEventQA )
		book->get( "eventCuts" )->Fill( "Triggered", 1 );

	// give the event vars a default
	refMult = -1;
	cBin = -1;
	eventWeight = 1.0;

	// get the corrected ref mult
	if ( rmc ){
		refMult = rmc->refMult( pico->refMult(), pico->vZ() );
	}
	else {
		logger->error(__FUNCTION__) << "No RMC Parameters" << endl;
		return false;
	}
	
	
	// define the member centrality bin and event weight for the analysis functions to use	
	cBin 		= centralityBin( refMult );
	eventWeight = rmc->eventWeight( refMult, pico->vZ() );
	// has it already
	if ( "rcpPicoDst" == cfg->getString( nodePath + "input.dst:treeName" ) ){
		
	}
	
	double z = pico->vZ();
	double x = pico->vX() + cutVertexROffset->x;
	double y = pico->vY() + cutVertexROffset->y;
	double r = TMath::Sqrt( x*x + y*y );

	nTofMatchedTracks = pico->numTofMatchedTracks();


	/**
	 * Pre Event Cut QA
	 */
	if ( makeEventQA  ) {
		book->fill( "pre_nTof_corrRefMult", refMult, nTofMatchedTracks );
		book->fill( "pre_vZ", z );
		book->fill( "pre_vR", r );
		book->fill( "pre_vY_vX", x, y );
		book->fill( "pre_refMult", pico->refMult() );
		book->fill( "pre_nTofMatch", nTofMatchedTracks );
	}
	
	
	if ( z < cutVertexZ->min || z > cutVertexZ->max )
		return false;
	if ( makeEventQA )
		book->get( "eventCuts" )->Fill( "vZ", 1 );
	if ( r < cutVertexR->min || r > cutVertexR->max )
		return false;
	if ( makeEventQA )
		book->get( "eventCuts" )->Fill( "vR", 1 );
	if ( nTofMatchedTracks < cutNTofMatchedTracks->min )
		return false;
	if ( makeEventQA )
		book->get( "eventCuts" )->Fill( "nTofMatched", 1 );

	if ( refMult < 6 )
		return false;

	/**
	 * Post Event Cut QA
	 */
	if ( makeEventQA  ){
		book->fill( "nTof_corrRefMult", refMult, nTofMatchedTracks );
		book->fill( "vZ", z );
		book->fill( "vR", r );
		book->fill( "vY_vX", x, y );

		book->fill( "refMult", pico->refMult() );
		book->fill( "corrRefMult", refMult, eventWeight );
		book->fill( "refMultBin9", rmc->bin9( refMult ), eventWeight );
		book->fill( "refMultBin16", rmc->bin16( refMult ), eventWeight );
		book->fill( "mappedRefMultBins", cBin, eventWeight );
		book->fill( "nTofMatch", nTofMatchedTracks );
	}
	
	return true;
}

bool InclusiveSpectra::keepTrack( Int_t iTrack ){

	bool isRcpPicoDst = "rcpPicoDst" == cfg->getString( nodePath + "input.dst:treeName" );
	

	if ( makeTrackQA ){
		book->cd( "TrackQA" );
		book->get( "trackCuts" )->Fill( "All", 1 );
	}
	
	double ptPrimary = pico->trackPt( iTrack );
	if ( 0.01 > ptPrimary )
		return false;
	if ( makeTrackQA )
		book->get( "trackCuts" )->Fill( "primaryTracks", 1 );

	
	
	// alias
	double 	dca 			= pico->trackDca( iTrack );
	int 	nHitsDedx 		= pico->trackNHitsDedx( iTrack );
	int 	nHitsFit 		= pico->trackNHitsFit( iTrack );
	int 	nHitsPossible 	= pico->trackNHitsPossible( iTrack );
	double 	fitPoss 		= ( (double)nHitsFit /  (double)nHitsPossible);
	double 	yLocal 			= pico->trackYLocal( iTrack );
	double 	zLocal 			= pico->trackZLocal( iTrack );
	double 	ptGlobal 		= pico->globalPt( iTrack );
	double 	pt 		 		= pico->trackPt( iTrack );
	int 	matchFlag		= pico->trackTofMatch( iTrack );
	double 	eta 			= pico->trackEta( iTrack );
	

	/**
	 * Pre Track Cut QA
	 */
	if ( makeTrackQA ){

		book->fill( "pre_dca", 					dca );
		book->fill( "pre_nHitsDedx", 			nHitsDedx );
		book->fill( "pre_nHitsFit", 			nHitsFit );
		book->fill( "pre_nHitsPossible", 		nHitsPossible );
		book->fill( "pre_nHitsFitOverPossible", fitPoss );
		book->fill( "pre_nHitsFitVsPossible", 	nHitsPossible, nHitsFit );
		book->fill( "pre_ptPrimary", 			ptPrimary );
		book->fill( "pre_ptGlobal", 			ptGlobal );
		book->fill( "pre_ptGlobalOverPrimary", 	ptGlobal / ptPrimary );
		book->fill( "pre_ptGlobalVsPrimary", 	ptPrimary, ptGlobal );
		//book->fill( "pre_refMult", 				refMult );

		book->fill( "pre_eta", 					eta );
		

	}
	

	if ( !isRcpPicoDst ){
		if ( ptPrimary < cutPt->min || ptPrimary > cutPt->max )
			return false;
		if ( makeTrackQA )
			book->get( "trackCuts" )->Fill( "ptPrimary", 1 );
		if ( (ptGlobal / ptPrimary) < cutPtGlobalOverPrimary->min || (ptGlobal / ptPrimary) > cutPtGlobalOverPrimary->max )
			return false;
		if ( makeTrackQA )
			book->get( "trackCuts" )->Fill( "ptRatio", 1 );
		if ( dca < cutDca->min || dca > cutDca->max )
			return false;
		if ( makeTrackQA )
			book->get( "trackCuts" )->Fill( "dca", 1 );
		if ( nHitsDedx < cutNHitsDedx->min || nHitsDedx > cutNHitsDedx->max )
			return false;
		if ( makeTrackQA )
			book->get( "trackCuts" )->Fill( "nHitsDedx", 1 );
		if ( nHitsFit < cutNHitsFit->min || nHitsFit > cutNHitsFit->max )
			return false;
		if ( makeTrackQA )
			book->get( "trackCuts" )->Fill( "nHitsFit", 1 );
		if ( fitPoss < cutNHitsFitOverPossible->min || fitPoss > cutNHitsFitOverPossible->max )
			return false; 
		if ( makeTrackQA )
			book->get( "trackCuts" )->Fill( "nHitsRatio", 1 );

	} // isRcpPicoDst

	


	// Post Track Cut QA
	if ( makeTrackQA ){

		book->fill( "dca", 					dca );
		book->fill( "nHitsDedx", 			nHitsDedx );
		book->fill( "nHitsFit", 			nHitsFit );
		book->fill( "nHitsPossible", 		nHitsPossible );
		book->fill( "nHitsFitOverPossible", fitPoss );
		book->fill( "nHitsFitVsPossible", 	nHitsPossible, nHitsFit );
		book->fill( "ptPrimary", 			ptPrimary );
		book->fill( "ptGlobal", 			ptGlobal );
		book->fill( "ptGlobalOverPrimary", 	ptGlobal / ptPrimary );
		book->fill( "ptGlobalVsPrimary", 	ptPrimary, ptGlobal );

		book->fill( "eta", 					eta );
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

		book->fill( "pre_yLocal", 				yLocal );
		book->fill( "pre_zLocal", 				zLocal );
		book->fill( "pre_matchFlag", 			matchFlag );
	}

	// TOF track cuts
	if ( 0 >= matchFlag )
		return false;
	if ( makeTrackQA )
		book->get( "trackCuts" )->Fill( "tofMatch", 1 );
	if ( yLocal < cutYLocal->min || yLocal > cutYLocal->max )
		return false;
	if ( makeTrackQA )
		book->get( "trackCuts" )->Fill( "yLocal", 1 );
	if ( zLocal < cutZLocal->min || zLocal > cutZLocal->max )
		return false;
	if ( makeTrackQA )
			book->get( "trackCuts" )->Fill( "zLocal", 1 );


	// Post Track Cut QA
	if ( makeTrackQA ){

		book->fill( "yLocal", 				yLocal );
		book->fill( "zLocal", 				zLocal );
		book->fill( "matchFlag", 			matchFlag );
	}

	return true;
}






