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

		vector<int> charges = config.getIntVector( "binning.charges" );
		// centralityBinMap = config.getIntMap( nodePath + ".CentralityMap" );
    	vector<int> centralityBins = config.getIntVector( nodePath + ".CentralityBins" );
    	HistoBins binsPt( config, "binning.pt" );

    	INFO( classname(), vts( charges ) );
    	INFO( classname(), vts( centralityBins ) );

    	book->make( config, config.q( nodePath + ".histograms.Histo{name==pT}" ) );

    	TF1 * gf = new TF1( "gf", "gaus" );

    	string plc = config.getString( nodePath + ".input:plc" );

    	for ( int cb : centralityBins ) {

    		double nE = hCen->GetBinContent( cb + 1 );
    		INFO( classname(), "nE[" << cb << "] = " << nE );

			for ( int c : charges ) {	
		
				string pTname = "pT_" + Common::speciesName( plc, c, cb );
				book->clone( "pT", pTname );
				INFO( classname(), ANSIColors::color( "Making :" + pTname, "red" ) );
				for ( int iPt = 0; iPt < binsPt.nBins(); iPt++ ){
					if ( iPt >= 15 ) {
						// book->setBin( pTname, iPt + 1, 0.1, 0.01 );
						continue;
					}

					// create a PidPoint
					string name = "zd_" + Common::speciesName( plc, c, cb, iPt );
					INFO( classname(), "Looking for : " << name );
					TH1 * h = get1D( name );


					INFO( classname(), "TH1: " << h  );
					book->add( name, h );
					h->SetDirectory( gDirectory );


					double bw = h->GetBinWidth( 1 ); // fixed width so it does not matter which
					h->Fit( gf, "RL", "", -0.16, 0.4 );
					double mu = gf->GetParameter( 1 );
					double sig = gf->GetParameter( 2 );
					h->Fit( gf, "RL", "", mu - 2*sig, mu + 2 * sig );
					mu = gf->GetParameter( 1 );
					sig = gf->GetParameter( 2 );
					h->Fit( gf, "RL", "", mu - 2*sig, mu + 2 * sig );
					
					double y = gf->Integral( -1, 1 ) / bw;
					double ye = gf->IntegralError( -1, 1 ) / bw;



					// double bLEdge = book->get( pTname )->GetXaxis()->GetBinCenter( iPt + 1 );
					// INFO( classname(), "Bin Left = " << h->GetXaxis()->GetBinLowEdge( iPt + 1 ) << ", center = " << h->GetBinCenter( iPt + 1 ) );
					// double fdw = sc->feedDownWeight( plc, bLEdge, cb, c, 0 );
					// INFO( classname(), "FDW = " << fdw << " @ pT = " << bLEdge );
					// y = y * fdw;

					book->setBin( pTname, iPt + 1, y/nE, ye/nE );
					rpl.style( pTname ).set( "color", "red" );




					
				}
			}
		}


	}// make


	


protected:
	unique_ptr<SpectraCorrecter> sc;

};


#endif