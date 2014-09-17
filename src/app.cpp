
#include <iostream>
#include "allroot.h"
#include "constants.h"
#include "HistoBook.h"
#include "XmlConfig.h"
#include "jdbUtils.h"

using namespace jdbUtils;

/* 
*
*	GUI Application
*
*/
/*
int main(int argc, char* argv[]) {

  TApplication* rootapp = new TApplication("example",&argc, argv);

  TRandom1* myrand = new TRandom1();
  TH1F* myhist = new TH1F("stats","",100,0,10);
  for(int i=0;i<10000;++i) {
    myhist->Fill(myrand->Gaus(5,1));
  }
  myhist->Draw();
  rootapp->Run();
  return 0;
}
*/


int unitTest_jdbUtils(){

	cout << endl << endl << "jdbUtils Tests: " << endl << endl;
    cout << " int to string " << ts( 1000 ) << endl;
    cout << " double to string " << ts( 1000.123123 ) << endl;
    cout << " float to string " << ts( 1000.123f ) << endl;

    taskTimer tt;
    tt.start();

    taskProgress tp( "taskProgress Test", 2000000 );
    for ( int i = 0; i < 2000000; i++ ){
      
      tp.showProgress( i );
    }

    return 0;
}


int unitTest_HistoBook( XmlConfig * config ){

	cout << endl << endl << "HistoBook Tests: " << endl << endl;
	HistoBook* book = new HistoBook( "out.root", config );

    book->makeAll( "h" );

    book->make1D( "hTest", "testing histogram", 10, 0, 1);
    book->draw( "tommy" );
    
    //cout << " red is : " << book->color( "green" ) << endl;
    //book->style( "a" );
    //book->style( "b" );
    
    //book->style( "a" )->draw();
    //book->clone( "a", "aa" );

    delete book;

}



Logger logger;
int main( int argc, char* argv[] ) {

	logger.setLogLevel( Logger::llAll );
	
    if ( argc >= 2 ){
        XmlConfig config( argv[ 1 ] );
        
        //config.report();
        //config.childrenOf( "input" );
       
        logger.info(__FUNCTION__) << " input file : " << config.getString( "input.rootIn:file" ) << endl;
        
        unitTest_HistoBook( &config );
        
       
        
    }

    unitTest_jdbUtils();


	return 0;
}
