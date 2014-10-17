

#include "XmlConfig.h"
using namespace jdb;


#include <iostream>
#include "InclusiveSpectra.h"
#include "ParamSpectra.h"
#include "RefMultHelper.h"
#include "PidPhaseSpace.h"

#include <exception>

int main( int argc, char* argv[] ) {

	if ( argc >= 2 ){

		try{
			XmlConfig config( argv[ 1 ] );
			//config.report();

			string job = config.getString( "jobType" );

			if ( "RefMultHelper" == job ){
				RefMultHelper rmh( &config, "RefMultHelper." );
				rmh.eventLoop();
			} else if ( "InclusiveSpectra" == job ){
				InclusiveSpectra is( &config, "InclusiveSpectra.");
				is.make();
			} else if ( "ParamSpectra" == job ){
				ParamSpectra ps( &config, "ParamSpectra.");
				ps.make();
			} else if ( "PidPhaseSpace" == job ){
				PidPhaseSpace pps( &config, "PidPhaseSpace." );
				pps.make();
			}

			//ParamSpectra ps( &config, "ParamSpectra." );

			//ps.make();
		} catch ( exception &e ){
			cout << e.what() << endl;
		}

	}
	return 0;
}
