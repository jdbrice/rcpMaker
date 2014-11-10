#include "HistoAnalyzer.h"


HistoAnalyzer::HistoAnalyzer( XmlConfig * config, string np ){
	

	//Set the Root Output Level
	gErrorIgnoreLevel = kSysError;

	// Save Class Members for config
	cfg = config;
	nodePath = np;
	
	// make the Logger
	logger = LoggerConfig::makeLogger( cfg, np + "Logger" );
	Logger::setGlobalLogLevel( logger->getLogLevel() );
	
	logger->setClassSpace( "HistoAnalyzer" );
	logger->info(__FUNCTION__) << "Got config with nodePath = " << np << endl;
	
    // create the book
    logger->info(__FUNCTION__) << " Creating book " << config->getString( np + "output.data" ) << endl;
    book = new HistoBook( config->getString( np + "output.data" ), config, "", "" );
    logger->info(__FUNCTION__) << " Creating report " << config->getString( np + "output.report" ) << endl;
    reporter = new Reporter( cfg, np+"Reporter." );

    logger->info(__FUNCTION__) << " Loading data from " << config->getString( np + "input.data:url" ) << endl;
	inFile = new TFile( cfg->getString( np+"input.data:url" ).c_str(), "READ" );

}

HistoAnalyzer::~HistoAnalyzer(){
	delete book;
	delete reporter;
	delete logger;
}
