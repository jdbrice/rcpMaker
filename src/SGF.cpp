#include "SGF.h"


SGF::SGF( shared_ptr<SGFSchema> _schema, TFile * _in ){
	schema = _schema;
	inFile = _in;
}

void SGF::fit( string cs, int charge, int cenBin, int ptBin, int etaBin, bool fitRange ){

	title = cs + 	"_" + PidPhaseSpace::chargeString( charge ) + 
					"_" + ts(cenBin) + "_" + ts(ptBin) + "_" + ts(etaBin); 
	/**
	 * Get all of the data histograms
	 */
	map< string, TH1D* > zb;
	map< string, TH1D* > zd;
	zb[ "zb_All" ] 	= (TH1D*)inFile->Get( ("tof/" + PidPhaseSpace::tofName( cs, charge, cenBin, ptBin, etaBin )).c_str() );
	zd[ "zd_All" ] 	= (TH1D*)inFile->Get( ("dedx/" + PidPhaseSpace::dedxName( cs, charge, cenBin, ptBin, etaBin )).c_str() );
	
	// dEdx enhanced distributions
	zd[ "zd_K" ]	= (TH1D*)inFile->Get( ("dedx/" + PidPhaseSpace::dedxName( cs, charge, cenBin, ptBin, etaBin, "K" )).c_str() );
	zd[ "zd_Pi" ]	= (TH1D*)inFile->Get( ("dedx/" + PidPhaseSpace::dedxName( cs, charge, cenBin, ptBin, etaBin, "Pi" )).c_str() );
	zd[ "zd_P" ]	= (TH1D*)inFile->Get( ("dedx/" + PidPhaseSpace::dedxName( cs, charge, cenBin, ptBin, etaBin, "P" )).c_str() );

	// 1/beta enhanced distributions
	zb[ "zb_Pi" ] 	= (TH1D*)inFile->Get( ("tof/" + PidPhaseSpace::tofName( cs, charge, cenBin, ptBin, etaBin, "Pi" )).c_str() );
	zb[ "zb_K" ] 	= (TH1D*)inFile->Get( ("tof/" + PidPhaseSpace::tofName( cs, charge, cenBin, ptBin, etaBin, "K" )).c_str() );
	zb[ "zb_P" ] 	= (TH1D*)inFile->Get( ("tof/" + PidPhaseSpace::tofName( cs, charge, cenBin, ptBin, etaBin, "P" )).c_str() );

	// update the histogram data
	schema->updateData( zb, "zb" );
	schema->updateData( zd, "zd" );

	// combine the data into a simultaneous dataset
	schema->combineData();

	// gets the combined data
	RooDataHist * d = schema->data( );

	// do the simultaneous fit with the full simultaneous model
	RooSimultaneous * sim = schema->model();

	if ( !fitRange ) // Do not constrain fit range
		sim->fitTo( *d /*, PrintLevel(-1), Verbose( kFALSE )*/ );
	else { // constrain fit Range

		//double zbMin = schema->var( "zb_mu_Pi" )->getVal() - schema->var( "zb_sigma_Pi" )->getVal() * 4;
		//double zbMax = schema->var( "zb_mu_P" )->getVal() + schema->var( "zb_sigma_Pi" )->getVal() * 4;

		//schema->var( "zb" )->setRange( "roi", zbMin, zbMax  );

		//double zdMin = schema->var( "zd_mu_Pi" )->getVal() - schema->var( "zd_sigma_Pi" )->getVal() * 4;
		//double zdMax = schema->var( "zd_mu_P" )->getVal() + schema->var( "zd_sigma_Pi" )->getVal() * 4;

		//schema->var( "zd" )->setRange( "roi", zdMin, zdMax  );

		//sim->fitTo( *d, Range( "roi" ) );
	}
}


void SGF::showSample( string var, string sample ){

	const string cutStr 	= "sample==sample::";
	RooRealVar * rrv 		= schema->var( var.c_str() ); 
	RooPlot * f 			= rrv->frame( Title( (sample + ": " + title).c_str() ) );
	RooCategory * cat 		= schema->category();
	RooDataHist * d 		= schema->data( "" );
	RooSimultaneous * sim 	= schema->model();

	d->plotOn(f, Cut( ( cutStr + sample).c_str()  ));
	
	sim->plotOn( f, Slice(*cat, sample.c_str() ), ProjWData(*cat,*d) );
	sim->plotOn( f, Slice(*cat, sample.c_str() ), ProjWData(*cat,*d), Components( (sample + "_gauss_Pi").c_str() ), LineColor( kRed ) );
	sim->plotOn( f, Slice(*cat, sample.c_str() ), ProjWData(*cat,*d), Components( (sample + "_gauss_K").c_str() ), LineColor( kBlack ) );
	
	f->SetMinimum( .1 );
	f->SetMaximum( 1000000 );

	gPad->SetLogy(1);
	 
	f->Draw(); 
}


void SGF::report( Reporter* rp ){

	rp->newPage(2, 2);
		rp->cd( 1, 1 );
		showSample( "zd", "zd_All" );
		rp->cd( 2, 1 );
		showSample( "zd", "zd_K" );
		rp->cd( 1, 2 );
		showSample( "zd", "zd_Pi" ); 
		rp->cd( 2, 2 );
		showSample( "zd", "zd_P" ); 
	rp->savePage();
		rp->cd( 1, 1 );
		showSample( "zb", "zb_All" );
		rp->cd( 2, 1 );
		showSample( "zb", "zb_K" );
		rp->cd( 1, 2 );
		showSample( "zb", "zb_Pi" ); 
		rp->cd( 2, 2 );
		showSample( "zb", "zb_P" ); 
	rp->savePage();

}




