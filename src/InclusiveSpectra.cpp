
#include "InclusiveSpectra.h"
#include "ChainLoader.h"
#include "AnaPicoDst.h"

#include <limits.h>

/**
 * Constructor
 */
InclusiveSpectra::InclusiveSpectra( XmlConfig * config, string np, string fileList, string prefix ){

	//Set the Root Output Level
	gErrorIgnoreLevel=kSysError;

	// Save Class Members
	cfg = config;
	nodePath = np;
 
	// make the Logger
	lg = LoggerConfig::makeLogger( cfg, np + "Logger" );

	lg->info(__FUNCTION__) << "Got config with nodePath = " << np << endl;
	lg->info( __FUNCTION__) << "Making Chain and PicoDataStore Interface " << endl;
	//Create the chain
	TChain * chain = new TChain( cfg->getString(np+"input.dst:treeName", "tof" ).c_str() );

	if ( "" == fileList )
    	ChainLoader::load( chain, cfg->getString( np+"input.dst:url" ).c_str(), cfg->getInt( np+"input.dst:maxFiles" ) );
    else 
    	ChainLoader::loadList( chain, fileList );

    pico = new AnaPicoDst( chain );

    // create the book
    lg->info(__FUNCTION__) << " Creating book "<< endl;
    book = new HistoBook( prefix + config->getString( np + "output.data" ), config );

    if ( config->getString( np + "output.report", "" ).length() > 4 )
    	reporter = new Reporter( prefix + config->getString( np + "output.report" ) );

    /**
     * Setup the event cuts
     */
    cutVertexZ = new ConfigRange( cfg, np + "eventCuts.vertexZ", -200, 200 );
    cutVertexR = new ConfigRange( cfg, np + "eventCuts.vertexR", 0, 10 );
    cutVertexROffset = new ConfigPoint( cfg, np + "eventCuts.vertexROffset", 0.0, 0.0 );

    
    cutTriggers = cfg->getIntVector( np + "eventCuts.triggers" );
    vector<string> centralPaths = cfg->childrenOf( np + "centrality" );

    for ( int iC = 0; iC < centralPaths.size(); iC++ ){
    	
    	centrals.push_back( cfg->tagName( centralPaths[ iC ] ) );
    	ConfigRange * cut = new ConfigRange( cfg, centralPaths[ iC ] );
    	cutCentrality[ centrals[ iC ] ] = cut;
    	
    	lg->info(__FUNCTION__) << "Centrality Cut " << centrals[iC] << " ( " << cut->min << ", " << cut->max <<" )" << endl;
    }

    /**
     * Setup the Track Cuts
     */
    cutNHits = new ConfigRange( cfg, np + "trackCuts.nHits", 0, INT_MAX );
    cutNHitsDedx = new ConfigRange( cfg, np + "trackCuts.nHitsDedx", 0, INT_MAX );
    cutNHitsFitOverPossible = new ConfigRange( cfg, np + "trackCuts.nHitsFitOverPossible", 0, INT_MAX );
    cutDca = new ConfigRange( cfg, np + "trackCuts.dca", 0, INT_MAX );
    cutYLocal = new ConfigRange( cfg, np + "trackCuts.yLocal", -100, 100 );
    cutTofMatch = new ConfigRange( cfg, np + "trackCuts.tofMatch", 0, 3 );

    makeEventQA = cfg->getBool( np + "makeQA:event", false );
    makeTrackQA = cfg->getBool( np + "makeQA:track", false );

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


	// delete the book and reporter
	delete book;
	if ( reporter )
		delete reporter;

	// delete the logger
	lg->info(__FUNCTION__) << endl;
	delete lg;
}


void InclusiveSpectra::makeCentralityHistos() {
	
	/**
	 * Make centrality ptHistos
	 */
	for ( int iC = 0; iC < centrals.size(); iC ++ ){

		string hName = histoForCentrality( centrals[ iC ] );
		lg->info( __FUNCTION__ ) << hName << endl;
		book->clone( "ptBase", hName );
	}
}


void InclusiveSpectra::preLoop(){
	makeCentralityHistos();
}

void InclusiveSpectra::eventLoop(){
	lg->info(__FUNCTION__) << endl;

	Int_t nEvents = (Int_t)pico->getTree()->GetEntries();
	nEventsToProcess = cfg->getInt( nodePath+"input.dst:nEvents", nEvents );

	lg->info(__FUNCTION__) << "Reading " << nEvents << " events from chain" << endl;


	/**
	 * Make the Histograms
	 */
	book->cd("/");
	book->makeAll( nodePath + "histograms" );
	lg->info(__FUNCTION__) << "Making all histograms in : " << nodePath + "histograms" << endl;
	
	if ( cfg->nodeExists( "QAistograms.event" ) && makeEventQA ){
		lg->info(__FUNCTION__) << " Making event QA histograms " << endl;
		book->makeAll( "QAHistograms.event" );
	} 
	if ( cfg->nodeExists( "QAistograms.track" ) && makeTrackQA ){
		lg->info(__FUNCTION__) << " Making track QA histograms " << endl;
		book->makeAll( "QAHistograms.track" );
	}


	/**
	 * Run preloop hooks
	 */
	preLoop();

	TaskProgress tp( "Event Loop", nEventsToProcess );

	for ( Int_t iEvent = 0; iEvent < nEventsToProcess; iEvent ++ ){
		pico->GetEntry(iEvent);

		tp.showProgress( iEvent );

		
		if ( !eventCut( ) )
			continue;
		
		Int_t nTracks = pico->eventNumTracks();

		for ( Int_t iTrack = 0; iTrack < nTracks; iTrack ++ ){

			if ( !trackCut( iTrack ) )
				continue;
			
			analyzeTrack( iTrack );	

		}

	}

	postLoop();

	lg->info(__FUNCTION__) << "Complete" << endl;

}


void InclusiveSpectra::analyzeTrack( Int_t iTrack ){

	double pt = pico->trackPt( iTrack );
	int charge = pico->trackCharge( iTrack );
	Int_t refMult = pico->eventRefMult();

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

bool InclusiveSpectra::eventCut(){

	UShort_t refMult = pico->eventRefMult();
	
	double z = pico->eventVertexZ();
	double x = pico->eventVertexX() + cutVertexROffset->x;
	double y = pico->eventVertexY() + cutVertexROffset->y;
	double r = TMath::Sqrt( x*x + y*y );

	/**
	 * Pre Event Cut QA
	 */
	if ( makeEventQA  ) {
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
		book->fill( "vertexZ", z);
		book->fill( "vertexR", r);
		book->fill( "refMult", refMult );
	}
	
	return true;
}

bool InclusiveSpectra::trackCut( Int_t iTrack ){



	double beta = pico->trackBeta( iTrack );

	if ( (1.0 / beta) < 0.5 )
		return false; 

	double eta = pico->trackEta( iTrack );

	if ( eta > .6 )
		return false;

	double dcaX = pico->trackDcaX( iTrack );
	double dcaY = pico->trackDcaY( iTrack );
	double dcaZ = pico->trackDcaZ( iTrack );
	double dca = TMath::Sqrt( dcaX*dcaX + dcaY*dcaY + dcaZ*dcaZ );

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






