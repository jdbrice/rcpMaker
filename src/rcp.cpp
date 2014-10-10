

#include "XmlConfig.h"
using namespace jdb;


#include <iostream>
#include "InclusiveSpectra.h"

int main( int argc, char* argv[] ) {

  if ( argc >= 2 ){

    XmlConfig config( argv[ 1 ] );
    config.report();




  }
	return 0;
}
