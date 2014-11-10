
#include "TH1D.h"

void demoSim(){
	
	TFile * f = new TFile( "~/bnl/local/data/pidPSOct29.root", "READ" );

	TCanvas * c2 = new TCanvas( "c2", "c2", 400, 400 );

	TH2D* betaRaw = (TH2D*)f->Get("betaRaw");
	
	TH1D* py = betaRaw->ProjectionY("py", 15, 15 );
	py->GetXaxis()->SetRangeUser( 0, 1.6 );

	gStyle->SetOptStat( 0 );

	py->Sumw2();
	py->Scale( 1.0 / py->Integral() );
	py->Draw( "pe" );
	py->SetTitle( "" );

	py->SetLineWidth( 5 );
	py->SetLineColor( kBlack );
	gPad->SetLogy();
	TF1 * gPi = new TF1( "gPi", "gaus", .8, 1.2 );
	gPi->SetParameters( .01, 1.0, 0.012 );
	py->Fit( gPi, "RN" );
	gPi->SetParameter( 2, 0.012 );
	gPi->SetParameter( 0, .005 );

	TF1 * gK = new TF1( "gK", "gaus", 0, 6 );
	gK->SetParameters( .01, 1.1, 0.012 );
	py->Fit( gK, "RN" );
	gK->SetParameter( 2, 0.012 );
	gK->SetParameter( 0, .005 );

	TF1 * gP = new TF1( "gP", "gaus", 1.3, 1.6 );
	gP->SetParameters( .01, 1.1, 0.012 );
	py->Fit( gP, "RN" );
	gP->SetParameter( 2, 0.012 );
	gP->SetParameter( 0, .005 );

	gP->SetNpx( 1000 );
	gPi->SetNpx( 1000 );
	gK->SetNpx( 1000 );

	gP->SetLineWidth( 2 );
	gPi->SetLineWidth( 2 );
	gK->SetLineWidth( 2 );

	gP->Draw("SAME");
	gPi->Draw("SAME");
	gK->Draw("SAME");

}