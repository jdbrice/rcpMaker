#include "single_compare.C"
#include "cov_matrix.C"


double total_systematics( int iPt, vector<double> &weights, vector<string> &sources, string plc ="Pi", string charge="p", string iCen="0" ){

	vector<double> sigma;
	vector<double> sigmaNW;

	TH1 * nominal = yield_hist_for( "nominal", plc, charge, iCen );
	double yNominal = nominal->GetBinContent( iPt );

	TRACE( "Nominal Yield = " << yNominal );

	int i = 0;
	for ( string source : sources ){

		TRACE( "getting hist for " << source  );
		TH1 * h = yield_hist_for( source, plc, charge, iCen );
		TRACE( "" << source << "Yield = " << h->GetBinContent( iPt ) );

		double delta = nominal->GetBinContent( iPt ) - h->GetBinContent( iPt );
		
		// is it consistent with zero?
		if ( nominal->GetBinError( iPt ) + h->GetBinError( iPt ) > delta )
				delta = 0.0;
		TRACE( "\t\tRealtive = " << delta / yNominal );

		TRACE( "\t\tDelta = " << delta );
		sigma.push_back( weights[ i ] * delta );
		sigmaNW.push_back( delta );
		i++;
	}

	
	double total_sqr = 0;
	for ( double v : sigma ){
		total_sqr += v*v;
	}

	double total_sqr_nw = 0;
	for ( double v : sigmaNW ){
		total_sqr_nw += v*v;
	}
	double total = sqrt( total_sqr );
	double totalNW = sqrt( total_sqr_nw );

	TRACE( "Total Realtive = " << total / yNominal );
	TRACE( "Total Realtive (No Correlation)= " << totalNW / yNominal );

	return total;
}