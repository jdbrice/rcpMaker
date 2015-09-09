
#include "Logger.h"
#include "tof_eff_systematic.C"

string sig_file_name( string plc ){
	string base = "/Users/danielbrandenburg/bnl/local/data/RcpAnalysis/products/";
	string source = "fitSys";
	return base + source + "/Fit_PostCorr_" + plc +".root";
}

TH2 * sys_hist_for( string pre, string plc, string charge, string iCen){
	
	string fn = sig_file_name( plc );
	TRACE( "Loading from " << fn );
	
	TFile * f = new TFile( fn.c_str(), "READ" );

	if ( !f->IsOpen() )
		ERROR( "Cannot open " << fn );

	string name = pre + "_sigma_dist/sys_yield_" + plc + "_" + iCen + "_" + charge + "_" + plc;
	TH2 * h = (TH2D*) f->Get( name.c_str() );
	TRACE( "Hist = " << h );
	if ( !h ){
		ERROR( "Cannot Load " << name << " From " << fn );
		return new TH2D( "err", "err", 1, 0, 1, 1, 0, 1 );
	}

	h = (TH2*)h->Clone( (plc+charge+iCen+"sysyield").c_str() );

	h->SetDirectory( 0 );
	f->Close();

	return h;
}

vector<double> fit_sigma_systematic( string plc, string charge, string iCen ){
	

	TH2 * zb = sys_hist_for( "zb", plc, charge, iCen );
	TH2 * zd = sys_hist_for( "zb", plc, charge, iCen );

	vector<double> unc;
	for ( int i = 1; i <= zb->GetNbinsX(); i++ ){

		TH1D * zbproj = zb->ProjectionY( "_py", i, i );
		double uzb = zbproj->GetRMS();
		zbproj->Delete();

		TH1D * zdproj = zd->ProjectionY( "_py", i, i );
		double uzd = zdproj->GetRMS();
		zdproj->Delete();

		unc.push_back( sqrt( uzb*uzb + uzd*uzd ) );
		INFO( "", "["<< i << "] = " <<  unc[i] );
		
	}

	return unc;

}