#include "Correction/ApplyPostCorr.h"
#include "Common.h"

#include <algorithm>

ApplyPostCorr::ApplyPostCorr( XmlConfig * _cfg, string _nodePath ) : HistoAnalyzer( _cfg, _nodePath ){
	INFO( "ApplyPostCorr" )
	if ( cfg == nullptr )
		ERROR( "Invalid config" )
	else {
		setupCorrections();
		plc = cfg->getString( nodePath + "input:plc", "UNKNOWN" );
	}


	apply_feeddown = cfg->getBool( nodePath + "FeedDown:apply", true );
	apply_tofEff = cfg->getBool( nodePath + "TofEff:apply", true );
}

void ApplyPostCorr::setupCorrections(){
	INFO("")
	// Efficiency corrector
	sc = unique_ptr<SpectraCorrecter>( new SpectraCorrecter( cfg, nodePath ) ); 


	vector<string> cfgPaths = { "TofEff" };

	for ( string cPath : cfgPaths ){
		for ( string plc : Common::species ){
			for ( string c : Common::sCharges ){

				if ( cfg->exists( nodePath + cPath + "." + plc + "_" + c ) ){
					for ( int cb : cfg->getIntVector( nodePath + "CentralityBins" ) ){
						string path = nodePath + cPath + "." + plc + "_" + c + "." + cPath + "Params[" + ts(cb) + "]";
						
						if ( cb != cfg->getInt( path + ":bin" ) )
							ERROR( "Centrality Bin Mismatch" )

						// if ( "FeedDown" == cPath )
						// 	feedDown[ plc + "_" + c + "_" + ts(cb) ] = unique_ptr<ConfigFunction>( new ConfigFunction( cfg, path ) );
						// if ( "TpcEff" == cPath )
						// 	tpcEff[ plc + "_" + c + "_" + ts(cb) ] = unique_ptr<ConfigFunction>( new ConfigFunction( cfg, path ) );
						if ( "TofEff" == cPath )
							tofEff[ plc + "_" + c + "_" + ts(cb) ] = unique_ptr<ConfigGraph>( new ConfigGraph( cfg, path ) );
					}
				} else {
					WARN( "Cannot find " + cPath + " params for " << plc + "_" + c )
				}

			} // c
		} // plc
	} // cPath
}



void ApplyPostCorr::make(){
	INFO("")


	if ( !inFile || !inFile->IsOpen() ){
		ERROR( "InFile is invalid" )
		return;
	}

	double feedDownSysNSigma = cfg->getDouble( nodePath + "FeedDown:systematic", 0 );
	INFO( "ApplyPostCorr", "FeedDown Systematic " << feedDownSysNSigma << " sigma" );

	for ( int cg : Common::charges ){
		for ( int cb : cfg->getIntVector( nodePath + "CentralityBins" ) ){

			TRACE( "Working on charge=" << cg << ", cen=" << cb )

			string scg = Common::chargeString( cg );
			string scb = ts( cb );

			string cyn = Common::yieldName( plc, cb, cg );

			TH1 * h = (TH1*)inFile->Get( (plc + "_yield/" + cyn ).c_str() );

			if ( !h ){
				ERROR( "Histogram is invalid" )
				continue;
			}

			book->cd( plc + "_yield" );

			TH1 * full = (TH1*)h->Clone( cyn.c_str() );
			TH1 * orig = (TH1*)h->Clone( ( "orig_" + cyn ).c_str() );
			// TH1 * tpc = (TH1*)h->Clone( ( "tpc_" + cyn).c_str() );
			TH1 * tof = (TH1*)h->Clone( ( "tof_" + cyn).c_str() );
			TH1 * fd = (TH1*)h->Clone( ( "fd_" + cyn).c_str() );

			// TH1 * effTpc = (TH1*)h->Clone( ( "effTpc_" + cyn ).c_str() );
			TH1 * effTof = (TH1*)h->Clone( ( "effTof_" + cyn ).c_str() );
			TH1 * fdCorr = (TH1*)h->Clone( ( "fdCorr_" + cyn ).c_str() );

			book->cd( plc + "_yield" );
			book->add( cyn, 			full );
			book->add( "orig_" + cyn, 	orig );
			// book->add( "tpc_" + cyn, 	tpc );
			book->add( "tof_" + cyn, 	tof );
			book->add( "fd_" + cyn, 	fd );

			// book->add( "effTpc_" + cyn, 	effTpc );
			book->add( "fdCorr_" + cyn, 	fdCorr );

			//book->get( "tpc_" + cyn );

			string param = plc + "_" + scg + "_" + scb;
			for ( int iB = 1; iB <= h->GetNbinsX(); iB++ ){
				double bCon = h->GetBinContent( iB );
				double bCen = h->GetBinLowEdge( iB );
				double fc = bCon;

				// apply TPC eff
				// if ( tpcEff.count( param ) >= 1 ){
					
				// 	double weight = 1.0 / tpcEff[ param ]->eval( bCen );
				// 	fc = fc * ( weight );

				// 	book->setBin( "effTpc_" + cyn, iB, tpcEff[ param ]->eval( bCen ), 0 );
				// 	INFO( "TpcEff weight = " << weight )
				// 	book->setBinContent( "tpc_" + cyn, iB, bCon * ( weight ) );
				// } else {
				// 	ERROR( "Cannot find tpcEff[ " << param << " ] "  )
				// }

				// apply Tof Eff
				if ( tofEff.count( param ) >= 1 && apply_tofEff ){
					
					double weight = 1.0 / tofEff[ param ]->eval( bCen, "closest" );
					fc = fc * ( weight );

					book->setBin( "effTof_" + cyn, iB, tofEff[ param ]->eval( bCen, "closest" ), 0 );
					INFO( "tofEff weight = " << weight )
					book->setBinContent( "tof_" + cyn, iB, bCon * ( weight ) );
				} else {
					ERROR( "Cannot find tofEff[ " << param << " ] "  )
				}
				



				// Feed down
				if ( "K" != plc && apply_feeddown ) {
							
					double weight = sc->feedDownWeight( plc, bCen, cb, cg, feedDownSysNSigma );
					fc = fc * ( weight );

					book->setBin( "fdCorr_" + cyn, iB, weight, 0 );
					INFO( "feedDown weight = " << weight )
					
					book->setBinContent( "fd_" + cyn, iB, bCon * ( weight ) );
					
				}


				book->setBinContent( cyn, iB, fc );
				book->get( cyn )->Sumw2();
			}


		} // cb
	} // cs


}