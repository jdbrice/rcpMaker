#include "SGFRunner.h"
#include "PidPhaseSpace.h"
#include "SGF.h"

SGFRunner::SGFRunner( XmlConfig * _cfg, string _np) 
: HistoAnalyzer( _cfg, _np ){
	
	// Initialize the Phase Space Recentering Object
	tofSigmaIdeal = cfg->getDouble( nodePath+"PhaseSpaceRecentering.sigma:tof", 0.0012);
	dedxSigmaIdeal = cfg->getDouble( nodePath+"PhaseSpaceRecentering.sigma:dedx", 0.06);
	psr = new PhaseSpaceRecentering( dedxSigmaIdeal,
									 tofSigmaIdeal,
									 cfg->getString( nodePath+"Bichsel.table", "dedxBichsel.root"),
									 cfg->getInt( nodePath+"Bichsel.method", 0) );
	psrMethod = cfg->getString( nodePath+"PhaseSpaceRecentering.method", "traditional" );
	// alias the centered species for ease of use
	centerSpecies = cfg->getString( nodePath+"PhaseSpaceRecentering.centerSpecies", "K" );

	/**
	 * Make the momentum transverse, eta, charge binning
	 */
	binsPt = new HistoBins( cfg, "bin.pt" );
	binsEta = new HistoBins( cfg, "bin.eta" );
	binsCharge = new HistoBins( cfg, "bin.charge" );

	/**
	 * Setup the PID Params
	 */
	for ( string plc : PidPhaseSpace::species ){

		logger->info(__FUNCTION__) << "Preparing PID params for : " << plc << endl;

		tofParams[ plc ] = unique_ptr<TofPidParams>(new TofPidParams( cfg, nodePath + "TofPidParams." + plc + "." ));
		dedxParams[ plc ] = unique_ptr<DedxPidParams>( new DedxPidParams( cfg, nodePath + "DedxPidParams." + plc + "." ));

	}

	/**
	 * Create the schema
	 */
	schema = shared_ptr<SGFSchema>(new SGFSchema( cfg, nodePath + "SGFSchema" ));

}

SGFRunner::~SGFRunner(){


}

void SGFRunner::make(){

	book->cd();
	book->makeAll( nodePath + "histograms" );
	for ( string plc : PidPhaseSpace::species ){
		book->clone( "yield", "yield_"+plc );
	}

	SGF sgf( schema, inFile );

	int firstPtBin = cfg->getInt( nodePath + "FitPtBins:min", 0 );
	int lastPtBin = cfg->getInt( nodePath + "FitPtBins:max", 1 );
	if ( lastPtBin > binsPt->nBins() )
		lastPtBin = binsPt->nBins();

	for ( int iPt = firstPtBin; iPt <= lastPtBin; iPt++ ){

		double avgP = ((*binsPt)[iPt] + (*binsPt)[iPt+1] ) / 2.0;
		// set initals
		int iPlc = 0;
		vector<double> dedxMeans = psr->centeredDedxMeans( centerSpecies, avgP );
		logger->info(__FUNCTION__) << "pt Bin : " << iPt << endl;
		for ( string plc : PidPhaseSpace::species ){

			double m = psr->mass( plc );
			double mr = psr->mass( centerSpecies );
			double zbMean = tofParams[ plc ]->mean( avgP, m, mr );
			double zbSigma = tofParams[ plc ]->sigma( avgP, m, mr );

			double zdMean = dedxMeans[ iPlc ];
			double zdSigma = dedxParams[ plc ]->sigma( avgP );

			// update the schema
			logger->trace(__FUNCTION__) << plc << " : zb mu=" << zbMean << ", sig=" << zbSigma << endl;
			logger->trace(__FUNCTION__) << plc << " : zd mu=" << zdMean << ", sig=" << zdSigma << endl; 
			schema->setInitial( "zb", plc, zbMean, zbSigma );
			schema->setInitial( "zd", plc, zdMean, zdSigma );

			iPlc++;
		}

		sgf.fit( centerSpecies, 0, 1, iPt, 0 );
		sgf.report( reporter );

		for ( string plc : PidPhaseSpace::species ){
			book->cd();
			book->fill( "yield_"+plc, avgP, schema->var( "yield_"+plc )->getVal() );
		}

	}


}





