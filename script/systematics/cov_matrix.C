
#include "Reporter.h"
#include "RooPlotLib.h"


// row then column
string toString( TMatrixD &mat ){

	string str = "{ ";
	for ( int i = 0; i < mat.GetNrows(); i++ ){
		str += "{ ";
		for ( int j = 0; j <mat.GetNcols(); j++ ){
			if ( j < mat.GetNcols() - 1 )
				str += dts(mat( i, j )) + ", ";
			else 
				str += dts(mat( i, j ));
		}

		if ( i < mat.GetNrows() - 1 )
			str += " }, \n";
		else
			str += " }";
	}

	str += " }";
	return str;
}

double error_weight( TMatrixD &cov, int i ){
	int ncol = cov.GetNcols();
	int nrow = cov.GetNrows();

	TMatrixD inv_cov(cov);

	// avvoid singular matrices
	inv_cov(1, 1) = inv_cov(1, 1) + 0.00001;
	double det;
	inv_cov.InvertFast( &det );

	double num = 0;
	for ( int k = 0; k < ncol; k++ ){
		num += inv_cov( i, k );
	} 

	double den = 0;

	for ( int j = 0; j < nrow; j ++ ){
		for ( int k = 0; k < ncol; k ++ ){
			den += inv_cov( j, k );
		}
	}

	return num / den;
}

TMatrixD cov_matrix( vector<string> sources, string cut ="", bool corr = false ){

	Reporter rp( "rp_cov_matrix.pdf", 600, 600 );
	RooPlotLib rpl;

	TChain * c = new TChain( "rcpPicoDst" );
	c->Add( "/Users/danielbrandenburg/bnl/local/data/RcpAnalysis/data/RcpPicoDst/tuple_93477DF676758D2254762F71F52E3D44_1*.root" );

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
	TMatrixD cov_mat( sources.size(), sources.size() );

	int i = 0;
	for ( string source1 : sources ){
		int j = 0;
		for ( string source2 : sources ){
			if ( corr && source1 == source2 ){
				j++;
				continue;
			}
			if ( corr && j > i )
				break;

			// default to cov matrix
			string expr = "( " + source1 + " - " + dts( mu[source1] ) + " ) * ( " + source2 + " - " + dts( mu[source2] ) + " ) >> h_" + source1 + "_" + source2 ;
			

			if ( corr ) // make the correlation coeff instead
				expr = "( " + source1 + " - " + dts( mu[source1] ) + " ) * ( " + source2 + " - " + dts( mu[source2] ) + " ) / ( " + dts( sigma[source1] * sigma[source2] ) + " ) >> h_" + source1 + "_" + source2 ;

			c->Draw( expr.c_str(), cut.c_str() );

			string pName = source1+"_"+source2;
			pairs[ pName ] = (TH1*)gDirectory->Get( ("h_" + source1 + "_" + source2).c_str() );

			coef[ pName ] = pairs[ pName ]->GetMean();
			u_coef[ pName ] = pairs[ pName ]->GetRMS();

			rpl.style( pairs[ pName ] ) .set( "title", source1 + " X " + source2 ).draw();

			rp.savePage();

			cov_mat( i, j ) = pairs[ pName ]->GetMean();

			j++;
		}
		i++;
	}

	cout << toString( cov_mat ) << endl;



	return cov_mat;
}