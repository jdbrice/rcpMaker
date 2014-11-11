#ifndef HISTOGRAM_BINS
#define HISTOGRAM_BINS

#include "XmlConfig.h"
#include "Utils.h"

/*jdoc{
"name" : "",
"params" : [ ],
"paramDesc" : [ ],
"returns" : [  ],
"desc" : ""
}*/

namespace jdb{
	class HistoBins
	{
	public:
		
		/*jdoc{
		"name" : "static vector<double> makeNBins( int nBins, double low, double high )",
		"params" : ["int nBins", "double low", "double high" ],
		"paramDesc" : [	"Number of Bins", 
			"Lower edge of first bin", 
			"Upper edge of last bin" ],
		"returns" : [ "vector of bin edges" ],
		"desc" : "Divides the range high - low into a fixed number of bins from low to high"
		}*/
		static vector<double> makeNBins( int nBins, double low, double high ){

			vector<double> bins;
			double step = (high - low ) / (double) nBins;
			for (double i = low; i <= high; i += step ){
				bins.push_back( i );
			}
			return bins;
		}	// binsFrom
		
		/*jdoc{
		"name" : "static vector<double> makeFixedWidthBins( double binWidth, double low, double high )",
		"params" : ["double binWidth", "double low", "double high" ],
		"paramDesc" : [	"Nominal width of each bin", 
			"Lower edge of first bin", 
			"Upper edge of last bin" ],
		"returns" : [ "vector of bin edges" ],
		"desc" : "Divides the range high - low into a fixed number of bins from low to high"
		}*/
		static vector<double> makeFixedWidthBins( double binWidth, double low, double high ){

			vector<double> bins;
			for (double i = low; i <= high; i += binWidth ){
				bins.push_back( i );
			}
			return bins;
		}	// binsFrom

		/**
		 * Static findBin
		 * Used to fnd the bin corresponding to a value in user space
		 * @param  bins vector of in edes from low to high
		 * @param  val  the value whose bin is desired
		 * @return      bin index starting at zero, -1 for underflow and -2 for overflow
		 */
		static int findBin( vector<double> &bins, double val ){

			if ( bins.size() < 2 )
				return -1;

			int n = bins.size();

			// overflow and underflow
			if ( val < bins[ 0 ] )
				return -1;
			else if ( val > bins[ n - 1 ] )
				return -2;

			for ( int i = n-2; i >= 0; i-- ){
				if ( val >= bins[ i ] )
					return i;
			}

			return -3;

		}	// findBin

		/**
		 * Finds the bin for a given value 
		 * @param  val Value to find bin for
		 * @return     0 start - bin index
		 */
		int findBin( double val ){
			return findBin( bins, val );
		} // findBin

		int length() {
			return bins.size();
		}
		int size() {
			return bins.size();
		}
		int nBins(){
			return bins.size() - 1;
		}

		/**
		 * Constructor for fixed width bins
		 */
		HistoBins( double min, double max, double width ){
			this->bins = makeFixedWidthBins( width, min, max );
		}

		/**
		 * Constructor for variable bins
		 */
		HistoBins( vector<double> bins ){
			this->bins = bins;
		}

		/**
		 * Constructor from config
		 */
		HistoBins( XmlConfig * config, string nodePath, string mod = "" ){

			if ( config->nodeExists( nodePath ) && config->getDoubleVector( nodePath ).size() >= 2 ){
				bins = config->getDoubleVector( nodePath );
				min = bins[ 0 ];
				max = bins[ nBins() ];
				width = -1;
				return;
			}  

			string wTag 	= ":width" + mod;
			string minTag 	= ":min" + mod;
			string maxTag 	= ":max" + mod;
			

			if ( config->nodeExists( nodePath + wTag ) && config->nodeExists( nodePath + minTag ) && config->nodeExists( nodePath + maxTag ) ) {

				min = config->getDouble( nodePath + minTag );
				max = config->getDouble( nodePath + maxTag );
				width = config->getDouble( nodePath + wTag );

				bins = makeFixedWidthBins( width, min, max );
				return;
			}
		
			string nTag 	= ":nBins" + mod;
			if ( config->nodeExists( nodePath + nTag ) && config->nodeExists( nodePath + minTag ) && config->nodeExists( nodePath + maxTag ) ) {

				min = config->getDouble( nodePath + minTag );
				max = config->getDouble( nodePath + maxTag );
				double n = config->getInt( nodePath + nTag );
				width = (max - min ) / (double)n;

				bins = makeNBins( n, min, max );
				return;
			}

		}

		double& operator[] ( const int nIndex ){
			return bins[ nIndex ];
		}

		string toString() {
			if ( width > 0 ) 
				return "< " + ts( nBins() ) + " bins ( " + ts(min) + " -> " + ts(max) + " )  >";
			else {
				string ba = "< " + ts( nBins() ) + " bins { ";

				for ( int i = 0; i < bins.size(); i++  ){
					if ( i+1  < bins.size() )
						ba += ("( " + ts( bins[i] ) +" -> " + ts( bins[i+1] ) + " )" );
					if ( i+3  < bins.size() )
						ba += ", ";
				}
				ba += " } >";
				return ba;
			}

			return "";
		}


		~HistoBins(){}
		
		vector<double> bins;
		double width;
		double min, max;

	};
}





#endif