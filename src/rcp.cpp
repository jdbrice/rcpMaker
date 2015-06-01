
// STL
#include <iostream>
#include <exception>

// RooBarb
#include "XmlConfig.h"
using namespace jdb;

// Rcp Maker
#include "InclusiveSpectra.h"
#include "PidPhaseSpace.h"
#include "PidSpectraMaker.h"
#include "PidYieldPresenter.h"

#include "TSF/FitRunner.h"


//#include "PidParamMaker.h"
//#include "SimultaneousGaussians.h"
//#include "SimultaneousPid.h"







using namespace TSF;

int main( int argc, char* argv[] ) {

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
			} else if ( "PidPhaseSpace" == job ){
				PidPhaseSpace pps( &config, "PidPhaseSpace.", fileList, jobPrefix  );
				pps.make();
			} else if ( "SimultaneousTPid" == job ){
				FitRunner fr( &config, "SimultaneousPid." );
				fr.make();

			} else if ( "PresentPidYield" == job ){
				//cout << "making presenter" << endl;
 
				PidYieldPresenter pyp( &config, "PidYieldPresenter." );
				pyp.integrateEta();
				pyp.normalizeYield();
				pyp.compareYields();
				pyp.rcp( 6 );

				pyp.rcpPanel( 0, 6);
				pyp.rcpPanel( 1, 6);
				pyp.rcpPanel( 2, 6);
				pyp.rcpPanel( 3, 6);
				pyp.rcpPanel( 4, 6);
				pyp.rcpPanel( 5, 6);
				pyp.rcpPanel( 6, 6);

				pyp.chargeRatio();
				pyp.chargeRatioCompare( );

				//pyp.rcpVsNPart( config.getInt( "PidYieldPresenter.nPart:ptBin" ) );
				//pyp.rcpVsNPartCompare( config.getInt( "PidYieldPresenter.nPart:ptBin" ) );
				
				
				

			} else if ( "PidSpectraMaker" == job ){
				PidSpectraMaker psm( &config, "PidSpectraMaker." );
				psm.make();
			}
			else if ( "test" == job ){
				
				//PidProbabilityMapper ppm( &config, "MapTest." );
				//ppm.pidWeights( -1, 0, 0.91, 0.0, 0.1, 0.09 );

			} 

		} catch ( exception &e ){
			cout << e.what() << endl;
		}

	}
	return 0;
}
