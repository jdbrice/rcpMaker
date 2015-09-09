
#include "Logger.h"

string fit_file_name( string plc ){
	string base = "/Users/danielbrandenburg/bnl/local/data/RcpAnalysis/products/";
	string source = "fitTofSchema";
	return base + source + "/Fit_PostCorr_" + plc +".root";
}

TH2 * sys_hist_for( string plc, string charge, string iCen){
	
	string fn = fit_file_name( plc );
	TRACE( "Loading from " << fn );
	
	TFile * f = new TFile( fn.c_str(), "READ" );

	if ( !f->IsOpen() )
		ERROR( "Cannot open " << fn );

	string name = "tofEff_dist/sys_yield_" + plc + "_" + iCen + "_" + charge + "_" + plc;
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

vector<double> tof_eff_systematic( string plc, string charge, string iCen ){
	
	//Logger::setGlobalLogLevel( Logger::llAll );

	TH2 * h = sys_hist_for( plc, charge, iCen );

	vector<double> unc;
	for ( int i = 1; i <= h->GetNbinsX(); i++ ){

		TH1D * proj = h->ProjectionY( "_py", i, i );
		unc.push_back( proj->GetRMS() );
		INFO( "", "["<< i << "] = " <<  proj->GetRMS() );
		proj->Delete();
	}

	return unc;

}