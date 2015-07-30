#include "PidYieldPresenter.h"
#include "TMath.h"

#include "TLegend.h"

PidYieldPresenter::PidYieldPresenter( XmlConfig * _cfg, string _nodePath ){

	cfg = _cfg;
	np = _nodePath;

	logger = unique_ptr<Logger>(LoggerConfig::makeLogger( cfg, np + "Logger" ));
	
	logger->setClassSpace( "PidYieldPresenter" );

	logger->info(__FUNCTION__) << endl;


	// create the input files
	fPidFit 	= new TFile( cfg->getString( np + "input.fit:url" ).c_str(), "READ" );
	fPidPS 		= new TFile( cfg->getString( np + "input.ps:url" ).c_str(), "READ" );

	// Create the book
	book = unique_ptr<HistoBook>(new HistoBook( cfg->getString( np + "output:path" ) + cfg->getString( np + "output.data:url" ), cfg, "", "" ));
	reporter = unique_ptr<Reporter>(new Reporter( cfg, np+"Reporter." ) );

	//Make the momentum transverse binning	 
	binsPt = unique_ptr<HistoBins>(new HistoBins( cfg, "bin.pt" ));
	//Make the eta binning
	binsEta = unique_ptr<HistoBins>(new HistoBins( cfg, "bin.eta" ));
	//Make charge bins
	binsCharge = unique_ptr<HistoBins>(new HistoBins( cfg, "bin.charge" ));

	book->cd("/");
	book->add( "eventNorms", (TH1D*)fPidPS->Get( "/EventQA/mappedRefMultBins" )->Clone( "eventNorms" ) );

	gStyle->SetOptStat( 0 );

	cenBins = cfg->getIntVector( np + "FitRange.centralityBins" );
	cenLabels = cfg->getStringVector( np+"centralityLabels" );
	charges = cfg->getIntVector( np + "FitRange.charges" );
	colors = cfg->getIntVector( np+"colors" );

	nColl = cfg->getDoubleVector( np + "nColl" );
	nPart = cfg->getDoubleVector( np + "nPart" );
}


PidYieldPresenter::~PidYieldPresenter(){
	logger->info(__FUNCTION__) << endl;

	
}

/**
 * Integrate over eta for a single plc, charge, centrality
 * outputs into the book in /raw/yieldName( plc, charge, iCen )
 */
void PidYieldPresenter::integrateEta( string plc, int charge, int iCen ) {
	logger->info(__FUNCTION__) << endl;
	vector<int> etaFitBins = cfg->getIntVector( np + "FitRange.etaBins" );
	vector<TH1D *> h;
	// loop over eta bins and add histos that exist
	for ( int iEta : etaFitBins ){

		string dir = "/" + plc + "_yield/";
		string name = TSF::FitRunner::yieldName( plc, iCen, charge, iEta );
		logger->info( __FUNCTION__ ) << "Looking for " << dir+name << endl;

		TH1D * tmp = (TH1D*)fPidFit->Get( (dir+name).c_str() );

		if ( tmp ){
			h.push_back( tmp );
			logger->info(__FUNCTION__) << "found" << endl;
		}

	}

	logger->info(__FUNCTION__) << "Found " << h.size() << " to combine " << endl;

	if ( h.size() >= 1  ){
		book->cd("raw");
		string name = TSF::FitRunner::yieldName( plc, iCen, charge, 0 );
		book->add( name, (TH1D*) h[ 0 ]->Clone( name.c_str() ) );	

		for ( int i = 1; i < h.size(); i++ ){

			book->get( name )->Add( h[ i ] );
		}
	}
}


void PidYieldPresenter::integrateEta() {

	reporter->newPage(1, 2);

	for ( string plc : Common::species ){
		for ( int charge : charges ){
			for ( int iCen : cenBins ){
				integrateEta( plc, charge, iCen );
				
				//string name = TSF::FitRunner::yieldName( plc, iCen, charge, 0 );
				//book->style( name )->set( "title", "#eta Integrated : " + name )->set( "logY", 1 )->draw();
				//reporter->next();

			} //centralitiy bins
		} //charges
	} //species

}


void PidYieldPresenter::normalizeYield( string plc, int charge, int iCen ){

	book->cd( "raw" );
	string name = TSF::FitRunner::yieldName( plc, iCen, charge, 0 );
	if ( !book->exists( name ) ){
		logger->error(__FUNCTION__) << name << " does not exists" << endl;
	}
	TH1D * y = (TH1D*)book->get( name );

	// find out which bin is the last with a good fit
	int lastGoodBin = cfg->getInt( np + "LastYieldBin."+plc+":"+Common::chargeString( charge ), 1000 );
	logger->info(__FUNCTION__) << "Last Good Bin : " << lastGoodBin << endl;

	book->cd( "/" );
	double nEvents = book->get( "eventNorms" )->GetBinContent( iCen+1 );

	book->cd( "yield" );
	book->add( name, (TH1D*)y->Clone( name.c_str() ) );

	TH1D * ny = (TH1D*)book->get( name );
	for ( int iBin = 1; iBin < y->GetNbinsX() + 1; iBin++ ){

	
		double pt = ny->GetBinCenter( iBin );
		double sc = 1.0 / ( 2 * pt * 3.1415926 );

		if ( iBin > lastGoodBin ){
			sc = 0;	
		}

		ny->SetBinContent( iBin, ny->GetBinContent( iBin ) * sc );
		ny->SetBinError( iBin, ny->GetBinError( iBin ) * sc );
	}

}

void PidYieldPresenter::normalizeYield() {

	reporter->newPage(1, 2);

	for ( string plc : Common::species ){
		for ( int charge : charges ){
			for ( int iCen : cenBins ){
				normalizeYield( plc, charge, iCen );
				
				string name = TSF::FitRunner::yieldName( plc, iCen, charge, 0 );
				// book->style( name )->set("lineWidth", 2)->
				// 	set( "title", "Normalized Yield : " + name )->
				// 	set( "logY", 1 )->set( "domain", 0, 5 )->draw();
				// reporter->next();

			} //centralitiy bins
		} //charges
	} //species

}



void PidYieldPresenter::compareYields(){

	reporter->newPage( 2, 1 );
	for ( string plc : Common::species ){
		for ( int charge : charges ){
			compareYields( plc, charge);	
		}
	}

}

void PidYieldPresenter::compareYields( string plc, int charge ){


	book->cd( "yield" );

	TLegend *leg = new TLegend(0.65,0.6,0.9,0.9);
	for ( int iCen : cenBins ){


		string name = TSF::FitRunner::yieldName( plc, iCen, charge, 0 );
		TH1D * y = (TH1D*)book->get( name )->Clone( "tmp" );
		y->SetDirectory( 0 );

		int pow10 = iCen;

		logger->info(__FUNCTION__) << "Scale Factor: " << pow10 << endl;
		double scale = TMath::Power( 10, -pow10 );
		logger->info(__FUNCTION__) << "Scale : " << scale << endl;


		string drawOpt = "pe";
		if ( iCen > 0 )
			drawOpt = "same pe";


		y->SetTitle( ( plcName( plc, charge ) + " : Raw Spectra; pT [GeV] ;d^{2}N / ( N_{evt} 2 #pi pT dpT dy )" ).c_str() );
		y->GetYaxis()->SetTitleOffset( 1.6 );
		y->Scale( scale );
		y->SetLineColor( colors[ iCen ] );
		y->SetMarkerColor( colors[ iCen ] );
		y->GetYaxis()->SetRangeUser( 10e-16, 1000 );
		y->SetLineWidth( 2 );
		y->SetMarkerStyle( 20 + iCen );

		y->Draw( drawOpt.c_str() );
		gPad->SetLogy(1);


		string label = cenLabels[ iCen ] + " x 10^{-" + ts(pow10) +"}";
		if ( 0 == iCen  )
			label = cenLabels[ iCen ];
		leg->AddEntry( y, label.c_str(), "lep" );
	}
	leg->Draw();
	reporter->saveImage( cfg->getString( np + "output:path" ) + "export_yields_" + plc + ".pdf" );
	reporter->next();
	


}

void PidYieldPresenter::rcp( int iPer ){

	for ( int iCen : cenBins ){

		rcp( "Pi", 1, iCen, iPer );
		rcp( "K", 1, iCen, iPer );
		rcp( "P", 1, iCen, iPer );

		rcp( "Pi", -1, iCen, iPer );
		rcp( "K", -1, iCen, iPer );
		rcp( "P", -1, iCen, iPer );
	}
}


void PidYieldPresenter::rcp( string plc, int charge, int iCen, int iPer ){

	book->cd("yield");

	if ( 	!book->exists( TSF::FitRunner::yieldName( plc, iCen, charge, 0 ) ) ||
			!book->exists( TSF::FitRunner::yieldName( plc, iPer, charge, 0 ) )  ){
		logger->error( __FUNCTION__ ) << "central or peripheral yield missing" << endl;
		return;
	}

	// get the vector of ncolls mapped to centrality bins
	
	if ( nColl.size() <= iCen || nColl.size() <= iPer ){
		logger->error(__FUNCTION__) << "nColl doesnt contain value for this bin" << endl;
		return;
	}

	logger->info( __FUNCTION__ ) << "Central Bin : " << iCen << endl;
	logger->info( __FUNCTION__ ) << "Periferal Bin : " << iPer << endl;

	logger->info( __FUNCTION__ ) << "Scale Central Bin : " << nColl[ iPer ] << endl;
	logger->info( __FUNCTION__ ) << "Scale Periferal Bin : " << nColl[ iCen ] << endl;

	TH1D * cen = (TH1D*)book->get( TSF::FitRunner::yieldName( plc, iCen, charge, 0 ) );
	TH1D * per = (TH1D*)book->get( TSF::FitRunner::yieldName( plc, iPer, charge, 0 ) );

	book->cd( "rcp" );
	cen = (TH1D*) cen->Clone( "cen" );
	per = (TH1D*) per->Clone( "per" );
	cen->SetDirectory( 0 );
	per->SetDirectory( 0 );

	cen->Scale( nColl[ iPer ]  );
	per->Scale( nColl[ iCen ] );

	cen->SetLineColor( kRed );
	cen->SetLineWidth( 2 );
	per->SetLineColor( kBlue );
	per->SetLineWidth( 2 );

	reporter->newPage(1, 2);
	reporter->cd( 1, 1);
		
		per->Draw();
		cen->Draw("same");
		gPad->SetLogy(1);

	reporter->cd( 1, 2);

		TH1D * hRcp = (TH1D*)cen->Clone( rcpName( plc, charge, iCen, iPer ).c_str() );
		book->add( rcpName( plc, charge, iCen, iPer ), hRcp );
		hRcp->Divide( per );
		book->style( rcpName( plc, charge, iCen, iPer ) )->
		set("logY", 1)->draw();

	//reporter->savePage();

}


void PidYieldPresenter::rcpPanel( int iCen, int iPer ){

	book->cd( "rcp" );

	reporter->newPage( 3, charges.size() );

	for ( int charge : charges ){
		for ( string plc : Common::species ){
		

			book->style( rcpName( plc, charge, iCen, iPer ) )->
				set( "title", plcName( plc, charge ) + " R_{CP} " )->
				set( np + "Style.rcp" )->
				set( "title", "R_{CP} (" + cenLabels[iCen] + " ) / ( " + cenLabels[iPer] + " )"  )->
				draw();
			gPad->SetGrid(1);
			reporter->saveImage( cfg->getString( np + "output:path" ) + "export_" + rcpName( plc, charge, iCen, iPer ) + ".pdf" );
			reporter->next();
		}
	}
}


void PidYieldPresenter::chargeRatio( ){

	for ( int iCen : cenBins ){
		reporter->newPage( 3, 1 );
		chargeRatio( "Pi", iCen );
		chargeRatio( "K", iCen );
		chargeRatio( "P", iCen );
	}
}

void PidYieldPresenter::chargeRatio( string plc, int iCen ){

	book->cd("yield");

	if ( 	!book->exists( TSF::FitRunner::yieldName( plc, iCen, 1, 0 ) ) ||
			!book->exists( TSF::FitRunner::yieldName( plc, iCen, -1, 0 ) )  ){
		logger->error( __FUNCTION__ ) << "+/- yield missing" << endl;
		return;
	}

	TH1D * plus = (TH1D*)book->get( TSF::FitRunner::yieldName( plc, iCen, 1, 0 ) );
	TH1D * minus = (TH1D*)book->get( TSF::FitRunner::yieldName( plc, iCen, -1, 0 ) );

	book->cd( "chargeRatio" );

	minus = (TH1D *) minus->Clone( chargeRatioName( plc, iCen ).c_str() );
	minus->Divide( plus );
	book->add( chargeRatioName( plc, iCen ), minus );

	book->style( chargeRatioName( plc, iCen ) )->
		set( np + "Style.chargeRatio" )->
		set( np + "Style.chargeRatio_" + plc )->
		set( "title", "Charge Ratio : " + plc + "( " + cenLabels[ iCen ] + " )" )->draw();


	reporter->next();


}

void PidYieldPresenter::chargeRatioCompare( string plc ) {

	book->cd( "chargeRatio" );

	TLegend *leg = new TLegend(0.5,0.6,0.9,0.9);
	leg->SetFillColor( kWhite );
	leg->SetFillStyle( 1001 ); // solid
	for ( int iCen : cenBins ){
		string dOpt = "";
		if ( iCen != cenBins[ 0 ] )
			dOpt = "same";
		book->style( chargeRatioName( plc, iCen ) )->
			set( np + "Style.chargeRatio" )->
			set( np + "Style.chargeRatio_" + plc )->
			set( "title", "Charge Ratio : " + plc )->
			set( "lineColor", colors[ iCen ] )->
			set( "markerColor", colors[ iCen ] )->
			set( "MarkerStyle", 20 + iCen )->
			set( "draw", dOpt )->
			draw();
		string label = cenLabels[ iCen ];
		leg->AddEntry( book->get( chargeRatioName( plc, iCen ) ), label.c_str(), "lep" );
	}
	leg->Draw();
	reporter->saveImage( cfg->getString( np + "output:path" ) + "export_chargeRatioCen.pdf" );
	reporter->next();

	
}

void PidYieldPresenter::chargeRatioCompare( ) {

	reporter->newPage( 3, 1 );
	chargeRatioCompare( "Pi" );
	chargeRatioCompare( "K" );
	chargeRatioCompare( "P" );	

}


void PidYieldPresenter::rcpVsNPart( int ptBin, int iPer ){

	reporter->newPage();
	rcpVsNPart( "Pi", 1, ptBin );
	rcpVsNPart( "Pi", -1, ptBin );

	rcpVsNPart( "K", 1, ptBin );
	rcpVsNPart( "K", -1, ptBin );

	rcpVsNPart( "P", 1, ptBin );
	rcpVsNPart( "P", -1, ptBin );
}


void PidYieldPresenter::rcpVsNPart( string plc, int charge, int ptBin, int iPer  ){


	book->cd( "rcp" );

	HistoBins nPartBins( 0, 460, 5 );
	book->make1D( 	nPartName( plc, charge, ptBin ), 
					nPartName( plc, charge, ptBin ),
					nPartBins.nBins(),
					nPartBins.bins.data()
		 		);

	TH1 * h = book->get( nPartName( plc, charge, ptBin ) );  
	for ( int iCen : cenBins ){

		TH1* hC = book->get( rcpName( plc, charge, iCen, iPer ) );

		double ncPart = nPart[ iCen ];

		h->SetBinContent( nPartBins.findBin( ncPart ), hC->GetBinContent( ptBin ) );		
		h->SetBinError( nPartBins.findBin( ncPart ), hC->GetBinError( ptBin ) );		

	}

	book->get( nPartName( plc, charge, ptBin ) )->GetYaxis()->SetTitleOffset( 0.5 );
	book->style( nPartName( plc, charge, ptBin ) )->
	set( "lineColor", "red" )->
	set( "draw", "pe" )->
	set( "title", plcName( plc, charge ) + " R_{CP} @ pT = " + dts(binsPt->bins[ ptBin ]) )->
	set( np + "Style.nPart" )->
	draw();

	reporter->next();


}


void PidYieldPresenter::rcpVsNPartCompare( int ptBin, int iPer ){

	TLegend *leg = new TLegend(0.75,0.6,0.9,0.9);
	leg->SetFillColor( kWhite );
	leg->SetFillStyle( 1001 ); // solid

	reporter->newPage( 1, 1);
	int drawing = 0;
	for ( string plc : Common::species ){
		for ( int charge : charges ){

			string dOpt = "";
			if ( 0 < drawing )
				dOpt = "same";

			book->style( nPartName( plc, charge, ptBin ) )->
				
				set( "title", "R_{CP} @ pT = " + dts(binsPt->bins[ ptBin ]) )->
				set( np + "Style.nPart" )->
				set( "draw", dOpt )->
				set( "linecolor", colors[ drawing ] )->
				set( "markercolor", colors[ drawing ] )->
				set( "markerStyle", 20 + drawing )->
				draw();
				
				string label = plcName( plc, charge );
				leg->AddEntry( 	book->get( nPartName( plc, charge, ptBin ) ), 
								label.c_str(), 
								"lep" );

			drawing++;
		}
	} 
	leg->Draw();

	reporter->saveImage( cfg->getString( np + "output:path" ) + "export_rcp_nPart_pt_" + ts( ptBin ) + ".pdf" );
	reporter->savePage();
}










