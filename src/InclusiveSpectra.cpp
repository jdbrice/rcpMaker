
#include "InclusiveSpectra.h"
#include "ChainLoader.h"
#include "LBNLPicoDst.h"
#include "FemtoDst.h"
#include "ProdPicoDst.h"

#include <limits.h>

/**
 * Constructor
 */
InclusiveSpectra::InclusiveSpectra( XmlConfig * config, string np, string fileList, string prefix ) : TreeAnalyzer( config, np, fileList, prefix  ){

	logger->setClassSpace( "InclusiveSpectra" );
	logger->info(__FUNCTION__) << endl;
	/**
	 * Make the desired PicoDataStore Interface
	 */
	
	if ( ds && ds->getTreeName() == "FemtoDst" )
		pico = unique_ptr<PicoDataStore>( new FemtoDst( ds->getChain() ) );
	else if ( ds && ds->getTreeName() == "PicoDst" )
		pico = unique_ptr<PicoDataStore>( new ProdPicoDst( ds->getChain() ) );
	else if ( chain ){
		logger->info(__FUNCTION__) << "ProdPico" << endl;
		pico = unique_ptr<PicoDataStore>( new ProdPicoDst( chain ) );
	}

	logger->info(__FUNCTION__) << endl;
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


	normEvents = 0;


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
		book->clone( "ptBase", hName );
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
	
	book->cd();
	for ( int i = 0; i < nCentralityBins()-1; i++ ){

		for ( int iB = 1; iB < book->get( "pt_"+ts(i) )->GetNbinsX()+1; iB++ ){

			double v = book->get( "pt_"+ts(i) )->GetBinContent( iB );
			double vE = book->get( "pt_"+ts(i) )->GetBinError( iB );
			double pt = book->get( "pt_"+ts(i) )->GetBinCenter( iB );

			v = v * 1.0 / ( normEvents * 2.0 * pt * 3.14159 );
			vE = vE * 1.0 / ( normEvents * 2.0 * pt * 3.14159 );

			book->get( "pt_"+ts(i) )->SetBinContent( iB, v );
			book->get( "pt_"+ts(i) )->SetBinError( iB, vE );
		}

	}

	/*reporter->newPage();
	vector<int> color = { kRed, kGreen, kBlue, kCyan, kSpring, kOrange, kPink, kBlack };
	for ( int i = 0; i < nCentralityBins()-1; i++ ){
		if ( i == 0 )
			book->style( "yield_"+ts(i) )->set( "range", 10e-5, 1 )->set( "linecolor", color[i] )->draw();
		else 
			book->style( "yield_"+ts(i) )->set( "range", 10e-5, 1 )->set( "linecolor", color[i] )->set( "draw", "same" )->draw();
	}
	gPad->SetLogy(1);
	reporter->savePage();*/


}


void InclusiveSpectra::analyzeEvent(){

	normEvents++;
	nTofMatchedTracks = 0;
	Int_t nTracks = pico->numTracks();

	for ( Int_t iTrack = 0; iTrack < nTracks; iTrack ++ ){

		if ( !keepTrack( iTrack ) )
			continue;
		
		analyzeTrack( iTrack );	

	}
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
		string cName = "pt_" + ts( cBin );
		book->fill( cName, pt, eventWeight );		
	}
	
}

bool InclusiveSpectra::keepEvent(){


	if ( makeEventQA ){
		book->cd( "EventQA" );
		book->get( "eventCuts" )->Fill( "All", 1 );
	}

	/**
	 * Bad Run Rejection
	 */
	if ( rmc->isBad( pico->runId() ) ){
		logger->warn( __FUNCTION__ ) << "Rejecting Run : " << pico->runId() << endl;
		return false;
	} 
	if ( makeEventQA )
		book->get( "eventCuts" )->Fill( "GoodRuns", 1 );
	
	/**
	 * Trigger Selection
	 */
	UInt_t tword = pico->triggerWord();

	if ( !(tword & triggerMask) )
		return false;
	if ( makeEventQA )
		book->get( "eventCuts" )->Fill( "Triggered", 1 );

	// give the event vars a default
	refMult = -1;
	cBin = -1;
	eventWeight = 1.0;

	// get the corrected ref mult
	if ( rmc )
		refMult = rmc->refMult( pico->refMult(), pico->vZ() );
	else {
		logger->error(__FUNCTION__) << "No RMC Parameters" << endl;
		return false;
	}
	
	// define the member centrality bin and event weight for the analysis functions to use	
	cBin = centralityBin( refMult );
	eventWeight = rmc->eventWeight( refMult, pico->vZ() );

	
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

	if ( refMult < 5 )
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
		book->fill( "corrRefMult", refMult );
		book->fill( "refMultBin9", rmc->bin9( refMult ) );
		book->fill( "refMultBin16", rmc->bin16( refMult ) );
		book->fill( "mappedRefMultBins", cBin, eventWeight );
		book->fill( "refMultBin9_corrRefMult", refMult, rmc->bin9( refMult ) );
	}
	
	return true;
}

bool InclusiveSpectra::keepTrack( Int_t iTrack ){

	if ( makeTrackQA ){
		book->cd( "TrackQA" );
		book->get( "trackCuts" )->Fill( "All", 1 );
	}
	
	double ptPrimary = pico->trackPt( iTrack );
	// skip non-primary tracks
	if ( 0.2 > ptPrimary || 0 >= pico->trackTofMatch( iTrack ) )
		return false;
	
	double dca = pico->trackDca( iTrack );
	int nHitsDedx = pico->trackNHitsDedx( iTrack );
	int nHitsFit = pico->trackNHitsFit( iTrack );
	int nHitsPossible = pico->trackNHitsPossible( iTrack );
	double fitPoss = ( (double)nHitsFit /  (double)nHitsPossible);

	double yLocal = pico->trackYLocal( iTrack );
	double zLocal = pico->trackZLocal( iTrack );

	
	double ptGlobal = pico->globalPt( iTrack );

	/**
	 * Pre Track Cut QA
	 */
	if ( makeTrackQA ){
		book->cd( "TrackQA" );
		book->fill( "preYLocal", 				yLocal );
		book->fill( "preZLocal", 				zLocal );
		book->fill( "preDca", 					dca );
		book->fill( "preNHitsDedx", 			nHitsDedx );
		book->fill( "preNHitsFit", 				nHitsFit );
		book->fill( "preNHitsPossible", 		nHitsPossible );
		book->fill( "preNHitsFitOverPossible", 	fitPoss );
		book->fill( "preNHitsFitVsPossible", 	nHitsPossible, nHitsFit );
		book->fill( "prePtPrimary", 			ptPrimary );
		book->fill( "prePtGlobal", 				ptGlobal );
		book->fill( "prePtGlobalOverPrimary", 	ptGlobal / ptPrimary );
		book->fill( "prePtGlobalVsPrimary", 	ptPrimary, ptGlobal );
		book->fill( "pre_refMult", 				refMult );
	}

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




	/**
	 * Post Track Cut QA
	 */
	if ( makeTrackQA ){
		book->cd( "TrackQA" );
		book->fill( "yLocal", 				yLocal );
		book->fill( "zLocal", 				zLocal );
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

		book->fill( "refMult", 				refMult );
	}

	return true;
}






