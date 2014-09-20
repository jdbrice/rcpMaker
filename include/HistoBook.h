
#ifndef HISTOBOOK_H
#define HISTOBOOK_H

#include "allroot.h"
#include <map>
#include <string>
#include <sstream>
#include <stdarg.h>
#include "XmlConfig.h"
#include "Logger.h"

using namespace std;

namespace jdb{

	/**
	 * For legend Alignment
	 * may remove!!!
	 */
	class legendAlignment {
	public:
		static const int topLeft = 1;
		static const int topRight = 2;
		static const int topCenter = 3;
		static const int bottomLeft = 4;
		static const int bottomRight = 5;
		static const int bottomCenter = 6;

		static const int bottom = 0;
		static const int center = 1; // for x and y dirs
		static const int right = 2;
		static const int left = 3;
		static const int top = 4;

		// TODO: make a smart legend that trys to avoid data
		static const int best = 5;

		

	};





	/**
	 * HistoBook Class
	 */
	class HistoBook {

	protected:

		Logger * log;

		string currentDir;

		std::map<string, string> configPath;
		std::map<string, TH1*> book;
		
		string filename;
		
		TFile *file;

		/* Style and display */
		string styling;
		string drawOption;
		TLegend * legend;

		// optional config to use for all config related calls
		XmlConfig * config;



	public:

		/**
		 * Static Usage
		 */
		static vector<double> binsFrom( int nBins, double low, double high );
		static int findBin( vector<double> &bins, double value );
		HistoBook( string name, string input = "", string inDir = "", Logger * nLog = NULL );
		HistoBook( string name, XmlConfig* config, string input = "", string inDir = "", Logger * nLog = NULL );
		~HistoBook();

		
		string cd( string dir = "/" );
		void add( string name, TH1 * );
		TH1* get( string name, string sdir = "" );
		TH2* get2D( string name, string sdir = "" );
		TH3* get3D( string name, string sdir = "" );
		void fill( string name, double bin, double weight = 1);
		void make1F( string name, string title, uint nBins, double low, double hi );
		void make1D( string name, string title, uint nBins, double low, double hi );
		void make1D( string name, string title, uint nBins, const Double_t* bins );
		void make2D( 	string name, string title, 
						uint nBinsX, double lowX, double hiX, uint nBinsY, double lowY, double hiY );
		void make2D( 	string name, string title, 
						uint nBinsX, const Double_t* xBins, uint nBinsY, double lowY, double hiY );
		void make2D( 	string name, string title, 
						uint nBinsX, double x1, double x2, uint nBinsY, const Double_t* yBins );
		void make2D( 	string name, string title, 
						uint nBinsX, const Double_t* xBins, uint nBinsY, const Double_t*yBins );
		void make( XmlConfig * config, string nodeName );
		void make( string nodeName );
		void makeAll( string nodeName );
		void makeAll( XmlConfig * config, string nodeName );
		void clone( string existing, string create );

		TLegend* getLegend() { return legend; }

		HistoBook* draw(string name = "", Option_t* opt= "" );
		

		HistoBook* clearLegend() { legend->Clear(); return this; };
		
		// add a legend by setting the legendName
		

		TDirectory* getDirectory( ) { return gDirectory; }

		void save();

		HistoBook* style( string hName );

		//set( string param, ... )
		// for function chaining and rapid styling
		HistoBook* set( string param, string p1, string p2 = "", string p3 = "", string p4 = "" );
		HistoBook* set( string param, double p1, double p2 = -1, double p3 = -1, double p4 = -1 );
		HistoBook* set( XmlConfig* config, string nodePath );
		HistoBook* set( string nodePath );
		HistoBook* set( string opt, vector<string> nodePath );

		HistoBook* exportAs( string filename = "" );

		bool exists( string name, string sdir = "" ){
			if ( NULL != get( name, sdir ) )
				return true;
			return false;
		}

		int color( string color ) {
			if ( "red" == color )
				return kRed;
			if ( "green" == color )
				return kGreen;
			if ( "blue" == color )
				return kBlue;
			if ( "black" == color )
				return kBlack;
			return -1;
		}

	private:
		void globalStyle();
		HistoBook* placeLegend( int alignmentX, int alignmentY, double width = -1, double height = -1 );
		void loadRootDir( TDirectory*, string path = "" );

		string sParam( vector<string> params, int p, string def="" ) {
			if ( p < params.size() )
				return params[ p ];
			else 
				return def;
		}
		char* cParam( vector<string> params, int p, string def="" ) {
			if ( p < params.size() )
				return (char*)(params[ p ].c_str());
			else 
				return (char*)(def.c_str());
		}
		double dParam( vector<string> params, int p, double def=0 ) {
			if ( p < params.size() )
				return atof( params[ p ].c_str() );
			else 
				return def;
		}

		



	};	
}






#endif