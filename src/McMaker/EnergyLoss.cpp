#include "McMaker/EnergyLoss.h"

#include "TProfile.h"


void EnergyLoss::initialize(){
	InclusiveSpectra::initialize();
}


void EnergyLoss::preEventLoop() {


	InclusiveSpectra::preEventLoop();

	book->cd();

	for ( int iC = 0; iC < nCentralityBins(); iC ++ ){
		book->clone( "energyLoss", "energyLoss_" + ts(iC) );
	}

}


void EnergyLoss::postEventLoop(){
	DEBUG( classname(), "" )

	string params_file = config.getXString( nodePath + ".output.params" );
	if ( "" == params_file ){
		ERROR( "Specifiy an output params file for the parameters" )
		return;
	}


	DEBUG( classname(), "Opening : " << params_file );
	ofstream out( params_file.c_str() );

	out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
	out << "<config>" << endl;
	

	book->cd();
	vector<int> cBins = config.getIntVector( nodePath + ".CentralityBins" );

	for ( int bin : cBins ){

		if ( !book->exists( "energyLoss_" + ts( bin ) ) ){
			ERROR( "Histogram not Found" )
			break;
		}

		TH2 * h2 = book->get2D( "energyLoss_" + ts( bin ) );
		TProfile * h1 = h2->ProfileX( ("eloss_" + ts(bin)).c_str() );

		TF1 * f = new TF1( "ff", "[0] + [1] * pow( x, -[2] )" );
		
		h1->Fit( f );
		
		exportParams( bin, f, out );

		delete f; 

	}


	out << "</config>" << endl;
	out.close();

}



void EnergyLoss::analyzeTrack( int iTrack, bool isTofTrack ){

	book->cd();
	book->fill( "energyLoss", pico->trackPt( iTrack ), pico->trackPt( iTrack ) - pico->mcPt( iTrack ) );
	
	if ( cBin >= 0 )
		book->fill( "energyLoss_" + ts( cBin ), pico->trackPt( iTrack ), pico->trackPt( iTrack ) - pico->mcPt( iTrack ) );

}


void EnergyLoss::exportParams( int bin, TF1 * f, ofstream &out ){
	
	out << "\t<EnergyLossParams plc=\"" << config.getXString( nodePath + ".input:plc" ) << "\" bin=\"" << bin << "\" "; 
	out << Common::toXml( f ) ;
	out << " />" << endl;
}