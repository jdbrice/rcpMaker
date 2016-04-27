
#include "/Users/danielbrandenburg/bnl/local/work/rcpMaker/include/Spectra/PidProjector.h"
#include "UnitTest/UnitTest.h"

#include "run_simple_fit.C"

int cW = 350;
int cH = 350;

TCanvas *  newCan( string title,int nW = 1, int nH = 1 ){
	return new TCanvas( title.c_str(), title.c_str(), cW * nW, cH * nH );
}

void high_pt( int iPt = 12, string df = "/Users/danielbrandenburg/bnl/local/data/RcpAnalysis/products/nominal/PidData_PostCorr_Pi.root", string plc = "Pi", string charge = "p", string iCen = "0" ){

	string name = plc + "_" + charge + "_" + iCen + "_" + ts(iPt);
	RooPlotLib rpl;


	string tag = "UnitTest";
	TFile * f = new TFile( df.c_str(), "READ" );

	TH1 * h_pt = (TH1*) f->Get( "pt" );
	double pt_low = h_pt->GetBinLowEdge( iPt );
	double pt_high = h_pt->GetBinLowEdge( iPt ) + h_pt->GetBinWidth( iPt );

	Logger::setGlobalLogLevel( Logger::llAll );
	PidProjector pp( f, 0.006, 0.007 );
	
	// for 15th ptBin
	pp.cutDeuterons( 0.06, 0.012, 3 );
	pp.cutElectrons( -0.005, 0.012, 3 );

	TCanvas * c1 = newCan( "2D projection" );
	INFO( tag, "Projecting 2D" );
	TH2 * h_2d = pp.project2D( name );
	string test = UT_R_NAMED( h_2d, plc + "_p_0_15_2D" );
	rpl.style( h_2d ).set( "title", dts(pt_low) + " < pT < " + dts(pt_high) );
	INFO( tag, test);
	c1->SetWindowPosition( 1, 1 );

	TCanvas * c2 = newCan( "1D projection of zb", 3, 3 );
	TH1 * h_zb = pp.project1D( name, "zb" );
	test = UT_R_NAMED( h_zb, plc + "_p_0_15_zb_1D" );
	INFO( tag, test);
	c2->SetWindowPosition( cW, 0 );

	// iPt = 29
	// run_simple_fit( h_zb, -0.01, 0.01, 0.06 );
	// iPt = 30
	// run_simple_fit( h_zb, -0.01, 0.01, 0.06 );
	// iPt = 31
	// run_simple_fit( h_zb, -0.01, 0.01, 0.06 );
	// iPt = 33
	// run_simple_fit( h_zb, -0.01, 0.001, 0.03, 0.012, 0.006 );
	// iPt = 33
	run_simple_fit( h_zb, -0.015, -0.01, 0.02, 0.012, 0.006 );

	// TCanvas * c3 = newCan( "1D projection of zd" );
	// test = UT_R_NAMED( pp.project1D( name, "zd" ), plc + "_p_0_15_zd_1D" );
	// INFO( tag, test);
	// c3->SetWindowPosition( 2*cW, 0 );


	

}