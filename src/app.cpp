
#include <iostream>
#include "allroot.h"
#include "constants.h"
#include "histoBook.h"
#include "xmlConfig.h"
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

int main( int argc, char* argv[] ) {

	cout << "const: " << Constants::nChannels << endl;
    if ( argc >= 2 ){
        xmlConfig config( argv[ 1 ] );
        config.report();

        config.childrenOf( "input" );
        cout << "file: " << config.getString( "input.rootIn:file" ) << endl;

        histoBook* book = new histoBook( "out.root", &config, config.getString( "input.rootIn:file" ) );

        cout << " book : " << book->get( "nSig_K_All" ) << endl;

        if ( book->exists( "nSig_K_All" ) )
            book->style( "nSig_K_All" )->draw( )->exportAs();

        book->set( &config, "style.s1" );
        book->set( "legend", "help", "lpf" );

        cout << "bins.p" << config.getString( "bins.p" ) << endl;

        config.getDoubleVector( "textText" );

        book->makeAll( "h" );

        cout << " red is : " << book->color( "green" ) << endl;
        book->style( "a" );
        book->style( "b" );
        //book->set( "hello", vector<string>( {"1", "2"} ) ) ;

        vector<double> t = config.getDoubleVector( "bins.p2" );
        

        delete book;

        cout << "jdbUtils Tests: " << endl;
        cout << " int to string " << ts( 1000 ) << endl;
        cout << " double to string " << ts( 1000.123123 ) << endl;
        cout << " float to string " << ts( 1000.123f ) << endl;

        taskTimer tt;
        tt.start();
        for ( int i = 0; i < 2000; i++ ){
          progressBar( i, 2000 );
        }
        cout << "Elapsed: " << tt.elapsed() << " [sec] " << endl;
        
    }


	return 0;
}
