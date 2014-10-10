
#include "InclusiveSpectra.h"


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
	TChain * chain = new TChain( config->getString(np+"input.chain:name", "tof" ).c_str() );
    ChainLoader::load( chain, config.getString( "input.dataDir" ).c_str(), config.getInt( "input.dataDir:maxFiles" ) );

}
/**
 * Destructor
 */
InclusiveSpectra::~InclusiveSpectra(){

}

void InclusiveSpectra::LoopTree(){

}