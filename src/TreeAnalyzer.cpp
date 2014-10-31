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
	logger->setClassSpace( "TreeAnalyzer" );
	logger->info(__FUNCTION__) << "Got config with nodePath = " << np << endl;
	
    // create the book
    logger->info(__FUNCTION__) << " Creating book " << config->getString( np + "output.data" ) << endl;
    book = new HistoBook( config->getString( np + "output.data" ), config, "", "", logger->getLogLevel() );
    logger->info(__FUNCTION__) << " Creating report " << config->getString( np + "output.report" ) << endl;
    reporter = new Reporter( cfg, np+"Reporter.", jobPrefix );

    
    if ( "" == fileList ){
    	logger->info(__FUNCTION__) << " Loading data from " << config->getString( np + "input.tree:url" ) << endl;
    	ChainLoader::load( chain, cfg->getString( np+"input.tree:url" ), cfg->getInt( np+"input.tree:maxFiles", -1 ) );
    } else {
    	logger->info(__FUNCTION__) << " Parallel Job From " << fileList << ", prefix : " << jobPrefix << endl;
    	ChainLoader::loadList( chain, fileList, cfg->getInt( np+"input.tree:maxFiles", -1 ) );
    }


}

TreeAnalyzer::~TreeAnalyzer(){
	delete book;
	delete reporter;
	delete logger;
}

void TreeAnalyzer::make(){

	TaskTimer t;
	t.start();

	if ( !chain ){
		logger->error(__FUNCTION__) << " ERROR: Invalid chain " << endl;
		return;
	}

	Int_t nEvents = (Int_t)chain->GetEntries();
	nEventsToProcess = cfg->getInt( nodePath+"input.dst:nEvents", nEvents );
	logger->info(__FUNCTION__) << "Loaded: " << nEventsToProcess << " events " << endl;

	book->cd( "" );
	
	// loop over all events
	for(Int_t i=0; i<nEvents; i++) {
    	chain->GetEntry(i);

    	analyzeEvent();
    	
    	
	} // end loop on events

	logger->info(__FUNCTION__) << "Completed in " << t.elapsed() << endl;
}

void TreeAnalyzer::preEventLoop(){

	// make some histos here
	if ( cfg->nodeExists( nodePath+"histograms" ) )
		book->makeAll( nodePath+"histograms" );

}
