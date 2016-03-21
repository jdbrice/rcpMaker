

// STL
#include <iostream>
#include <exception>
#include <stdlib.h> // for atoi

// RooBarb
#include "Logger.h"
#include "LoggerConfig.h"
#include "XmlConfig.h"
using namespace jdb;

// Rcp Maker

// Spectra 
	#include "Spectra/InclusiveSpectra.h"
	#include "Spectra/PidHistoMaker.h"
	#include "Spectra/PidDataMaker.h"
	#include "Spectra/TofEffSpectra.h"


// // McMaker
	#include "McMaker/TofEffMaker.h"
	#include "McMaker/EnergyLoss.h"
	#include "McMaker/TofEffFitter.h"
	#include "McMaker/TpcEffFitter.h"
	#include "McMaker/TpcEffMaker.h"

// // Presentation
// 	#include "Present/PidYieldPresenter.h"

// // Feed down
	#include "McMaker/FeedDownMaker.h"
	#include "McMaker/FeedDownFitter.h"


// // PID Fitting
	#include "TSF/FitRunner.h"
	using namespace TSF;

// // Corrections
	#include "Correction/ApplyPostCorr.h"


void ssGo( XmlConfig &config, string fileList, string jobPrefix );
void conGo( XmlConfig &config, int jobIndex );

int main( int argc, char* argv[] ) {

	Logger::setGlobalLogLevel( "all" );

	cout << "Arguments " << argc << endl;

	if ( argc >= 2 ){

		try{
			XmlConfig config( argv[ 1 ] );
			//config.report();

			LoggerConfig::setup( &config, "Logger" );

			string fileList = "";
			string jobPrefix = "";

			cout << "Arguments " << argc << endl;
			if ( argc >= 4 ){
				fileList = argv[ 2 ];
				jobPrefix = argv[ 3 ];
				ssGo( config, fileList, jobPrefix );
			} else if ( argc >= 3 ){
				int jobIndex = atoi( argv[2] );
				conGo( config, jobIndex );
			} else {
				conGo( config, -1 );
			}
		}catch ( exception &e ){
			cout << "HEllo" << endl;
			cout << e.what() << endl;
		}
	}
	
	return 0;

}



void conGo( XmlConfig &config, int jobIndex ){

	cout << "conGo" << endl;
	string job = config.getString( "jobType" );

	if ( "InclusiveSpectra" == job ){
		InclusiveSpectra is;
		is.init( config, "InclusiveSpectra.", jobIndex );
		is.run();
	}
	else if ( "TofEffSpectra" == job ){
		TofEffSpectra tes;
		tes.init( config, "TofEffSpectra.", jobIndex );
		tes.run();
	} 
	else if ( "TofEffFitter" == job ){
		TofEffFitter tef( config, "TofEffFitter." );
		tef.make();     
	} 
	else if ( "EnergyLoss" == job ){
		EnergyLoss el;
		el.init( config, "EnergyLoss.", jobIndex );
		el.run();
	} 
	else if ( "TpcEffMaker" == job ){
		TpcEffMaker tem;
		tem.init( config, "TpcEffMaker.", jobIndex );
		tem.run();
	} 
	else if ( "TpcEffFitter" == job ){
		TpcEffFitter tef( config, "TpcEffFitter." );
		tef.make();
	} 
	else if ( "FeedDownMaker" == job ){
		FeedDownMaker fdm;
		fdm.init( config, "FeedDownMaker.", jobIndex );
		fdm.run();
	}
	else if ( "FeedDownFitter" == job ){
		FeedDownFitter fdf( config, "FeedDownFitter." );
		fdf.make();
	}
	else if ( "PidHistoMaker" == job ){
		PidHistoMaker pps;
		pps.init( config, "PidHistoMaker.", jobIndex  );
		pps.run();
	} 
	else if ( "PidDataMaker" == job ){
		PidDataMaker pdm;
		pdm.init( config, "PidDataMaker.", jobIndex  );
		pdm.run();
	} 
	else if ( "SimultaneousTPid" == job ){
		cout << "WOW:" << endl;
		// int iCharge = atoi( fileList.c_str() );
		// if ( "" == fileList )
		// 	iCharge = -2;
		// int iCen = atoi( jobPrefix.c_str() );
		// if ( "" == jobPrefix )
		// 	iCen = -1;

		// FitRunner fr( config, "SimultaneousPid.", iCharge, iCen );
		// fr.make();

	}  
	else if ( "ApplyPostCorr" == job ){
		ApplyPostCorr apc( config, "ApplyPostCorr." );
		apc.make();
	} 
	
}

void ssGo( XmlConfig &config, string fileList, string jobPrefix ){
	cout << "ssGo" << endl;
	string job = config.getString( "jobType" );

	// if ( "InclusiveSpectra" == job ){
	// 	InclusiveSpectra is( config, "InclusiveSpectra.", fileList, jobPrefix );
	// 	is.run();
	// }
	// else if ( "TofEffSpectra" == job ){
	// 	TofEffSpectra tes( config, "TofEffSpectra.", fileList, jobPrefix );
	// 	tes.run();
	// } 
	
}


/*
if ( "InclusiveSpectra" == job ){
				InclusiveSpectra is( config, "InclusiveSpectra.", fileList, jobPrefix );
				is.run();
			} 
			// else if ( "TofEffSpectra" == job ){
			// 	TofEffSpectra tes( config, "TofEffSpectra.", fileList, jobPrefix );
			// 	tes.run();
			// } 
			// if ( "EnergyLoss" == job ){
			// 	EnergyLoss el( &config, "EnergyLoss.", fileList, jobPrefix );
			// 	el.run();
			// } if ( "TpcEffFitter" == job ){
			// 	TpcEffFitter tef( &config, "TpcEffFitter." );
			// 	tef.make();
			// } if ( "TpcEffMaker" == job ){
			// 	TpcEffMaker tem( &config, "TpcEffMaker." );
			// 	tem.run();
			// } 
			else if ( "PidHistoMaker" == job ){
				PidHistoMaker pps( config, "PidHistoMaker.", fileList, jobPrefix  );
				pps.run();
			} 
			else if ( "PidDataMaker" == job ){
				PidDataMaker pdm( config, "PidDataMaker.", fileList, jobPrefix  );
				pdm.run();
			} 
			// else if ( "TofEffFitter" == job ){
			// 	TofEffFitter tef( &config, "TofEffFitter." );
			// 	tef.make();     
			// } else if ( "SimultaneousTPid" == job ){
			// 	int iCharge = atoi( fileList.c_str() );
			// 	if ( "" == fileList )
			// 		iCharge = -2;
			// 	int iCen = atoi( jobPrefix.c_str() );
			// 	if ( "" == jobPrefix )
			// 		iCen = -1;

			// 	FitRunner fr( &config, "SimultaneousPid.", iCharge, iCen );
			// 	fr.make();

			// }  else if ( "FeedDownMaker" == job ){
			// 	FeedDownMaker fdm( &config, "FeedDownMaker.", fileList, jobPrefix );
			// 	fdm.run();
			// } else if ( "FeedDownFitter" == job ){
			// 	FeedDownFitter fdf( &config, "FeedDownFitter." );
			// 	fdf.make();
			// } else if ( "ApplyPostCorr" == job ){
			// 	ApplyPostCorr apc( &config, "ApplyPostCorr." );
			// 	apc.make();
			// } else if ( "PresentPidYield" == job ){
			// 	//cout << "making presenter" << endl;
 
			// 	PidYieldPresenter pyp( &config, "PidYieldPresenter." );
			// 	pyp.make();

			// }
*/