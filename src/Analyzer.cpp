
#include "constants.h"
#include "Analyzer.h"

// provides my own string shortcuts etc.
using namespace jdb;


/**
 * Constructor - Initializes all of the pidHistogram parameters from the configuration file
 * @param chain       The chain object containing all data compatible with the TOFrPicoDST format
 * @param con         The xml configuration defining key aspects of the calibration
 *					such as number of tot bins to use, data location etc. See repo Readme
 *					for a sample configuration.
 */
Analyzer::Analyzer( TChain* chain, XmlConfig* con )  {
	cout << "[Analyzer.Analyzer] " << endl;
	
	gErrorIgnoreLevel=kError;

	config = con;

	// set the histogram info verbosity to show nothing
	gStyle->SetOptStat( 0 );
	
	// create the histogram book
	book = new HistoBook( ( config->getString( "output.base" ) + config->getString( "output.root" ) ), config );
	
	// create a report builder 
	report = new Reporter( config->getString( "output.base" ) + config->getString( "output.report" ) );

	_chain = chain;
}

/**
 *	Destructor - Deletes the histoBook ensuring it is saved.
 */
Analyzer::~Analyzer() {
	
	delete book;
	delete report;
	
	cout << "[Analyzer.~Analyzer] " << endl;
}



void Analyzer::loopEvents() {

	TaskTimer t;
	t.start();

	if ( !_chain ){
		cout << "[Analyzer." << __FUNCTION__ << "] ERROR: Invalid chain " << endl;
		return;
	}

	Int_t nEvents = (Int_t)_chain->GetEntries();
	cout << "[Analyzer." << __FUNCTION__ << "] Loaded: " << nEvents << " events " << endl;

	book->cd( "" );

	// make some histos here
	
	// loop over all events
	for(Int_t i=0; i<nEvents; i++) {
    	_chain->GetEntry(i);

    	
    	
	} // end loop on events

	


	cout << "[Analyzer." << __FUNCTION__ << "] completed in " << t.elapsed() << " seconds " << endl;
}
