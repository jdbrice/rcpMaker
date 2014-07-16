
#include <iostream>
#include "allroot.h"
#include "constants.h"
#include "histoBook.h"
#include "xmlConfig.h"
#include "rcpMaker.h"
#include "chainLoader.h"



int main( int argc, char* argv[] ) {

  if ( argc >= 2 ){
    xmlConfig config( argv[ 1 ] );
    config.report();


    TChain * chain = new TChain( "tof" );
    chainLoader::load( chain, config.getString( "input.dataDir" ).c_str(), config.getInt( "input.dataDir:maxFiles" ) );

    //cout << "Available branches : " << endl;
    //chain->Print();

    rcpMaker* rcp = new rcpMaker( chain, &config  );

    rcp->loopEvents();

    delete rcp;

  }
	return 0;
}
