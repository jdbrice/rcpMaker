
#include "InclusiveSpectra.h"
#include "ChainLoader.h"
#include "AnaPicoDst.h"
#include "DataSourceWrapper.h"

#include <limits.h>

/**
 * Constructor
 */
InclusiveSpectra::InclusiveSpectra( XmlConfig * config, string np, string fileList, string prefix ) : TreeAnalyzer( config, np, fileList, prefix  ){

	logger->setClassSpace( "InclusiveSpectra" );
	/**
	 * Make the desired PicoDataStore Interface
	 */
	if (ds )
		pico = new DataSourceWrapper( ds );
	else 
		pico = new AnaPicoDst( chain );
    /**
     * Setup the event cuts
     */
    cutVertexZ = new ConfigRange( cfg, np + "eventCuts.vertexZ", -200, 200 );
    cutVertexR = new ConfigRange( cfg, np + "eventCuts.vertexR", 0, 10 );
    cutVertexROffset = new ConfigPoint( cfg, np + "eventCuts.vertexROffset", 0.0, 0.0 );

   	/**
   	 * Setup the centrality bins
   	 */
    vector<string> centralPaths = cfg->childrenOf( np + "centrality" );
    for ( int iC = 0; iC < centralPaths.size(); iC++ ){
    	
    	centrals.push_back( cfg->tagName( centralPaths[ iC ] ) );
    	ConfigRange * cut = new ConfigRange( cfg, centralPaths[ iC ] );
    	cutCentrality[ centrals[ iC ] ] = cut;
    	
    	logger->info(__FUNCTION__) << "Centrality Cut " << centrals[iC] << " ( " << cut->min << ", " << cut->max <<" )" << endl;
    }

    /**
     * Setup the Track Cuts
     */
    cutNHits = new ConfigRange( cfg, np + "trackCuts.nHits", 0, INT_MAX );
    cutNHitsDedx = new ConfigRange( cfg, np + "trackCuts.nHitsDedx", 0, INT_MAX );
    cutNHitsFitOverPossible = new ConfigRange( cfg, np + "trackCuts.nHitsFitOverPossible", 0, INT_MAX );
    cutDca = new ConfigRange( cfg, np + "trackCuts.dca", 0, INT_MAX );
    cutYLocal = new ConfigRange( cfg, np + "trackCuts.yLocal", -1000, 1000 );
    cutTofMatch = new ConfigRange( cfg, np + "trackCuts.tofMatch", 0, 3 );

    /**
     * Setup the options
     */
    makeEventQA = cfg->getBool( np + "makeQA:event", false );
    makeTrackQA = cfg->getBool( np + "makeQA:track", false );
    if ( makeEventQA )
	    logger->info( __FUNCTION__ ) << "Making Event QA" << endl;
	if( makeTrackQA )
	    logger->info( __FUNCTION__ ) << "Making Track QA" << endl;

	if ( cfg->exists(np + "RMCParams" ) )
		rmc = new RefMultCorrection( config->getString( np + "RMCParams" ) );


}
/**
 * Destructor
 */
InclusiveSpectra::~InclusiveSpectra(){

	// delete the config cuts
	delete cutVertexZ;
	delete cutVertexR;
	delete cutVertexROffset;

	delete cutNHits;
	delete cutNHitsDedx;
	delete cutNHitsFitOverPossible;

	delete cutDca;
	delete cutYLocal;
	delete cutTofMatch;

	for ( int iC = 0; iC < centrals.size(); iC++ ){
		delete cutCentrality[ centrals[ iC ] ];
	}

	delete rmc;
}


void InclusiveSpectra::makeCentralityHistos() {
	
	/**
	 * Make centrality ptHistos
	 */
	book->cd();
	for ( int iC = 0; iC < centrals.size(); iC ++ ){

		string hName = histoForCentrality( centrals[ iC ] );
		logger->info( __FUNCTION__ ) << hName << endl;
		book->clone( "ptBase", hName );
	}
}


void InclusiveSpectra::preEventLoop(){
	
	TreeAnalyzer::preEventLoop();

	book->cd();
	makeCentralityHistos();

	book->cd();
	if ( cfg->exists( "QAHistograms.event" ) && makeEventQA ){
		logger->info(__FUNCTION__) << " Making event QA histograms " << endl;
		book->makeAll( "QAHistograms.event" );
	} 
	if ( cfg->exists( "QAHistograms.track" ) && makeTrackQA ){
		logger->info(__FUNCTION__) << " Making track QA histograms " << endl;
		book->makeAll( "QAHistograms.track" );
	}
}


void InclusiveSpectra::analyzeEvent(){

	// get the corrected ref mult
	if ( rmc )
		correctedRefMult = rmc->refMult( pico->eventRefMult(), pico->vZ() );
	else 
		correctedRefMult = -1;

	Int_t nTracks = pico->eventNumTracks();

	for ( Int_t iTrack = 0; iTrack < nTracks; iTrack ++ ){

		if ( !keepTrack( iTrack ) )
			continue;
		
		analyzeTrack( iTrack );	

	}
}


void InclusiveSpectra::analyzeTrack( Int_t iTrack ){

	double pt = pico->trackPt( iTrack );
	int charge = pico->trackCharge( iTrack );

	//cout << "nHitsFit : " << ds->getInt("Tracks.mNHitsFit", iTrack) << endl; 
	
	Int_t refMult = pico->eventRefMult();
	
	if ( correctedRefMult >= 0 ){
		//if ( abs(pico->vZ()) > 20 )
			//cout << correctedRefMult << "co, old =  " << refMult << endl;
		refMult = correctedRefMult;
	}
	
	book->cd();
	book->fill( "ptAll", pt );
	if ( 1 == charge )
		book->fill( "ptPos", pt );
	else if ( -1 == charge  )
		book->fill( "ptNeg", pt );
	/**
	 * Centrality cuts
	 */
	 for ( int iC = 0; iC < centrals.size(); iC++ ){
	 	string cent = centrals[ iC ];
	 	string hName = histoForCentrality( cent );
	 	if ( 	refMult >= cutCentrality[ cent ]->min && refMult <= cutCentrality[ cent ]->max ){
	 		book->fill( hName, pt );
	 	}
	 }
	
}

bool InclusiveSpectra::keepEvent(){

	UShort_t refMult = pico->eventRefMult();
	
	double z = pico->vZ();
	double x = pico->vX() + cutVertexROffset->x;
	double y = pico->vY() + cutVertexROffset->y;
	double r = TMath::Sqrt( x*x + y*y );

	/**
	 * Pre Event Cut QA
	 */
	if ( makeEventQA  ) {
		book->cd();
		book->fill( "preRefMult", refMult );
		book->fill( "preVertexZ", z);
		book->fill( "preVertexR", r);
	}
	

	if ( z < cutVertexZ->min || z > cutVertexZ->max )
		return false;
	if ( r < cutVertexR->min || r > cutVertexR->max )
		return false;


	/**
	 * Post Event Cut QA
	 */
	if ( makeEventQA  ){
		book->cd();
		book->fill( "vertexZ", z);
		book->fill( "vertexR", r);
		book->fill( "refMult", refMult );
	}
	
	return true;
}

bool InclusiveSpectra::keepTrack( Int_t iTrack ){



	double beta = pico->trackBeta( iTrack );

	if ( (1.0 / beta) < 0.5 )
		return false; 

	double eta = pico->trackEta( iTrack );

	if ( eta > .6 )
		return false;

	double dcaX = pico->trackDcaX( iTrack );
	double dcaY = pico->trackDcaY( iTrack );
	double dcaZ = pico->trackDcaZ( iTrack );
	double dca = pico->trackDca( iTrack );
	//TMath::Sqrt( dcaX*dcaX + dcaY*dcaY + dcaZ*dcaZ );

	int tofMatch = pico->trackTofMatch( iTrack );
	int nHits = pico->trackNHits( iTrack );
	int nHitsDedx = pico->trackNHitsDedx( iTrack );
	int nHitsFit = pico->trackNHitsFit( iTrack );
	int nHitsPossible = pico->trackNHitsPossible( iTrack );
	double fitPoss = ( (double)nHitsFit /  (double)nHitsPossible);

	double yLocal = pico->trackYLocal( iTrack );

	/**
	 * Pre Track Cut QA
	 */
	if ( makeTrackQA ){
		book->cd();
		book->fill( "preTofMatch", tofMatch );
		book->fill( "preYLocal", yLocal );
		book->fill( "preDca", dca );
		book->fill( "preNHits", nHits );
		book->fill( "preNHitsDedx", nHitsDedx );
		book->fill( "preNHitsFit", nHitsFit );
		book->fill( "preNHitsPossible", nHitsPossible );
		book->fill( "preNHitsFitOverPossible", fitPoss );	
	}

	if ( tofMatch < cutTofMatch->min || tofMatch > cutTofMatch->max )
		return false;
	if ( yLocal < cutYLocal->min || yLocal > cutYLocal->max )
		return false;
	if ( dca < cutDca->min || dca > cutDca->max )
		return false;
	if ( nHitsDedx < cutNHitsDedx->min || nHitsDedx > cutNHitsDedx->max )
		return false;
	if ( nHits < cutNHits->min || nHits > cutNHits->max )
		return false;
	if ( fitPoss < cutNHitsFitOverPossible->min || fitPoss > cutNHitsFitOverPossible->max )
		return false; 




	/**
	 * Post Track Cut QA
	 */
	if ( makeTrackQA ){
		book->cd();
		book->fill( "tofMatch", tofMatch );
		book->fill( "yLocal", yLocal );
		book->fill( "dca", dca );
		book->fill( "nHits", nHits );
		book->fill( "nHitsDedx", nHitsDedx );
		book->fill( "nHitsFit", nHitsFit );
		book->fill( "nHitsPossible", nHitsPossible );
		book->fill( "nHitsFitOverPossible", fitPoss );
	}

	return true;
}






