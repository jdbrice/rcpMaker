
#include "TSF/FitRunner.h"
#include "Spectra/PidHistoMaker.h"

#include "TGraph.h"
#include "TBox.h"


namespace TSF{
	FitRunner::FitRunner( ){
	}

	void FitRunner::initialize(  ) {
		


		// Initialize the Phase Space Recentering Object
		tofSigmaIdeal = config.getDouble( nodePath+".ZRecentering.sigma:tof", 0.0012);
		dedxSigmaIdeal = config.getDouble( nodePath+".ZRecentering.sigma:dedx", 0.06);
		psr = new ZRecentering( dedxSigmaIdeal,
										 tofSigmaIdeal,
										 config.getString( nodePath+".Bichsel.table", "dedxBichsel.root"),
										 config.getInt( nodePath+".Bichsel.method", 0) );
		psrMethod = config.getString( nodePath+".ZRecentering.method", "traditional" );
		// alias the centered species for ease of use
		centerSpecies = config.getXString( nodePath+".ZRecentering.centerSpecies", "K" );
		
		
		//Make the momentum transverse, eta, charge binning 
		binsPt = new HistoBins( config, "binning.pt" );


		INFO( classname(), "Image Name Modifier: " << imgNameMod );
		zbReporter = unique_ptr<Reporter>(new Reporter( config.getXString( nodePath + ".output:path" ) + imgNameMod + "_TSF_zb.pdf",
			config.getInt( nodePath + ".Reporter.output:width", 400 ), config.getInt( nodePath + ".Reporter.output:height", 400 ) ) );
		zdReporter = unique_ptr<Reporter>(new Reporter( config.getXString( nodePath + ".output:path" ) + imgNameMod + "_TSF_zd.pdf",
			config.getInt( nodePath + ".Reporter.output:width", 400 ), config.getInt( nodePath + ".Reporter.output:height", 400 ) ) );

		// Logger::setGlobalLogLevel( Logger::logLevelFromString( config.getString( nodePath + ".Logger:globalLogLevel" ) ) );

		for ( string plc : Common::species ){
			for ( string pre : { "zb", "zd" } ){
			
				XmlRange tpecr( &config, nodePath + ".ParameterFixing.tofPidEff." + plc );
				tofPidEffRanges[ plc ] = tpecr;

				if ( config.exists( nodePath + ".ParameterFixing." + pre + "." + plc ) ){
					INFO( tag, "Creating Sigma Fixing range for " << pre << "_" << plc );
					XmlRange cr( &config, nodePath + ".ParameterFixing." + pre + "." + plc );
					sigmaRanges[ pre + "_" + plc ] = cr;
					INFO(tag, cr.toString() )
				}
			}
		}


		rnd = unique_ptr<TRandom3>( new TRandom3() );
		rnd->SetSeed( 0 );

		//The bins to fit over
		centralityFitBins = config.getIntVector( nodePath + ".FitRange.centralityBins" );
		chargeFit = config.getIntVector( nodePath + ".FitRange.charges" );


		
		INFO( classname(), "Fitting Charges[ " << vts( chargeFit ) << " ]" );
		INFO( classname(), "Fitting centralityBins[ " << vts( centralityFitBins ) << " ]" );
		

		// override if we are running parallel jobs
		// if ( iCen >= 0 && iCen <= 6){
		// 	centralityFitBins.clear();
		// 	centralityFitBins.push_back( iCen );
		// }
		// if ( -1 == iCharge || 1 == iCharge){
		// 	chargeFit.clear();
		// 	chargeFit.push_back( iCharge );
		// }

		// HistoAnalyzer::setup();
	}

	FitRunner::~FitRunner(){
	} 


	void FitRunner::makeHistograms(){

		book->cd();
		book->makeAll( nodePath + ".histograms" );
		for ( int iCen : centralityFitBins ){
			for ( int iCharge : chargeFit ){
				for ( string plc : Common::species ){
					// yield Histos
					book->cd( plc + "_yield" );
					string name = Common::yieldName( plc, iCen, iCharge );
					book->clone( "/", "yield", plc+"_yield", name );

					book->cd( "sys_sigma" );
					name = Common::yieldName( plc, iCen, iCharge );
					book->clone( "/", "yield", "sys_sigma", name );

					book->cd( "sys_tofEff" );
					name = Common::yieldName( plc, iCen, iCharge );
					book->clone( "/", "yield", "sys_tofEff", name );

					// Mean Histos
					book->cd( plc + "_zbMu" );
					name = Common::muName( plc, iCen, iCharge );
					book->clone( "/", "yield", plc+"_zbMu", name );

					book->cd( plc + "_zbMu" );
					name = "delta"+Common::muName( plc, iCen, iCharge );
					book->clone( "/", "yield", plc+"_zbMu", name );

					book->cd( plc + "_zdMu" );
					name = Common::muName( plc, iCen, iCharge );
					book->clone( "/", "yield", plc+"_zdMu", name );

					book->cd( plc + "_zdMu" );
					name = "delta" + Common::muName( plc, iCen, iCharge );
					book->clone( "/", "yield", plc+"_zdMu", name );

					// Sigma Histos
					book->cd( plc + "_zbSigma" );
					name = Common::sigmaName( plc, iCen, iCharge );
					book->clone( "/", "yield", plc+"_zbSigma", name );
					name = Common::sigmaName( plc, iCen, iCharge ) + "_rel";
					book->clone( "/", "yield", plc+"_zbSigma", name );

					book->cd( plc + "_zdSigma" );
					name = Common::sigmaName( plc, iCen, iCharge );
					book->clone( "/", "yield", plc+"_zdSigma", name );

					// Plc Tof Efficiency 
					book->cd( "tofEff" );
					book->clone( "/", "yield", "tofEff", Common::effName( plc, iCen, iCharge )  );

					for ( string plc2 : Common::species ){
						book->cd( "zb_enhanced" );
						book->clone("/", "yield", "zb_enhanced", Common::yieldName( plc, iCen, iCharge, plc2 ) );

						book->cd( "zd_enhanced" );
						book->clone("/", "yield", "zd_enhanced", Common::yieldName( plc, iCen, iCharge, plc2 ) );
					}


					book->cd( "tofEff_dist" );
					book->clone( "/", "eff_dist", "tofEff_dist", Common::yieldName( plc, iCen, iCharge ) );
					for ( string plc2 : Common::species ){
						book->clone( "/", "sys_dist", "tofEff_dist", "sys_" + Common::yieldName( plc, iCen, iCharge, plc2 ) );
					}

					book->cd( "zd_sigma_dist" );
					book->clone( "/", "sigma_dist", "zd_sigma_dist", Common::yieldName( plc, iCen, iCharge ) );
					for ( string plc2 : Common::species ){
						book->clone( "/", "sys_dist", "zd_sigma_dist", "sys_" + Common::yieldName( plc, iCen, iCharge, plc2 ) );
					}

					book->cd( "zb_sigma_dist" );
					book->clone( "/", "sigma_dist", "zb_sigma_dist", Common::yieldName( plc, iCen, iCharge ) );
					for ( string plc2 : Common::species ){
						book->clone( "/", "sys_dist", "zb_sigma_dist", "sys_" + Common::yieldName( plc, iCen, iCharge, plc2 ) );
					}



				} // loop plc
			} // loop iCharge
		} // loop iCen
	} // makeHistograms()

	void FitRunner::prepare( double avgP, int iCen ){
		TRACE( tag, "( avgP=" << avgP << ", iCen=" << iCen << ")" )

		//Constraints on the mu 	 
		double zbDeltaMu = config.getDouble( nodePath + ".ParameterFixing.deltaMu:zb", 1.5 );
		double zdDeltaMu = config.getDouble( nodePath + ".ParameterFixing.deltaMu:zd", 1.5 );

		// fit roi
		

		activePlayers.clear();
		
		for ( string plc : Common::species ){

			INFO( tag, "Setting up " << plc )
			if ( !schema->exists( "yield_" + plc ) ){
				WARN( "No Yield exists for " << plc << " so we are skipping it" )
				continue;
			}

			// zb Parameters
			double zbMu = zbMean( plc, avgP );
			double zbSig = zbSigma( );

			// zd Parameters
			double zdMu = zdMean( plc, avgP );
			double zdSig = zdSigma( );
			

			// default low pt settings
			schema->setInitialSigma( "zb_sigma_"+plc, zbSig, zbSig * 0.5, zbSig * 12 );
			
			// start the proton out with a bigger sigma to help with the enhancement cuts
			// this is just the initial value
			if ( "P" == plc && avgP < 0.75 )
				schema->setInitialSigma( "zb_sigma_"+plc, zbSig * 3, zbSig * 0.5, zbSig * 12 );

			if ( 0 >= zbDeltaMu ) // only used for testing
				schema->fixParameter( "zb_mu_"+plc, zbMu );
			else if ( !sigmaRanges[ "zb_" + plc ].above( avgP ) )	// actual default for running
				schema->setInitialMu( "zb_mu_"+plc, zbMu, zbSig, 10.0 );

			if ( sigmaRanges[ "zb_" + plc ].above( avgP ) ){	
				
				double hm = sigmaSets[ "zb_" + plc ].mean();
				if ( 0 >= hm  )	// for running not in order so the sigma set isn't filled
					hm = zbSig;

				schema->setInitialMu( "zb_mu_"+plc, zbMu, hm, zbDeltaMu );

				INFO( tag, "Fixing zb_sigma_" << plc << " to " << sigmaSets[ "zb_" + plc ].mean() )
				//schema->setInitialSigma( "zb_sigma_"+plc, hm, hm, hm );
				schema->fixParameter( "zb_sigma_"+plc, hm );
			}

			// default low pt settings for zd
			if ( 0 >= zdDeltaMu ) // for testing
				schema->fixParameter( "zd_mu_"+plc, zdMu );
			else if ( !sigmaRanges[ "zd_" + plc ].above( avgP ) )
				schema->setInitialMu( "zd_mu_"+plc, zdMu, zdSig, 10.0 );

			schema->setInitialSigma( "zd_sigma_"+plc, zdSig, 0.04, 0.24);

			if ( sigmaRanges[ "zd_" + plc ].above( avgP ) ){	
				
				double hm = sigmaSets[ "zd_" + plc ].mean();
				if ( 0 >= hm  )	// for running not in order so the sigma set isn't filled
					hm = zdSig;
				schema->setInitialSigma( "zd_sigma_"+plc, hm, hm - 0.002, hm + 0.002  );
				schema->setInitialMu( "zd_mu_"+plc, zdMu, hm, zbDeltaMu );
			}
				
				
			// choose the active players
			choosePlayers( avgP, plc );
			
			// double eff_fudge = 0.01;
			// leave it as whatever it is in config
			// schema->var( "eff_" + plc )->val = 1.0 ;//+ ( rnd->Rndm() * (2 * eff_fudge) - eff_fudge );
			schema->var( "eff_" + plc )->fixed = false;
			if ( tofPidEffRanges[ plc ].above( avgP ) ){
				schema->var( "eff_" + plc )->val = tofPidEffSets[ plc ].mean();
				schema->var( "eff_" + plc )->error = tofPidEffSets[ plc ].std() * 2;
				schema->var( "eff_" + plc )->fixed = true;
			}


			
		} // loop on plc to set initial vals
	} // perpare(...)

	void FitRunner::choosePlayers( double avgP, string plc ){

		double zdOnly = config.getDouble( nodePath + ".Timing:zdOnly" , 0.5 );
		double useZdEnhanced = config.getDouble( nodePath + ".Timing:useZdEnhanced" , 0.6 );
		double useZbEnhanced = config.getDouble( nodePath + ".Timing:useZbEnhanced" , 0.6 );
		double nSigZbEnhanced = config.getDouble( nodePath + ".Timing:nSigZbEnhanced" , 3.0 );
		double nSigZdEnhanced = config.getDouble( nodePath + ".Timing:nSigZdEnhanced" , 3.0 );
		double roi = config.getDouble( nodePath + ".FitSchema:roi", -1 );

		if ( roi > 0 ){
			double zbSig = zbSigma( );
			double zbMu = zbMean( plc, avgP );
			double zdMu = zdMean( plc, avgP );
			double zdSig = zdSigma(  );

			if ( avgP > zdOnly ){
				schema->addRange( "zb_All", zbMu - zbSig * roi, zbMu + zbSig * roi, "zb_mu_" + plc, "zb_sigma_" + plc, roi );
			}
			schema->addRange( "zd_All", zdMu - zdSig * roi, zdMu + zdSig * roi, "zd_mu_" + plc, "zd_sigma_" + plc, roi );

		}

		// always include the total yields
		activePlayers.push_back( "zd_All_g" + plc );
		schema->var( "yield_" + plc )->exclude = false;

		// exclude all enhanced yields for a clean slate
		for ( string plc2 : Common::species ){
			schema->var( "zb_"+plc+"_yield_"+plc2 )->exclude = true;
			schema->var( "zd_"+plc+"_yield_"+plc2 )->exclude = true;
		}

		if ( avgP >= zdOnly ){
			// remove the zb variables
			activePlayers.push_back( "zb_All_g" + plc );
			schema->var( "zb_sigma_" + plc )->exclude 	= false;
			schema->var( "zb_mu_" + plc )->exclude 	= false;
		} else {
			schema->var( "zb_sigma_" + plc )->exclude = true;
			schema->var( "zb_mu_" + plc )->exclude 	= true;
		}

		if ( avgP < useZdEnhanced ){
			for ( string plc2 : Common::species ){
				schema->var( "zd_"+plc+"_yield_"+plc2 )->exclude = true;
			}
		} else {

			double zbSig = zbSigma(  );
			double zbMu = zbMean( plc, avgP);

			for ( string plc2 : Common::species ){
				double zdMu2 = zdMean( plc2, avgP );
				double zdSig2 = zdSigma(  );
				string var = "zd_"+plc+"_yield_"+plc2;
				
				bool firstTimeIncluded = false;
				if ( schema->var( var )->exclude )
					firstTimeIncluded = true;

				double zbMu2 = zbMean( plc2, avgP );
				double zbNd = ( zbMu - zbMu2 ) / zbSig;

				if ( abs( zbNd ) < nSigZbEnhanced /*&& schema->datasetActive( "zd_" + plc )*/ ){ // TODO: remove broke codes
					activePlayers.push_back( "zd_" + plc + "_g" + plc2 );
					

					schema->var( var )->exclude = false;
					schema->var( var )->min = 0;
					schema->var( var )->max = 1.0;

					if ( roi > 0 )
						schema->addRange( "zd_" + plc, zdMu2 - zdSig2 * roi, zdMu2 + zdSig2 * roi, "zd_mu_" + plc2, "zd_sigma_" + plc2, roi );

					if ( firstTimeIncluded && plc != plc2 ){
						schema->var( var )->val = 0.01;
						schema->var( var )->error = 0.001;
					}
					// TODO: initial yield?
					// schema->var( var )->val = 0.00001;
					

				} else {
					schema->var( "zd_"+plc+"_yield_"+plc2 )->exclude = true;
				}
			}

		}


		if ( avgP < useZbEnhanced ){
			for ( string plc2 : Common::species ){
				schema->var( "zb_"+plc+"_yield_"+plc2 )->exclude = true;
			}
		} else {

			double zdMu = zdMean( plc, avgP );
			double zdSig = zdSigma( );

			for ( string plc2 : Common::species ){
				double zbSig2 = zbSigma(  );
				double zbMu2 = zbMean( plc2, avgP );

				string var = "zb_"+plc+"_yield_"+plc2;
				bool firstTimeIncluded = false;
				if ( schema->var( var )->exclude )
					firstTimeIncluded = true;

				double zdMu2 = zdMean( plc2, avgP );
				double zdNd = ( zdMu - zdMu2 ) / zdSig;

				if ( abs( zdNd ) < nSigZdEnhanced /*&& schema->datasetActive( "zb_" + plc ) */ ){ // TODO: remove broke codes
					activePlayers.push_back( "zb_" + plc + "_g" + plc2 );
					

					schema->var( var )->exclude = false;
					schema->var( var )->min = 0;
					schema->var( var )->max = 1.0;

					if ( roi > 0 )
						schema->addRange( "zb_" + plc, zbMu2 - zbSig2 * roi, zbMu2 + zbSig2 * roi, "zb_mu_" + plc2, "zb_sigma_" + plc2, roi );

					if ( firstTimeIncluded && plc != plc2){
						schema->var( var )->val = 0.01;
						schema->var( var )->error = 0.001;
					}
					// TODO: initial yield?
					// schema->var( var )->val = 0.01;

				} else {
					schema->var( "zb_"+plc+"_yield_"+plc2 )->exclude = true;;
				}
			}

		}
	} // choosePlayers(...)


	void FitRunner::respondToStats(double avgP){
		for ( string plc : Common::species ){

			schema->var( "yield_" + plc )->min = 0;
			schema->var( "yield_" + plc )->max = schema->getNormalization() * 2;

			for ( string pre : { "zb_", "zd_" } ){
				for ( string pplc : Common::species ){
					schema->var( pre + pplc + "_yield_" + plc )->min = 0;
					schema->var( pre + pplc + "_yield_" + plc )->max = schema->getNormalization() * 2;
				}
			}

			// if ( !schema->datasetActive( "zd_" + plc ) ){
			// 	double zdMu = zdMean( plc, avgP );
			// 	schema->fixParameter( "zd_mu_"+plc, zdMu );
			// }

		}
	} // respondToStats(...)

	void FitRunner::runNominal( int iCharge, int iCen, int iPt, shared_ptr<FitSchema> _schema ) {
		WARN( tag, "(iCharge=" << iCharge << ", iCen=" << iCen << ", iPt=" << iPt << ")" );

		double avgP = binAverageP( iPt );
		auto zbMu = psr->centeredTofMap( centerSpecies, avgP );
		auto zdMu = psr->centeredDedxMap( centerSpecies, avgP );

		schema->clearRanges();

		// create the fitter
		shared_ptr<FitSchema> mySchema = schema;
		if ( nullptr != _schema )
			mySchema = _schema;
		
		Fitter fitter( mySchema, inFile );
		
		

		// loads the default values used for data projection
		fitter.registerDefaults( &config, nodePath );
		
		// prepare initial values, ranges, etc. for fit
		prepare( avgP, iCen );

		// double nSigmaAbovePOverride = 50;
		// if ( avgP > 1.4 )
		// 	nSigmaAbovePOverride = -1;

		bool enhanced = config.getBool( nodePath + ".FitSchema:enhanced", true );
		bool use_zb = config.getBool( nodePath + ".FitSchema:use_zb", true );

		// load the datasets from the file
		fitter.loadDatasets(centerSpecies, iCharge, iCen, iPt, enhanced, zbMu, zdMu );

		respondToStats( avgP ); 

		// build the minuit interface
		fitter.setupFit();
		// assign active players to this fit
		fitter.addPlayers( activePlayers );
			
		
		for ( int i = 0; i < 3; i ++){
			// gets close on yield with fixed shapes
			fitter.fit1(  );
			// gets close on shapes with fixed yields
			fitter.fit2(  );
		}
		
		if ( enhanced ){
			for ( int i = 0; i < 3; i ++){
				fitter.loadDatasets(centerSpecies, iCharge, iCen, iPt, enhanced, zbMu, zdMu );
				// gets close on yield with fixed shapes
				fitter.fit1(  );
				// gets close on shapes with fixed yields
				fitter.fit2(  );
			}
		}
		
		int tries = 0;
		while( fitter.isFitGood() == false && tries < 3 ){
			fitter.fit3( );
			tries ++;
		}

		fitter.fitErrors();
		

		if ( nullptr == _schema ){


			reportFit( &fitter, iPt );
		
			// fill histograms if we converged
			if ( fitter.isFitGood())
				fillFitHistograms(iPt, iCen, iCharge, fitter );



			for ( string plc : Common::species ){

				// tof pid efficiency
				if ( tofPidEffRanges[ plc ].inInclusiveRange( avgP ) ){
					tofPidEffSets[ plc ].add( schema->var( "eff_" + plc )->val );
				}


				// Keep track of the sigma for each species for fixing at high pt
				for ( string pre : {"zb", "zd"} ){
					XmlRange &range = sigmaRanges[ pre + "_" + plc ];
					// if we are in the good p range then add this value to the set
					if ( range.inInclusiveRange( avgP ) )
						sigmaSets[ pre+"_"+plc ].add( schema->var( pre + "_sigma_" + plc )->val ); 	
				} // plc
			} // pre
		}
	} // runNominal(...)


	void FitRunner::runSigmaSystematic( int iCharge, int iCen, int iPt ){
		// Run systematics on sigma
		
		double avgP = binAverageP( iPt );

		int N = config.getInt( nodePath + ".Systematics:nSigma", 5 );

		for ( string pre : { "zb", "zd" } ){
			for ( string plc : Common::species ){
				
				XmlRange &range = sigmaRanges[ pre + "_" + plc ];
				if ( !range.above( avgP ) )
					continue;

				INFO( tag, "Do Systematics for " << pre << "_" << plc );
				
				book->cd( pre + "_sigma_dist" );
				string d_name = Common::yieldName( plc, iCen, iCharge );

				for ( int i = 0; i < N; i++ ){

					double mean = sigmaSets[ pre+"_"+plc ].mean();
					double stdev = sigmaSets[ pre+"_"+plc ].std();

					// for testing only
					if ( 0 == mean && "zd" == pre )
						mean = 0.07;
					else if ( 0 == mean && "zb" == pre )
						mean = 0.012;
					if ( 0 == stdev && "zd" == pre )
						stdev = 0.007;
					else if ( 0 == stdev && "zb" == pre )
						stdev = 0.0012; 

					double delta = rnd->Gaus( mean, stdev ) - mean;
					INFO( tag, "Gaus( " << mean <<", " << stdev << " ) = " << delta );
					book->fill( d_name, avgP, delta );

					shared_ptr<FitSchema> sysSchema = prepareSystematic( pre + "_sigma", plc, delta );
					runSystematic( sysSchema, iCharge, iCen, iPt );		

					for (string pplc : Common::species ){
						double y_delta = sysSchema->var( "yield_" + pplc )->val - schema->var( "yield_" + pplc )->val;
						y_delta = y_delta /  schema->var( "yield_" + pplc )->val;
						string sd_name = "sys_" + Common::yieldName( plc, iCen, iCharge, pplc );
						book->fill( sd_name, avgP, y_delta );
					}
				}
			} // plc
		} // { zb, zd }
	} // runSigmaSystematic(...)


	void FitRunner::runTofEffSystematic( int iCharge, int iCen, int iPt ){
		double avgP = binAverageP( iPt );
		book->cd( "tofEff_dist" );

		int N = config.getInt( nodePath + ".Systematics:nTofEff=", 5 );
		double amt = config.getDouble( nodePath + ".Systematics:tofEffAmount", 0.15 );

		shared_ptr <FitSchema> rSchema = shared_ptr<FitSchema>( new FitSchema( *schema ) );

		map<string, vector<double> > sys;
		for ( int i = 0; i < N; i ++ ){
			for ( string plc : Common::species ){
				string d_name = Common::yieldName( plc, iCen, iCharge );
				double delta = rnd->Rndm() * (amt*2) + ( 1.0 - amt );
				INFO( tag, "TOF EFF AMOUNT " << (amt * 2) << " + " << 1.0 - amt );
				for ( string oplc : Common::species ){
					rSchema->var( "eff_" + oplc )->val = 1.0;
				}
				rSchema->var( "eff_" + plc )->val = delta;

				book->fill( d_name, avgP, delta );

				runNominal( iCharge, iCen, iPt, rSchema );

				// now get the yields and add them to the list
				for ( string pplc : Common::species ){
					WARN( "DAN", "yield_" << pplc << " = " << rSchema->var( "yield_" + pplc )->val );
					string s_name = "sys_" + Common::yieldName( plc, iCen, iCharge, pplc );

					double _sys = (rSchema->var( "yield_" + pplc )->val - schema->var( "yield_" + pplc )->val ) / schema->var( "yield_" + pplc )->val;
					sys[ pplc ].push_back( _sys );
					
					book->fill( s_name, avgP, _sys );
				}
			}
		}

		book->cd( "tofEff_dist" );
		// 	INFO( tag, "SYSTEMATIC from TofEff" );
		for ( auto k : sys ){
			for ( auto v : k.second ){
				INFO( tag, "systematic = " << v );
			}
			vector<double> v = k.second;
			double sum = std::accumulate(v.begin(), v.end(), 0.0);
			double mean = sum / v.size();

			double max = *std::max_element( v.begin(), v.end() );
			double min = *std::min_element( v.begin(), v.end() );
			if ( abs(min) > abs(max) )
				max = min;

			double sq_sum = std::inner_product(v.begin(), v.end(), v.begin(), 0.0);
			double stdev = std::sqrt(sq_sum / v.size() - mean * mean);

			INFO( tag, "MAX = " << max );
			INFO( tag, "AVG = " << mean );
			INFO( tag, "STDEV = " << stdev );
			
		}

	}

	void FitRunner::make(){

		if ( inFile == nullptr || inFile->IsOpen() == false ){
			ERROR( "Invalid input data file - can't make" )
			return;
		}


		// Make the histograms for storing the results
		makeHistograms();

		int firstPtBin = config.getInt( nodePath + ".FitRange.ptBins:min", 0 );
		int lastPtBin = config.getInt( nodePath + ".FitRange.ptBins:max", 1 );

		if ( lastPtBin >= binsPt->nBins() )
			lastPtBin = binsPt->nBins() - 1;

		 for ( int iCen : centralityFitBins ){
			for ( int iCharge : chargeFit ){
							
				//Create the schema and fitter 
				schema = shared_ptr<FitSchema>(new FitSchema( &config, nodePath + ".FitSchema" ));

				sigmaSets.clear();
				tofPidEffSets.clear();
				for ( int iPt = firstPtBin; iPt <= lastPtBin; iPt++ ){

					runNominal( iCharge, iCen, iPt );

					reportYields();

					if ( config.getBool( nodePath + ".Systematics:tofEff" ) ){
						runTofEffSystematic( iCharge, iCen, iPt );
					}
					else {
						INFO( tag, "Skipping runTofEffSystematic" );
					}

										
					if ( config.getBool( nodePath + ".Systematics:sigma" ) ){
						runSigmaSystematic( iCharge, iCen, iPt );
					} else{
						INFO( tag, "Skipping runSigmaSystematic" );
					}

				}// loop iPt
			} // loop iCharge
		} // loop iCen
	} // make()

	void FitRunner::drawSet( string v, Fitter * fitter, int iPt ){
		INFO( tag,  v << ", fitter=" << fitter << ", iPt=" << iPt );

		double avgP = binAverageP( iPt );
		auto zbMu = psr->centeredTofMap( centerSpecies, avgP );
		auto zdMu = psr->centeredDedxMap( centerSpecies, avgP );

		double zbMin = 100;
		double zbMax = -100;
		for ( auto k : zbMu ){
			if ( k.first == "D" || k.first == "E" )
				continue;
			if ( k.second < zbMin )
				zbMin = k.second;
			if ( k.second > zbMax )
				zbMax = k.second;
		}

		double zdMin = 100;
		double zdMax = -100;
		for ( auto k : zdMu ){
			if ( k.first == "D" || k.first == "E" )
				continue;
			if ( k.second < zdMin )
				zdMin = k.second;
			if ( k.second > zdMax )
				zdMax = k.second;
		}


		TH1 * h = fitter->getDataHist( v );
		if ( !h ){
			WARN( tag, "Data histogram not found" );
			return ;
		}
		h->Draw("pe");
		h->SetLineColor( kBlack );
		double scaler = 1e-8;

		int binmax = h->GetMaximumBin();
		double max = h->GetBinContent( binmax ) * 5;
		h->GetYaxis()->SetRangeUser( 0.1 / fitter->getNorm(), max );

		double xMin = 0, xMax = 0;
		if ( v.substr(0, 2) == "zb" ){
			INFO( tag, "Using zb range ( " << zbMin << " -> " << zbMax << " )" );
			xMin = zbMin - 0.2;
			xMax = zbMax + 0.2;

			// TODO : fix hard code center species
			h->SetTitle( (" ; z_{b}^{" + Common::plc_label( centerSpecies ) + "} ; dN/dz_{b}").c_str() );
		} else {
			INFO( tag, "Using zd range( " << zdMin << " -> " << zdMax << " )" );
			xMin = zdMin - 0.8;
			xMax = zdMax + 0.8;

			h->SetTitle( (" ; z_{d}^{" + Common::plc_label( centerSpecies ) + "}; dN/dz_{d}").c_str() );
		}

		h->GetXaxis()->SetRangeUser( xMin, xMax );
		h->GetXaxis()->SetNdivisions( 505 );
		h->GetYaxis()->SetTitleOffset( 1.4 );

		h->SetTitle( ( setTitle( v ) + " : " +  dts((*binsPt)[ iPt ]) + " < p_{T} [GeV/c] < " + dts( (*binsPt)[ iPt + 1 ] ) ).c_str() );



		// draw boxes to show the fit ranges
		for ( FitRange range : fitter->getSchema()->getRanges() ){
			if ( range.dataset != v )
				continue;
			TBox * b1 = new TBox( range.min, 0.1 / fitter->getNorm(), range.max, max  );
			b1->SetFillColorAlpha( kBlack, 0.25 );
			b1->SetFillStyle( 1001 );
			b1->Draw(  );
		}

		h->Draw("pe same");

		TGraph * sum = fitter->plotResult( v );
		sum->SetLineColor( kBlack );
		sum->SetLineWidth( 2 );
		sum->Draw( "same" );
		
		vector<TGraph*> comps;
		vector<double> colors = { kRed + 1, 	kAzure - 3, 	kGreen + 1, kBlue - 3 };
		int i = 0;
		
		for ( string plc : Common::species ){
			TGraph * g = fitter->plotResult( v+"_g"+plc );
			comps.push_back( g );
			g->SetLineColor( colors[ i ] );
			g->SetLineWidth( 2 );
			g->Draw( "same" );

			i++;
		}

		gPad->SetGrid( 1, 1 );
		gPad->SetLogy(1);
	} // drawSet(...)

	void FitRunner::reportFit( Fitter * fitter, int iPt ){

		bool export_images = true;

		TCanvas* imgCanvas = new TCanvas( "imgCanvas", "imgCanvas", 400, 400 );

		gStyle->SetOptStat(0);
		bool drawBig = false;
		bool drawFitRatios=false;

		if ( drawBig ){
			INFO( tag, "Reporting zd" );
			zdReporter->newPage( 1, 2 );
			{
				zdReporter->cd( 1, 1 );
				drawSet( "zd_All", fitter, iPt );
			}
		
			INFO( tag, "Drawing fit vs. data ratios for zd" )
			{
				zdReporter->cd( 1, 2 );
				drawFitRatio( "zd_All", fitter, iPt );
			}
			zdReporter->savePage();


			INFO( tag, "Reporting zb" );
			zbReporter->newPage( 1, 2 );
			{
				zbReporter->cd( 1, 1 );
				drawSet( "zb_All", fitter, iPt );
			}
		
			INFO( tag, "Drawing fit vs. data ratios for zb" )
			{
				zbReporter->cd( 1, 2 );
				drawFitRatio( "zb_All", fitter, iPt );
			}
			zbReporter->savePage();
		}

		string imgPartA = config.getXString( nodePath + ".output:path" ) + "img/" + imgNameMod + "_";
		string imgPartB =  "_" + ts(iPt) + ".png";
		

		INFO( tag, "Reporting zd" );
		zdReporter->newPage( 2, 2 );
		{

			int pageXY = 1;
			for ( string set : { "zd_All", "zd_Pi", "zd_K", "zd_P" } ){
					
				if ( export_images ){
					imgCanvas->cd();
					drawSet( set, fitter, iPt );
					string imgName = imgPartA + set + imgPartB;
					gPad->Print( imgName.c_str() );
				} 
				zdReporter->cd( pageXY );
				drawSet( set, fitter, iPt );
				
				pageXY++;
			}
		} // reporting zd
		zdReporter->savePage();

		if ( drawFitRatios ){
			INFO( tag, "Drawing fit vs. data ratios for zd" )
			zdReporter->newPage( 2, 2 );
			{
				zdReporter->cd( 1, 1 );
				drawFitRatio( "zd_All", fitter, iPt );
				zdReporter->cd( 2, 1 );
				drawFitRatio( "zd_Pi", fitter, iPt );
				zdReporter->cd( 1, 2 );
				drawFitRatio( "zd_K", fitter, iPt );
				zdReporter->cd( 2, 2 );
				drawFitRatio( "zd_P", fitter, iPt );
			}
			zdReporter->savePage();
		}



		INFO( tag, "Reporting zb" )
		zbReporter->newPage( 2, 2 );
		{
			int pageXY = 1;
			for ( string set : { "zb_All", "zb_Pi", "zb_K", "zb_P" } ){
					
				if ( export_images ){
					imgCanvas->cd();
					drawSet( set, fitter, iPt );
					string imgName = imgPartA + set + imgPartB;
					gPad->Print( imgName.c_str() );
				} 
				zbReporter->cd( pageXY );
				drawSet( set, fitter, iPt );
				
				pageXY++;
			}
		}// reporting zb

		zbReporter->savePage();

		if ( drawFitRatios ){
			INFO( tag, "Drawing fit vs. data ratios for zb" )
			zbReporter->newPage( 2, 2 );
			{
				zbReporter->cd( 1, 1 );
				drawFitRatio( "zb_All", fitter, iPt );
				zbReporter->cd( 2, 1 );
				drawFitRatio( "zb_Pi", fitter, iPt );
				zbReporter->cd( 1, 2 );
				drawFitRatio( "zb_K", fitter, iPt );
				zbReporter->cd( 2, 2 );
				drawFitRatio( "zb_P", fitter, iPt );
			}
			zbReporter->savePage();
		}


		delete imgCanvas;
	} // reportFit(...)

	void FitRunner::reportYields(){

		double total = 0;
		for ( string plc  : Common::species ){
			if ( !schema->exists("yield_" + plc ) )
				continue;
			
			INFO( tag, "Yield of " << plc << " = " << schema->var( "yield_" + plc )->val );
			total += schema->var( "yield_" + plc )->val;
		}

		INFO( tag, "Total Fit Yield  = " <<  total );
		double yzb = schema->datasets[ "zb_All" ].yield();
		double yzd = schema->datasets[ "zd_All" ].yield();
		INFO( tag, "Total Yield in zb_All = " << yzb );
		INFO( tag, "Total Yield in zd_All = " << yzd );

		INFO( tag, "Total Fit Yield / Total Data Yield (zd) = " << total / yzd );
		INFO( tag, "Total Fit Yield / Total Data Yield (zb) = " << total / yzb );

		double roiyzb = schema->datasets[ "zb_All" ].yield( schema->getRanges() );
		double roiyzd = schema->datasets[ "zd_All" ].yield( schema->getRanges() );
		INFO( tag, "zb_All / zd_All in roi = " << roiyzb / roiyzd );
	} // reportYields()

	void FitRunner::fillFitHistograms(int iPt, int iCen, int iCharge, Fitter &fitter ){

		double avgP = binAverageP( iPt );

		for ( string plc : Common::species ){

			double zbMu = zbMean( plc, avgP );
			double zdMu = zdMean( plc, avgP );

			INFO( tag, "Filling Histograms for " << plc );
			int iiPt = iPt + 1;

			// Yield			
			INFO( tag, "Filling Yield for " << plc );
			string name = Common::yieldName( plc, iCen, iCharge );
			book->cd( plc+"_yield");
			double sC = schema->var( "yield_"+plc )->val;
			double sE = schema->var( "yield_"+plc )->error;
			
			book->get( name )->SetBinContent( iiPt, sC );
			book->get( name )->SetBinError( iiPt, sE );

			// block for sep yields
			if ( schema->exists( "zd_yield_" + plc ) && schema->exists( "zb_yield_" + plc ) ){
				string name = Common::yieldName( plc, iCen, iCharge );
				book->cd( plc+"_yield");
				double zdC = schema->var( "zd_yield_"+plc )->val / book->get( name )->GetBinWidth( iiPt );
				double zdE = schema->var( "zd_yield_"+plc )->error / book->get( name )->GetBinWidth( iiPt );
				book->setBin( name, iiPt, zdC, zdE );

				double zbC = schema->var( "zb_yield_"+plc )->val / book->get( name )->GetBinWidth( iiPt );
				double zbE = schema->var( "zb_yield_"+plc )->error / book->get( name )->GetBinWidth( iiPt );

				book->cd( "tofEff" );
				book->setBin( Common::effName( plc, iCen, iCharge ), iiPt, 
					zbC / zdC, ((zbE / zbC) + ( zdE / zdC )) * ( zbC / zdC )  );

			}

			INFO( tag, "Filling Mus for " << plc );
			//Mu
			// zb
			name = Common::muName( plc, iCen, iCharge );
			book->cd( plc+"_zbMu");
			sC = schema->var( "zb_mu_"+plc )->val;
			sE = schema->var( "zb_mu_"+plc )->error;
			
			book->get( name )->SetBinContent( iiPt, sC );
			book->get( name )->SetBinError( iiPt, sE );

			name = "delta"+Common::muName( plc, iCen, iCharge );
			book->cd( plc+"_zbMu");
			sC = schema->var( "zb_mu_"+plc )->val - zbMu;
			sE = schema->var( "zb_mu_"+plc )->error;
			
			book->get( name )->SetBinContent( iiPt, sC );
			book->get( name )->SetBinError( iiPt, sE );

			// zd
			name = Common::muName( plc, iCen, iCharge );
			book->cd( plc+"_zdMu");
			sC = schema->var( "zd_mu_"+plc )->val;
			sE = schema->var( "zd_mu_"+plc )->error;
			
			book->get( name )->SetBinContent( iiPt, sC );
			book->get( name )->SetBinError( iiPt, sE );

			name = "delta"+Common::muName( plc, iCen, iCharge );
			book->cd( plc+"_zdMu");
			sC = schema->var( "zd_mu_"+plc )->val - zdMu;
			sE = schema->var( "zd_mu_"+plc )->error;
			
			book->get( name )->SetBinContent( iiPt, sC );
			book->get( name )->SetBinError( iiPt, sE );

			INFO( tag, "Filling Sigmas for " << plc );
			//Sigma
			name = Common::sigmaName( plc, iCen, iCharge );;
			book->cd( plc+"_zbSigma");
			sC = schema->var( "zb_sigma_"+plc )->val;
			sE = schema->var( "zb_sigma_"+plc )->error;
			
			book->get( name )->SetBinContent( iiPt, sC );
			book->get( name )->SetBinError( iiPt, sE );


			name = Common::sigmaName( plc, iCen, iCharge ) + "_rel";
			sC = schema->var( "zb_sigma_"+plc )->val / schema->var( "zb_mu_"+plc )->val;
			sE = schema->var( "zb_sigma_"+plc )->error / schema->var( "zb_mu_"+plc )->val;
			
			book->get( name )->SetBinContent( iiPt, sC );
			book->get( name )->SetBinError( iiPt, sE );



			name = Common::sigmaName( plc, iCen, iCharge );
			book->cd( plc+"_zdSigma");
			sC = schema->var( "zd_sigma_"+plc )->val;
			sE = schema->var( "zd_sigma_"+plc )->error;
			
			book->get( name )->SetBinContent( iiPt, sC );
			book->get( name )->SetBinError( iiPt, sE );

			if ( schema->exists( "eff_" + plc ) ){
				book->cd( "tofEff" );
				book->setBin( Common::effName( plc, iCen, iCharge ), iiPt, 
						schema->var( "eff_" + plc )->val, schema->var( "eff_" + plc )->error );
			}

			for ( string plc2 : Common::species ){
				fillEnhancedYieldHistogram( plc, iiPt, iCen, iCharge, plc2, fitter );
			}
			

		}
	} // fillFitHistograms(...)

	void FitRunner::fillEnhancedYieldHistogram( string plc1, int iPt, int iCen, int iCharge, string plc2, Fitter &fitter ){
		
		for ( string pre : { "zb_", "zd_" } ){
			book->cd( pre + "enhanced");
			
			string vName = pre + plc1 + "_yield_" + plc2;
			string hName = Common::yieldName( plc1, iCen, iCharge, plc2 );

			double sC = schema->var( vName )->val;
			double sE = schema->var( vName )->error;
			book->setBin( hName, iPt, sC, sE );
		}
	} // fillEnhancedYieldHistogram(...)

	void FitRunner::fillSystematicHistogram( string type, string plc, int iPt, int iCen, int iCharge, double sys ){
		
		double yNominal = schema->var( "yield_" + plc )->val;

		book->cd( "sys_" + type );
		string hName = Common::yieldName( plc, iCen, iCharge );
		book->setBin( hName, iPt, sys / yNominal, 0 );	
	} // fillEnhancedYieldHistogram(...)

	void FitRunner::drawFitRatio( string ds, Fitter * fitter, int iPt ){

		book->cd();
		// clone and renaim to avoid naming clashes
		TH1 * h = (TH1*)fitter->getDataHist( ds )->Clone( (ds + "_vs_" + ts(iPt) ).c_str() );
		TH1 * h2 = (TH1*)fitter->getDataHist( ds )->Clone( (ds + "_vs2_" + ts(iPt) ).c_str() );
		h->SetTitle( (ts(iPt) + "; z;(fit - data) / K_{yield}").c_str() );
		h->Reset();

		h2->SetTitle( " ; z;(fit - data)^{2} / K_{yield}" );
		h2->Reset();

		shared_ptr<FitSchema> schema = fitter->getSchema();
		for ( auto dp : schema->datasets[ ds ]){
			

			double yTh = fitter->modelEval( ds, dp.x );
			
			double ratio = yTh - dp.y;


			int bin = h->GetXaxis()->FindBin( dp.x );
			DEBUG( tag, "x, y = " << dp.x << ", " << dp.y )
			DEBUG( tag, "yTh = " << yTh )
			DEBUG( tag, "ratio = " << ratio )
			
			double scale = schema->var( "yield_K" )->val;
			double r = 0.02;
			h->GetYaxis()->SetRangeUser( -r, r );
			h->SetBinContent( bin, ratio / scale );
			h->SetBinError( bin, dp.ey);

			h2->SetBinContent( bin, ratio*ratio * 10 );
			h2->SetBinError( bin, dp.ey);

		}

		h->Draw();
		//h2->SetMarkerColor( kRed );
		//h2->SetLineColor( kRed );
		//h2->Draw("same");
	} // drawFitRatio(...)

	shared_ptr<FitSchema> FitRunner::prepareSystematic( string sys, string plc, double delta ){
		INFO( tag, "(sys=" << sys << ", plc=" << plc << ", delta=" << delta << ")" );

		shared_ptr <FitSchema> rSchema = shared_ptr<FitSchema>( new FitSchema( *schema ) );

		string var = sys + "_" + plc;
		// for instance zb_sigma
		// we want to fix sigma zb to given value and repeat the fit

		INFO( tag, var << " (was) = " << schema->var( var )->val );

		rSchema->var( var )->val += delta;

		INFO( tag, var << " (now) = " << rSchema->var( var )->val );

		return rSchema;
	} // prepareSystematic(...)

	void FitRunner::runSystematic( shared_ptr<FitSchema> tmpSchema, int iCharge, int iCen, int iPt ){
		WARN( tag, "(schema=" << tmpSchema << "iCharge=" << iCharge << ", iCen=" << iCen << ", iPt=" << iPt << ")" );

	
		double avgP = binAverageP( iPt );
		auto zbMu = psr->centeredTofMap( centerSpecies, avgP );
		auto zdMu = psr->centeredDedxMap( centerSpecies, avgP );

		schema->clearRanges();

		// create the fitter
		Fitter fitter( tmpSchema, inFile );

		// loads the default values used for data projection
		fitter.registerDefaults( &config, nodePath );
		
		// load the datasets from the file
		fitter.loadDatasets(centerSpecies, iCharge, iCen, iPt, true, zbMu, zdMu );

		// build the minuit interface
		fitter.setupFit();
		// assign active players to this fit
		fitter.addPlayers( activePlayers );

		// reload the datasets from the file
		// now that we have better idea of mu, sigma ( for enhancement cuts )
		if ( avgP < 1.0 )
			fitter.loadDatasets(centerSpecies, iCharge, iCen, iPt, true, zbMu, zdMu );

		int tries = 0;
		while( fitter.isFitGood() == false && tries < 3 ){
			fitter.fit3( );
			tries ++;
		}

	} // runSystematic(...)
}


