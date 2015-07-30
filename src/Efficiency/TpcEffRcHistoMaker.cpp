#include "Efficiency/TpcEffRcHistoMaker.h"
#include "Adapter/RcpPicoDst.h"
#include "Common.h"

TpcEffRcHistoMaker::TpcEffRcHistoMaker( XmlConfig * _config, string _nodePath, string _fileList, string _jobPrefix )
	: InclusiveSpectra( _config, _nodePath, _fileList, _jobPrefix ) {


}

TpcEffRcHistoMaker::~TpcEffRcHistoMaker() {

}


void TpcEffRcHistoMaker::analyzeEvent(){
	DEBUG( "" )
	int nTracks = pico->numTracks();

	for ( int i = 0; i < nTracks; i++ ){

		if ( keepTrack( i ) )
			analyzeTrack( i );
	}

}


bool TpcEffRcHistoMaker::keepEvent(){
	// the skimmer already did event cuts
	return true;
}


bool TpcEffRcHistoMaker::keepTrack( int iTrack ){

	float y = Common::rapidity( pico->trackPt( iTrack ), pico->trackEta( iTrack ) /*, mass */ );
	book->fill( "pre_rapidity", y );

	if ( y < cut_rapidity->min || y > cut_rapidity->max )
		return false;

	

	return true;
}

void TpcEffRcHistoMaker::analyzeTrack( int iTrack ){
	DEBUG( "( " << iTrack << " )" )
	float y = Common::rapidity( pico->trackPt( iTrack ), pico->trackEta( iTrack ) /*, mass */ );
	
	book->fill( "eta", pico->trackEta(iTrack) );
	book->fill( "rapidity", y );
	book->fill( "pt", pico->trackPt( iTrack ) );
}