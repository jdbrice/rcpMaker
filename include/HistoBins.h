#ifndef HISTOGRAM_BINS
#define HISTOGRAM_BINS

#include "XmlConfig.h"

namespace jdb{
	class HistoBins
	{
	public:

		/**
		 * Static makeNBins
		 * @param nBins int number of bins
		 * @param low double low end of bins
		 * @param high double high end of bins
		 * @return a vector containing the bin edges
		 */
		static vector<double> makeNBins( int nBins, double low, double high ){

			vector<double> bins;
			double step = (high - low ) / (double) nBins;
			for (double i = low; i <= high; i += step ){
				bins.push_back( i );
			}
			return bins;
		}	// binsFrom
		
		/**
		 * Static makeFixedWidthBins
		 * @param binWidth double width of each bin
		 * @param low double low end of bins
		 * @param high double high end of bins
		 * @return a vector containing the bin edges
		 */
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

			return -1;

		}	// findBin

		/**
		 * Finds the bin for a given value 
		 * @param  val Value to find bin for
		 * @return     0 start - bin index
		 */
		int findBin( double val ){
			return findBin( bins, val );
		} // findBin

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
		HistoBins( XmlConfig * config, string nodePath ){

			if ( config->nodeExists( nodePath ) && config->getDoubleVector( nodePath ).size() >= 2 ){
				bins = config->getDoubleVector( nodePath );
			} else if (
						config->nodeExists( nodePath + ":width" ) &&
						config->nodeExists( nodePath + ":min" ) &&
						config->nodeExists( nodePath + ":max" )
						) {
				
				// build the bins from the range and width
				bins = makeFixedWidthBins( 
					config->getDouble( nodePath + ":width" ), 
					config->getDouble( nodePath + ":min" ), 
					config->getDouble( nodePath + ":max" ) 
				);
			}

		}


		~HistoBins(){}
		
		vector<double> bins;

	};
}





#endif