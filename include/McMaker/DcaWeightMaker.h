#ifndef DCA_WEIGHT_MAKER_H
#define DCA_WEIGHT_MAKER_H


// ROOBARB
#include "XmlConfig.h"
#include "HistoBook.h"
#include "HistoAnalyzer.h"
#include "RooPlotLib.h"
#include "HistoBins.h"
	
using namespace jdb;

#include "Common.h"

// STL
#include <memory>
#include <string>
#include <fstream>

// ROOT
#include "TF1.h"
#include "TFitResultPtr.h"

class DcaWeightMaker : public HistoAnalyzer
{
protected:

public:
	virtual const char* classname() const { return "DcaWeightMaker"; }

	DcaWeightMaker( ){};
	~DcaWeightMaker( ) {};

	// TODO: inheritance should make this unneeded
	virtual void initialize(  ){
		HistoAnalyzer::initialize();
		

		ptBins.load( config, "binning.pt" );
		// Reporter rp( config, nodePath + ".Reporter." );
	}

	virtual void make(){

		book->cd();
		book->makeAll( config, nodePath + ".histograms" );
		for ( string plc : Common::species ){
			for ( string sc : Common::sCharges ){
				makeMapFor( plc, sc );	
			}
		}
		

	}


	void makeMapFor(string plc, string sc ){

		string nDcaPass = "dcaEff_Pass_" + plc + "_" + sc;
		string nDcaTotal = "dcaEff_Total_" + plc + "_" + sc;

		string nDcaEff = "dcaEff_" + plc + "_" + sc;
		string nEmbDcaEff = "embdcaEff_" + plc + "_" + sc;
		book->clone( "dcaEff", nDcaEff );
		book->clone( "dcaEff", nDcaPass );
		book->clone( "dcaEff", nDcaTotal );
		book->clone( "dcaEff", nEmbDcaEff );
		RooPlotLib rpl;
		rpl.link( book );

		TH2D *data2 = (TH2D*)get2D( "dca_vs_pt_" + plc + "_" + sc );
		TH2D *emb2 = (TH2D*)get2D( "emb_dca_vs_pt_" + plc + "_" + sc );

		INFO( "got 2D data : " << data2 );
		INFO( "got 2D emb  : " << emb2 );

		data2->RebinX( config.getInt( nodePath + ".Rebin:x", 1 ) );
		emb2->RebinX( config.getInt( nodePath + ".Rebin:x", 1 ) );

		// data2->RebinX( ptBins )

		vector<double> dcaBins = config.getDoubleVector( nodePath + ".dcaBins" );
		
		data2->RebinY( config.getInt( nodePath + ".Rebin:y", 1 ) );
		emb2->RebinY( config.getInt( nodePath + ".Rebin:y", 1 ) );

		int nPtBins = data2->GetNbinsX();
		for ( int i = 1; i <= nPtBins; i++ ){
			string dName = "dca_" + plc + "_" + sc + "_ptBin_" + ts( i );
			string eName = "emb_dca_" + plc + "_" + sc + "_ptBin_" + ts( i );
			TH1D * data1 = data2->ProjectionY( dName.c_str(), i, i );
			TH1D * emb1 = emb2->ProjectionY( eName.c_str(), i, i );

			if ( config.exists( nodePath + ".dcaBins" ) ){
				data1->SetDirectory(0);
				emb1->SetDirectory(0);
				data1 = (TH1D*)data1->Rebin( dcaBins.size() - 1,  ("rebin_" + dName).c_str(), dcaBins.data() );
				emb1 = (TH1D*)emb1->Rebin( dcaBins.size() - 1,  ("rebin_" + eName).c_str(), dcaBins.data() );
			}

			// data1->SetDirectory(0);
			// emb1->SetDirectory(0);

			HistoBook::weightByBinWidth( data1 );
			HistoBook::weightByBinWidth( emb1 );

			double pt1 = data2->GetXaxis()->GetBinLowEdge( i );
			double pt2 = pt1 + data2->GetXaxis()->GetBinWidth( i );

			if ( data1->Integral(  ) < 10 || emb1->Integral(  ) < 10 ) continue;

			data1->Scale( 1.0 / data1->Integral(  ) );
			emb1->Scale( 1.0 / emb1->Integral(  ) ); 

			rpl.style( data1 ).set( "c", "red" );

			reporter->newPage();
			emb1->GetYaxis()->SetRangeUser( 0.001, emb1->GetMaximum() * 2 );
			emb1->GetXaxis()->SetRangeUser( 0, 3.0 );
			rpl.style( emb1 ).set( "title", Common::plc_label( plc, sc ) + " " + dts( pt1 ) + " < pT < " + dts( pt2 ) + "[GeV/c] ; DCA [cm]; dN/dDCA" );
			emb1->Draw();

			data1->Draw("same");
			gPad->SetLogy(1);
			

			// reporter->savePage();

			string rName = "dca_ratio_" + plc + "_" + sc + "_ptBin_" + ts( i );
			TH1D * ratio1 = (TH1D*)data1->Clone( rName.c_str() );
			// ratio1->SetDirectory(0);

			ratio1->Divide( emb1 );
			// ratio1->Fit( "pol3", "R", "", 0, 2 );

			rpl.style( ratio1 ).set( "logy", 0 ).set( "yr", 0.5, 10 ).set( "y", "data / embedding" ).draw();
			
			// reporter->savePage();


			int dcaCutBin = data1->FindBin( 1.0 );
			double passError = 0;
			double pass = data1->IntegralAndError( 1, dcaCutBin, passError );
			double totalError = 0;
			double total = data1->IntegralAndError(1, -1, totalError);

			book->setBin( nDcaEff, i, pass / total, 0.01 );
			book->setBin( nDcaPass, i, pass, passError );
			book->setBin( nDcaTotal, i, total, totalError );
			

			double passEmb = emb1->Integral( 1, dcaCutBin );
			double totalEmb = emb1->Integral();


			book->setBin( nEmbDcaEff, i, passEmb / totalEmb, 0.01 );
			rpl.style( book->get( nEmbDcaEff ) ).set( "c", "red" );



		}

		// TF1 * fitFunc = new TF1( ("dcaeffFitFunc" + plc + sc).c_str(), "[0] * exp( - pow( [1] / x, [2] ) )", 0.0, 2.5 );

		// fitFunc->SetParameters( .65, 0.05, 5.0, -0.05 );
		// fitFunc->SetParLimits( 0, 0.5, 1.0 );
		// fitFunc->SetParLimits( 1, 0.0, 0.5 );
		// fitFunc->SetParLimits( 2, 0.0, 100000 );
		// book->get( nDcaEff )->Fit( fitFunc, "R", "", 0, 2 );
		rpl.style( nDcaEff ).set( "yr", 0.6, 1.0 ).draw();
		rpl.style( nEmbDcaEff ).set( "draw", "same" ).draw();
		reporter->savePage();




	}

	// void exportParams( int cbin, TF1 * f, TFitResultPtr result, ofstream &out );
	

	protected:
		HistoBins ptBins;
};


#endif