
#include <iostream>
#include "allroot.h"
#include "constants.h"
#include "HistoBook.h"
#include "XmlConfig.h"
#include "Utils.h"
#include "LoggerConfig.h"
#include "ChainLoader.h"

using namespace jdb;

#include "TChain.h"
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
Logger *logger;

int unitTest_Utils(){

	cout << endl << endl << "jdbUtils Tests: " << endl << endl;
	cout << " int to string " << ts( 1000 ) << endl;
	cout << " double to string " << ts( 1000.123123 ) << endl;
	cout << " float to string " << ts( 1000.123f ) << endl;

	TaskTimer tt;
	tt.start();

	int n = 10000000;
	TaskProgress tp( "taskProgress Test", n );
	for ( int i = 0; i < n; i++ ){
	  
	  tp.showProgress( i ); 
	  
	}

	return 0;
}


int unitTest_HistoBook( XmlConfig * config ){

	cout << endl << endl << "HistoBook Tests: " << endl << endl;
	HistoBook* book = new HistoBook( "out.root", config );

	book->makeAll( "h" );
	book->clone( "histo1", "histo2" );

	book->make1D( "hTest", "testing histogram", 10, 0, 1);
	book->draw( "tommy" );
	
	logger->info() << " green is : " << book->color( "green" ) << endl;
	
	vector<double> bins = HistoBook::makeNBins( 10, 0, 10 );

	for ( int i = -1; i< 12; i ++ ){
		cout << " value " << i << " is in bin [ " << HistoBook::findBin( bins, i ) << " ] " << endl; 
	}

	vector<double> bins2 = HistoBook::makeFixedWidthBins( .25, 0, 10 );
	for ( double i = -1; i< 12; i += .5 ){
		cout << " value " << i << " is in bin [ " << HistoBook::findBin( bins2, i ) << " ] " << endl; 
	}

	// HistoBins helper class
	HistoBins hbX( config, "h.histo1", "X" );
	logger->info(__FUNCTION__) << hbX.toString() << endl;
	HistoBins hbY( config, "h.histo1", "Y" );
	logger->info(__FUNCTION__) << hbY.toString() << endl;


	delete book;

	return 0;
}




int main( int argc, char* argv[] ) {


	
	
	if ( argc >= 2 ){
		XmlConfig config( argv[ 1 ] );
		
		// create a logger from the configuration options
		logger = LoggerConfig::makeLogger( &config, "Logger" );
		
		//config.report();
		//config.childrenOf( "input" );
	   	logger->warn() << " this will show if the log level is at least warning " << endl;
	   	logger->error() << " this will show if the log level is at least error " << endl;
		logger->info(__FUNCTION__) << " this will show if the log level is at least info and will display the calling function name " << endl;
		
		logger->info( __FUNCTION__ ) << "trimming trailing 0s from 10.00000 : " << trimTrailingZeros( "10.00000" ) << endl; 

		unitTest_HistoBook( &config );
		

		if ( argc >= 4 ){

			logger->info(__FUNCTION__) << " List File : " << argv[ 2 ] << ", job prefix " << argv[3] << endl;

			TChain * chain = new TChain( "tree" );
			ChainLoader::loadList( chain, (string)argv[2] );

		}
	   
		
	} 

	unitTest_Utils();


	return 0;
}
