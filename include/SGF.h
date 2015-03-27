#ifndef SGF_H
#define SGF_H

#include "SGFSchema.h"
#include "Reporter.h"
using namespace jdb;
#include <memory>
#include <map>

class SGF
{
protected:
	shared_ptr<SGFSchema> schema;
	TFile * inFile;
	string title;
public:
	SGF( shared_ptr<SGFSchema> _schema, TFile * _in ){
		schema = _schema;
		inFile = _in;
	}
	~SGF(){}

	void fit( string cs, int charge, int cenBin, int ptBin, int etaBin ){

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

		schema->updateData( zb, "zb" );
		schema->updateData( zd, "zd" );
		
		schema->combineData();

		// get the combined data
		RooDataHist * d = schema->data( "" );
		// do the simultaneous fit
		RooSimultaneous * sim = schema->model();
		sim->fitTo( *d );
	}

	void showSample( string var, string sample ){

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

	void report( Reporter* rp ){

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

protected:

};





#endif