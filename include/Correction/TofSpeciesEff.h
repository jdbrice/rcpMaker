#ifndef TOF_SPECIES_EFF_H
#define TOF_SPECIES_EFF_H

#include "HistoAnalyzer.h"
#include "RooPlotLib.h"
#include "FitConfidence.h"

class TofSpeciesEff : public HistoAnalyzer
{
public:
	virtual const char* classname() const { return "TofSpeciesEff"; }
	TofSpeciesEff() {}
	~TofSpeciesEff() {}

	virtual void initialize(){
		HistoAnalyzer::initialize();
	}

	virtual void make(){

		Reporter rp( config.getString( nodePath + ".output.PDF:url" ), config.getInt( nodePath + ".output.PDF:w", 1024 ), config.getInt( nodePath + ".output.PDF:h", 512 ) );
		RooPlotLib rpl;


		ofstream out( config.getString( nodePath+".output.File:url" ).c_str() );

		out << XmlConfig::declarationV1 << endl;
		out << "<config>" << endl;

		// gStyle->Set
		// gStyle->SetOptFit( 10 );

		book->cd();
		// 
		for ( string plc : Common::species ){
			if ( "K" == plc ) continue;
			// for ( int cg : Common::charges ){
			int cg = 1;
			for ( int cb : config.getIntVector( nodePath + ".CentralityBins" ) ){
				string name = plc+"_"+ts(cb)+"_"+Common::chargeString( cg );

				if ( !fileExists( "tpcfit_"+plc ) ){
					INFOC( "Skipping " << plc << ", file DNE" );
					continue;
				}

				INFOC( name );
				TH1 * hTpc = get<TH1>( plc+"_yield/yield_"+name, "tpcfit_"+plc );
				TH1 * hSim = get<TH1>( plc+"_yield/yield_"+name, "simfit_"+plc );

				DEBUGC( "hTpc = " << hTpc );
				DEBUGC( "hSim = " << hSim );

				XmlRange rng;
				string qName = "pT_" + plc + "_" + Common::chargeString(cg);
				rng.loadConfig( config, config.q( "XmlRange{name=="+qName+"}" ) );
				INFOC( "XmlRange : " << rng.toString() );

				
				book->addClone( "zd_" + name, hTpc );
				book->addClone( "sim_" + name, hSim );
				TH1 * hRatio = book->addClone( "ratio_" + name, HistoBook::cloneRange( hSim, "ratio_"+name, rng.min, rng.max ) );

				hRatio->Divide( hTpc );

				
				rpl.style( hRatio ).set( config, "style.plot" ).set( "xr", 0.0, 1.0 ).draw();
				TF1 * f = new TF1( "f", "pol0" );
				TFitResultPtr fitPointer = hRatio->Fit( f, "S", "" );
				// TGraphErrors * band = FitConfidence::choleskyBands( fitPointer, f, 200, 100 );
				TGraphAsymmErrors* band = FitConfidence::fitUncertaintyBand( f, config.getDouble( "SysUnc:high", 0.0 ), config.getDouble( "SysUnc:low", 0.0 ), 50, 0.0, 4.5 );
				band->SetFillColorAlpha( kBlue, 0.3 );
				band->Draw("same e3");

				rp.savePage();

				string pname = plc+"_"+Common::chargeString( cg )+"_"+ts(cb);
				out << "\t\t<TofSpeciesEff name=" << quote( pname );
				out << " value=" << quote( dts(f->GetParameter(0)) );
				out << " uncertainty=" << quote( dts( config.getDouble( "SysUnc:high", 0.0 ) ) );
				out << "/>" << endl;


			} // loop centrality
			// } // charge
		} // loop particle

		out << "</config>" << endl;
		out.close();


	}
protected:

	
};


#endif