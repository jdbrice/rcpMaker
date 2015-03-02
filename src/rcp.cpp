

#include "XmlConfig.h"
using namespace jdb;

#include "DataSourceWrapper.h"


#include <iostream>
#include "InclusiveSpectra.h"
#include "ParamSpectra.h"
#include "PidPhaseSpace.h"
#include "PidParamMaker.h"
#include "SimultaneousPid.h"

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
				ParamSpectra ps( &config, "ParamSpectra.", fileList, jobPrefix );
				ps.make();
			} else if ( "PidPhaseSpace" == job ){
				PidPhaseSpace pps( &config, "PidPhaseSpace.", fileList, jobPrefix  );
				pps.make();
				//TreeAnalyzer ta( &config, "PidPhaseSpace.", fileList, jobPrefix  );
				//ta.make();
			} else if ( "MakePidParams" == job ){
				PidParamMaker ppm( &config, "PidParamMaker." );
				ppm.make();
			} else if ( "SimultaneousPid" == job ){
				SimultaneousPid sg( &config, "SimultaneousPid." );
				sg.make();
			} else if ( "test" == job ){
				//TreeAnalyzer ta( &config, "" );
				DataSource * ds = new DataSource( &config, "DataSource" );
				DataSourceWrapper dsw(ds );

				for ( int i = 0; i < 100; i++  ){
					dsw.GetEntry(i);
					cout << "#Tracks  " << dsw.eventNumTracks() << endl;;
					for ( int j = 0; j < dsw.eventNumTracks(); j++ ){
						if ( dsw.trackTofMatch( j ) )
							cout << "beta " << dsw.trackBeta( j ) << endl;
					}
				}
			}

		} catch ( exception &e ){
			cout << e.what() << endl;
		}

	}
	return 0;
}
