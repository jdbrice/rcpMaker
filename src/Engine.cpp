
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
//	#include "Spectra/PidHistoMaker.h"


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
			} else if ( "TofEffMaker" == job ){
				// TofEffMaker tem( &config, "TofEffMaker.", fileList, jobPrefix  );
				// tem.make();
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
			} else if ( "FeedDownPresentation" == job ){
				// Logger::setGlobalLogLevel( Logger::llAll );
				// RooPlotter plt( argv[1] );
			} else if ( "test" == job ){
			} else if ( "PresentPidYield" == job ){
				//cout << "making presenter" << endl;
 
				// PidYieldPresenter pyp( &config, "PidYieldPresenter." );
				// pyp.integrateEta();
				// pyp.normalizeYield();
				// pyp.compareYields();
				// pyp.rcp( 5 );

				// pyp.rcpPanel( 0, 5);
				// pyp.rcpPanel( 1, 5);
				// pyp.rcpPanel( 2, 5);
				// pyp.rcpPanel( 3, 5);
				// pyp.rcpPanel( 4, 5);
				// pyp.rcpPanel( 5, 5);

				// pyp.chargeRatio();
				// pyp.chargeRatioCompare( );

			}

		} catch ( exception &e ){
			cout << e.what() << endl;
		}

	}
	return 0;
}
