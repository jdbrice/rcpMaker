#include "McMaker/EnergyLoss.h"



EnergyLoss::EnergyLoss( XmlConfig * _config, string _nodePath, string _fileList, string _jobPrefix )
	: InclusiveSpectra( _config, _nodePath, _fileList, _jobPrefix ){

}


void EnergyLoss::preEventLoop() {


	InclusiveSpectra::preEventLoop();

	book->cd();

	for ( int iC = 0; iC < nCentralityBins(); iC ++ ){
		book->clone( "energyLoss", "energyLoss_" + ts(iC) );
	}

}

void EnergyLoss::analyzeTrack( int iTrack ){

	book->cd();
	book->fill( "energyLoss", pico->trackPt( iTrack ), pico->trackPt( iTrack ) - pico->mcPt( iTrack ) );

	if ( cBin >= 0 )
		book->fill( "energyLoss_" + ts( cBin ), pico->trackPt( iTrack ), pico->trackPt( iTrack ) - pico->mcPt( iTrack ) );

}