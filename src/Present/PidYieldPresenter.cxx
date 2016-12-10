#include "Present/PidYieldPresenter.h"
#include "TMath.h"

#include "TLegend.h"

PidYieldPresenter::PidYieldPresenter( XmlConfig * _cfg, string _nodePath ){

	cfg = _cfg;
	np = _nodePath;

	// create the input files
	fPidFit 	= new TFile( cfg->getString( np + "input.fit:url" ).c_str(), "READ" );

	if ( fPidFit->IsOpen() ){
		INFO (tag,  "Fit File Opened" );
	}
	else{
		ERROR( tag, "Cannot open fit histos" );
	}

	// Create the book
	INFO( tag, "Making book" );
	book = unique_ptr<HistoBook>(new HistoBook( cfg->getString( np + "output:path" ) + cfg->getString( np + "output.data:url" ), cfg, "", "" ));

	INFO( tag, "Making Reporter" );
	reporter = unique_ptr<Reporter>(new Reporter( cfg, np+"Reporter." ) );

	//Make the momentum transverse binning
	INFO( tag, "Making pt Bins" );	 
	binsPt = unique_ptr<HistoBins>(new HistoBins( cfg, "binning.pt" ));

	book->cd("/");

	gStyle->SetOptStat( 0 );

	cenBins = cfg->getIntVector( np + "FitRange.centralityBins" );
	cenLabels = cfg->getStringVector( np + "centralityLabels" );
	charges = cfg->getIntVector( np + "FitRange.charges" );
	colors = cfg->getIntVector( np + "colors" );

	nColl = cfg->getDoubleVector( np + "nColl" );
	nPart = cfg->getDoubleVector( np + "nPart" );

	plc = cfg->getString( np + "input:plc", "ERROR" );
}


PidYieldPresenter::~PidYieldPresenter(){
	INFO( tag, "()" );

	
}


void PidYieldPresenter::make(){

	if ( !fPidFit->IsOpen() ){
		ERROR( tag, "Fit File NOT Opened" );
		return;
	}
	
	normalizeYield();
	compareYields();
	rcp( 6 );

}


void PidYieldPresenter::normalizeYield( string plc, int charge, int iCen ){

	
	string name = Common::yieldName( plc, iCen, charge );
	if ( !fPidFit->Get( (plc + "_yield/" + name).c_str()  ) ){
		ERROR( tag, name << " does not exists" );

	}
	TH1D * y = (TH1D*)fPidFit->Get( (plc + "_yield/" + name).c_str()  );

	// find out which bin is the last with a good fit
	int lastGoodBin = cfg->getInt( np + "LastYieldBin."+plc+":"+Common::chargeString( charge ), 1000 );
	INFO( tag, "Last Good Bin : " << lastGoodBin );


	book->cd( "yield" );
	book->add( name, (TH1D*)y->Clone( name.c_str() ) );

	TH1D * ny = (TH1D*)book->get( name );
	for ( int iBin = 1; iBin < y->GetNbinsX() + 1; iBin++ ){

	
		// double pt = ny->GetBinCenter( iBin );
		// double sc = 1.0 / ( 2 * pt * 3.1415926 );

		// if ( iBin > lastGoodBin || iBin ==1 ){
		// 	sc = 0;	
		// }

		// ny->SetBinContent( iBin, ny->GetBinContent( iBin ) * sc );
		// ny->SetBinError( iBin, ny->GetBinError( iBin ) * sc );
	}

}

void PidYieldPresenter::normalizeYield() {

	reporter->newPage(1, 2);

	
	for ( int charge : charges ){
		for ( int iCen : cenBins ){
			normalizeYield( plc, charge, iCen );
			
			string name = Common::yieldName( plc, iCen, charge );
			// book->style( name )->set("lineWidth", 2)->
			// 	set( "title", "Normalized Yield : " + name )->
			// 	set( "logY", 1 )->set( "domain", 0, 5 )->draw();
			// reporter->next();

		} //centralitiy bins
	} //charges
	

}



void PidYieldPresenter::compareYields(){

	reporter->newPage( 2, 1 );
	
	for ( int charge : charges ){
		compareYields( plc, charge);	
	}
	

}

void PidYieldPresenter::compareYields( string plc, int charge ){


	book->cd( "yield" );

	TLegend *leg = new TLegend(0.65,0.6,0.9,0.9);
	for ( int iCen : cenBins ){


		string name = Common::yieldName( plc, iCen, charge );
		TH1D * y = (TH1D*)book->get( name )->Clone( "tmp" );
		y->SetDirectory( 0 );

		int pow10 = iCen;

		INFO( tag, "Scale Factor: " << pow10 );
		double scale = TMath::Power( 10, -pow10 );
		INFO( tag, "Scale : " << scale );


		string drawOpt = "pe";
		if ( iCen > 0 )
			drawOpt = "same pe";


		y->SetTitle( ( plcName( plc, charge ) + " : Spectra; pT [GeV] ;d^{2}N / ( N_{evt} 2 #pi pT dpT dy )" ).c_str() );
		y->GetYaxis()->SetTitleOffset( 1.6 );
		y->Scale( scale );
		y->SetLineColor( colors[ iCen ] );
		y->SetMarkerColor( colors[ iCen ] );
		y->GetYaxis()->SetRangeUser( 10e-16, 1e6 );
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

	for ( int charge : charges ){
		for ( int iCen : cenBins ){
			rcp( plc, charge, iCen, iPer );
		}
	}
}


void PidYieldPresenter::rcp( string plc, int charge, int iCen, int iPer ){

	book->cd("yield");

	if ( 	!book->exists( Common::yieldName( plc, iCen, charge ) ) ||
			!book->exists( Common::yieldName( plc, iPer, charge ) )  ){
		ERROR( tag, "central or peripheral yield missing" );
		return;
	}

	// get the vector of ncolls mapped to centrality bins
	
	if ( nColl.size() <= iCen || nColl.size() <= iPer ){
		ERROR( tag, "nColl doesnt contain value for this bin" );
		return;
	}

	INFO( tag, "Central Bin : " << iCen );
	INFO( tag, "Periferal Bin : " << iPer );

	INFO( tag, "Scale Central Bin : " << nColl[ iPer ] );
	INFO( tag, "Scale Periferal Bin : " << nColl[ iCen ] );

	TH1D * cen = (TH1D*)book->get( Common::yieldName( plc, iCen, charge ) );
	TH1D * per = (TH1D*)book->get( Common::yieldName( plc, iPer, charge ) );

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

	reporter->savePage();

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

	if ( 	!book->exists( Common::yieldName( plc, iCen, 1 ) ) ||
			!book->exists( Common::yieldName( plc, iCen, -1 ) )  ){
		ERROR( tag,  "+/- yield missing" );
		return;
	}

	TH1D * plus = (TH1D*)book->get( Common::yieldName( plc, iCen, 1 ) );
	TH1D * minus = (TH1D*)book->get( Common::yieldName( plc, iCen, -1 ) );

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










