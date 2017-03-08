#ifndef YIELD_EXPORTER_H
#define YIELD_EXPORTER_H

#include "HistoAnalyzer.h"
#include "Common.h"

#include "ANSIColors.h"

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
		plc = config.getXString( nodePath + ".input:plc" );
		energy = config.getXString( nodePath + ".input:energy", "14p5" );

		loadSystematics( nodePath + ".systematics" );
	}



	virtual void make(){
		INFO( classname(), "");

		string base = config.getXString( nodePath + ".output:path" );
		vector<string> centralityBins = config.getStringVector( nodePath + ".CentralityBins" );
		for ( string cen : centralityBins ){
			for ( string charge : Common::sCharges ){

				string hName = plc + "_yield/" + Common::yieldName( plc, cen, charge );
				TH1D* hSpectra =  (TH1D*)inFile->Get( hName.c_str() );
				string name = base + "spectra_" + energy + "_" + plc + "_" + charge + "_" + cen + ".dat";
				string sysName = plc + "_" + charge + "_" + cen;
				

				if ( nullptr == hSpectra ){
					ERROR( classname(), "Histogram invalid : " << hName );
					continue;
				}

				if ( config.getBool( nodePath + ".make:dat", false ) ){
					INFO( classname(), "Exporting " << hName << " --> " << name );
					ofstream fout( name.c_str() );
						exportSpectraHist( hSpectra, fout, sysName );
					fout.close();
				}
				

				if ( config.getBool( nodePath + ".make:root", true ) ){
					book->cd();
					// now export to ROOT file
					string hSpectraName = "Yield_" + energy + "_" + plc + "_" + charge + "_" + cen + "_stat_normed";
					book->add( hSpectraName, makeSpectraHistogram( hSpectraName, hSpectra, "" ) );

					hSpectraName = "Yield_" + energy + "_" + plc + "_" + charge + "_" + cen + "_sys_normed";
					book->add( hSpectraName, makeSpectraHistogram( hSpectraName, hSpectra, sysName ) );

					hSpectraName = "Yield_" + energy + "_" + plc + "_" + charge + "_" + cen + "_rels_normed";
					book->add( hSpectraName, makeRelSysHistogram( hSpectraName, hSpectra, sysName ) );
				}


			}
		}
	}// make

	TH1* makeSpectraHistogram( string _name, TH1* _h, string _error="stat" ){
		INFO( classname(), "Cloning " << _h->GetName() << " as " << _name << " with " << _error << " uncertainties" );
		if ( "" == _error ){
			TH1 * hstat = HistoBook::cloneRange( _h, _name, config.getFloat( nodePath + ".min_pT", 0.5 ), 100 );
			hstat->SetDirectory( gDirectory );
			return hstat;
		} 

		
		TH1 * hsys = HistoBook::cloneRange( _h, _name, config.getFloat( nodePath + ".min_pT", 0.5 ), 100 );
		hsys->SetDirectory( gDirectory );

		for ( int i = 1; i <= hsys->GetNbinsX(); i++ ){
			double pT = hsys->GetBinCenter( i );
			double sys_unc = 0.0;

			if ( sysMap.count( _error ) > 0 && sysMap[ _error ].count( pT ) > 0 ){
				hsys->SetBinError( i, sysMap[ _error ][ pT ] );
			}
		}


		// TODO: set the errors to systematics 
		return  hsys;
	}

	TH1* makeRelSysHistogram( string _name, TH1* _h, string _error="stat" ){
		// INFO( classname(), "Cloning " << _h->GetName() << " as " << _name << " with " << _error << " uncertainties" );

		
		TH1 * hsys = HistoBook::cloneRange( _h, _name+"_temp", config.getFloat( nodePath + ".min_pT", 0.5 ), 100 );
		hsys->SetDirectory( gDirectory );

		for ( int i = 1; i <= hsys->GetNbinsX(); i++ ){
			double pT = hsys->GetBinCenter( i );
			double sys_unc = 0.0;

			if ( sysMap.count( _error ) > 0 && sysMap[ _error ].count( pT ) > 0 ){
				hsys->SetBinError( i, sysMap[ _error ][ pT ] );
			}
		}


		// TODO: set the errors to systematics 
		return  HistoBook::relativeErrors( hsys, _name );
	}


	void loadSystematics( string _path ){

		vector<string> centralityBins = config.getStringVector( nodePath + ".CentralityBins" );

		for ( string plc : Common::species ){
			for ( string chg : Common::sCharges ){
				for ( string cen : centralityBins ){
					string name = plc + "_" + chg + "_" + cen;
					if ( config.exists( _path + "." + name ) ){
						INFO( classname(), ANSIColors::color("Systematics uncertainties : ", "green" ) << name );
						sysMap[ name ] = config.getFloatMap( _path + "." + name );
						INFO( classname(), "[0.55] = +/-" << sysMap[name][0.55] );
					} else {
						ERROR( classname(), "Cannot find Systematic Uncertainties for " << name );
					}

				}
			}
		}
	}

	void exportSpectraHist( TH1D * hSpectra, ofstream &fout, string sysName ){
		DEBUG( classname(), "( h=" << hSpectra->GetName() << ", file=" << ")" );

		// file header
		fout << std::left << std::setw(20) << "pT Bin" << std::setw(20) << "value" << std::setw(20) << "stat_unc" << std::setw(20) << "sys_unc" << endl;

		
		for ( int i = 1; i <= hSpectra->GetNbinsX(); i++ ){

			double pT = hSpectra->GetBinCenter( i );
			if ( pT < config.getFloat( nodePath + ".min_pT", 0.5 ) ) continue; //TODO: add config range for stuff
			double value = hSpectra->GetBinContent( i );
			double stat_unc = hSpectra->GetBinError( i );
			double sys_unc = 0.0;

			if ( sysMap.count( sysName ) > 0 && sysMap[ sysName ].count( pT ) > 0 ){
				sys_unc = sysMap[ sysName ][ pT ];
			}
			
			if ( value < 10e-20 || pT < 0.54 || sys_unc == 0) continue;

			fout << std::setprecision( 10 ) << std::left << std::setw(20) << pT << std::left << std::setw(20) << value << std::left << std::setw(20) << stat_unc << std::left << std::setw(20) << sys_unc << endl; 
		}

	} // exportSpectraHist


protected:
	map<string, map<float, float> > sysMap;

};


#endif