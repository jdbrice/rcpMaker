

#include "Utils.h"

double correlation_coef( string source1, string source2 ){

	TChain * c = new TChain( "rcpPicoDst" );

	c->Add( "/Users/danielbrandenburg/bnl/local/data/RcpAnalysis/data/RcpPicoDst/tuple_93477DF676758D2254762F71F52E3D44_14*.root" );

	TCanvas * c1 = new TCanvas( "c1", source1.c_str() );
	c->Draw( (source1 + " >> hSource1 ").c_str() );

	TCanvas * c2 = new TCanvas( "c2", source2.c_str() );
	c->Draw( (source2 + " >> hSource2 ").c_str() );

	TH1 * hSource1 = (TH1*)gDirectory->Get( "hSource1" );
	TH1 * hSource2 = (TH1*)gDirectory->Get( "hSource2" );

	double m1 = hSource1->GetMean();
	double m2 = hSource2->GetMean();

	double s1 = hSource1->GetRMS();
	double s2 = hSource2->GetRMS();

	TCanvas * c3 = new TCanvas( "c3", "correlation" );
	string expr = "( " + source1 + " - " + dts( m1 ) + " ) * ( " + source2 + " - " + dts( m2 ) + " ) / ( " + dts( s1 * s2 ) + " ) >> hCorr";
	c->Draw( expr.c_str() );

	TH1 * hCorr = (TH1*)gDirectory->Get( "hCorr" );


	return m1;

}



