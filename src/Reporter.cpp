
#include "Reporter.h"
#include "Utils.h"
#include "LoggerConfig.h"

namespace jdb{

	int Reporter::instances = 0;

	Reporter::Reporter( string filename, int w, int h, int logLevel ){

		
		logger = new Logger( logLevel, "Reporter" );
		

		this->filename = filename;

		canvas = new TCanvas( ("Reporter"+ts(instances)).c_str() , "canvas", w, h);
		canvas->Print( ( filename + "[" ).c_str() );
		logger->info(__FUNCTION__) << " Opening " << filename << endl;
		instances++;

		logger->info(__FUNCTION__) << " Instance #" << instances << endl;
	}
	Reporter::Reporter( XmlConfig*config, string np, string prefix ){

		cfg = config;
		nodePath = np;

		logger = LoggerConfig::makeLogger( config, np+"Logger" );
		
		this->filename = prefix + cfg->getString( np+"output:url" );

		int w = cfg->getInt( np+"output:width", 400 );
		int h = cfg->getInt( np+"output:height", 400 );

		canvas = new TCanvas( ("Reporter"+ts(instances)).c_str() , "canvas", w, h);
		canvas->Print( ( filename + "[" ).c_str() );
		
		logger->info(__FUNCTION__) << " Opening " << filename << endl;
		
		instances++;

		logger->info(__FUNCTION__) << " Instance #" << instances << endl;

	}

	Reporter::~Reporter() {
		// properly close the report file
		logger->info(__FUNCTION__) << " " << endl;
		
		canvas->Print( ( filename + "]" ).c_str() );
		delete canvas;

		logger->info(__FUNCTION__) << filename << " Closed " << endl;
		delete logger;
	}

	void Reporter::newPage( int dx, int dy){
		logger->info(__FUNCTION__) << " New Page ( " << dx << ", " << dy << " ) " << endl;

		this->dx = dx;
		this->dy = dy;

		canvas->Clear();
		canvas->Divide( dx, dy );
		cd( 1 );
	}

	void Reporter::cd( int pad ){
		logger->info(__FUNCTION__) << " Current Pad " << pad << endl;
		canvas->cd( pad );
		currentPad = pad;
	}

	void Reporter::cd( int x, int y){
		cd( ( (y - 1 ) * dx) + x );
	}

	void Reporter::next(){
		logger->info(__FUNCTION__) << " Pushing to next pad " << endl;
		currentPad++;
		if ( currentPad > dx*dy){
			savePage();
			newPage( dx, dy);
		} else {
			cd( currentPad );
		}
	}

	void Reporter::savePage( string name ) {

		if ( "" == name ){
			canvas->Print( ( filename ).c_str() );
			logger->info(__FUNCTION__) << " Saving Page to " << filename << endl;
		}
		else {
			canvas->Print( name.c_str() );
			logger->info(__FUNCTION__) << " Saving Page to " << name << endl;
		}
	}

	void Reporter::saveImage( string name ){
		logger->info(__FUNCTION__) << " Saving Image to " << name << endl;
		canvas->SaveAs( name.c_str() );
	}
	
}
