#ifndef TOF_EFF_FITTER
#define TOF_EFF_FITTER

// RooBarb
#include "HistoAnalyzer.h"
#include "RooPlotLib.h"

//ROOT
#include "TGraphAsymmErrors.h"

// STL
#include <fstream>
using namespace std;


class TofEffFitter : HistoAnalyzer {

protected:


public:
	TofEffFitter( XmlConfig * cfg, string np ) : HistoAnalyzer( cfg, np )  {
		DEBUG("")
	}
	~TofEffFitter(){}

	/* Efficiency plot for a given centrality and charge
	 * @cen 	mapped centrality bin ( 0, 5)
	 * @charge 	"p" for plus and "n" minus
	 */
	void effFor( int cen, string charge ){
		TGraphAsymmErrors * eff = new TGraphAsymmErrors();
		RooPlotLib plt;

		string ntof = "pt_tof_"+ ts(cen) +"_" + charge;
		string ntpc = "pt_"+ ts(cen) +"_" + charge;
		TH1 * pass = (TH1*)inFile->Get(ntof.c_str());
		TH1 * total = (TH1*)inFile->Get(ntpc.c_str());
		eff->Divide( pass, total );

		TH1D * frame = new TH1D( "frame", "frame", 1, 0.0, 5 );

		plt.style( frame ).set( "yr", 0.0, 1.4 ).draw();
		
		plt.style( eff ).set( "yr", 0.0, 1.4 ).set( "t", "cen_" +ts(cen) + "_" + charge ).set("draw", "").draw();
		
		vector<double> fY( &eff->GetY()[0], &eff->GetY()[0] + eff->GetN() );
		int bin = 1;
		for ( double y : fY ){

			double xlow = pass->GetBinLowEdge( bin );
			double xhi = xlow + pass->GetBinWidth( bin );
			INFO( "[" << xlow << ", " << xhi << "] = " << y )
			bin++;
		}

		exportParams( cen, charge, fY  );

		reporter->savePage();
		delete eff;
	}


	void exportParams(int cen, string charge, vector<double> &vals ){

	logger->info(__FUNCTION__) << endl;
	logger->info(__FUNCTION__) << "Writing to " << cfg->getString(nodePath + "output.param") << endl;

	string path = cfg->getString( nodePath + "output:path" ) + cfg->getString( nodePath + "output.param" );
	ofstream out( path.c_str(), ios::out | ios::app );

	out << "<TofEffParams";
	out << " cen=\"" << cen << "\"";
	out << " charge=\"" << charge << "\"";
	out << ">";

	for ( double y : vals ){
		if ( y != vals[ vals.size()-1 ] )
			out << y << ", ";
		else 
			out << y;
	}

	out << "</TofEffParams>" << endl;

	out.close();


}

	virtual void make(){

		for ( int cen : { 0, 1, 2, 3, 4, 5 } ){
			for ( string charge : { "p", "n" } ){
				effFor( cen, charge );
			}
		}
		
	}






protected:


};

#endif