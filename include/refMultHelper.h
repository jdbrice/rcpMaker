#ifndef REF_MULT_HELPER_H
#define REF_MULT_HELPER_H

#include "XmlConfig.h"
#include "ChainLoader.h"
#include "Utils.h"
#include "Logger.h"
#include "LoggerConfig.h"
#include "ConfigPoint.h"
#include "ConfigRange.h"
using namespace jdb;

#include "TofPicoDst.h"

class RefMultHelper
{
protected:

	XmlConfig * cfg;
	string nodePath;

	Logger * lg;
	HistoBook * book;

	TofPicoDst * pico;

	ConfigRange * cutVertexZ;
	ConfigRange * cutVertexR;
	ConfigPoint * cutVertexROffset;

public:
	RefMultHelper(XmlConfig * config, string np ){

		//Set the Root Output Level
		gErrorIgnoreLevel=kSysError;

		// Save Class Members
		cfg = config;
		nodePath = np;

		// make the Logger
		lg = LoggerConfig::makeLogger( cfg, np + "Logger" );

		lg->info( __FUNCTION__) << "Making Chain and PicoDataStore Interface " << endl;
		//Create the chain
		TChain * chain = new TChain( cfg->getString(np+"input.dst:treeName", "tof" ).c_str() );
	    ChainLoader::load( chain, cfg->getString( np+"input.dst:url" ).c_str(), cfg->getInt( np+"input.dst:maxFiles" ) );

	    pico = new TofPicoDst( chain );

	    // create the book
	    lg->info(__FUNCTION__) << " Creating book "<< endl;
	    book = new HistoBook( config->getString( np + "output.data" ), config );

	    cutVertexZ = new ConfigRange( cfg, np + "eventCuts.vertexZ", -200, 200 );
	    cutVertexR = new ConfigRange( cfg, np + "eventCuts.vertexR", 0, 10 );
	    cutVertexROffset = new ConfigPoint( cfg, np + "eventCuts.vertexROffset", 0.0, 0.0 );


	}
	~RefMultHelper(){

		delete book;
		delete cutVertexZ;
		delete cutVertexR;
		delete cutVertexROffset;

		lg->info(__FUNCTION__) << endl;
		delete lg;
	}

	void eventLoop() {
		lg->info(__FUNCTION__) << endl;

		Int_t nEvents = (Int_t)pico->getTree()->GetEntries();
		lg->info(__FUNCTION__) << "Reading " << nEvents << " events from chain" << endl;
		
		book->makeAll( nodePath + "histograms" );
		/**
		 *  Loop over all the events and build the refMult histogram
		 */
		TaskProgress tp( "Ref Mult Loop", nEvents );
		uint totalEventsKept = 0;
		for(Int_t i = 0; i < nEvents; i++) {
	    	pico->GetEntry(i);

	    	tp.showProgress( i );
	    	

	    	if ( keepEvent() )
	    		continue;

	    	totalEventsKept ++ ;

	    	Int_t refMult = pico->refMult;
	    	book->fill( "refMult", refMult );
	   
		} // end loop on events for refMult

		lg->info(__FUNCTION__) << "Total Events Kept: " << totalEventsKept << endl;
	
	}

	bool keepEvent() {

		double z = pico->eventVertexZ();
		double x = pico->eventVertexX() + cutVertexROffset->x;
		double y = pico->eventVertexY() + cutVertexROffset->y;
		double r = TMath::Sqrt( x*x + y*y );

		int nT0 = pico->eventNTZero();

		book->fill( "vertexZ", z);
		book->fill( "vertexR", r);
		
		if ( z < cutVertexZ->min || z > cutVertexZ->max )
			return false;

		book->fill( "vertexZ_zCut", z);
		book->fill( "vertexR_zCut", r);
		if ( r < cutVertexR->min || r > cutVertexR->max )
			return false;
		book->fill( "vertexZ_r_zCut", z);
		book->fill( "vertexR_r_zCut", r);

		return true;
	}


	/**
	 * Finds the cut for a given centrality value
	 * @param  centrality	the centrality given as a value from 0 to 1.0 as the percent spectators
	 * @param  histo 		the histogram containing all RefMult data
	 * @return				the absolute cut to use for selecting a given centrality
	 */
	double findCut( double centrality, TH1* histo ) {

		if ( histo ){

			int nbins = histo->GetNbinsX();
			Int_t total = histo->Integral();
			cout << "[RefMultHelper.findCut] Total RefMult Events: " << total << endl;

			double percent = (1 - centrality );
			Int_t threshold = percent  * total;
			cout << "[RefMultHelper.findCut] RefMult Threshold : " << threshold << " for " << (centrality * 100) << "%" << " centrality"  << endl;

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
	 * @param  value		the value returned by RefMult.
	 * @param  histo 		the histogram containing all RefMult data
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