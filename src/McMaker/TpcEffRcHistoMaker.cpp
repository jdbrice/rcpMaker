#include "McMaker/TpcEffRcHistoMaker.h"
#include "Adapter/RcpPicoDst.h"
#include "Common.h"

TpcEffRcHistoMaker::TpcEffRcHistoMaker( XmlConfig * _config, string _nodePath, string _fileList, string _jobPrefix )
	: InclusiveSpectra( _config, _nodePath, _fileList, _jobPrefix ) {


}

TpcEffRcHistoMaker::~TpcEffRcHistoMaker() {

}



void TpcEffRcHistoMaker::analyzeTrack( int iTrack ){
	DEBUG( "( " << iTrack << " )" )

	book->fill( "pt", pico->trackPt( iTrack ) );
}