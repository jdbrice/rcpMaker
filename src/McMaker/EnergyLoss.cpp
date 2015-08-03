#include "McMaker/EnergyLoss.h"

#include "TProfile.h"


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


void EnergyLoss::postEventLoop(){


	string params_file = cfg->getString( nodePath + "output.params" );
	if ( "" == params_file ){
		ERROR( "Specifiy an output params file for the parameters" )
		return;
	}

	ofstream out( params_file.c_str() );

	out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
	out << "<config>" << endl;
	

	book->cd();
	vector<int> cBins = cfg->getIntVector( nodePath + "CentralityBins" );

	for ( int bin : cBins ){

		if ( !book->exists( "energyLoss_" + ts( bin ) ) ){
			ERROR( "Histogram not Found" )
			break;
		}

		TH2 * h2 = book->get2D( "energyLoss_" + ts( bin ) );
		TProfile * h1 = h2->ProfileX( ("eloss_" + ts(bin)).c_str() );

		TF1 * f = new TF1( "ff", "[0] + [1] * pow( x, -[2] )" );

		
		h1->Fit( f );
		
		exportParams( bin, f, "[0] + [1] * pow( x, -[2] )", out );

		delete f; 

	}


	out << "</config>" << endl;
	out.close();

}



void EnergyLoss::analyzeTrack( int iTrack ){

	book->cd();
	book->fill( "energyLoss", pico->trackPt( iTrack ), pico->trackPt( iTrack ) - pico->mcPt( iTrack ) );
	
	if ( cBin >= 0 )
		book->fill( "energyLoss_" + ts( cBin ), pico->trackPt( iTrack ), pico->trackPt( iTrack ) - pico->mcPt( iTrack ) );

}


void EnergyLoss::exportParams( int bin, TF1 * f, string formula, ofstream &out ){
	double * params = f->GetParameters();
	out << "\t<EnergyLossParams plc=\"" << cfg->getString( nodePath + "input:plc" ) << "\" bin=\"" << bin << "\" formula=\"" << formula << "\" p0=\"" << params[ 0 ] << "\" p1=\"" << params[ 1 ] << "\" p2=\"" << params[ 2 ] << "\" />" << endl;
}