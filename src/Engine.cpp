
// STL
#include <iostream>
#include <exception>

// RooBarb
#include "Logger.h"
#include "XmlConfig.h"
using namespace jdb;

// Rcp Maker

// Spectra 
	#include "Spectra/InclusiveSpectra.h"
	#include "Spectra/PidHistoMaker.h"
	#include "Spectra/PidDataMaker.h"
	// #include "Spectra/PidPoint.h"


// McMaker
//	#include "McMaker/TofEffMaker.h"
	#include "McMaker/EnergyLoss.h"
	#include "McMaker/TofEffFitter.h"
	#include "McMaker/TpcEffFitter.h"

// Presentation
	#include "Present/PidYieldPresenter.h"

// Feed down
	#include "McMaker/FeedDownMaker.h"


// PID Fitting
	#include "TSF/FitRunner.h"

// Corrections
	#include "Correction/ApplyPostCorr.h"




using namespace TSF;

int main( int argc, char* argv[] ) {

	Logger::setGlobalLogLevel( "all" );

	if ( argc >= 2 ){

		try{
			XmlConfig config( argv[ 1 ] );
			//config.report();

			string fileList = "";
			string jobPrefix = "";

			if ( argc >= 4 ){
				fileList = argv[ 2 ];
				jobPrefix = argv[ 3 ];
			}

			string job = config.getString( "jobType" );

			if ( "InclusiveSpectra" == job ){
				InclusiveSpectra is( &config, "InclusiveSpectra.", fileList, jobPrefix );
				is.make();
			} if ( "EnergyLoss" == job ){
				EnergyLoss el( &config, "EnergyLoss.", fileList, jobPrefix );
				el.make();
			} if ( "TpcEffFitter" == job ){
				TpcEffFitter tef( &config, "TpcEffFitter." );
				tef.make();
			} else if ( "PidHistoMaker" == job ){
				PidHistoMaker pps( &config, "PidHistoMaker.", fileList, jobPrefix  );
				pps.make();
			} else if ( "PidDataMaker" == job ){
				PidDataMaker pdm( &config, "PidDataMaker.", fileList, jobPrefix  );
				pdm.make();
			} else if ( "TofEffFitter" == job ){
				TofEffFitter tef( &config, "TofEffFitter." );
				tef.make();     
			} else if ( "SimultaneousTPid" == job ){
				FitRunner fr( &config, "SimultaneousPid." );
				fr.make();

			}  else if ( "FeedDownMaker" == job ){
				FeedDownMaker fdm( &config, "FeedDownMaker." );
				fdm.make();
			} else if ( "ApplyPostCorr" == job ){
				ApplyPostCorr apc( &config, "ApplyPostCorr." );
				apc.make();
			} else if ( "PresentPidYield" == job ){
				//cout << "making presenter" << endl;
 
				PidYieldPresenter pyp( &config, "PidYieldPresenter." );
				pyp.make();

			}

		} catch ( exception &e ){
			cout << e.what() << endl;
		}

	}


	// TFile * f = new TFile( "test.root", "RECREATE" );

	// PidPoint p;

	// p.zb.push_back( 100 );
	// p.zd.push_back( 200 );

	// p.Write( "test_point" );

	// f->Close();


	return 0;
}
