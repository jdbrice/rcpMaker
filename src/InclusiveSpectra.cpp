
#include "InclusiveSpectra.h"
#include "ChainLoader.h"

/**
 * Constructor
 */
InclusiveSpectra::InclusiveSpectra( XmlConfig * config, string np){

	// Save Class Members
	cfg = config;
	nodePath = np;

	// make the Logger
	lg = LoggerConfig::makeLogger( cfg, np + "Logger" );

	//Create the chain
	TChain * chain = new TChain( cfg->getChar(np+"input.dst:treeName", "tof" ) );
    ChainLoader::load( chain, cfg->getChar( np+"input.dst:url" ), cfg->getInt( np+"input.dst:maxFiles" ) );

    pico = new TofPicoDst( chain );


}
/**
 * Destructor
 */
InclusiveSpectra::~InclusiveSpectra(){

}

void InclusiveSpectra::eventLoop(){
	lg->info(__FUNCTION__) << endl;

	Int_t nEvents = pico->getTree()->GetEntriesFast();
	lg->info(__FUNCTION__) << "Reading " << nEvents << " events from chain" << endl;

	TaskPogress tp( "Event Loop", nEvents );

	for ( Int_t iEvent = 0; iEvent < nEvents; iEvent ++ ){

		tp.progess( iEvent );

	}

	lg->info(__FUNCTION__) << "event loop complete" << endl;

}









