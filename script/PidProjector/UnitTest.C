
#include "/Users/danielbrandenburg/bnl/local/work/rcpMaker/include/Spectra/PidProjector.h"
#include "UnitTest/UnitTest.h"

int cW = 350;
int cH = 350;

TCanvas *  newCan( string title ){
	return new TCanvas( title.c_str(), title.c_str(), cW, cH );
}

void UnitTest( string df, string plc = "Pi" ){

	string name = plc + "_p_0_15";


	string tag = "UnitTest";
	TFile * f = new TFile( df.c_str(), "READ" );

	Logger::setGlobalLogLevel( Logger::llAll );
	PidProjector pp( f, 0.006, 0.007 );
	
	// for 15th ptBin
	pp.cutDeuterons( 0.5, 0.012, 3 );
	pp.cutElectrons( 	-0.15, -0.11, 0.012, 0.07,		// pion 
						0, 0, 0.012, 0.07,		// kaon
						-0.16, 0.3, 0.012, 0.07, // electron
						4, 12, 3 ); // illustrates the proper behavior as a UT should

	TCanvas * c1 = newCan( "2D projection" );
	INFO( tag, "Projecting 2D" );
	string test = UT_R_NAMED( pp.project2D( name ), "K_p_0_15_2D" );
	INFO( tag, test);
	c1->SetWindowPosition( 0, 0 );

	TCanvas * c2 = newCan( "1D projection of zb" );
	test = UT_R_NAMED( pp.project1D( name, "zb" ), "K_p_0_15_zb_1D" );
	INFO( tag, test);
	c2->SetWindowPosition( cW, 0 );

	TCanvas * c3 = newCan( "1D projection of zd" );
	test = UT_R_NAMED( pp.project1D( name, "zd" ), "K_p_0_15_zd_1D" );
	INFO( tag, test);
	c3->SetWindowPosition( 2*cW, 0 );

	TCanvas * c4 = newCan( "1D projection of zd with 1sigma Kaon cut on zb" );
	test = UT_R_NAMED( pp.projectEnhanced( name, "zd", "K", 0.0-0.012, 0.0+0.012 ), "K_p_0_15_zd_1D_K" );
	INFO( tag, test);
	c4->SetWindowPosition( 0, cH );
}