
#include "Reporter.h"
#include "RooPlotLib.h"

map< string, double> cov_matrix( vector<string> sources, string cut ){

	Reporter rp( "rp_cov_matrix.pdf", 600, 600 );
	RooPlotLib rpl;

	TChain * c = new TChain( "rcpPicoDst" );
	c->Add( "/Users/danielbrandenburg/bnl/local/data/RcpAnalysis/data/RcpPicoDst/tuple_93477DF676758D2254762F71F52E3D44_14*.root" );

	// first make histograms of each source
	map< string, TH1* > hSource;
	map< string, double > mu;
	map< string, double > sigma;

	for ( string source : sources ){
		c->Draw( (source + " >> h_" + source).c_str(), cut.c_str() );
		hSource[ source ] = (TH1*) gDirectory->Get( ("h_" + source ).c_str() );

		mu[ source ] = hSource[ source ]->GetMean();
		sigma[ source ] = hSource[ source ]->GetRMS();

		rpl.style( hSource[ source ] ) .set( "title", source ).draw();

		rp.savePage();

	}

	map< string, TH1 * > pairs;
	map< string, double > coef;
	map< string, double > u_coef;
	
	int i = 0;
	for ( string source1 : sources ){
		int j = 0;
		for ( string source2 : sources ){
			if ( source1 == source2 ){
				j++;
				continue;
			}
			if ( j > i )
				break;

			string expr = "( " + source1 + " - " + dts( mu[source1] ) + " ) * ( " + source2 + " - " + dts( mu[source2] ) + " ) / ( " + dts( sigma[source1] * sigma[source2] ) + " ) >> h_" + source1 + "_" + source2 ;
			c->Draw( expr.c_str(), cut.c_str() );

			string pName = source1+"_"+source2;
			pairs[ pName ] = (TH1*)gDirectory->Get( ("h_" + source1 + "_" + source2).c_str() );

			coef[ pName ] = pairs[ pName ]->GetMean();
			u_coef[ pName ] = pairs[ pName ]->GetRMS();

			rpl.style( pairs[ pName ] ) .set( "title", source1 + " X " + source2 ).draw();

			rp.savePage();
			j++;
		}
		i++;
	}

	return coef;
}