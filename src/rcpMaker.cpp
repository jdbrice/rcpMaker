
#include "constants.h"
#include "rcpMaker.h"
#include "histoBook.h"
#include <fstream>
#include <sstream>
#include "refMultHelper.h"
#include "pidLUTHelper.h"

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



	pidLUT = new histoBook( config->getString( "output.pidLUT" ), config );
	TFile * f = new TFile( config->getString( "input.pidLUT" ).c_str(), "READ" );

	TH1* h = (TH1*)f->Get( "xMeanPi" );
	pidLUT->add( "xMeanPi", h );
	h = (TH1*)f->Get( "yMeanPi" );
	pidLUT->add( "yMeanPi", h );
	h = (TH1*)f->Get( "xSigmaPi" );
	pidLUT->add( "xSigmaPi", h );
	h = (TH1*)f->Get( "ySigmaPi" );
	pidLUT->add( "ySigmaPi", h );

	h = (TH1*)f->Get( "xMeanK" );
	pidLUT->add( "xMeanK", h );
	h = (TH1*)f->Get( "yMeanK" );
	pidLUT->add( "yMeanK", h );
	h = (TH1*)f->Get( "xSigmaK" );
	pidLUT->add( "xSigmaK", h );
	h = (TH1*)f->Get( "ySigmaK" );
	pidLUT->add( "ySigmaK", h );

}

/**
 *	Destructor - Deletes the histoBook ensuring it is saved.
 */
rcpMaker::~rcpMaker() {
	
	delete book;
	//delete pidLUT;
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
	book->makeAll( "histograms" );

	
	vector<double> avgP;
	vector<double> avgC;

	/**
	 *  Loop over all the events and build the refMult histogram
	 */
	cout << " Determining RefMult" << endl;
	uint totalEventsKept = 0;
	for(Int_t i=0; i<nevents; i++) {
    	_chain->GetEntry(i);
    	progressBar( i, nevents, 60 );

    	if ( keepEvent() )
    		continue;
    	totalEventsKept ++ ;

    	double tStart = pico->tStart;
    	Int_t refMult = pico->refMult;
    	book->fill( "refMult", refMult );
   
	} // end loop on events for refMult

	cout << "Total Events Kept: " << totalEventsKept << endl;
	cout << "refMult Histo : " << book->exists( "refMult" ) << endl;

	refMultHelper *rmh = new refMultHelper();

	double cutCentral 	= rmh->findCut( config->getDouble( "cuts.central", 0.2 ), 	book->get("refMult" ) );
	double cutPeripheralMin = rmh->findCut( config->getDouble( "cuts.peripheral:min", 0.6 ), book->get("refMult" ) );
	double cutPeripheralMax = rmh->findCut( config->getDouble( "cuts.peripheral:max", 0.8 ), book->get("refMult" ) );


	book->get( "ptRatio" )->Sumw2();
	
	vector<string> pTypes = config->getStringVector( "pid.species" );
	pidLUTHelper pidHelper( pidLUT );

	double cutNSigDedx = config->getDouble( "pid.nSigma:dedx", 1 );
	double cutNSigDB = config->getDouble( "pid.nSigma:deltaBeta", 1 );
	
	// loop over all events
	for(Int_t i=0; i<nevents; i++) {
    	_chain->GetEntry(i);

    	progressBar( i, nevents, 60 );

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

    	
		book->fill( "vzCut", vz );    	

		
		book->fill( "vrCut", vr );
		book->fill( "vxyCut", vx, vy );    	    	

		if ( keepEvent() )
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

    		if ( !keepTrack( iHit ) )
    			continue;

    		double pt = pico->pt[ iHit ];


    		
    		// loop over the particle types
    		for ( int iP = 0; iP < pTypes.size(); iP++ ){
    			string pType = pTypes[ iP ];
    			// nSigmaDeDx
    			double nDedx = nSigDedx( pType, iHit );
    			// delta 1/beta
    			double db = dBeta( pType, iHit );
    			// momentum
    			double p = pico->p[ iHit ];

    			
 	   			bool passPID = pidHelper.isParticle( pType, p, 0, 
 	   							nDedx, db, cutNSigDedx, cutNSigDB );

 	   			if ( passPID  ){
 	   				book->fill( "ptAll"+pType, pt );
 	   				if ( refMult >= cutCentral ){
		    			book->fill( "ptCentral" + pType, pt );
		    			book->fill( "ptRatio" + pType, pt   );
		    			
		    		}
		    		if ( refMult >= cutPeripheralMax && refMult <= cutPeripheralMin  ){
		    			book->fill( "ptPeripheral" + pType, pt );
		    			book->fill( "ptPeripheralScaled" + pType, pt );
		 
		    		}
 	   			}
    			

    		}

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
	book->style( "refMult" )->set( "s.refMult" )->draw();
	book->style( "central" )->set( "s.peripheral" )->draw();
	book->style( "peripheral" )->set( "s.central" )->draw();
	report->savePage();

	report->newPage(2, 2);
	book->clearLegend();
	book->style( "vzOffset" )->set( "s.log1D" )->draw();
	//book->style( "vzCut" )->set( "s.cut" )->draw();

	book->clearLegend();
	report->cd( 2, 1 );
	book->style( "vrOffset" )->set( "draw", "h" )->set( "logY", 1)->draw();
	//book->style( "vrCut" )->set( "s.cut" )->draw();

	report->cd( 1, 2 );
	book->style( "vxyOffset" )->set( "s.2D" )->draw();

	report->cd( 2, 2 );
	book->style( "vxyCut" )->set( "s.2D" )->draw();
	report->savePage();

	report->newPage();
	gPad->SetLogy( 1 );
	book->clearLegend();
	book->style( "ptAll" )->draw();
	book->style( "ptCentral" )->draw();
	book->style( "ptPeripheral" )->draw();
	report->savePage();

	/*report->newPage();
	book->style( "ptPeripheralScaled" )->set( "draw", "pe" )->set( "linecolor", kBlack )->draw();
	book->style( "ptRatio" )->set( "draw", "samepe" )->set( "linecolor", kRed )->draw();
	report->savePage();*/

	double cScale = 196.96657 * ( 1 - ( totalC / avgC.size() ) );
	double pScale = 196.96657 * ( 1 - ( totalP / avgP.size() ) ); 

	cout << " cScale : " << cScale << endl;
	cout << " pScale : " << pScale << endl;

	book->get( "ptPeripheralScaled" )->Scale( 790 );
	book->get( "ptRatio" )->Scale( 19 );
	book->get( "ptRatio" )->Divide( book->get( "ptPeripheralScaled" ) );

	report->newPage();
	book->style( "ptRatio" )->set( "s.rcp" )->draw();
	report->savePage();

	for ( int iP = 0; iP < pTypes.size(); iP++ ){

		string pType = pTypes[ iP ];
		book->get( "ptPeripheralScaled"+pType )->Scale( 790 );
		book->get( "ptRatio"+pType )->Scale( 19 );
		book->get( "ptRatio"+pType )->Divide( book->get( "ptPeripheralScaled"+pType ) );

		report->newPage();
		book->style( "ptRatio"+pType )->set("title", "R_{cp} " + pType )->draw();
		report->savePage();

	}

	


	cout << "[rcpMaker." << __FUNCTION__ << "] completed in " << elapsed() << " seconds " << endl;
}


bool rcpMaker::keepEvent() {

	double vz = pico->vertexZ;
	double vx = pico->vertexX;
	double vy = pico->vertexY;


	

	double vr = TMath::Sqrt( vx*vx + vy*vy );

	double vrOff = TMath::Sqrt( (vx - vOffsetX)*(vx - vOffsetX) + (vy - vOffsetY)*(vy - vOffsetY) );

	/*
	 * Vertex cuts
	 */
	if ( TMath::Abs( vz ) > config->getDouble( "cut.vZ", 30 ) )
		return false;
	if (  vrOff > config->getDouble( "cut.vR", 10 ) )
		return false;

	/**
	 * Number of hits cuts
	 */
	int nT0 = pico->nTZero;
	int nTofHits = pico->nTofHits;

	if ( nT0 < config->getDouble( "cut.nT0", 0 ) )
		return false;
	if ( nTofHits < config->getDouble( "cut.nTof", 0 ) )
		return false;


	return true;
}

bool rcpMaker::keepTrack( int iHit ) {

	double xDCA = pico->dcaZ[ iHit ];
	double yDCA = pico->dcaX[ iHit ];
	double zDCA = pico->dcaY[ iHit ];

	double dca = TMath::Sqrt( xDCA*xDCA + yDCA*yDCA + zDCA*zDCA );

	/*
	 * Distance to closest approach cut
	 */
	book->fill( "dcaMag", dca );
	if ( dca > 1 )
		return false;

	if ( pico->nHits[iHit] <= 15 )
		return false;
	if ( pico->nHitsDedx[ iHit ] <= 10 )
		return false;
	if ( pico->nHitsFit[ iHit ] / pico->nHitsPossible[ iHit ] <= 0.52 )
		return false;

	if ( TMath::Abs( pico->eta[ iHit ] ) > 0.5  )
		return false;

	return true;

}


double rcpMaker::dBeta( string pType, int iHit ){

	double tof = pico->tof[ iHit ];
	double length = pico->length[ iHit ];
	double p = pico->p[ iHit ];
	double beta = pico->beta[ iHit ];
	double m2 = p*p * ( constants::c*constants::c * tof*tof / ( length*length ) - 1  );


	double deltaB = 1 - (beta) * TMath::Sqrt( (constants::kaonMass*constants::kaonMass) / (p*p) + 1 );

	if ( "Pi" == pType )
		deltaB = 1 - (beta) * TMath::Sqrt( (constants::piMass*constants::piMass) / (p*p) + 1 );		
	if ( "P" == pType )
		deltaB = 1 - (beta) * TMath::Sqrt( (constants::protonMass*constants::protonMass) / (p*p) + 1 );		
	
	return deltaB;
}

double rcpMaker::nSigInvBeta( string pType, int iHit  ){

	double b = pico->beta[ iHit ];
	double p = pico->p[ iHit ];
	double expectedBeta = eBeta( eMass( pType ), p );
	double invBetaSig = config->getDouble( "binning.invBetaSig" );

	double deltaInvBeta = ( 1.0 / b ) - ( 1.0 / expectedBeta );

	return (deltaInvBeta / invBetaSig);
}
