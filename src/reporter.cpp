
#include "Reporter.h"
#include "Utils.h"


namespace jdb{

	int Reporter::instances = 0;

	Reporter::Reporter( string filename, int w, int h, Logger * pLog ){

		if ( NULL == pLog )
			log = new Logger( Logger::llDefault, "HistoBook" );
		else 
			log = pLog;

		this->filename = filename;

		canvas = new TCanvas( ("Reporter"+ts(instances)).c_str() , "canvas", w, h);
		canvas->Print( ( filename + "[" ).c_str() );
		log->info(__FUNCTION__) << " Opening " << filename << endl;
		instances++;

		log->info(__FUNCTION__) << " Instance #" << instances << endl;
	}

	Reporter::~Reporter() {
		// properly close the report file
		log->info(__FUNCTION__) << " " << endl;
		
		canvas->Print( ( filename + "]" ).c_str() );
		delete canvas;

		log->info(__FUNCTION__) << filename << " Closed " << endl;
	}

	void Reporter::newPage( int dx, int dy){
		log->info(__FUNCTION__) << " New Page ( " << dx << ", " << dy << " ) " << endl;

		this->dx = dx;
		this->dy = dy;

		canvas->Clear();
		canvas->Divide( dx, dy );
		cd( 1 );
	}

	void Reporter::cd( int pad ){
		log->info(__FUNCTION__) << " Current Pad " << pad << endl;
		canvas->cd( pad );
		currentPad = pad;
	}

	void Reporter::cd( int x, int y){
		cd( ( (y - 1 ) * dx) + x );
	}

	void Reporter::next(){
		log->info(__FUNCTION__) << " Pushing to next pad " << endl;
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
			log->info(__FUNCTION__) << " Saving Page to " << filename << endl;
		}
		else {
			canvas->Print( name.c_str() );
			log->info(__FUNCTION__) << " Saving Page to " << name << endl;
		}
	}

	void Reporter::saveImage( string name ){
		log->info(__FUNCTION__) << " Saving Image to " << name << endl;
		canvas->SaveAs( name.c_str() );
	}
	
}
