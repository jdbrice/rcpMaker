

#include "allroot.h"
#include "HistoBook.h"
#include "XmlConfig.h"
#include "ChainLoader.h"
using namespace jdb;

#include <iostream>


int main( int argc, char* argv[] ) {

  if ( argc >= 2 ){

    XmlConfig config( argv[ 1 ] );
    config.report();

    TChain * chain = new TChain( "tof" );
    ChainLoader::load( chain, config.getString( "input.dataDir" ).c_str(), config.getInt( "input.dataDir:maxFiles" ) );

    //cout << "Available branches : " << endl;
    //chain->Print();



  }
	return 0;
}
