#include "Spectra/InclusiveSpectra.h"
#include "ChainLoader.h"
#include "Adapter/ProdPicoDst.h"
#include "Adapter/RcpPicoDst.h"

#include <limits.h>
#include "Spectra/PidHistoMaker.h"

void InclusiveSpectra::initialize(  ){
	DEBUG( classname(), "" )

	if ( !chain ){
		ERROR( classname(), "FATAL : Invalid Chain" );
		return;
	}

	/**
	 * Make the desired PicoDataStore Interface
	 */
	if ( ds && ds->getTreeName() == "PicoDst" ){
		INFO( "Using Prod PicoDst from DataStore" );
		pico = unique_ptr<PicoDataStore>( new ProdPicoDst( ds->getChain() ) );
	} else if ( chain && "PicoDst" == treeName ){
		INFO( "Using ProdPico" );
		pico = unique_ptr<PicoDataStore>( new ProdPicoDst( chain ) );
	} else if ( chain && ("rcpPicoDst" == treeName || "SpectraPicoDst" == treeName) ){
		INFO( "Using Rcp Pico" );
		pico = unique_ptr<PicoDataStore>( new RcpPicoDst( chain ) );
	}

	INFO( "Got a valid Data Adapter" );


    // Tracks cuts
    cut_nHitsFit				= unique_ptr<XmlRange>(new XmlRange( &config, "TrackCuts.nHitsFit", 				0, 		INT_MAX ) );
    cut_dca 					= unique_ptr<XmlRange>(new XmlRange( &config, "TrackCuts.dca", 					0, 		INT_MAX ) );
	cut_nHitsFitOverPossible 	= unique_ptr<XmlRange>(new XmlRange( &config, "TrackCuts.nHitsFitOverPossible", 	0, 		INT_MAX ) );
    cut_nHitsDedx 				= unique_ptr<XmlRange>(new XmlRange( &config, "TrackCuts.nHitsDedx", 				0, 		INT_MAX ) );
    cut_pt 						= unique_ptr<XmlRange>(new XmlRange( &config, "TrackCuts.pt", 					0, 		INT_MAX ) );
    cut_ptGlobalOverPrimary 	= unique_ptr<XmlRange>(new XmlRange( &config, "TrackCuts.ptGlobalOverPrimary", 	0.7, 	1.42 ) );
    cut_yLocal 					= unique_ptr<XmlRange>(new XmlRange( &config, "TrackCuts.yLocal", 				-1.6, 	1.6 ) );
    cut_zLocal 					= unique_ptr<XmlRange>(new XmlRange( &config, "TrackCuts.zLocal", 				-2.8, 	2.8 ) );
    cut_matchFlag				= unique_ptr<XmlRange>(new XmlRange( &config, "TrackCuts.matchFlag", 				1, 		3 ) );
    cut_rapidity				= unique_ptr<XmlRange>(new XmlRange( &config, "TrackCuts.rapidity", 				-0.25, 	0.25 ) );


   	// Setup the centrality bins
   	INFO( classname(), "Loading Centrality Map" );
    centralityBinMap = config.getIntMap( nodePath + ".CentralityMap" );
    centralityBins = config.getIntVector( nodePath + ".CentralityBins" );
    INFO( classname(), "c[ 0 ] = " << centralityBinMap[ 0 ] );
    
    // make the inclusive spectra
    makeSpectra 	= config.getBool( nodePath + ".Spectra:all", true );
    INFO( classname(), "Make Inclusive Spectra : " << makeSpectra );
    
    // make the inclusive tof spectra
    makeTofSpectra 	= config.getBool( nodePath + ".Spectra:tof", true );
    INFO( classname(), "Make Inclusive Tof Spectra : " << makeTofSpectra );
    

    // Setup the options
	makeTrackQA = config.getBool( nodePath + ".MakeQA:track", false );
	if( makeTrackQA )
	    INFO( classname(), "Making Track QA" );

	makeEventQA = config.getBool( nodePath + ".MakeQA:event", false );
	if( makeEventQA )
	    INFO( classname(), "Making Event QA" );



	plc = config.getString( nodePath + ".input:plc", "" );
	mass = Common::mass( plc );
	if ( mass < 0 ){
		ERROR( classname(), "Invalid Particle Species " << plc );
	}

	book->cd();

}

/* Destructor
 *
 */
InclusiveSpectra::~InclusiveSpectra(){
	
}


void InclusiveSpectra::makeCentralityHistos() {
	
	/**
	 * Make centrality ptHistos
	 */
	book->cd();
	


	INFO( classname(), "Make Inclusive Spectra : " << makeSpectra );
	INFO( classname(), "Make Inclusive Tof Spectra : " << makeTofSpectra );

	for ( int iC : centralityBins ){
		
		if ( makeSpectra ){
			string hName = "pt_" + ts(iC);
			INFO ( classname(), hName );
			book->cd( "inclusive" );
			book->clone( "", "pt", "inclusive", hName + "_p" );
			book->clone( "", "pt", "inclusive", hName + "_n" );	
		}
		
		if ( makeTofSpectra ){
			string hName = "pt_" + ts(iC);
			INFO ( classname(), hName );
			book->cd( "inclusiveTof" );
			book->clone( "", "pt", "inclusiveTof", hName + "_p" );
			book->clone( "", "pt", "inclusiveTof", hName + "_n" );
		}
		
	}
	for ( int iB = 0; iB < 11; iB++ ){
		if( centralityBinMap.find( iB ) != centralityBinMap.end() ){
			INFO( classname(), "[" << iB << "] = " << centralityBinMap[ iB ] );
		}
	}
}


void InclusiveSpectra::preEventLoop(){
	
	TreeAnalyzer::preEventLoop();

	book->cd();
	makeCentralityHistos();

	if ( makeEventQA ){
		book->cd( "EventQA" );
		INFO( classname(), "Making Event QA histograms " );
		book->makeAll( config, "QAHistograms.Event" );
		book->setBin( "mass", 1, mass, 0 );
	}
	if ( makeTrackQA ){
		book->cd( "TrackQA" );
		INFO( classname(), "Making track QA histograms " );
		book->makeAll( config, "QAHistograms.Track" );
	}




}

void InclusiveSpectra::postEventLoop(){

}


void InclusiveSpectra::analyzeEvent(){

	// Number of tracks
	Int_t nTracks = pico->numTracks();

	for ( Int_t iTrack = 0; iTrack < nTracks; iTrack ++ ){

		if ( !keepTrack( iTrack ) )
			continue;
		
		analyzeTrack( iTrack );	

		if ( !keepTofTrack( iTrack ) )
			continue;
		
		analyzeTofTrack( iTrack );			
	}
}


void InclusiveSpectra::analyzeTrack( Int_t iTrack ){

	if ( !makeSpectra )
		return;

	double pt = pico->trackPt( iTrack );
	int charge = pico->trackCharge( iTrack );
	 
	book->cd( "inclusive" );
	//book->fill( "pt", pt, eventWeight );
	//book->fill( "pt_" + Common::chargeString( charge ) , pt, eventWeight );
	
	if ( cBin >= 0 ){
		string cName = "pt_" + ts( cBin ) + "_" + Common::chargeString( charge );
		book->fill( cName, pt, eventWeight );
	}
	book->cd();
	
}

void InclusiveSpectra::analyzeTofTrack( Int_t iTrack ){

	if ( !makeTofSpectra )
		return;

	double pt = pico->trackPt( iTrack );
	int charge = pico->trackCharge( iTrack );
	 
	book->cd( "inclusiveTof" );
	// book->fill( "pt", pt, eventWeight );
	// book->fill( "pt_" + Common::chargeString( charge ), pt, eventWeight );
	
	if ( cBin >= 0 ){
		string cName = "pt_" + ts( cBin ) + "_" + Common::chargeString( charge );
		book->fill( cName, pt, eventWeight );		
	}
	book->cd( );
	
}


bool InclusiveSpectra::keepEvent(){

	if ( "rcpPicoDst" == treeName || "SpectraPicoDst" == treeName ){

		if ( isRunBad( pico->runId() ) ){
			WARN( "Run " << pico->runId() << " Rejected as bad" )
			return false;
		}

		eventWeight = pico->eventWeight();
		refMult 	= pico->refMult();
		cBin 		= centralityBin();	// gets the mapped centrality bin

		// Report event counts for normalization
		if ( makeEventQA ){
			book->cd( "EventQA" );
			
			book->fill( "cuts", 1 );
			book->fill( "corrRefMult", 			refMult, 		eventWeight );
			book->fill( "mappedRefMultBins", 	cBin, 			eventWeight );
			book->fill( "refMultBin9", 			pico->b9(), 	eventWeight );
			book->fill( "refMultBin16", 		pico->b16(), 	eventWeight );

			
		}

		if ( cBin < 0 )
			return false;

		return true;
	}

	ERROR( "RcpPicoDsts are required" )
	return false;
}

bool InclusiveSpectra::keepTrack( Int_t iTrack ){

	if ( makeTrackQA ){
		book->cd( "TrackQA" );
		book->get( "cuts" )->Fill( "All", 1 );
	}
	
	
	// alias
	double 	dca 			= pico->trackDca( iTrack );
	int 	nHitsDedx 		= pico->trackNHitsDedx( iTrack );
	int 	nHitsFit 		= pico->trackNHitsFit( iTrack );
	int 	nHitsPossible 	= pico->trackNHitsPossible( iTrack );
	double 	fitRatio 		= ( (double)nHitsFit /  (double)nHitsPossible);
	double 	ptPrimary 		= pico->trackPt( iTrack );
	double 	ptGlobal 		= pico->globalPt( iTrack );
	double 	eta 			= pico->trackEta( iTrack );
	double 	rapidity 		= Common::rapidity( ptPrimary, eta, mass );
	

	/**
	 * Pre Track Cut QA
	 */
	if ( makeTrackQA ){

		book->fill( "pre_dca", 					dca );
		book->fill( "pre_nHitsDedx", 			nHitsDedx );
		book->fill( "pre_nHitsFit", 			nHitsFit );
		book->fill( "pre_nHitsPossible", 		nHitsPossible );
		book->fill( "pre_nHitsFitOverPossible", fitRatio );
		book->fill( "pre_nHitsFitVsPossible", 	nHitsPossible, nHitsFit );
		book->fill( "pre_ptPrimary", 			ptPrimary );
		book->fill( "pre_ptGlobal", 			ptGlobal );
		book->fill( "pre_ptGlobalOverPrimary", 	ptGlobal / ptPrimary );
		book->fill( "pre_ptGlobalVsPrimary", 	ptPrimary, ptGlobal );
		book->fill( "pre_eta", 					eta );
		book->fill( "pre_rapidity", 			rapidity );
		

	}
	


	if ( ptPrimary < cut_pt->min || ptPrimary > cut_pt->max )
		return false;
	if ( makeTrackQA )
		book->get( "cuts" )->Fill( "ptPrimary", 1 );
	
	if ( (ptGlobal / ptPrimary) < cut_ptGlobalOverPrimary->min || (ptGlobal / ptPrimary) > cut_ptGlobalOverPrimary->max )
		return false;
	if ( makeTrackQA )
		book->get( "cuts" )->Fill( "ptRatio", 1 );
	
	if ( dca < cut_dca->min || dca > cut_dca->max )
		return false;
	if ( makeTrackQA )
		book->get( "cuts" )->Fill( "dca", 1 );
	
	if ( nHitsDedx < cut_nHitsDedx->min || nHitsDedx > cut_nHitsDedx->max )
		return false;
	if ( makeTrackQA )
		book->get( "cuts" )->Fill( "nHitsDedx", 1 );
	
	if ( nHitsFit < cut_nHitsFit->min || nHitsFit > cut_nHitsFit->max )
		return false;
	if ( makeTrackQA )
		book->get( "cuts" )->Fill( "nHitsFit", 1 );
	
	if ( fitRatio < cut_nHitsFitOverPossible->min || fitRatio > cut_nHitsFitOverPossible->max )
		return false; 
	if ( makeTrackQA )
		book->get( "cuts" )->Fill( "nHitsRatio", 1 );

	if ( rapidity < cut_rapidity->min || rapidity > cut_rapidity->max )
		return false;
	if ( makeTrackQA )
		book->get( "cuts" )->Fill( "rapidity", 1 );


	
	// Post Track Cut QA
	if ( makeTrackQA ){

		book->fill( "dca", 					dca );
		book->fill( "nHitsDedx", 			nHitsDedx );
		book->fill( "nHitsFit", 			nHitsFit );
		book->fill( "nHitsPossible", 		nHitsPossible );
		book->fill( "nHitsFitOverPossible", fitRatio );
		book->fill( "nHitsFitVsPossible", 	nHitsPossible, nHitsFit );
		book->fill( "ptPrimary", 			ptPrimary );
		book->fill( "ptGlobal", 			ptGlobal );
		book->fill( "ptGlobalOverPrimary", 	ptGlobal / ptPrimary );
		book->fill( "ptGlobalVsPrimary", 	ptPrimary, ptGlobal );
		book->fill( "eta", 					eta );
		book->fill( "rapidity", 			rapidity );
	}

	return true;
}


bool InclusiveSpectra::keepTofTrack( Int_t iTrack ){

	// alias
	double 	yLocal 			= pico->trackYLocal( iTrack );
	double 	zLocal 			= pico->trackZLocal( iTrack );
	double  beta 			= pico->trackBeta( iTrack );
	int 	matchFlag		= pico->trackTofMatch( iTrack );
	

	/**
	 * Pre Track Cut QA
	 */
	if ( makeTrackQA ){
		book->cd( "TrackQA" );
		book->fill( "pre_yLocal", 				yLocal );
		book->fill( "pre_zLocal", 				zLocal );
		book->fill( "pre_matchFlag", 			matchFlag );
	}

	// TOF track cuts
	if ( matchFlag < cut_matchFlag->min || matchFlag > cut_matchFlag->max )
		return false;
	if ( makeTrackQA )
		book->fill( "cuts", "tofMatch" );
	
	if ( yLocal < cut_yLocal->min || yLocal > cut_yLocal->max )
		return false;
	if ( makeTrackQA )
		book->fill( "cuts", "yLocal" );
	
	if ( zLocal < cut_zLocal->min || zLocal > cut_zLocal->max )
		return false;
	if ( makeTrackQA )
		book->fill( "cuts", "zLocal" );

	// reject tracks that pass the above but have no timing info
	if ( beta <= 0 ) 
		return false;
	if ( makeTrackQA )
		book->fill( "cuts", "beta" );


	// Post Track Cut QA
	if ( makeTrackQA ){

		book->fill( "yLocal", 				yLocal );
		book->fill( "zLocal", 				zLocal );
		book->fill( "matchFlag", 			matchFlag );
	}

	return true;
}






vector<int> InclusiveSpectra::badRuns = { 
15046073, 
15046089, 
15046094, 
15046096, 
15046102, 
15046103, 
15046104, 
15046105, 
15046106, 
15046107, 
15046108, 
15046109, 
15046110, 
15046111, 
15047004, 
15047015, 
15047016, 
15047019, 
15047021, 
15047023, 
15047024, 
15047026, 
15047027, 
15047028, 
15047029, 
15047030, 
15047039, 
15047040, 
15047041, 
15047044, 
15047047, 
15047050, 
15047052, 
15047053, 
15047056, 
15047057, 
15047061, 
15047062, 
15047063, 
15047064, 
15047065, 
15047068, 
15047069, 
15047070, 
15047071, 
15047072, 
15047074, 
15047075, 
15047082, 
15047085, 
15047086, 
15047087, 
15047093, 
15047096, 
15047097, 
15047098, 
15047100, 
15047102, 
15047104, 
15047106, 
15048003, 
15048004, 
15048012, 
15048013, 
15048014, 
15048016, 
15048017, 
15048018, 
15048019, 
15048020, 
15048021, 
15048023, 
15048024, 
15048025, 
15048026, 
15048028, 
15048029, 
15048030, 
15048031, 
15048033, 
15048034, 
15048074, 
15048075, 
15048076, 
15048077, 
15048078, 
15048079, 
15048080, 
15048081, 
15048082, 
15048083, 
15048084, 
15048085, 
15048086, 
15048087, 
15048088, 
15048089, 
15048091, 
15048092, 
15048093, 
15048094, 
15048095, 
15048096, 
15048097, 
15048098, 
15049002, 
15049003, 
15049009, 
15049013, 
15049014, 
15049015, 
15049016, 
15049017, 
15049018, 
15049019, 
15049020, 
15049021, 
15049022, 
15049023, 
15049025, 
15049026, 
15049027, 
15049028, 
15049030, 
15049031, 
15049032, 
15049033, 
15049037, 
15049038, 
15049039, 
15049040, 
15049041, 
15049074, 
15049077, 
15049083, 
15049084, 
15049085, 
15049086, 
15049087, 
15049088, 
15049089, 
15049090, 
15049091, 
15049092, 
15049093, 
15049094, 
15049096, 
15049097, 
15049098, 
15049099, 
15050001, 
15050002, 
15050003, 
15050004, 
15050005, 
15050006, 
15050010, 
15050011, 
15050012, 
15050013, 
15050014, 
15050015, 
15050016, 
15051131, 
15051132, 
15051133, 
15051134, 
15051137, 
15051141, 
15051144, 
15051146, 
15051147, 
15051148, 
15051149, 
15051156, 
15051157, 
15051159, 
15051160, 
15052001, 
15052004, 
15052005, 
15052006, 
15052007, 
15052008, 
15052009, 
15052010, 
15052011, 
15052014, 
15052015, 
15052016, 
15052017, 
15052018, 
15052019, 
15052020, 
15052021, 
15052022, 
15052023, 
15052024, 
15052025, 
15052026, 
15052040, 
15052041, 
15052042, 
15052043, 
15052060, 
15052061, 
15052062, 
15052063, 
15052064, 
15052065, 
15052066, 
15052067, 
15052068, 
15052069, 
15052070, 
15052073, 
15052074, 
15052075,  
15053027, 
15053028, 
15053029, 
15053034, 
15053035, 
15053052, 
15053054, 
15053055, 
15054053, 
15054054, 
15055018, 
15055137, 
15056117, 
15057055, 
15057059, 
15058006, 
15058011, 
15058021, 
15059057, 
15059058, 
15061001, 
15061009, 
15062006, 
15062069, 
15065012, 
15065014, 
15066070, 
15068013, 
15068014, 
15068016, 
15068018, 
15069036, 
15070008, 
15070009, 
15070010
};