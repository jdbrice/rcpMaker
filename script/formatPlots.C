

#include "TCanvas.h"

void formatPlots(){

	TCanvas * c = new TCanvas( "c", "C", 400, 400 );

	TFile * f = new TFile( "pidPhaseSpace.root", "READ" );

	gStyle->SetOptStat(0);


	/**
	 * dEdx
	 */
	TH1D* dedx = (TH1D*)f->Get( "dedx" );
	dedx->Draw( "colz" );
	dedx->GetXaxis()->SetTitle("P[GeV]");
	gPad->SetLogz(1);
	c->Print( "dedx.pdf" );

	TH1D* trDedx = (TH1D*)f->Get( "trDedx" );
	trDedx->Draw( "colz" );
	trDedx->GetXaxis()->SetTitle("P[GeV]");
	gPad->SetLogz(1);
	c->Print( "trDedx.pdf" );

	TH1D* nlDedx = (TH1D*)f->Get( "nlDedx" );
	nlDedx->Draw( "colz" );
	nlDedx->GetXaxis()->SetTitle("P[GeV]");
	gPad->SetLogz(1);
	c->Print( "nlDedx.pdf" );

	/**
	 * Tof
	 */
	TH1D* tof = (TH1D*)f->Get( "beta" );
	tof->Draw( "colz" );
	tof->GetXaxis()->SetTitle("P[GeV]");
	gPad->SetLogz(1);
	c->Print( "tof.pdf" );

	TH1D* trBeta = (TH1D*)f->Get( "trBeta" );
	trBeta->Draw( "colz" );
	//trBeta->GetXaxis()->SetTitle("P[GeV]");
	gPad->SetLogz(1);
	c->Print( "trBeta.pdf" );

	TH1D* nlBeta = (TH1D*)f->Get( "nlBeta" );
	nlBeta->Draw( "colz" );
	//nlBeta->GetXaxis()->SetTitle("P[GeV]");
	gPad->SetLogz(1);
	c->Print( "nlBeta.pdf" );

	TCanvas * c1 = new TCanvas( "c1", "C1", 400, 400 );

	TH1D* dedxtof = (TH1D*)f->Get( "dedx_tof/dedx_tof_a_K_4_0" );
	dedxtof->Draw("colz");
	gPad->SetLogz(1);
	c1->Print( "dedxtof.pdf");

}