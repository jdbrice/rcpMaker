
#include "/Users/danielbrandenburg/bnl/local/work/rcpMaker/include/Spectra/PidProjector.h"
#include "UnitTest/UnitTest.h"

TCanvas *  newCan( string title ){
	return new TCanvas( title.c_str(), title.c_str(), 400, 400 );
}

void UnitTest( string df ){

	string tag = "UnitTest";
	TFile * f = new TFile( df.c_str(), "READ" );

	Logger::setGlobalLogLevel( Logger::llAll );
	PidProjector pp( f, 0.006, 0.007 );
	pp.cutDeuterons( 0.5, 0.012, 3 );

	TCanvas * c1 = newCan( "2D projection" );
	INFO( tag, "Projecting 2D" );
	pp.project2D( "PidPoints/Pi_p_0_15" );

	TCanvas * c2 = newCan( "1D projection of zb" );
	pp.project1D( "PidPoints/Pi_p_0_15", "zb" );

	TCanvas * c3 = newCan( "1D projection of zd" );
	pp.project1D( "PidPoints/Pi_p_0_15", "zd" );

	TCanvas * c4 = newCan( "1D projection of zd with 1sigma Kaon cut on zb" );
	pp.projectEnhanced( "PidPoints/Pi_p_0_15", "zd", 0.14-0.012, 0.14+0.012 );

}