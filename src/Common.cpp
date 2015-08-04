#include "Common.h"

 vector<string> Common::species = { "Pi", "K", "P" };

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