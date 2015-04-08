#include "PidYieldPresenter.h"
#include "TMath.h"

#include "Tlegend.h"

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
		string name = SGFRunner::yieldName( plc, iCen, charge, iEta );
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
		string name = SGFRunner::yieldName( plc, iCen, charge, 0 );
		book->add( name, (TH1D*) h[ 0 ]->Clone( name.c_str() ) );	

		for ( int i = 1; i < h.size(); i++ ){

			book->get( name )->Add( h[ i ] );
		}

		
			
	}



}


void PidYieldPresenter::integrateEta() {

	reporter->newPage(1, 2);
	vector<int> cenBins = cfg->getIntVector( np + "FitRange.centralityBins" );
	vector<int> charges = cfg->getIntVector( np + "FitRange.charges" );
	for ( string plc : PidPhaseSpace::species ){
		for ( int charge : charges ){
			for ( int iCen : cenBins ){
				integrateEta( plc, charge, iCen );
				
				string name = SGFRunner::yieldName( plc, iCen, charge, 0 );
				book->style( name )->set( "title", "#eta Integrated : " + name )->set( "logY", 1 )->draw();
				reporter->next();

			} //centralitiy bins
		} //charges
	} //species

}


void PidYieldPresenter::normalizeYield( string plc, int charge, int iCen ){

	book->cd( "raw" );
	string name = SGFRunner::yieldName( plc, iCen, charge, 0 );
	if ( !book->exists( name ) ){
		logger->error(__FUNCTION__) << name << " does not exists" << endl;
	}
	TH1D * y = (TH1D*)book->get( name );

	book->cd( "/" );
	double nEvents = book->get( "eventNorms" )->GetBinContent( iCen+1 );

	book->cd( "yield" );
	book->add( name, (TH1D*)y->Clone( name.c_str() ) );

	TH1D * ny = (TH1D*)book->get( name );
	for ( int iBin = 1; iBin < y->GetNbinsX() + 1; iBin++ ){

		double pt = ny->GetBinCenter( iBin );
		double sc = 1.0 / ( nEvents * 2 * pt * 3.1415926 );
		ny->SetBinContent( iBin, ny->GetBinContent( iBin ) * sc );
		ny->SetBinError( iBin, ny->GetBinError( iBin ) * sc );
	}

}

void PidYieldPresenter::normalizeYield() {

	reporter->newPage(1, 2);
	vector<int> cenBins = cfg->getIntVector( np + "FitRange.centralityBins" );
	vector<int> charges = cfg->getIntVector( np + "FitRange.charges" );
	for ( string plc : PidPhaseSpace::species ){
		for ( int charge : charges ){
			for ( int iCen : cenBins ){
				normalizeYield( plc, charge, iCen );
				
				string name = SGFRunner::yieldName( plc, iCen, charge, 0 );
				book->style( name )->set("lineWidth", 2)->
					set( "title", "Normalized Yield : " + name )->
					set( "logY", 1 )->set( "domain", 0, 5 )->draw();
				reporter->next();

			} //centralitiy bins
		} //charges
	} //species

}


void PidYieldPresenter::compareYields( string plc, int charge ){

	vector<int> colors = cfg->getIntVector( np+"colors" );
	vector<string> cLabels = cfg->getStringVector( np+"centralityLabels" );
	reporter->newPage( 1, 1 );
	book->cd( "yield" );
	vector<int> cenBins = cfg->getIntVector( np + "FitRange.centralityBins" );

	TLegend *leg = new TLegend(0.7,0.6,0.9,0.9);
	for ( int iCen : cenBins ){


		string name = SGFRunner::yieldName( plc, iCen, charge, 0 );
		TH1D * y = (TH1D*)book->get( name )->Clone( "tmp" );
		y->SetDirectory( 0 );

		int pow10 = iCen;

		logger->info(__FUNCTION__) << "Scale Factor: " << pow10 << endl;
		double scale = TMath::Power( 10, -pow10 );
		logger->info(__FUNCTION__) << "Scale : " << scale << endl;


		string drawOpt = "pe";
		if ( iCen > 0 )
			drawOpt = "same pe";

		y->SetTitle( (plc + "^{" + chargeString( charge )+ "} Yields" ).c_str() );
		y->Scale( scale );
		y->SetLineColor( colors[ iCen ] );
		y->GetYaxis()->SetRangeUser( 10e-16, 100 );
		y->Draw( drawOpt.c_str() );
		gPad->SetLogy(1);


		string label = cLabels[ iCen ] + " x 10e-" + ts(pow10);
		if ( 0 == iCen  )
			label = cLabels[ iCen ];
		leg->AddEntry( y, label.c_str(), "lep" );
	}
	leg->Draw();
	reporter->savePage();


}


void PidYieldPresenter::rcp( string plc, int charge, int iCen, int iPer ){

	book->cd("yield");

	if ( 	!book->exists( SGFRunner::yieldName( plc, iCen, charge, 0 ) ) ||
			!book->exists( SGFRunner::yieldName( plc, iPer, charge, 0 ) )  ){
		logger->error( __FUNCTION__ ) << "central or peripheral yield missing" << endl;
		return;
	}

	// get the vector of ncolls mapped to centrality bins
	vector<double> nColl = cfg->getDoubleVector( np + "nColl" );
	if ( nColl.size() <= iCen || nColl.size() <= iPer ){
		logger->error(__FUNCTION__) << "nColl doesnt contain value for this bin" << endl;
		return;
	}

	logger->info( __FUNCTION__ ) << "Central Bin : " << iCen << endl;
	logger->info( __FUNCTION__ ) << "Periferal Bin : " << iPer << endl;

	logger->info( __FUNCTION__ ) << "Scale Central Bin : " << nColl[ iPer ] << endl;
	logger->info( __FUNCTION__ ) << "Scale Periferal Bin : " << nColl[ iCen ] << endl;

	TH1D * cen = (TH1D*)book->get( SGFRunner::yieldName( plc, iCen, charge, 0 ) );
	TH1D * per = (TH1D*)book->get( SGFRunner::yieldName( plc, iPer, charge, 0 ) );

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

		TH1D * hRcp = (TH1D*)cen->Clone( rcpName( plc, charge ).c_str() );
		book->add( rcpName( plc, charge ), hRcp );
		hRcp->Divide( per );
		book->style( rcpName( plc, charge ) )->
		set("logY", 1)->draw();

	reporter->savePage();

}


void PidYieldPresenter::rcpPannel(){

	book->cd( "rcp" );
	vector<int> charges = cfg->getIntVector( np + "FitRange.charges" );
	vector<string> cLabels = cfg->getStringVector( np+"centralityLabels" );

	reporter->newPage( charges.size(), 3 );


	for ( string plc : PidPhaseSpace::species ){
		for ( int charge : charges ){

			book->style( rcpName( plc, charge ) )->
			set("range", .2, 10 )->set( "logY", 1 )->
			set( "title", "R_{CP} : " + plc + chargeString( charge ) )->
			set( "y", "(0-5%) / (60-80%)" )->draw();
			gPad->SetGrid(1);
			reporter->next();
		}
	}

	//reporter->savePage();
}














