#ifndef PID_PROJECTOR_H
#define PID_PROJECTOR_H


// ROOBARB
#include "Utils.h"
#include "Logger.h"

using namespace jdb;


// ROOT
#include "TH2D.h"
#include "TH1D.h"
#include "TNtuple.h"
#include "TCut.h"

// TODO : include the track weight!!!!


class PidProjector {
protected:

	double _zbBinWidth, _zdBinWidth;
	TFile * _f;


	TCut _deuteronCut;
	double _zbCutMax = -1;

public:
	static constexpr auto tag = "PidProjector";
	
	PidProjector( TFile * inFile, double zbBinWidth, double zdBinWidth ){
		INFO( tag, "( file=" << inFile << ", zbBinWidth=" << zbBinWidth << ", zdBinWidth=" << zdBinWidth <<" )" );
		_f = inFile;
		_zbBinWidth = zbBinWidth;
		_zdBinWidth = zdBinWidth;
	}

	~PidProjector(){
		INFO( tag, "()" );
	}

	void cutDeuterons( double protonCenter, double protonSigma, float nSigma = 3 ){
		
		// less than because we want to keep what isn't deuterons
		string cutstr = "zb - " + dts(protonCenter) + " <= " + dts( protonSigma * nSigma );
		INFO( tag, "Deuteron Cut : " << cutstr );
		TCut cut = cutstr.c_str(); 
		_deuteronCut = cut;

		_zbCutMax = protonCenter + protonSigma * nSigma;
	}



	TH2D * project2D( string name, string cut = "" ){

		TNtuple * data = (TNtuple*)_f->Get( ("PidPoints/" + name).c_str() );
		if ( !data ){
			ERROR( tag, "ubable to open " << name );
			return new TH2D( "err", "err", 1, 0, 1, 1, 0, 1 );
		}

		double zbMax = data->GetMaximum( "zb" );
		double zdMax = data->GetMaximum( "zd" );

		if ( zbMax > _zbCutMax )
			zbMax = _zbCutMax;

		double zbMin = data->GetMinimum( "zb" );
		double zdMin = data->GetMinimum( "zd" );

		int zbNBins = (int) ((zbMax - zbMin) / _zbBinWidth + 0.5 );
		int zdNBins = (int) ((zdMax - zdMin) / _zdBinWidth + 0.5 );

		INFO( tag, "Projecting " << name << " in 2D " ) ;
		INFO ( tag, "zb from ( " << zbMin <<", " << zbMax << " ) / " << _zbBinWidth << ", = " << zbNBins << " bins" << ")" );
		INFO ( tag, "zd from ( " << zdMin <<", " << zdMax << " ) / " << _zdBinWidth << ", = " << zdNBins << " bins" << ")" );

		string hist = name + "_2D ( " + ts(zdNBins) + ", " + dts( zdMin ) + ", " + dts(zdMax) + ", " + ts(zbNBins) + ", " + dts( zbMin ) + ", " + dts(zbMax) + " )";

		data->Draw( ("zb:zd >> " + hist).c_str(), _deuteronCut && TCut( cut.c_str() ), "colz"  );

		TH2D * h = (TH2D*)gDirectory->Get( (name + "_2D").c_str() );
		return h;
	}

	TH1D * project1D( string name, string var, string cut = "" ){
		TNtuple * data = (TNtuple*)_f->Get( ("PidPoints/" + name).c_str() );
		if ( !data ){
			ERROR( tag, "ubable to open " << name );
			return new TH1D( "err", "err", 1, 0, 1 );
		}

		double max = data->GetMaximum( var.c_str() );
		double min = data->GetMinimum( var.c_str() );

		if ( "zb" == var && max > _zbCutMax )
			max = _zbCutMax + (_zbCutMax - min) * .10;
		
		double binWidth = _zbBinWidth;
		if ( "zd" == var )
			binWidth = _zdBinWidth;

		int nBins = (int) ((max - min) / binWidth + 0.5 );
		

		string hist = name + "_" + var + "_1D";
		TH1D * h = new TH1D( hist.c_str(), hist.c_str(), nBins, min, max );
		h->GetDirectory()->cd();

		INFO( tag, "Projecting " << name << " in 1D on " << var << " from ( " << min <<", " << max << " ) / " << binWidth << ", = " << nBins << " bins" << ")" )

		TCut allCuts = cut.c_str();
		if ( "zd" == var )
			allCuts = allCuts && _deuteronCut;

		data->Draw( ( var + " >>" + hist).c_str(), allCuts );

		INFO( tag, "Integral(h) = " << h->Integral() );

		return h;
	}

	TH1D * projectEnhanced( string name, string var, string plc, double cl, double cr ){
		TNtuple * data = (TNtuple*)_f->Get( ("PidPoints/" + name).c_str() );
		if ( !data ){
			ERROR( tag, "ubable to open " << name );
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
			allCuts = allCuts && _deuteronCut;

		double max = data->GetMaximum( var.c_str() );
		double min = data->GetMinimum( var.c_str() );

		if ( "zb" == var && max > _zbCutMax )
			max = _zbCutMax + (_zbCutMax - min) * .10;
		
		double binWidth = _zbBinWidth;
		if ( "zd" == var )
			binWidth = _zdBinWidth;
		int nBins = (int) ((max - min) / binWidth + 0.5 );
	
		string hist = name + "_" + var + "_1D_" + plc;
		TH1D * h = new TH1D( hist.c_str(), hist.c_str(), nBins, min, max );	
		h->Sumw2();	

		INFO( tag, "Projecting " << name << " in 1D enhanced around " << plc << " on " << var << " from ( " << min <<", " << max << " ) / " << binWidth << ", = " << nBins << " bins" << ")" )
		INFO( tag, "Cutting on " << cutVar << "( " << cl << ", " << cr << " )" );

		data->Draw( ( var + " >>" + hist).c_str(), allCuts );
		return h;
	}





};
#endif
