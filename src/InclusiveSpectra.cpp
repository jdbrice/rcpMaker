
#include "InclusiveSpectra.h"
#include "ChainLoader.h"

/**
 * Constructor
 */
InclusiveSpectra::InclusiveSpectra( XmlConfig * config, string np){

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
    ChainLoader::load( chain, cfg->getString( np+"input.dst:url" ).c_str(), cfg->getInt( np+"input.dst:maxFiles" ) );

    pico = new TofPicoDst( chain );

    // create the book
    lg->info(__FUNCTION__) << " Creating book "<< endl;
    book = new HistoBook( config->getString( np + "output.data" ), config );

    reporter = new Reporter( config->getString( np + "output.report" ) );

    /**
     * Setup the ana cuts
     */
    cutVertexZ = new ConfigRange( cfg, np + "eventCuts.vertexZ", -200, 200 );
    cutVertexR = new ConfigRange( cfg, np + "eventCuts.vertexR", 0, 10 );
    cutVertexROffset = new ConfigPoint( cfg, np + "eventCuts.vertexROffset", 0.0, 0.0 );

    cutNTZero = new ConfigRange( cfg, np + "eventCuts.nTZero", 0, 10000 );
    cutTriggers = cfg->getIntVector( np + "eventCuts.triggers" );
    vector<string> centralPaths = cfg->childrenOf( np + "centrality" );

    for ( int iC = 0; iC < centralPaths.size(); iC++ ){
    	centrals.push_back( cfg->tagName( centralPaths[ iC ] ) );
    	ConfigRange * cut = new ConfigRange( cfg, centralPaths[ iC ] );
    	cutCentrality[ centrals[ iC ] ] = cut;
    	lg->info(__FUNCTION__) << "Centrality Cut " << centrals[iC] << " ( " << cut->min << ", " << cut->max <<" )" << endl;
    }

}
/**
 * Destructor
 */
InclusiveSpectra::~InclusiveSpectra(){

	/** 
	 * DELETE CONFIGRNGES ETC.!!!
	 */
	delete book;
	delete reporter;

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
	lg->info(__FUNCTION__) << "Reading " << nEvents << " events from chain" << endl;


	/**
	 * Make the Histograms
	 */
	lg->info(__FUNCTION__) << "Making all histograms in : " << nodePath + "histograms" << endl;
	book->makeAll( nodePath + "histograms" );

	preLoop();

	TaskProgress tp( "Event Loop", nEvents );

	for ( Int_t iEvent = 0; iEvent < nEvents; iEvent ++ ){
		pico->GetEntry(iEvent);

		tp.showProgress( iEvent );

		
		if ( !eventCut( ) )
			continue;
		
		Int_t nTracks = pico->numTofTracks();

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

	// if a trigger list is given then filter,
	// if not don't worry about triggers
	if ( cutTriggers.size() >= 1  ){
		vector<UInt_t> triggerIds = pico->eventTriggerIds();
		bool findTrigger = false;
		for ( int iTrig = 0; iTrig < cutTriggers.size(); iTrig++ ){
			if ( triggerIds.end() != find( triggerIds.begin(), triggerIds.end(), (UInt_t)cutTriggers[ iTrig ] ) )
				findTrigger = true;
		}
		if ( !findTrigger )
			return false;
	}

	double z = pico->eventVertexZ();
	double x = pico->eventVertexX() + cutVertexROffset->x;
	double y = pico->eventVertexY() + cutVertexROffset->y;
	double r = TMath::Sqrt( x*x + y*y );

	int nT0 = pico->eventNTZero();

	//book->fill( "vertexZ", z);
	//book->fill( "vertexR", r);
	
	if ( z < cutVertexZ->min || z > cutVertexZ->max )
		return false;

	//book->fill( "vertexZ_zCut", z);
	//book->fill( "vertexR_zCut", r);
	if ( r < cutVertexR->min || r > cutVertexR->max )
		return false;
	//book->fill( "vertexZ_r_zCut", z);
	//book->fill( "vertexR_r_zCut", r);

	if ( nT0 < cutNTZero->min || nT0 > cutNTZero->max )
		return false;

	//book->fill( "refMult", pico->eventRefMult() );

	return true;
}

bool InclusiveSpectra::trackCut( Int_t iTrack ){

	double eta = pico->trackEta( iTrack );

	if ( eta > .2 )
		return false;

	return true;
}






