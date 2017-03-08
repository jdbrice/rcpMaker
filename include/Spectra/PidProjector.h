#ifndef PID_PROJECTOR_H
#define PID_PROJECTOR_H


// ROOBARB
#include "Utils.h"
#include "Logger.h"
#include "IObject.h"

using namespace jdb;

// ROOT
#include "TH2D.h"
#include "TH1D.h"
#include "TNtuple.h"
#include "TCut.h"

class PidProjector : public IObject {

protected:

	// bin width
	double _zbBinWidth, _zdBinWidth;
	TFile * _f;

	TCut _deuteronCut;
	TCut _electronCut;
	double _zbCutMax = 999;
	double _zbCutMin = -999;

public:
	virtual const char* classname() const { return "PidProjector"; }
	
	PidProjector( TFile * inFile, double zbBinWidth, double zdBinWidth ){
		INFOC( "( file=" << inFile << ", zbBinWidth=" << zbBinWidth << ", zdBinWidth=" << zdBinWidth <<" )" );
		_f = inFile;
		_zbBinWidth = zbBinWidth;
		_zdBinWidth = zdBinWidth;

		_deuteronCut = "";
		_electronCut = "";
	}

	~PidProjector(){
		INFOC( "()" );
	}

	string path( string name ){
		return "PidPoints/" + name;
	}

	void setZbCutMinMax( double min, double max ){
		_zbCutMin = min;
		_zbCutMax = max;
		INFOC( "zbCut = (" << _zbCutMin << ", " << _zbCutMax << ")" );
	}

	void cutDeuterons( double protonCenter, double protonSigma, float nSigma = 3 ){
	

		// less than because we want to keep what isn't deuterons
		string cutstr = "zb - " + dts(protonCenter) + " <= " + dts( protonSigma * nSigma );
		INFOC( "Deuteron Cut : " << cutstr );
		TCut cut = cutstr.c_str(); 
		_deuteronCut = cut;

		_zbCutMax = protonCenter + protonSigma * nSigma;
	}

	void cutElectrons( 	double zb_Pi, double zb_Pi_sigma, double nSigma = 3 ){

		string cutstr = "zb - " + dts(zb_Pi) + " >= " + dts( - zb_Pi_sigma * nSigma );

		_electronCut = TCut( cutstr.c_str() );
		INFOC( "Electron Cut : \"" << _electronCut << " \" "  );

		_zbCutMin = zb_Pi - zb_Pi_sigma * nSigma;
		INFOC( "zb cut min = " << _zbCutMin );

	}

	TH2D * project2D( string name, string cut = "" ){

		TNtuple * data = (TNtuple*)_f->Get( path(name).c_str() );
		if ( !data ){
			ERRORC( "ubable to open " << name );
			return new TH2D( "err", "err", 1, 0, 1, 1, 0, 1 );
		}

		double zbMax = data->GetMaximum( "zb" );
		double zdMax = data->GetMaximum( "zd" );

		INFOC( "Max from tree zbMax = " << zbMax );
		if ( zbMax > _zbCutMax )
			zbMax = _zbCutMax;

		double zbMin = data->GetMinimum( "zb" );
		double zdMin = data->GetMinimum( "zd" );

		if ( zbMin < _zbCutMin )
			zbMin = _zbCutMin;

		int zbNBins = (int) ((zbMax - zbMin) / _zbBinWidth + 0.5 );
		int zdNBins = (int) ((zdMax - zdMin) / _zdBinWidth + 0.5 );

		INFOC( "Projecting " << name << " in 2D " ) ;
		INFOC( "zb from ( " << zbMin <<", " << zbMax << " ) / " << _zbBinWidth << ", = " << zbNBins << " bins" << ")" );
		INFOC( "zd from ( " << zdMin <<", " << zdMax << " ) / " << _zdBinWidth << ", = " << zdNBins << " bins" << ")" );

		TCut allCuts = _deuteronCut && _electronCut && TCut( cut.c_str() );
		allCuts = allCuts * TCut( "w" ); // apply the track weight

		INFOC( "Cut string : " << allCuts );

		TH1 * hExisting = (TH1*)gDirectory->Get( (name + "_2D").c_str() );
		if ( nullptr != hExisting ) delete hExisting;

		TH2D * h = new TH2D( (name + "_2D").c_str(), (name + "_2D").c_str(), zdNBins, zdMin, zdMax, zbNBins, zbMin, zbMax );
		h->GetDirectory()->cd();
		string hist = name + "_2D";

		data->Draw( ("zb:zd >> " + hist).c_str(), allCuts, "colz"  );

		return h;
	}

	TH1D * project1D( string name, string var, string cut = "", bool cut_zb = true, double _min = -5, double _max = -5 ){
		TNtuple * data = (TNtuple*)_f->Get( path(name).c_str() );
		if ( !data ){
			ERRORC( "unbable to open " << name );
			return new TH1D( "err", "err", 1, 0, 1 );
		}

		double max = _max; //data->GetMaximum( var.c_str() );
		double min = _min; //data->GetMinimum( var.c_str() );

		if ( min < -5 ) min = -5;
		if ( max > 5 ) max = 5;

		if ( "zb" == var && max > _zbCutMax )
			max = _zbCutMax + (_zbCutMax - min) * .30;

		if ( "zb" == var && min < _zbCutMin )
			min = _zbCutMin - (max - _zbCutMin) * .30;
		
		double binWidth = _zbBinWidth;
		if ( "zd" == var )
			binWidth = _zdBinWidth;

		int nBins = (int) ((max - min) / binWidth + 0.5 );
		

		string hist = name + "_" + var + "_1D";
		
		TH1 * hExisting = (TH1*)gDirectory->Get( hist.c_str() );
		if ( nullptr != hExisting ) delete hExisting;

		TH1D * h = new TH1D( hist.c_str(), hist.c_str(), nBins, min, max );
		h->GetDirectory()->cd();

		INFOC( "Projecting " << name << " in 1D on " << var << " from ( " << min <<", " << max << " ) / " << binWidth << ", = " << nBins << " bins" << ")" )

		TCut allCuts = cut.c_str();
		if ( "zd" == var && cut_zb)
			allCuts = allCuts && _deuteronCut && _electronCut;

		TCut wCut = "w";
		allCuts = allCuts * wCut;
		INFOC( "Cut string : " << allCuts );

		data->Draw( ( var + " >>" + hist).c_str(),  allCuts );


		INFOC( "Integral(h) = " << h->Integral() );

		return h;
	}

	TH1D * projectEnhanced( string name, string var, string plc, double cl, double cr, double _min, double _max ){
		TNtuple * data = (TNtuple*)_f->Get( path(name).c_str() );
		if ( !data ){
			ERRORC( "unable to open " << name );
			return new TH1D( "err", "err", 1, 0, 1 );
		}

		// we cut on the variable we aren't projecting
		string cutVar = "zb";
		if ( "zb" == var )
			cutVar = "zd";

		string cut = cutVar + " < " + dts( cr ) + " && " + cutVar + " > " + dts( cl );

		// include deuteron cut if needed
		TCut allCuts = cut.c_str();
		if ( "zd" == var )
			allCuts = allCuts && _deuteronCut && _electronCut;

		double max = _max; //data->GetMaximum( var.c_str() );
		double min = _min; //data->GetMinimum( var.c_str() );

		if ( min < -5 ) min = -5;
		if ( max > 5 ) max = 5;

		if ( "zb" == var && max > _zbCutMax )
			max = _zbCutMax + (_zbCutMax - min) * .50;
		if ( "zb" == var && min < _zbCutMin )
			min = _zbCutMin - (max - _zbCutMin) * .50;
		
		double binWidth = _zbBinWidth;
		if ( "zd" == var )
			binWidth = _zdBinWidth;
		int nBins = (int) ((max - min) / binWidth + 0.5 );
	
		string hist = name + "_" + var + "_1D_" + plc;

		TH1 * hExisting = (TH1*)gDirectory->Get( hist.c_str() );
		if ( nullptr != hExisting ) delete hExisting;

		TH1D * h = new TH1D( hist.c_str(), hist.c_str(), nBins, min, max );	
		h->Sumw2();	

		INFOC( "Projecting " << name << " in 1D enhanced around " << plc << " on " << var << " from ( " << min <<", " << max << " ) / " << binWidth << ", = " << nBins << " bins" << ")" )
		INFOC( "Cutting on " << cutVar << "( " << cl << ", " << cr << " )" );

		data->Draw( ( var + " >>" + hist).c_str(), allCuts );
		return h;
	}





};
#endif
