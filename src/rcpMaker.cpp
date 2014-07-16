
#include "constants.h"
#include "rcpMaker.h"
#include "histoBook.h"
#include <fstream>
#include <sstream>
#include "refMultHelper.h"

// provides my own string shortcuts etc.
using namespace jdbUtils;


/**
 * Constructor - Initializes all of the pidHistogram parameters from the configuration file
 * @param chain       The chain object containing all data compatible with the TOFrPicoDST format
 * @param con         The xml configuration defining key aspects of the calibration
 *					such as number of tot bins to use, data location etc. See repo Readme
 *					for a sample configuration.
 */
rcpMaker::rcpMaker( TChain* chain, xmlConfig* con )  {
	cout << "[rcpMaker.rcpMaker] " << endl;
	
	gErrorIgnoreLevel=kError;

	config = con;


	// set the histogram info verbosity to show nothing
	gStyle->SetOptStat( 0 );
	
	// create the histogram book
	book = new histoBook( ( config->getString( "output.base" ) + config->getString( "output.root" ) ) );
	
	// create a report builder 
	report = new reporter( config->getString( "output.base" ) + config->getString( "output.report" ) );


	_chain = chain;
	pico = new TOFrPicoDst( _chain );
}

/**
 *	Destructor - Deletes the histoBook ensuring it is saved.
 */
rcpMaker::~rcpMaker() {
	
	delete book;
	delete report;
	
	cout << "[rcpMaker.~rcpMaker] " << endl;
}


/**
 *	Offsets
 *	Calculates the initial offsets for each channel with respect to channel 1 on the west side.
 *	Then performs the offset calculation again after all corrections have been applied
 */
void rcpMaker::loopEvents() {

	startTimer();

	if ( !_chain ){
		cout << "[rcpMaker." << __FUNCTION__ << "] ERROR: Invalid chain " << endl;
		return;
	}

	Int_t nevents = (Int_t)_chain->GetEntries();
	cout << "[rcpMaker." << __FUNCTION__ << "] Loaded: " << nevents << " events " << endl;



	book->cd( "" );
	book->make( config, "histograms.refMult" );
	book->make( config, "histograms.central" );
	book->make( config, "histograms.peripheral" );
	
	/**
	 *  Loop over all the events and build the refMult histogram
	 */
	for(Int_t i=0; i<nevents; i++) {
    	_chain->GetEntry(i);

    	double tStart = pico->tStart;
    	Int_t refMult = pico->refMult;
    	book->fill( "refMult", refMult );
   
	} // end loop on events for refMult

	

	refMultHelper *rmh = new refMultHelper();

	double cutCentral 	= rmh->findCut( config->getDouble( "cuts.central", 0.2 ), 	book->get("refMult" ) );
	double cutPeripheral = rmh->findCut( config->getDouble( "cuts.peripheral", 0.8 ), book->get("refMult" ) );

	

	book->make( config, "histograms.ptAll" );
	book->make( config, "histograms.ptCentral" );
	book->make( config, "histograms.ptPeripheral" );
	book->make( config, "histograms.ptPeripheralScaled" );
	book->make( config, "histograms.ptRatio" );

	
	
	// loop over all events
	for(Int_t i=0; i<nevents; i++) {
    	_chain->GetEntry(i);

    	double tStart = pico->tStart;
    	Int_t refMult = pico->refMult;

    	double central = rmh->findCentrality( refMult, book->get( "refMult" ) );
    	if ( refMult < 5 )
    		continue;
    	//cout << "centrality : " << refMult << " --> " << rmh->findCentrality( refMult, book->get( "refMult" ) ) << endl;
    	
		if ( refMult >= cutCentral ){
			book->fill( "central", refMult );
		}
		if ( refMult <= cutPeripheral ){
			book->fill( "peripheral", refMult );
		}

    	int nTofHits = pico->nTofHits;
    	for ( int iHit = 0; iHit < nTofHits; iHit++ ){

    		double pt = pico->pt[ iHit ];

    		book->fill( "ptAll", pt );
    		if ( refMult >= cutCentral ){
    			book->fill( "ptCentral", pt );
    			book->fill( "ptRatio", pt );
    		}
    		if ( refMult <= cutPeripheral ){
    			book->fill( "ptPeripheral", pt );
    			book->fill( "ptPeripheralScaled", pt );
 
    		}

    	}
    	

		
	} // end loop on events

	delete rmh;

	report->newPage();
	gPad->SetLogy();
	book->style( "refMult" )->set( "draw", "h" )->set( "range", 0.1, 10000 )->draw();
	book->style( "central" )->set( "draw", "same" )->set("linecolor", kRed)->set( "range", 0.1, 10000 )->draw();
	book->style( "peripheral" )->set( "draw", "same" )->set( "linecolor", kGreen )->set( "range", 0.1, 10000 )->draw();
	report->savePage();

	report->newPage();
	gPad->SetLogy( 1 );
	book->style( "ptAll" )->set( "draw", "h" )->draw();
	book->style( "ptCentral" )->set( "draw", "same" )->set("linecolor", kRed)->draw();
	book->style( "ptPeripheral" )->set( "draw", "same" )->set( "linecolor", kGreen )->draw();
	report->savePage();

	report->newPage();
	book->style( "ptPeripheralScaled" )->set( "draw", "pe" )->set( "linecolor", kBlack )->draw();
	report->savePage();

	book->get( "ptRatio" )->Divide( book->get( "ptPeripheralScaled" ) );

	report->newPage();
	//gPad->SetLogy( 1 );
	book->style( "ptRatio" )->set( "draw", "pe" )->draw();
	//book->style( "ptCentral" )->set( "draw", "same" )->set("linecolor", kRed)->draw();
	//book->style( "ptPeripheral" )->set( "draw", "same" )->set( "linecolor", kGreen )->draw();
	report->savePage();


	cout << "[rcpMaker." << __FUNCTION__ << "] completed in " << elapsed() << " seconds " << endl;
}
