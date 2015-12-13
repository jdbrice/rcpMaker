#include "Logger.h"
#include "total_systematics.C"

map< string, double> max_yield = {
	/* Pi Plus */
	{ "Pi_p_0", 3.5 },
	{ "Pi_p_1", 3.5 },
	{ "Pi_p_2", 3.0 },
	{ "Pi_p_3", 3.0 },
	{ "Pi_p_4", 3.0 },
	{ "Pi_p_5", 3.0 },
	{ "Pi_p_6", 3.0 },
	/* Pi Minus */
	{ "Pi_n_0", 3.5 },
	{ "Pi_n_1", 3.5 },
	{ "Pi_n_2", 3.5 },
	{ "Pi_n_3", 3.5 },
	{ "Pi_n_4", 2.9 },
	{ "Pi_n_5", 2.8 },
	{ "Pi_n_6", 2.8 },

	/* P Plus */
	{ "P_p_0", 3.5 },
	{ "P_p_1", 3.5 },
	{ "P_p_2", 3.5 },
	{ "P_p_3", 3.5 },
	{ "P_p_4", 3.5 },
	{ "P_p_5", 3.5 },
	{ "P_p_6", 3.5 },
	/* P Minus */
	{ "P_n_0", 3.0 },
	{ "P_n_1", 3.0 },
	{ "P_n_2", 3.0 },
	{ "P_n_3", 3.0 },
	{ "P_n_4", 3.0 },
	{ "P_n_5", 2.8 },
	{ "P_n_6", 2.4 },

	/* K Plus */
	{ "K_p_0", 3.0 },
	{ "K_p_1", 3.0 },
	{ "K_p_2", 3.0 },
	{ "K_p_3", 3.0 },
	{ "K_p_4", 2.8 },
	{ "K_p_5", 2.8 },
	{ "K_p_6", 2.6 },
	/* K Minus */
	{ "K_n_0", 3.0 },
	{ "K_n_1", 3.0 },
	{ "K_n_2", 3.0 },
	{ "K_n_3", 2.8 },
	{ "K_n_4", 2.8 },
	{ "K_n_5", 2.6 },
	{ "K_n_6", 2.4 },
};


vector<string> plcs 	= { "Pi", "K", "P" };
vector<string> charges 	= { "p", "n" };
vector<string> centralities = { "0", "1", "2", "3", "4", "5", "6" };

string energy = "14.5";
string tag = "export_spectra";


void write_spectra( string plc, string charge, string iCen, vector<string> &sources, vector<double> &weights ){
	INFO( tag, "(plc=" << plc << ", charge=" << charge << ", iCen=" << iCen <<" )" );

	string base = "/Users/danielbrandenburg/bnl/local/data/RcpAnalysis/spectra/";
	TH1 * h = yield_hist_for( "qm_nominal", plc, charge, iCen );

	vector<double> sys_unc = total_systematics( weights, sources, plc, charge, iCen );


	// open the files
	string name = base + "spectra_" + energy + "_" + plc + "_" + charge + "_" + iCen + ".dat";
	ofstream fout( name.c_str() );

	fout << std::left << std::setw(20) << "pT" << std::setw(20) << "value" << std::setw(20) << "stat_unc" << std::setw(20) << "sys_unc" << endl;

	for ( int i = 1; i <= h->GetNbinsX(); i++ ){

		double pT = h->GetBinCenter( i );
		if ( pT < 0.5 )
			continue;
		double max_pT = max_yield[ plc + "_" + charge + "_" + iCen ];
		if ( pT > max_pT )
			continue;
		
		double value = h->GetBinContent( i );
		double stat = h->GetBinError( i );
		double sys =  sys_unc[ i - 1 ];
		if ( 0 >= sys )
			sys = 1e-10;


		fout << std::setprecision( 10 ) << std::left << std::setw(20) << pT << std::left << std::setw(20) << value << std::left << std::setw(20) << stat << std::left << std::setw(20) << sys << endl; 

	}



	fout.close();

}

void export_spectra(  ){
	Logger::setGlobalLogLevel( Logger::llAll );
	
	// first calculate the cov matrix for our cut variables
	vector<string> sources = { "zLocal_right", "dca_low", "nHitsFit_low", "nHitsDedx_low", "nHitsRatio_low", "tpcEff_low", "feedDown_low" };
	vector<string> source_vars = { "zLocal", "dca", "nHitsFit", "nHitsDedx", "nHitsPossible" };
	vector<double> weights;
	
	// calculate the cov_matrix
	TMatrixD cut_cov = cov_matrix( source_vars, "matchFlag>=1" );
	for ( int i = 0; i < source_vars.size(); i++ ){
		weights.push_back( error_weight( cut_cov, i ) );
		INFO( "w[" << source_vars[ i ] << "] = " << weights[ i ] );
	}	

	weights.push_back( 1.0 );
	weights.push_back( 1.0 );

	// return;

	Logger::setGlobalColor( true );
	Logger::setGlobalLogLevel( Logger::llWarn );
	for ( string plc : plcs ){
		for ( string charge : charges  ){
			for ( string iCen : centralities ){
				write_spectra( plc, charge, iCen, sources, weights );			
			}
		}
	}
}