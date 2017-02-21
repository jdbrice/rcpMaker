#ifndef ZD_SPECTRA_HISTO_FILE_MAKER_H
#define ZD_SPECTRA_HISTO_FILE_MAKER_H

#include "HistoAnalyzer.h"
#include "Common.h"

#include "ANSIColors.h"

#include "RooPlotLib.h"

class ZdSpectraHistoFileMaker : public HistoAnalyzer
{

protected:

	string plc, energy;
public:
	virtual const char* classname() const { return "ZdSpectraHistoFileMaker"; }
	ZdSpectraHistoFileMaker() {}
	~ZdSpectraHistoFileMaker() {}


	virtual void overrideConfig(){
		DEBUG( classname(), "" );
	}

	virtual void initialize() {
		HistoAnalyzer::initialize();
		INFO( classname(), "" );

		sc = unique_ptr<SpectraCorrecter>( new SpectraCorrecter( config, nodePath ) );
	}



	virtual void make(){
		INFO( classname(), "" );

		setCurrentFile( "spectra" );

		RooPlotLib rpl;
		rpl.link( book );
		

		book->cd();

		TH1 * hCen = get1D( "EventQA/mappedRefMultBins" );
		INFO( classname(), "MappedCentrality Bins: " << hCen );

		if ( nullptr == hCen ){
			ERROR( classname(), "Invalid file give!" );
			return;
		}

		string histoToFit = config.getXString( nodePath + ".input.FitTo:url", "zd_" );

		vector<int> charges = config.getIntVector( "binning.charges" );
		// centralityBinMap = config.getIntMap( nodePath + ".CentralityMap" );
    	vector<int> centralityBins = config.getIntVector( nodePath + ".CentralityBins" );
    	HistoBins binsPt( config, "binning.pt" );

    	INFO( classname(), vts( charges ) );
    	INFO( classname(), vts( centralityBins ) );

    	book->make( config, config.q( nodePath + ".histograms.Histo{name==pT}" ) );

    	TF1 * gf = new TF1( "gf", "gaus" );

    	string plc = config.getXString( nodePath + ".input:plc" );

    	INFO( classname(), "Starting fits" );
    	for ( int cb : centralityBins ) {

    		double nE = hCen->GetBinContent( cb + 1 );
    		INFO( classname(), "nE[" << cb << "] = " << nE );

			for ( int c : charges ) {	
		
				string pTname = "pT_" + Common::speciesName( plc, c, cb );
				book->clone( "pT", pTname );
				INFO( classname(), ANSIColors::color( "Making :" + pTname, "red" ) );


				for ( int iPt = 0; iPt < binsPt.nBins(); iPt++ ){
					double bc = book->get( pTname )->GetBinCenter( iPt+1 );
					if ( bc < 0.0 ) {
						book->setBin( pTname, iPt + 1, 0.000001, 0.00000001 );
						continue;
					}

					// create a PidPoint
					string name = histoToFit + Common::speciesName( plc, c, cb, iPt );
					INFO( classname(), "Looking for : " << name );
					TH1 * h = get1D( name );

					if ( h->Integral() <= 1 ){
						WARN( classname(), "Skipping empty histogram " );
						continue;
					}

					INFO( classname(), "TH1: " << h  );
					book->add( name, h );
					h->SetDirectory( gDirectory );


					double bw = h->GetBinWidth( 1 ); // fixed width so it does not matter which
					INFO( classname(), "dEdx bin width = " << bw );
					gf->SetParameters( 10, 0.0, 0.012 );
					h->Fit( gf, "RL", "", -0.16, 0.4 );
					double mu = gf->GetParameter( 1 );
					double sig = gf->GetParameter( 2 );
					if ( mu != mu ) mu = 0.0;
					if ( sig != sig ) sig = 0.012;
					h->Fit( gf, "RL", "", mu - 2*sig, mu + 2 * sig );
					mu = gf->GetParameter( 1 );
					sig = gf->GetParameter( 2 );
					TFitResultPtr r = h->Fit( gf, "RLS", "", mu - 2*sig, mu + 2 * sig );
					
					INFO( classname(), "Integral" );


					// mu = gf->GetParameter( 1 );
					// sig = gf->GetParameter( 2 );
					// mu = gf->GetParameter( 1 );
					// sig = gf->GetParameter( 2 );
					// if ( mu != mu ) mu = 0.0;
					// if ( sig != sig ) sig = 0.012;

					double y = gf->Integral( -1, 1 ) / bw;
					double ye = sqrt( y );//gf->IntegralError( -1, 1, r->GetParams(), r->GetCovarianceMatrix().GetMatrixArray() ) / bw;
					INFO( classname(), "y = " << y << ", +/- " << ye );
					if ( y != y ) {
						ERROR( classname(), "Skipping NaN on " << name );
						continue;
					}

					// // double bLEdge = book->get( pTname )->GetXaxis()->GetBinCenter( iPt + 1 );
					// // INFO( classname(), "Bin Left = " << h->GetXaxis()->GetBinLowEdge( iPt + 1 ) << ", center = " << h->GetBinCenter( iPt + 1 ) );
					// // double fdw = sc->feedDownWeight( plc, bLEdge, cb, c, 0 );
					// // INFO( classname(), "FDW = " << fdw << " @ pT = " << bLEdge );
					// // y = y * fdw;

					INFO( classname(), "Setting Bin" );
					book->setBin( pTname, iPt + 1, y/nE, ye/nE );

				}

				rpl.style( pTname ).set( "color", "red" );
			}
		}


	}// make


	


protected:
	unique_ptr<SpectraCorrecter> sc;

};


#endif