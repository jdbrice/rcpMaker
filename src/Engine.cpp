

// STL
#include <iostream>
#include <exception>
#include <stdlib.h> // for atoi

// RooBarb
#include "Logger.h"
#include "LoggerConfig.h"
#include "XmlConfig.h"
#include "TaskEngine.h"
using namespace jdb;

// Rcp Maker

// Spectra 
	#include "Spectra/InclusiveSpectra.h"
	#include "Spectra/PidHistoMaker.h"
	#include "Spectra/PidDataMaker.h"
	#include "Spectra/TofEffSpectra.h"
	#include "Spectra/ZdDataMaker.h"
	#include "Spectra/DcaMapMaker.h"


// // McMaker
	#include "McMaker/EnergyLoss.h"
	#include "McMaker/TofEffFitter.h"
	#include "McMaker/TpcEffFitter.h"
	#include "McMaker/TpcEffMaker.h"
	#include "McMaker/EmbDcaMapMaker.h"
	#include "McMaker/DcaWeightMaker.h"

// // Presentation
	#include "Present/YieldExporter.h"
	#include "Present/ZdSpectraHistoFileMaker.h"


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

	TaskFactory::registerTaskRunner<EnergyLoss>( "EnergyLoss" );
	TaskFactory::registerTaskRunner<FeedDownMaker>( "FeedDownMaker" );
	TaskFactory::registerTaskRunner<FeedDownFitter>( "FeedDownFitter" );
	TaskFactory::registerTaskRunner<EmbDcaMapMaker>( "EmbDcaMapMaker" );
	TaskFactory::registerTaskRunner<DcaMapMaker>( "DcaMapMaker" );
	TaskFactory::registerTaskRunner<DcaWeightMaker>( "DcaWeightMaker" );
	TaskFactory::registerTaskRunner<TpcEffMaker>( "TpcEffMaker" );
	TaskFactory::registerTaskRunner<TpcEffFitter>( "TpcEffFitter" );
	TaskFactory::registerTaskRunner<TofEffSpectra>( "TofEffSpectra" );
	TaskFactory::registerTaskRunner<TofEffFitter>( "TofEffFitter" );

	TaskFactory::registerTaskRunner<PidHistoMaker>( "PidHistoMaker" );
	TaskFactory::registerTaskRunner<PidDataMaker>( "PidDataMaker" );
	TaskFactory::registerTaskRunner<ZdDataMaker>( "ZdDataMaker" );

	TaskFactory::registerTaskRunner<FitRunner>( "FitRunner" );
	
	TaskFactory::registerTaskRunner<ApplyPostCorr>( "ApplyPostCorr" );
	TaskFactory::registerTaskRunner<YieldExporter>( "YieldExporter" );
	TaskFactory::registerTaskRunner<ZdSpectraHistoFileMaker>( "ZdSpectraHistoFileMaker" );  

	TaskEngine engine( argc, argv );

	// Logger::setGlobalLogLevel( "all" );

	// cout << "Arguments " << argc << endl;

	// if ( argc >= 2 ){

	// 	try{
	// 		XmlConfig config( argv[ 1 ] );
	// 		//config.report();

	// 		LoggerConfig::setup( &config, "Logger" );

	// 		string fileList = "";
	// 		string jobPrefix = "";

	// 		cout << "Arguments " << argc << endl;
	// 		if ( argc >= 4 ){
	// 			fileList = argv[ 2 ];
	// 			jobPrefix = argv[ 3 ];
	// 			ssGo( config, fileList, jobPrefix );
	// 		} else if ( argc >= 3 ){
	// 			int jobIndex = atoi( argv[2] );
	// 			conGo( config, jobIndex );
	// 		} else {
	// 			conGo( config, -1 );
	// 		}
	// 	}catch ( exception &e ){
	// 		cout << "HEllo" << endl;
	// 		cout << e.what() << endl;
	// 	}
	// }
	
	return 0;

}


// void conGo( XmlConfig &config, int jobIndex ){


// 	cout << "conGo" << endl;
// 	string job = config.getString( "jobType" );

// 	if ( "InclusiveSpectra" == job ){
// 		InclusiveSpectra is;
// 		is.init( config, "InclusiveSpectra.", jobIndex );
// 		is.run();
// 	}
// 	else if ( "TofEffSpectra" == job ){
// 		TofEffSpectra tes;
// 		tes.init( config, "TofEffSpectra.", jobIndex );
// 		tes.run();
// 	} 
// 	else if ( "TofEffFitter" == job ){
// 		TofEffFitter tef;
// 		tef.init( config, "TofEffFitter.", jobIndex );
// 		tef.make();     
// 	} 
// 	else if ( "EnergyLoss" == job ){
// 		EnergyLoss el;
// 		el.init( config, "EnergyLoss.", jobIndex );
// 		el.run();
// 	} 
// 	else if ( "TpcEffMaker" == job ){
// 		TpcEffMaker tem;
// 		tem.init( config, "TpcEffMaker.", jobIndex );
// 		tem.run();
// 	} 
// 	else if ( "TpcEffFitter" == job ){
// 		TpcEffFitter tef;
// 		tef.init( config, "TpcEffFitter.", jobIndex );
// 		tef.run();
// 	} 
// 	else if ( "FeedDownMaker" == job ){
// 		FeedDownMaker fdm;
// 		fdm.init( config, "FeedDownMaker.", jobIndex );
// 		fdm.run();
// 	}
// 	else if ( "FeedDownFitter" == job ){
// 		FeedDownFitter fdf( config, "FeedDownFitter." );
// 		fdf.make();
// 	}
// 	else if ( "PidHistoMaker" == job ){
// 		PidHistoMaker pps;
// 		pps.init( config, "PidHistoMaker.", jobIndex  );
// 		pps.run();
// 	} 
// 	else if ( "PidDataMaker" == job ){
// 		PidDataMaker pdm;
// 		pdm.init( config, "PidDataMaker.", jobIndex  );
// 		pdm.run();
// 	} 
// 	else if ( "SimultaneousTPid" == job ){
// 		cout << "TSF Straight Config Run:" << endl;
	
// 		// FitRunner fr;
// 		// fr.init( config, "SimultaneousPid.", jobIndex );
// 		// fr.run();

// 	}  
// 	else if ( "ApplyPostCorr" == job ){
// 		ApplyPostCorr apc;
// 		apc.init( config, "ApplyPostCorr.", -1 );
// 		apc.run();
// 	} else if ( "YieldExporter" == job ){
// 		YieldExporter yxp;
// 		yxp.init( config, "YieldExporter", -1 );
// 		yxp.run();
// 	}
	
// }

// void ssGo( XmlConfig &config, string fileList, string jobPrefix ){
// 	cout << "ssGo" << endl;
// 	string job = config.getString( "jobType" );
// 	if ( "SimultaneousTPid" == job ){
		
// 		int iCharge = atoi( fileList.c_str() );
// 		if ( "" == fileList )
// 			iCharge = -2;
// 		int iCen = atoi( jobPrefix.c_str() );
// 		if ( "" == jobPrefix )
// 			iCen = -1;
// 		INFO( "Engine", "iCharge = " << iCharge << ", iCen = " << iCen );

// 		FitRunner fr;
// 		fr.setup( config, "SimultaneousPid.", iCharge, iCen );
// 		fr.run();

// 	}
	
	
	
// }

