


void track(){

	XmlConfig cfg( "styleTrack.xml" );

	RooPlotLib rpl;
	Reporter rp( "qaTrack.pdf", 300, 300 );
	TFile * f = new TFile( "allQA.root", "READ" );
	string ipath = "imageTrack/";

	rpl.defaultStyle( &cfg, "DefaultStyle" );

	rp.newPage();
		rpl.style( (TH1*)f->Get( "track_cuts" ) ).set( "logy", 1 ).set( "xbr", 1, 12 ).draw();
	rp.saveImage( ipath + "track_cuts.eps" );
	rp.savePage();

	rp.newPage();
		rpl.style( (TH1*)f->Get( "track_pre_nHitsFit" ) ).set( "logy", 1 ).set( "yr", 1e5, 2e8 ).set( "title", "nHits Fit; # hits" ).draw();
		rpl.style( (TH1*)f->Get( "track_nHitsFit" ) ).set( &cfg, "StylePost" ).draw();
		rp.saveImage( ipath + "track_nHitsFit.eps");
	rp.savePage();

	// rp.newPage();
	// 	rpl.style( (TH1*)f->Get( "track_pre_nHitsDedx" ) ).draw();
	// 	rp.saveImage( ipath + "track_pre_nHitsDedx.eps");
	// rp.savePage();

	rp.newPage();
		rpl.style( (TH1*)f->Get( "track_pre_nHitsDedx" ) ).set( "logy", 1 ).set( "yr", 1e5, 2e8 ).set( "title", "nHits dE/dx; # hits" ).draw();
		rpl.style( (TH1*)f->Get( "track_nHitsDedx" ) ).set( &cfg, "StylePost" ).draw();
		rp.saveImage( ipath + "track_nHitsDedx.eps");
	rp.savePage();

	// rp.newPage();
	// 	rpl.style( (TH1*)f->Get( "track_pre_nHitsFitOverPoss" ) ).draw();
	// 	rp.saveImage( ipath + "track_pre_nHitsFitOverPoss.eps");
	// rp.savePage();

	rp.newPage();
		rpl.style( (TH1*)f->Get( "track_pre_nHitsFitOverPoss" ) ).set( "logy", 1 ).set( "yr", 1e5, 2e8 ).set( "title", "nHits Fit / nHits Possible; # hits" ).draw();
		rpl.style( (TH1*)f->Get( "track_nHitsFitOverPoss" ) ).set( &cfg, "StylePost" ).draw();
		rp.saveImage( ipath + "track_nHitsFitOverPoss.eps");
	rp.savePage();

	// rp.newPage();
	// 	rpl.style( (TH1*)f->Get( "track_pre_ptRatio" ) ).draw();
	// 	rp.saveImage( ipath + "track_pre_ptRatio.eps");
	// rp.savePage();

	rp.newPage();
		rpl.style( (TH1*)f->Get( "track_pre_ptRatio" ) ).set( "logy", 1 ).set( "title", "pT_{gloabl} / pT_{primary}" ).draw();
		rpl.style( (TH1*)f->Get( "track_ptRatio" ) ).set( &cfg, "StylePost" ).draw();
		rp.saveImage( ipath + "track_ptRatio.eps");
	rp.savePage();

	rp.newPage();
		rpl.style( (TH1*)f->Get( "track_pre_ptRatio2D" ) ).set( "draw", "colz" ).draw();
		rp.saveImage( ipath + "track_pre_ptRatio2D.eps");
	rp.savePage();

	rp.newPage();
		rpl.style( (TH1*)f->Get( "track_ptRatio2D" ) ).draw();
		rp.saveImage( ipath + "track_ptRatio2D.eps");
	rp.savePage();

	// rp.newPage();
	// 	rpl.style( (TH1*)f->Get( "track_pre_dca" ) ).draw();
	// 	rp.saveImage( ipath + "track_pre_dca.eps");
	// rp.savePage();

	rp.newPage();
		rpl.style( (TH1*)f->Get( "track_pre_dca" ) ).set( "title", "Global DCA; [cm]" ).set( "logy", 1 ).draw();
		rpl.style( (TH1*)f->Get( "track_dca" ) ).set( &cfg, "StylePost" ).draw();
		rp.saveImage( ipath + "track_dca.eps");
	rp.savePage();

	// rp.newPage();
	// 	rpl.style( (TH1*)f->Get( "track_pre_yLocal" ) ).draw();
	// 	rp.saveImage( ipath + "track_pre_yLocal.eps");
	// rp.savePage();

	rp.newPage();
		rpl.style( (TH1*)f->Get( "track_pre_yLocal" ) ).set( "title", "Y Local; [cm]" ).set( "logy", 1 ).draw();
		rpl.style( (TH1*)f->Get( "track_yLocal" ) ).set( &cfg, "StylePost" ).draw();
		rp.saveImage( ipath + "track_yLocal.eps");
	rp.savePage();

	// rp.newPage();
	// 	rpl.style( (TH1*)f->Get( "track_pre_zLocal" ) ).draw();
	// 	rp.saveImage( ipath + "track_pre_zLocal.eps");
	// rp.savePage();

	rp.newPage();
		rpl.style( (TH1*)f->Get( "track_pre_zLocal" ) ).set( "title", "Z Local; [cm]" ).set( "logy", 1 ).draw();
		rpl.style( (TH1*)f->Get( "track_zLocal" ) ).set( &cfg, "StylePost" ).draw();
		rp.saveImage( ipath + "track_zLocal.eps");
	rp.savePage();

	rp.newPage();
		rpl.style( (TH1*)f->Get( "trackBeta" ) ).draw();
		rp.saveImage( ipath + "trackBeta.eps");
	rp.savePage();


	f->Close();
}