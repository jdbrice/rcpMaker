#include "McMaker/UrQMDDcaMapMaker.h"
#include "Common.h"


// Roobarb
#include "RooPlotLib.h"

// ROOT
#include "TGraphAsymmErrors.h"
#include "TLine.h"

// STL
#include <math.h>
#include <limits>
#include <cstdint>

vector<int> UrQMDDcaMapMaker::plcID = { 8, 9, 11, 12, 14, 15 };
vector<float> UrQMDDcaMapMaker::plcMass = { 0.1396, 0.1396, 0.4937, 0.4937, 0.9383, 0.9383 };

void UrQMDDcaMapMaker::initialize(){
		TreeAnalyzer::initialize();
	DEBUG( classname(), "" );
	if ( ds && ds->getTreeName() == "StMiniMcTree" ){
		INFO( classname(), "Using DataStore" )
	} else {
		ERROR( classname(), "No Data Source. Specify one at <DataSourcce ... > </DataSource>" )
	}

	

	// map of GEANT PID -> histogram name
	plcName[ 8 ] = "Pi_p";
	plcName[ 9 ] = "Pi_n";
	plcName[ 11 ] = "K_p";
	plcName[ 12 ] = "K_n";
	plcName[ 14 ] = "P_p";
	plcName[ 15 ] = "P_n";


	// Tracks cuts
	cut_nHitsFit             = unique_ptr<XmlRange>(new XmlRange( &config , "TrackCuts.nHitsFit"             , 0     , std::numeric_limits<int>::max() ) );
	cut_dca                  = unique_ptr<XmlRange>(new XmlRange( &config , "TrackCuts.dca"                  , 0     , std::numeric_limits<int>::max() ) );
	cut_nHitsFitOverPossible = unique_ptr<XmlRange>(new XmlRange( &config , "TrackCuts.nHitsFitOverPossible" , 0     , std::numeric_limits<int>::max() ) );
	cut_nHitsDedx            = unique_ptr<XmlRange>(new XmlRange( &config , "TrackCuts.nHitsDedx"            , 0     , std::numeric_limits<int>::max() ) );
	cut_pt                   = unique_ptr<XmlRange>(new XmlRange( &config , "TrackCuts.pt"                   , 0     , std::numeric_limits<int>::max() ) );
	cut_ptGlobalOverPrimary  = unique_ptr<XmlRange>(new XmlRange( &config , "TrackCuts.ptGlobalOverPrimary"  , 0.7   , 1.42 ) );
	cut_rapidity             = unique_ptr<XmlRange>(new XmlRange( &config , "TrackCuts.rapidity"             , -0.25 , 	0.25 ) );

	rmb = unique_ptr<HistoBins>( new HistoBins( config, nodePath + ".RefMultBins" ) );

	// Setup the centrality bins
   	INFO( classname(), "Loading Centrality Map" ); 
    centralityBinMap = config.getIntMap( nodePath + ".CentralityMap" );
    centralityBins = config.getIntVector( nodePath + ".CentralityBins" );
    INFO( classname(), "c[ 0 ] = " << centralityBinMap[ 0 ] );
}


UrQMDDcaMapMaker::~UrQMDDcaMapMaker(){
}



void UrQMDDcaMapMaker::preEventLoop(){
	DEBUG( classname(), "" );
	TreeAnalyzer::preEventLoop();

	string sCharge = config.getXString( nodePath + ".input:charge", "p" );
	string plc     = config.getXString( nodePath + ".input:plc", "Pi" );
	book->cd();
	// for ( int iC : centralityBins ){
	// 	INFO( classname(), "urqmd_dca_vs_pt_" + plc + "_" + ts( iC ) + "_" + sCharge );
	// 	book->clone( "dca_vs_pt", "urqmd_dca_vs_pt_" + plc + "_" + ts( iC ) + "_" + sCharge );
	// }

	for ( string plc : { "Pi", "K", "P" } ){
		for ( string charge : { "p", "n" } ){
			INFO( classname(), "Making : " << "urqmd_dca_vs_pt_" + plc + "_" + charge  );
			book->clone( "dca_vs_pt", "urqmd_dca_vs_pt_" + plc + "_" + charge );
		}
	}
	
}





void UrQMDDcaMapMaker::postEventLoop(){

	return;
}



bool UrQMDDcaMapMaker::keepTrack( Int_t iTrack ){



	float pt          = ds->get<float>( "mMatchedPairs.mPtPr", iTrack );
	float dca         = ds->get<float>( "mMatchedPairs.mDcaGl", iTrack );
	float ptg         = ds->get<float>( "mMatchedPairs.mPtGl", iTrack );
	float eta         = ds->get<float>( "mMatchedPairs.mEtaPr", iTrack );

	short fitPts      = ds->get<short>( "mMatchedPairs.mFitPts", iTrack );
	short dedxPts     = ds->get<short>( "mMatchedPairs.mDedxPts", iTrack );
	short possiblePts = ds->get<short>( "mMatchedPairs.mNPossible", iTrack );

	float ptr = ptg / pt;
	float fitr = (float)fitPts / (float)possiblePts;

	// book->fill( "pre_dca", dca );
	// book->fill( "pre_fitPts", fitPts );
	// book->fill( "pre_dedxPts", dedxPts );
	// book->fill( "pre_possiblePts", possiblePts );
	// book->fill( "pre_fitr", fitr );
	// book->fill( "pre_ptr", ptr );


	if ( fitPts < cut_nHitsFit->min )
		return false;
	if ( dedxPts < cut_nHitsDedx->min )
		return false;
	if ( fitr < cut_nHitsFitOverPossible->min )
		return false;
	if ( ptr < cut_ptGlobalOverPrimary->min || ptr > cut_ptGlobalOverPrimary->max )
		return false;
	// if ( dca > cut_dca->max )
	// 	return false;

	// book->fill( "dca", dca );
	// book->fill( "fitPts", fitPts );
	// book->fill( "dedxPts", dedxPts );
	// book->fill( "possiblePts", possiblePts );
	// book->fill( "fitr", fitr );
	// book->fill( "ptr", ptr );
	// book->fill( "eta", eta );

	return true;
}

void UrQMDDcaMapMaker::analyzeTrack( Int_t iTrack ){

	book->cd();

	int geantID = (int)ds->get<UShort_t>( "mMatchedPairs.mGeantId", iTrack );
	int pgeantID = (int)ds->get<UShort_t>( "mMatchedPairs.mParentGeantId", iTrack );

	float pt = ds->get<float>( "mMatchedPairs.mPtPr", iTrack );
	float eta = ds->get<float>( "mMatchedPairs.mEtaPr", iTrack );
	float dca = ds->get<float>( "mMatchedPairs.mDcaGl", iTrack );

	int refMult = ds->get<int>( "mNUncorrectedPrimaries" );

	auto pos = find( plcID.begin(), plcID.end(), geantID );
	if ( pos != plcID.end() ){

		string name = plcName[ geantID ];
		int index = pos - plcID.begin();
		
		float y = Common::rapidity( pt, eta, plcMass[ index ] );

		// rapidity cut
		if ( abs( y ) > 0.25 ){
			return;
		}

		// // signal
		// if ( pgeantID == 0 ){
		// 	book->fill( "sig_" + name, pt, refMult );
		// 	book->fill( "sig_" + name + "_" + ts(cBin), pt, refMult );
		// }
		// else if ( pgeantID > 0 ){ // backgrounds
		// 	book->fill( "back_" + name, pt, refMult );
		// 	book->fill( "back_" + name + "_" + ts(cBin), pt, refMult );
		// }

		book->fill( "urqmd_dca_vs_pt_" + name, pt, dca );

	}
}

bool UrQMDDcaMapMaker::keepEvent(){


	int refMult = ds->get<int>( "mNUncorrectedPrimaries" );

	float vX = ds->get<float>( "mVertexX" );
	float vY = ds->get<float>( "mVertexY" ) + 0.89;
	float vZ = ds->get<float>( "mVertexZ" );

	float vR = sqrt( vX * vX + vY * vY);

	// book->fill( "vR", vR );
	// book->fill( "vX", vX );
	// book->fill( "vY", vY );
	// book->fill( "vZ", vZ );

	// book->fill( "refMult", refMult );
	// book->fill( "refMultVz", vZ, refMult );

	if ( refMult < 5 )
		return false;

	return true;
}

void UrQMDDcaMapMaker::analyzeEvent(){


	// Getting the Centrality bin
	int refMult = ds->get<int>( "mNUncorrectedPrimaries" );
	int bin16 = rmb->findBin( refMult, BinEdge::upper );
	
	int bin9 = (int)bin16 / 2;
	
	if ( bin16 == 15 )
		bin9 = 8;
	else if ( bin16 < 0 )
		bin9 = -1;

	if ( bin9 >= 0 && bin9 <= 8 )
		cBin = centralityBinMap[ bin9 ];
	else
		cBin = -1;

	if ( cBin < 0 || bin16 < 0 )
		return;
	
	// book->fill( "refMultBins", refMult, bin9 );
	// book->fill( "refMultMappedBins", refMult, cBin );
	

	int nTracks = ds->get<int>( "mNMatchedPair" );

	for ( Int_t iTrack = 0; iTrack < nTracks; iTrack ++ ){

		if ( !keepTrack( iTrack ) )
			continue;
		
		analyzeTrack( iTrack );			
	}

}




