#include "single_compare.C"



double total_systematics( int iPt, string plc ="Pi", string charge="p", string iCen="0" ){
	Logger::setGlobalLogLevel( Logger::llAll );

	vector<string> sources = { "yLocal_left", "zLocal_right", "dca_low", "nHitsFit_low", "nHitsDedx_low", "nHitsRatio_low" };

	vector<double> sigma;

	TH1 * nominal = yield_hist_for( "nominal", plc, charge, iCen );
	double yNominal = nominal->GetBinContent( iPt + 1 );

	INFO( "Nominal Yield = " << yNominal );

	for ( string source : sources ){

		TH1 * h = yield_hist_for( source, plc, charge, iCen );
		INFO( "" << source << "Yield = " << h->GetBinContent( iPt + 1 ) );

		double delta = nominal->GetBinContent( iPt + 1 ) - h->GetBinContent( iPt + 1 );
		INFO( "\t\tRealtive = " << delta / yNominal );

		INFO( "\t\tDelta = " << delta );
		sigma.push_back( delta );

	}

	
	double total_sqr = 0;
	for ( double v : sigma ){
		total_sqr += v*v;
	}
	double total = sqrt( total_sqr );

	INFO( "Total Realtive = " << total / yNominal );

	return total;
}