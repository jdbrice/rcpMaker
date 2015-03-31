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

	RooMsgService::instance().setGlobalKillBelow(ERROR);

	// temp inputs tht I haven't automated yet
	int etaBin = 0;
	int cenBin = 0;
	int charge = 0;

	//
	double zbDeltaMu = cfg->getDouble( nodePath + "ParameterFixing.deltaMu:zb", 1.5 );
	double zdDeltaMu = cfg->getDouble( nodePath + "ParameterFixing.deltaMu:zd", 1.5 );

	double zbDeltaSigma = cfg->getDouble( nodePath + "ParameterFixing.deltaSigma:zb", 0.25 );
	double zdDeltaSigma = cfg->getDouble( nodePath + "ParameterFixing.deltaSigma:zd", 0.25 );

	book->cd();
	book->makeAll( nodePath + "histograms" );
	for ( int iCen = 0; iCen < 2; iCen++ ){
		for ( int iCharge = -1; iCharge < 2; iCharge++ ){
			for ( int iEta = 0; iEta < binsEta->nBins(); iEta++ ){
				for ( string plc : PidPhaseSpace::species ){
					string name = "yield_" + plc + "_" + ts(iCen) + "_" + PidPhaseSpace::chargeString( iCharge ) + "_" + ts(iEta);
					book->clone( "yield", name );
				}
			}
		}
	}

	SGF sgf( schema, inFile );

	int firstPtBin = cfg->getInt( nodePath + "FitPtBins:min", 0 );
	int lastPtBin = cfg->getInt( nodePath + "FitPtBins:max", 1 );
	if ( lastPtBin >= binsPt->nBins() )
		lastPtBin = binsPt->nBins() - 1;

	//for ( int iCen = 0; iCen < 2; iCen++ ){
	//	for ( int iCharge = -1; iCharge < 2; iCharge++ ){
	//		for ( int iEta = 0; iEta < binsEta->nBins(); iEta++ ){
	{
		{
			{
				int iCen = 1;
				int iCharge = 0;
				int iEta = 0;
				// reset yields here but need to reset all yields
				// the zb and zd yields also
				// TODO
				for ( int iPt = firstPtBin; iPt <= lastPtBin; iPt++ ){

					cout << "PtBin : " << iPt << endl;

					double avgP = averageP( iPt, etaBin );
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
						schema->setInitial( "zb", plc, zbMean, zbSigma, zbDeltaMu, zbDeltaSigma );
						schema->setInitial( "zd", plc, zdMean, zdSigma, zdDeltaMu, zdDeltaSigma );

						// check if the sigmas should be fixed
						double zbMinParP = cfg->getDouble( nodePath + "ParameterFixing." + plc + ":zbSigma", 0 );
						double zdMinParP = cfg->getDouble( nodePath + "ParameterFixing." + plc + ":zdSigma", 0 );

						if ( zbMinParP > 0 && avgP >= zbMinParP)
							schema->fixSigma( "zb", plc, zbSigma );
						if ( zdMinParP > 0 && avgP >= zdMinParP )
							schema->fixSigma( "zd", plc, zdSigma );

						iPlc++;
					}

					sgf.fit( centerSpecies, charge, cenBin, iPt, etaBin );
					sgf.report( reporter );

					for ( string plc : PidPhaseSpace::species ){
						string name = "yield_" + plc + "_" + ts(iCen) + "_" + PidPhaseSpace::chargeString( iCharge ) + "_" + ts(iEta);
						book->cd();
						double sC = schema->var( "yield_"+plc )->getVal() / book->get( name )->GetBinWidth( iPt + 1 );

						double sE = schema->var( "yield_"+plc )->getError() / book->get( name )->GetBinWidth( iPt + 1 );
						cout << "averageP = " << avgP << endl;
						cout << "Bin Width : " << book->get( name )->GetBinWidth( iPt + 1 ) << endl;
						book->fill( name, avgP, sC );
						book->get(  name )->SetBinError( iPt, sE );
					}

				}// loop pt Bins
			}
		}
	}


}





