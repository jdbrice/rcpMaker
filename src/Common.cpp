#include "Common.h"

// ROOT
#include "TVirtualFitter.h"

 vector<string> Common::species = { "E", "Pi", "K", "P" };

 vector<int> Common::charges = { -1, 1 };
 vector<string> Common::sCharges = { "n", "p" };


 string Common::toXml( TF1 * f ){

 	string line = "";
 	line += "formula=\"";
 	line += f->GetTitle();
 	line += "\"";

 	for ( int ip = 0; ip < f->GetNpar(); ip++ ){

 		line += " p" + ts(ip) + "=\"";
 		line += dts(f->GetParameter(ip));
 		line += "\"";

 		line += " e" + ts(ip) + "=\"";
 		line += dts(f->GetParError(ip));
 		line += "\"";
 	}

 	return line;
 }

// TODO: finish the histogram toXml
string Common::toXml( TH1 * h ){

	string line1 = "<bins>";
	// bins frst
	for ( int i = 1; i < h->GetNbinsX() + 1; i++ ){
		line1 += dts( h->GetBinLowEdge( i ) ) + ", ";
	}
	line1 += dts( h->GetBinLowEdge( h->GetNbinsX() ) + h->GetBinWidth( h->GetNbinsX() ) );
	line1 += "</bins";

	return line1;
}


string Common::toXml(  TGraph * g, string linePrefix  ){

	string lx = "<x>";
	string ly = "<y>";

	for ( int iP = 0; iP < g->GetN(); iP++ ){
		double x, y;
		g->GetPoint( iP, x, y );
		lx += dts( x );
		ly += dts( y );

		if ( iP < g->GetN() - 1 ){
			lx += ", ";
			ly += ", ";
		}
	}
	lx += "</x>";
	ly += "</y>";

	return linePrefix + lx + "\n" + linePrefix + ly;
}

string Common::toXml( TGraphErrors * g ){
	string a = Common::toXml( (TGraph*) g );

	string lex = "<ex>";
	string ley = "<ey>";

	for ( int iP = 0; iP < g->GetN(); iP++ ){
		double ex, ey;
		ex = g->GetErrorX( iP );
		ey = g->GetErrorY( iP );
		lex += dts( ex );
		ley += dts( ey );

		if ( iP < g->GetN() - 1 ){
			lex += ", ";
			ley += ", ";
		}
	}

	return a + "\n" + lex + "\n" + ley;
}

string Common::toXml( TGraphAsymmErrors * g, string linePrefix ){
	string a = Common::toXml( (TGraph*) g, linePrefix );

	string lexh = "<exh>";
	string leyh = "<eyh>";
	string lexl = "<exl>";
	string leyl = "<eyl>";

	for ( int iP = 0; iP < g->GetN(); iP++ ){
		double exh, eyh;
		double exl, eyl;
		exh = g->GetErrorXhigh( iP );
		eyh = g->GetErrorYhigh( iP );
		lexh += dts( exh );
		leyh += dts( eyh );

		exl = g->GetErrorXlow( iP );
		eyl = g->GetErrorYlow( iP );
		lexl += dts( exl );
		leyl += dts( eyl );

		if ( iP < g->GetN() - 1 ){
			lexh += ", ";
			leyh += ", ";
			lexl += ", ";
			leyl += ", ";
		}
	}

	lexh += "</exh>";
	leyh += "</eyh>";
	lexl += "</exl>";
	leyl += "</eyl>";

	return a + "\n" + linePrefix + lexh + "\n" + linePrefix + leyh +"\n" + linePrefix + lexl + "\n" + linePrefix + leyl;
}

 TH1 * Common::fitCL( TF1 * f, string name, int nPoints, double x1, double x2 ){

 	// calculate instead
 	if ( -1.0 == x1  && -1.0 == x2 )
 		f->GetRange( x1, x2 );
 	
 	if ( nPoints < 2 )
 		nPoints = 100;

 	// histo to hold CL
 	TH1F * hCL = new TH1F( name.c_str(), name.c_str(), nPoints, x1, x2 );
 	(TVirtualFitter::GetFitter())->GetConfidenceIntervals(hCL);


 	return hCL;
 }