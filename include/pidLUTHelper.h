#ifndef PID_LUT_HELPER_H
#define PID_LUT_HELPER_H


class pidLUTHelper
{
public:
	pidLUTHelper( histoBook * lut ){

		pidLUT = lut;

	}
	~pidLUTHelper(){

	}


	bool isParticle( string pType, double p, double eta,
						double dedx, double deltaB, 
						double nSigDedx, double nSigDB ){

		double mDB = meanDB( pType, p, eta );
		double mDedx = meanDedx( pType, p, eta );
		double sDB = sigmaDB( pType, p, eta );
		double sDedx = sigmaDedx( pType, p, eta );

		double nSigMeasuredDB = ( deltaB - mDB ) / sDB;
		double nSigMeasuredDedx = ( dedx - mDedx ) / sDedx;

		double alpha = nSigMeasuredDB / nSigDB;
		alpha *= alpha;

		double beta = nSigMeasuredDedx / nSigDedx;
		beta *= beta;

		if ( alpha + beta < 1 )
			return true;

		return false;
	}

	double meanDB( string pType, double p, double eta ){

		// for now eta is not used since my lut only has one eta bin

		if ( pidLUT->exists( "xMean" + pType ) ){
			TH1* h = pidLUT->get( "xMean" + pType );
			int pBin = h->GetXaxis()->FindBin( p );
			double mean = h->GetBinContent( pBin );
			return mean;
		}

		return -999.9;
	}

	double meanDedx( string pType, double p, double eta ){

		// for now eta is not used since my lut only has one eta bin

		if ( pidLUT->exists( "yMean" + pType ) ){
			TH1* h = pidLUT->get( "yMean" + pType );
			int pBin = h->GetXaxis()->FindBin( p );
			double mean = h->GetBinContent( pBin );
			return mean;
		}

		return -999.9;
	}

	double sigmaDB( string pType, double p, double eta ){

		// for now eta is not used since my lut only has one eta bin

		if ( pidLUT->exists( "xSigma" + pType ) ){
			TH1* h = pidLUT->get( "xSigma" + pType );
			int pBin = h->GetXaxis()->FindBin( p );
			double sigma = h->GetBinContent( pBin );
			return sigma;
		}

		return -999.9;
	}

	double sigmaDedx( string pType, double p, double eta ){

		// for now eta is not used since my lut only has one eta bin

		if ( pidLUT->exists( "ySigma" + pType ) ){
			TH1* h = pidLUT->get( "ySigma" + pType );
			int pBin = h->GetXaxis()->FindBin( p );
			double sigma = h->GetBinContent( pBin );
			return sigma;
		}

		return -999.9;
	}

protected:

	histoBook * pidLUT;


	
};

#endif