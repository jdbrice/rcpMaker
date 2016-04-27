#ifndef YIELD_EXPORTER_H
#define YIELD_EXPORTER_H

#include "HistoAnalyzer.h"
#include "Common.h"

class YieldExporter : public HistoAnalyzer
{

protected:

	string plc, energy;
public:
	virtual const char* classname() const { return "YieldExporter"; }
	YieldExporter() {}
	~YieldExporter() {}


	virtual void overrideConfig(){
		DEBUG( classname(), "" );
	}

	virtual void initialize() {
		INFO( classname(), "" );
		plc = config.getString( nodePath + ".input:plc" );
		energy = config.getString( nodePath + ".input:energy", "14.5" );
	}



	virtual void make(){
		INFO( classname(), "");

		string base = config.getString( nodePath + ".output:path" );
		vector<string> centralityBins = config.getStringVector( nodePath + ".CentralityBins" );
		for ( string cen : centralityBins ){
			for ( string charge : Common::sCharges ){

				string hName = plc + "_yield/" + Common::yieldName( plc, cen, charge );
				TH1D* hSpectra =  (TH1D*)inFile->Get( hName.c_str() );
				string name = base + "spectra_" + energy + "_" + plc + "_" + charge + "_" + cen + ".dat";

				INFO( classname(), "Exporting " << hName << " --> " << name );

				if ( nullptr == hSpectra ){
					ERROR( classname(), "Histogram invalid : " << hName );
					continue;
				}

				ofstream fout( name.c_str() );
					exportSpectraHist( hSpectra, fout );
				fout.close();


			}
		}
	}// make

	void exportSpectraHist( TH1D * hSpectra, ofstream &fout ){
		DEBUG( classname(), "( h=" << hSpectra->GetName() << ", file=" << ")" );

		// file header
		fout << std::left << std::setw(20) << "pT Bin" << std::setw(20) << "value" << std::setw(20) << "stat_unc" << std::setw(20) << "sys_unc" << endl;

		
		for ( int i = 1; i <= hSpectra->GetNbinsX(); i++ ){

			double pT = hSpectra->GetBinCenter( i );
			// if ( pT < min_pT ) continue; TODO: add config range for stuff
			double value = hSpectra->GetBinContent( i );
			double stat_unc = hSpectra->GetBinError( i );
			double sys_unc = 0.0;

			fout << std::setprecision( 10 ) << std::left << std::setw(20) << pT << std::left << std::setw(20) << value << std::left << std::setw(20) << stat_unc << std::left << std::setw(20) << sys_unc << endl; 
		}

	} // exportSpectraHist

};


#endif