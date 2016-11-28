#include "Correction/ApplyPostCorr.h"
#include "Common.h"

#include <algorithm>

ApplyPostCorr::ApplyPostCorr(){
}
void ApplyPostCorr::initialize(){
	setupCorrections();
	plc = config.getString( nodePath + ".input:plc", "UNKNOWN" );
	


	/*************************** Only if we are NOT applying trac-by-track ****************************/
	apply_pTFactor = config.getBool( nodePath + ".Factors:pTFactor", false );
	apply_binWidth = config.getBool( nodePath + ".Factors:binWidth", false );
	apply_dy = config.getBool( nodePath + ".Factors:dy", false );
	apply_twopi = config.getBool( nodePath + ".Factors:twopi", false );
	apply_tpcEff = config.getBool( nodePath + ".TpcEff:apply", false );
	/*************************** Only if we are NOT applying trac-by-track ****************************/
		

	apply_feeddown = config.getBool( nodePath + ".FeedDown:apply", true );
	apply_tofEff = config.getBool( nodePath + ".TofEff:apply", true );

	INFO( classname(), "Apply TpcEff (Nominal is FALSE) : " << bts( apply_tpcEff ) );
	INFO( classname(), "Apply pT Factor : " << bts( apply_pTFactor ) );
	INFO( classname(), "Apply bin Width : " << bts( apply_binWidth ) );
	INFO( classname(), "Apply dy : " << bts( apply_dy ) );
	INFO( classname(), "Apply two pi : " << bts( apply_twopi ) );


	INFO( classname(), "Apply TofEff : " << bts( apply_tofEff ) );
	INFO( classname(), "Apply Feed-down : " << bts( apply_feeddown ) );
}

void ApplyPostCorr::setupCorrections(){
	DEBUG( classname(), "");
	// Load corrections
	sc = unique_ptr<SpectraCorrecter>( new SpectraCorrecter( config, nodePath ) ); 
}


void ApplyPostCorr::setupCloneArmy( TH1 * h_origin, string cyn, string plc ){
	TRACE( classname(), "Clones for " << cyn << ", h=" << h_origin );
	
	book->cd( plc + "_yield" );
	// the yield after each correction is applied
	book->addClone( "original_" + cyn, h_origin );
	// book->addClone( "post_tofEff_" + cyn, h_origin );
	// book->addClone( "post_tpcEff_" + cyn, h_origin );
	// book->addClone( "post_pTFactor_" + cyn, h_origin );
	// book->addClone( "post_dy_" + cyn, h_origin );
	// book->addClone( "post_pTBin_" + cyn, h_origin );
	book->addClone( cyn, h_origin );



	// plot the "raw" correction applied to each bin
	book->cd( plc + "_yield/corr_fd" );
		book->addClone( "corr_fd_" + cyn, h_origin );
	book->cd( plc + "_yield/corr_tof" );
		book->addClone( "corr_tofEff_" + cyn, h_origin );
	book->cd( plc + "_yield/corr_tpc" );
		book->addClone( "corr_tpcEff_" + cyn, h_origin );
	book->cd( plc + "_yield/corr_pTFactor" );
		book->addClone( "corr_pTFactor_" + cyn, h_origin );
	book->cd( plc + "_yield/corr_dy" );
		book->addClone( "corr_dy_" + cyn, h_origin );
	book->cd( plc + "_yield/corr_pTBinWidth" );
		book->addClone( "corr_pTBinWidth_" + cyn, h_origin );

	book->cd( plc + "_yield/corr_full" );
		book->addClone( "corr_full_" + cyn, h_origin );


}


void ApplyPostCorr::make(){
	DEBUG( classname(), "");
	setCurrentFile( "data" );

	if ( !inFile || !inFile->IsOpen() ){
		ERROR( classname(), "InFile is invalid" );
		return;
	}


	double feedDownSysNSigma = config.getDouble( nodePath + ".FeedDown:systematic", 0 );
	INFO( classname(), "FeedDown Systematic " << feedDownSysNSigma << " sigma" );

	if ( !config.exists( "TrackCuts" ) ){
		ERROR( "TrackCuts not provided" );
	}
	double dy = ( config.getDouble( "TrackCuts.rapidity:max" ) - config.getDouble( "TrackCuts.rapidity:min" ) );
	INFO( classname(), "dy = " << dy );


	for ( int cg : Common::charges ){
		for ( int cb : config.getIntVector( nodePath + ".CentralityBins" ) ){
			INFO( classname(), "Working on charge=" << cg << ", cen=" << cb );

			string scg   = Common::chargeString( cg );
			string scb   = ts( cb );
			string cyn   = Common::yieldName( plc, cb, cg );
			string hname = plc + "_yield/" + cyn;
			INFO( classname(), "Getting : " << hname );
			TH1 * h = get1D( hname );

			if ( !h ){
				ERROR( classname(), "Histogram is invalid" );
				continue;
			}
			h->SetDirectory(0);

			book->cd( plc + "_yield" );

			setupCloneArmy( h, cyn, plc );
			

			string param = plc + "_" + scg + "_" + scb;
			for ( int iB = 1; iB <= h->GetNbinsX(); iB++ ){
				
				double bCon = h->GetBinContent( iB );
				double bLEdge = h->GetBinLowEdge( iB );
				double bWidth = h->GetBinWidth( iB );
				double bCen = bLEdge + bWidth / 2.0;
				double fc = bCon;


				/***********************************************************/
				// Plot raw corrections
				book->cd( plc + "_yield/corr_tof" );
				book->setBin( 	"corr_tofEff_" + cyn,
								iB,
								sc->tofEffCorr( plc, bCen, cb, cg ),
								0.0 );

				book->cd( plc + "_yield/corr_tpc" );
				book->setBin( 	"corr_tpcEff_" + cyn,
								iB,
								sc->tpcEffCorr( plc, bCen, cb, cg ),
								0.0 );

				book->cd( plc + "_yield/corr_pTFactor" );
				book->setBin( 	"corr_pTFactor_" + cyn,
								iB,
								1.0 / bCen,
								0.0 );

				book->cd( plc + "_yield/corr_pTBinWidth" );
				book->setBin( 	"corr_pTBinWidth_" + cyn,
								iB,
								1.0 / bWidth,
								0.0 );

				book->cd( plc + "_yield/corr_dy" );
				book->setBin( 	"corr_dy_" + cyn,
								iB,
								1.0 / dy,
								0.0 );

				book->cd( plc + "_yield/corr_full" );
				book->setBin( 	"corr_full_" + cyn,
								iB,
								(1.0 / dy) * ( 1.0 / bCen) * ( 1.0 / bWidth ) * ( 1.0 / (2 * 3.1415926) ) * sc->tpcEffWeight( plc, bCen, cb, cg, 0 ),
								0.0 );
				/***********************************************************/


				/***********************************************************/
				// Correct spectra and plot
				if ( apply_dy ){
					fc = fc * (1.0 / dy);
				}
				if ( apply_pTFactor ){
					fc = fc * ( 1.0 / bCen );
				}
				if ( apply_binWidth ){
					fc = fc * ( 1.0 / bWidth );
				} 
				if ( apply_tofEff ){
					fc = fc * sc->tofEffWeight( plc, bLEdge, cb, cg, 0 );
				}
				if ( apply_feeddown ){
					fc = fc * sc->feedDownWeight( plc, bLEdge, cb, cg, feedDownSysNSigma );
				}
				if ( apply_tpcEff ){
					fc = fc * sc->tpcEffWeight( plc, bCen, cb, cg, 0 );
				}
				if ( apply_twopi ){
					fc = fc * (1.0 / (2 * 3.1415926));
				}

				/***********************************************************/
				
				book->cd( plc + "_yield" );
				book->setBinContent( cyn, iB, fc );
			
			} // iBin
		} // cb
	} // cs


}