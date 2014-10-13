
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
    lg->info(__FUNCTION__) << " Creating book "<< endl;

}
/**
 * Destructor
 */
InclusiveSpectra::~InclusiveSpectra(){

	delete book;

	lg->info(__FUNCTION__) << endl;
	delete lg;
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


	return true;
}

bool InclusiveSpectra::trackCut( Int_t iTrack ){


	return true;
}






