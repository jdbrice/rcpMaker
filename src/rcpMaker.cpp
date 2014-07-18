
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
	book = new histoBook( ( config->getString( "output.base" ) + config->getString( "output.root" ) ), config );
	
	// create a report builder 
	report = new reporter( config->getString( "output.base" ) + config->getString( "output.report" ) );


	_chain = chain;
	pico = new TOFrPicoDst( _chain );


	vOffsetX = config->getDouble( "cuts.vertexOffset:x", 0);
	vOffsetY = config->getDouble( "cuts.vertexOffset:y", 0);
	vOffsetZ = config->getDouble( "cuts.vertexOffset:z", 0);
	vzMin = config->getDouble( "cuts.vz:min", -5);
	vzMax = config->getDouble( "cuts.vz:max", 5);
	vrMax = config->getDouble( "cuts.vrMax", 2);

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
	
	
	vector<double> avgP;
	vector<double> avgC;

	/**
	 *  Loop over all the events and build the refMult histogram
	 */
	for(Int_t i=0; i<nevents; i++) {
    	_chain->GetEntry(i);

    	if ( eventCut() )
    		continue;

    	double tStart = pico->tStart;
    	Int_t refMult = pico->refMult;
    	book->fill( "refMult", refMult );
   
	} // end loop on events for refMult

	

	refMultHelper *rmh = new refMultHelper();

	double cutCentral 	= rmh->findCut( config->getDouble( "cuts.central", 0.2 ), 	book->get("refMult" ) );
	double cutPeripheralMin = rmh->findCut( config->getDouble( "cuts.peripheral:min", 0.6 ), book->get("refMult" ) );
	double cutPeripheralMax = rmh->findCut( config->getDouble( "cuts.peripheral:max", 0.8 ), book->get("refMult" ) );

	

	book->make( config, "histograms.ptAll" );
	book->make( config, "histograms.ptCentral" );
	book->make( config, "histograms.ptPeripheral" );
	book->make( config, "histograms.ptPeripheralScaled" );
	book->make( config, "histograms.ptRatio" );

	book->make( "histograms.vz" );
	book->make( "histograms.vr" );
	book->make( "histograms.vxy" );
	book->make( "histograms.vzOffset" );
	book->make( "histograms.vrOffset" );
	book->make( "histograms.vxyOffset" );

	book->get( "ptRatio" )->Sumw2();
	
	
	// loop over all events
	for(Int_t i=0; i<nevents; i++) {
    	_chain->GetEntry(i);

    	double tStart = pico->tStart;
    	Int_t refMult = pico->refMult;

    	double vz = pico->vertexZ;
    	double vx = pico->vertexX;
    	double vy = pico->vertexY;
    	double vr = TMath::Sqrt( vx*vx + vy*vy );

    	book->fill( "vxy", vx, vy );
    	book->fill( "vr", vr );
    	book->fill( "vz", vz );

    	vz = pico->vertexZ - vOffsetZ;
    	vx = pico->vertexX - vOffsetX;
    	vy = pico->vertexY - vOffsetY;
    	vr = TMath::Sqrt( vx*vx + vy*vy );

    	book->fill( "vxyOffset", vx, vy );
    	book->fill( "vrOffset", vr );
    	book->fill( "vzOffset", vz );

    	if ( eventCut() )
    		continue;


    	double central = rmh->findCentrality( refMult, book->get( "refMult" ) );

    	//cout << "centrality : " << refMult << " --> " << rmh->findCentrality( refMult, book->get( "refMult" ) ) << endl;
    	
		if ( refMult >= cutCentral ){
			book->fill( "central", refMult );
			avgC.push_back( central );
		}
		if ( refMult >= cutPeripheralMax && refMult <= cutPeripheralMin ){
			book->fill( "peripheral", refMult );
			avgP.push_back( central );
		}


    	int nTofHits = pico->nTofHits;
    	for ( int iHit = 0; iHit < nTofHits; iHit++ ){

    		double pt = pico->pt[ iHit ];

    		book->fill( "ptAll", pt );
    		if ( refMult >= cutCentral ){
    			book->fill( "ptCentral", pt );
    			book->fill( "ptRatio", pt   );
    			
    		}
    		if ( refMult >= cutPeripheralMax && refMult <= cutPeripheralMin  ){
    			book->fill( "ptPeripheral", pt );
    			book->fill( "ptPeripheralScaled", pt );
 
    		}

    	}
    	

		
	} // end loop on events

	delete rmh;


	double totalP = 0;
	double totalC = 0;
	for ( int i = 0; i < avgP.size(); i++ )
		totalP += avgP[ i ];
	for ( int i = 0; i < avgC.size(); i++ )
		totalC += avgC[ i ];
	
	cout << " Average Peripheral : " << ( totalP / avgP.size() ) << endl;
	cout << " Average Central : " << ( totalC / avgC.size() ) << endl;

	report->newPage();
	gPad->SetLogy();
	book->style( "refMult" )->set( "draw", "h" )->set( "range", 0.1, 10000 )->draw();
	book->style( "central" )->set( "draw", "same" )->set("linecolor", kRed)->set( "range", 0.1, 10000 )->draw();
	book->style( "peripheral" )->set( "draw", "same" )->set( "linecolor", kGreen )->set( "range", 0.1, 10000 )->draw();
	report->savePage();

	report->newPage();
	book->style( "vz" )->set( "draw", "h" )->draw();
	book->style( "vzOffset" )->set( "draw", "sameh" )->set( "linecolor", 2 )->draw();
	report->savePage();

	report->newPage();
	book->style( "vr" )->set( "draw", "h" )->draw();
	book->style( "vrOffset" )->set( "draw", "sameh" )->set( "linecolor", 2 )->draw();
	report->savePage();

	report->newPage();
	book->style( "vxy" )->set( "s.2D" )->draw();
	report->savePage();

	report->newPage();
	book->style( "vxyOffset" )->set( "s.2D" )->draw();
	report->savePage();

	report->newPage();
	gPad->SetLogy( 1 );
	book->style( "ptAll" )->set( "draw", "h" )->draw();
	book->style( "ptCentral" )->set( "draw", "same" )->set("linecolor", kRed)->draw();
	book->style( "ptPeripheral" )->set( "draw", "same" )->set( "linecolor", kGreen )->draw();
	report->savePage();

	/*report->newPage();
	book->style( "ptPeripheralScaled" )->set( "draw", "pe" )->set( "linecolor", kBlack )->draw();
	book->style( "ptRatio" )->set( "draw", "samepe" )->set( "linecolor", kRed )->draw();
	report->savePage();*/

	double cScale = 196.96657 * ( 1 - ( totalC / avgC.size() ) );
	double pScale = 196.96657 * ( 1 - ( totalP / avgP.size() ) ); 
	book->get( "ptPeripheralScaled" )->Scale( cScale );
	book->get( "ptRatio" )->Scale( pScale );
	book->get( "ptRatio" )->Divide( book->get( "ptPeripheralScaled" ) );

	report->newPage();
	book->style( "ptRatio" )->set( "s.rcp" )->draw();
	report->savePage();


	cout << "[rcpMaker." << __FUNCTION__ << "] completed in " << elapsed() << " seconds " << endl;
}



bool rcpMaker::eventCut() {

	double vz = pico->vertexZ;
	if ( vz - vOffsetZ > vzMax || vz - vOffsetZ < vzMin )
    		return true;

    double vx = pico->vertexX - vOffsetX;
	double vy = pico->vertexY - vOffsetY;
	double vr = TMath::Sqrt( vx*vx + vy*vy );

	if ( vr > vrMax )
		return true;


    return false;
}
