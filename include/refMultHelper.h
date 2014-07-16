#ifndef refMultHelper_h
#define refMultHelper_h

class refMultHelper
{
public:
	refMultHelper(){

	}
	~refMultHelper(){

	}


	/**
	 * Finds the cut for a given centrality value
	 * @param  centrality	the centrality given as a value from 0 to 1.0 as the percent spectators
	 * @param  histo 		the histogram containing all refMult data
	 * @return				the absolute cut to use for selecting a given centrality
	 */
	double findCut( double centrality, TH1* histo ) {

		if ( histo ){

			int nbins = histo->GetNbinsX();
			Int_t total = histo->Integral();
			cout << "[refMultHelper.findCut] Total refMult Events: " << total << endl;

			double percent = (1 - centrality );
			Int_t threshold = percent  * total;
			cout << "[refMultHelper.findCut] RefMult Threshold : " << threshold << " for " << (centrality * 100) << "%" << " centrality"  << endl;

			Int_t runningTotal = 0;

			for ( int i = 1; i < nbins; i++ ){
				runningTotal += histo->GetBinContent( i );

				if ( runningTotal >= threshold ){
					return histo->GetBinLowEdge( i );
				}
			}
		}

	}

	/**
	 * Finds the cut for a given centrality value
	 * @param  value		the value returned by refMult.
	 * @param  histo 		the histogram containing all refMult data
	 * @return				the absolute cut to use for selecting a given centrality
	 */
	double findCentrality( double value, TH1* histo ) {

		if ( histo ){

			int nbins = histo->GetNbinsX();
			Int_t total = histo->Integral();

			Int_t runningTotal = 0;

			for ( int i = 1; i < nbins; i++ ){
				if ( histo->GetBinLowEdge( i ) < value )
					runningTotal += histo->GetBinContent( i );
				else 
					break;
			}
			return 1.0 - ((double)runningTotal / (double)total );
		}

		return -1;

	}

	
};

#endif