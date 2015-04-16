

#include "XmlConfig.h"
using namespace jdb;

#include "LBNLPicoDst.h"


#include <iostream>
#include "InclusiveSpectra.h"
#include "ParamSpectra.h"
#include "PidPhaseSpace.h"
#include "PidParamMaker.h"
#include "SimultaneousGaussians.h"
#include "SimultaneousPid.h"

#include "XmlRooRealVar.h"
#include "SGFSchema.h"
#include "SGF.h"
#include "SGFRunner.h"
#include "FemtoDstMaker.h"
#include "PidYieldPresenter.h"

#include <exception>


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
			} else if ( "ParamSpectra" == job ){
				//ParamSpectra ps( &config, "ParamSpectra.", fileList, jobPrefix );
				//ps.make();
			} else if ( "PidPhaseSpace" == job ){
				PidPhaseSpace pps( &config, "PidPhaseSpace.", fileList, jobPrefix  );
				pps.make();
			} else if ( "MakePidParams" == job ){
				PidParamMaker ppm( &config, "PidParamMaker." );
				ppm.make();
			} else if ( "SimultaneousPid" == job ){
				SGFRunner sgfr( &config, "SimultaneousPid." );
				sgfr.make();

			} else if ( "FemtoDst" == job ){
				FemtoDstMaker fdst( &config, "FemtoDstMaker.", fileList, jobPrefix );
				fdst.make();

			} else if ( "PresentPidYield" == job ){
				//cout << "making presenter" << endl;
 
				PidYieldPresenter pyp( &config, "PidYieldPresenter." );
				pyp.integrateEta();
				pyp.normalizeYield();
				pyp.compareYields();
				pyp.rcp( 6 );

				pyp.rcpPanel( 0, 6);
				/*pyp.rcpPanel( 1, 6);
				pyp.rcpPanel( 2, 6);
				pyp.rcpPanel( 3, 6);
				pyp.rcpPanel( 4, 6);
				pyp.rcpPanel( 5, 6);
				pyp.rcpPanel( 6, 6);*/

				pyp.chargeRatio();
				pyp.chargeRatioCompare( );

				//pyp.rcpVsNPart( config.getInt( "PidYieldPresenter.nPart:ptBin" ) );
				//pyp.rcpVsNPartCompare( config.getInt( "PidYieldPresenter.nPart:ptBin" ) );
				



				
				

			} else if ( "test" == job ){
				
				Logger::setGlobalColor( true );
				Logger::setGlobalLogLevel( Logger::llAll );
				
				TFile * inFile = new TFile(config.getString( "input:url").c_str(), "READ" );

				shared_ptr<SGFSchema> sgfs = shared_ptr<SGFSchema>(new SGFSchema( &config, "Schema" ));
				SGF sgf( sgfs, inFile );
				shared_ptr<Reporter> rp = shared_ptr<Reporter>( new Reporter( "rpTest.pdf", 600, 600 ) );

				for ( int i = 4; i < 10; i++ ){
					// update limits
					sgf.fit( "K", 0, 1, i, 0 );
					//sgf.report( rp );	
				}
				
				

				inFile->Close();

			}

		} catch ( exception &e ){
			cout << e.what() << endl;
		}

	}
	return 0;
}
