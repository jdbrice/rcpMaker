
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
    ChainLoader::load( chain, cfg->getChar( np+"input.dst:url" ), config.getInt( np+"input.dst:maxFiles" ) );




}
/**
 * Destructor
 */
InclusiveSpectra::~InclusiveSpectra(){

}

void InclusiveSpectra::eventLoop(){

}