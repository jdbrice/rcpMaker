#include "TreeAnalyzer.h"
#include "ChainLoader.h"

TreeAnalyzer::TreeAnalyzer( XmlConfig * config, string np, string fileList, string jobPrefix ){
	

	//Set the Root Output Level
	gErrorIgnoreLevel = kSysError;

	// Save Class Members for config
	cfg = config;
	nodePath = np;
	
	// make the Logger
	logger = LoggerConfig::makeLogger( cfg, np + "Logger" );
	Logger::setGlobalLogLevel( logger->getLogLevel() );
	logger->setClassSpace( "TreeAnalyzer" );
	logger->info(__FUNCTION__) << "Got config with nodePath = " << np << endl;
	
    // create the book
    logger->info(__FUNCTION__) << " Creating book " << config->getString( np + "output.data" ) << endl;
    book = new HistoBook( jobPrefix + config->getString( np + "output.data" ), config, "", "" );
    logger->info(__FUNCTION__) << " Creating report " << config->getString( np + "output.report" ) << endl;
    
    if ( "" == jobPrefix && cfg->nodeExists( np+"Reporter.output:url" ) ) 
	    reporter = new Reporter( cfg, np+"Reporter.", jobPrefix );

    chain = new TChain( cfg->getString( np+"input.dst:treeName" ).c_str() );
    if ( "" == fileList ){
    	logger->info(__FUNCTION__) << " Loading data from " << config->getString( np + "input.dst:url" ) << endl;
    	ChainLoader::load( chain, cfg->getString( np+"input.dst:url" ), cfg->getInt( np+"input.dst:maxFiles", -1 ) );
    } else {
    	logger->info(__FUNCTION__) << " Parallel Job From " << fileList << ", prefix : " << jobPrefix << endl;
    	ChainLoader::loadList( chain, fileList, cfg->getInt( np+"input.dst:maxFiles", -1 ) );
    }


}

TreeAnalyzer::~TreeAnalyzer(){
	delete book;
	if ( reporter )
	delete reporter;
	delete logger;
}

void TreeAnalyzer::make(){

	/**
	 * Run the pre event loop
	 */
	book->cd( "" );
	preEventLoop();

	TaskTimer t;
	t.start();

	if ( !chain ){
		logger->error(__FUNCTION__) << " ERROR: Invalid chain " << endl;
		return;
	}

	Int_t nEvents = (Int_t)chain->GetEntries();
	nEventsToProcess = cfg->getInt( nodePath+"input.dst:nEvents", nEvents );
	logger->info(__FUNCTION__) << "Loaded: " << nEventsToProcess << " events " << endl;
	
	// loop over all events
	for(Int_t i=0; i<nEvents; i++) {
    	chain->GetEntry(i);

    	if ( !keepEvent() )
    		continue;

    	analyzeEvent();
    	
    	
	} // end loop on events
	logger->info(__FUNCTION__) << "Completed in " << t.elapsed() << endl;

	/**
	 * Run the post event loop
	 */
	postEventLoop();
}

void TreeAnalyzer::preEventLoop(){

	// make some histos here
	if ( cfg->nodeExists( nodePath+"histograms" ) )
		book->makeAll( nodePath+"histograms" );

}

/**
 * Reads data from the chain - cuts events
 * @return true or false
 */
bool TreeAnalyzer::keepEvent() {


	return true;
}