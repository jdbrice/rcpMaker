
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

	lg->info( __FUNCTION__) << "Making Chain and PicoDataStore Interface " << endl;
	//Create the chain
	TChain * chain = new TChain( cfg->getString(np+"input.dst:treeName", "tof" ).c_str() );
    ChainLoader::load( chain, cfg->getString( np+"input.dst:url" ).c_str(), cfg->getInt( np+"input.dst:maxFiles" ) );

    pico = new TofPicoDst( chain );

    // create the book
    lg->info(__FUNCTION__) << " Creating book "<< endl;
    book = new HistoBook( config->getString( np + "output.data" ), config );


    /**
     * Setup the ana cuts
     */
    cutVertexZ = new ConfigRange( cfg, np + "eventCuts.vertexZ", -200, 200 );
    cutVertexR = new ConfigRange( cfg, np + "eventCuts.vertexR", 0, 10 );
    cutVertexROffset = new ConfigPoint( cfg, np + "eventCuts.vertexROffset", 0.0, 0.0 );

    cutNTZero = new ConfigRange( cfg, np + "eventCuts.nTZero", 0, 10000 );
    cutTriggers = cfg->getIntVector( np + "eventCuts.triggers" );

}
/**
 * Destructor
 */
InclusiveSpectra::~InclusiveSpectra(){

	delete book;

	lg->info(__FUNCTION__) << endl;
	delete lg;
}


void InclusiveSpectra::refMultLoop() {

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

	book->fill( "ptAll", pt );
	if ( 1 == charge )
		book->fill( "ptPos", pt );
	else if ( -1 == charge  )
		book->fill( "ptNeg", pt );
}

bool InclusiveSpectra::eventCut(){

	vector<UInt_t> triggerIds = pico->eventTriggerIds();
	//440005, 440015
	bool findTrigger = false;
	for ( int iTrig = 0; iTrig < cutTriggers.size(); iTrig++ ){
		if ( triggerIds.end() != find( triggerIds.begin(), triggerIds.end(), (UInt_t)cutTriggers[ iTrig ] ) )
			findTrigger = true;
	}
	if ( !findTrigger )
		return false;

	double z = pico->eventVertexZ();
	double x = pico->eventVertexX() + cutVertexROffset->x;
	double y = pico->eventVertexY() + cutVertexROffset->y;
	double r = TMath::Sqrt( x*x + y*y );

	int nT0 = pico->eventNTZero();

	book->fill( "vertexZ", z);
	book->fill( "vertexR", r);
	
	if ( z < cutVertexZ->min || z > cutVertexZ->max )
		return false;

	book->fill( "vertexZ_zCut", z);
	book->fill( "vertexR_zCut", r);
	if ( r < cutVertexR->min || r > cutVertexR->max )
		return false;
	book->fill( "vertexZ_r_zCut", z);
	book->fill( "vertexR_r_zCut", r);

	if ( nT0 < cutNTZero->min || nT0 > cutNTZero->max )
		return false;

	return true;
}

bool InclusiveSpectra::trackCut( Int_t iTrack ){

	double eta = pico->trackEta( iTrack );

	if ( eta > .2 )
		return false;

	return true;
}






