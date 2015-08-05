

void event(){

	RooPlotLib rpl;
	Reporter rp( "eventQA.pdf", 300, 300 );
	
	TFile * f = new TFile( "allQA.root", "READ" );
	string ipath = "imageEvent/";

	rp.newPage();
		rpl.style( (TH1*)f->Get( "event_cuts" ) ).set( "optstat", 0 ).set( "logy", 1 ).set( "xbr", 3, 6 ).draw();
	rp.saveImage( ipath + "cuts.eps" );
	rp.savePage();

	rp.newPage();
		rpl.style( (TH1*)f->Get( "event_refMult" ) ).set( "optstat", 0 ).set( "logy", 1 ).draw();
	rp.saveImage( ipath + "refMult.eps" );
	rp.savePage();

	rp.newPage();
		rpl.style( (TH1*)f->Get( "event_corrRefMult" ) ).set( "optstat", 0 ).set( "logy", 1 ).draw();
	rp.saveImage( ipath + "corrRefMult.eps" );
	rp.savePage();

	rp.newPage();
		rpl.style( (TH1*)f->Get( "event_nTofMatchA" ) ).set( "t", "# Tof-Matched Tracks" ).set( "optstat", 0 ).set( "logy", 1 ).draw();
	rp.saveImage( ipath + "nTofMatchA.eps" );
	rp.savePage();

	// rp.newPage();
	// 	rpl.style( (TH1*)f->Get( "" ) ).set( "optstat", 0 ).set( "logy", 1 ).draw();
	// rp.saveImage( ipath + ".eps" );
	// rp.savePage();

	rp.newPage();
		rpl.style( (TH1*)f->Get( "event_refMultBins" ) ).set( "optstat", 0 ).set( "logy", 1 ).draw();
	rp.saveImage( ipath + "refMultBins.eps" );
	rp.savePage();

	rp.newPage();
		rpl.style( (TH1*)f->Get( "event_pre_vZ" ) ).set( "optstat", 0 ).set( "logy", 1 ).draw();
	rp.saveImage( ipath + "pre_vZ.eps" );
	rp.savePage();

	rp.newPage();
		rpl.style( (TH1*)f->Get( "event_vZ" ) ).set( "optstat", 0 ).set( "logy", 0 ).set( "yr", 1.2e5, 1.6e5 ).draw();
	rp.saveImage( ipath + "vZ.eps" );
	rp.savePage();

	rp.newPage();
		rpl.style( (TH1*)f->Get( "event_pre_vX_vY" ) ).set( "optstat", 0 ).set("draw", "colz").draw();
	rp.saveImage( ipath + "pre_vX_vY.eps" );
	rp.savePage();

	rp.newPage();
		rpl.style( (TH1*)f->Get( "event_vX_vY" ) ).set( "optstat", 0 ).set("draw", "colz").draw();
	rp.saveImage( ipath + "vX_vY.eps" );
	rp.savePage();

	rp.newPage();
		rpl.style( (TH1*)f->Get( "event_pre_vR" ) ).set( "optstat", 0 ).set( "logy", 1 ).draw();
	rp.saveImage( ipath + "pre_vR.eps" );
	rp.savePage();

	/*rp.newPage();
		rpl.style( (TH1*)f->Get( "" ) ).set( "optstat", 0 ).set( "logy", 1 ).draw();
	rp.saveImage( ipath + ".eps" );
	rp.savePage();

	rp.newPage();
		rpl.style( (TH1*)f->Get( "" ) ).set( "optstat", 0 ).set( "logy", 1 ).draw();
	rp.saveImage( ipath + ".eps" );
	rp.savePage();*/

	f->Close();






}