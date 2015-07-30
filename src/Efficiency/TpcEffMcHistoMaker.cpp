#include "Efficiency/TpcEffMcHistoMaker.h"
#include "Adapter/RcpPicoDst.h"
#include "Common.h"

TpcEffMcHistoMaker::TpcEffMcHistoMaker( XmlConfig * _config, string _nodePath, string _fileList, string _jobPrefix )
	: TreeAnalyzer( _config, _nodePath, _fileList, _jobPrefix ) {


	if ( chain && "rcpPicoDst" == cfg->getString( nodePath + "input.dst:treeName" ) ){
		logger->info(__FUNCTION__) << "Rcp Pico" << endl;
		pico = unique_ptr<PicoDataStore>( new RcpPicoDst( chain ) );
	}


	cut_rapidity = unique_ptr<ConfigRange>( new ConfigRange( cfg , "TrackCuts.rapidity" ) );
	plc = cfg->getString( nodePath + "input:plc", "" );
	mass = Common::mass( plc );
	if ( mass < 0 )
		ERROR( "Invalid Particle Species " << plc )


}

TpcEffMcHistoMaker::~TpcEffMcHistoMaker() {

}


void TpcEffMcHistoMaker::analyzeEvent(){
	DEBUG( "" )
	int nTracks = pico->numTracks();

	for ( int i = 0; i < nTracks; i++ ){

		if ( keepTrack( i ) )
			analyzeTrack( i );
	}

}


bool TpcEffMcHistoMaker::keepEvent(){
	// the skimmer already did event cuts
	return true;
}


bool TpcEffMcHistoMaker::keepTrack( int iTrack ){

	float y = Common::rapidity( pico->trackPt( iTrack ), pico->trackEta( iTrack ) /*, mass */ );
	book->fill( "pre_rapidity", y );

	if ( y < cut_rapidity->min || y > cut_rapidity->max )
		return false;

	return true;
}

void TpcEffMcHistoMaker::analyzeTrack( int iTrack ){
	DEBUG( "( " << iTrack << " )" )
	float y = Common::rapidity( pico->trackPt( iTrack ), pico->trackEta( iTrack ) /*, mass */ );
	
	book->fill( "eta", pico->trackEta(iTrack) );
	book->fill( "rapidity", y );
	book->fill( "pt", pico->trackPt( iTrack ) );
}