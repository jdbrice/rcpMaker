

#include "FemtoDstMaker.h"
  
FemtoDstMaker::FemtoDstMaker( XmlConfig * _cfg, string _np, string _fileList, string _jobPrefix )
: InclusiveSpectra( _cfg, _np, _fileList, _jobPrefix ){


	

}

FemtoDstMaker::~FemtoDstMaker(){


	delete tree;

}


void FemtoDstMaker::preEventLoop(){
	InclusiveSpectra::preEventLoop();

	output = new TFile( (outputPath + jobPrefix + cfg->getString( nodePath + "output.tree:suffix", "_femtoDst.root" )).c_str(), "RECREATE" );
	tree = new TTree( "FemtoDst", "A subset of the PicoDst" );
	tree->Branch( "Event.mTriggerWord", &event.mTriggerWord, "Event.mTriggerWord/i" );
	tree->Branch( "Event.mRunId", &event.mRunId, "Event.mRunId/I" );
	tree->Branch( "Event.mPrimaryVertex.mX1", &event.mPrimaryVertexX1, "Event.mPrimaryVertex.mX1/F");
	tree->Branch( "Event.mPrimaryVertex.mX2", &event.mPrimaryVertexX2, "Event.mPrimaryVertex.mX2/F");
	tree->Branch( "Event.mPrimaryVertex.mX3", &event.mPrimaryVertexX3, "Event.mPrimaryVertex.mX3/F");
	tree->Branch( "Event.mRefMult", &event.mRefMult, "Event.mRefMult/I" );
	tree->Branch( "Event.mNumberOfPrimaryTracks", &event.mNumberOfPrimaryTracks, "Event.mNumberOfPrimaryTracks/I" );
	tree->Branch( "Event.mNBTOFMatchedTracks", &event.mNBTOFMatchedTracks, "Event.mNBTOFMatchedTracks/I" );

	
	tree->Branch( "Tracks.mEta", tracks.mEta, "Tracks.mEta[Event.mNumberOfPrimaryTracks]/F" );
	tree->Branch( "Tracks.mPt", tracks.mPt, "Tracks.mPt[Event.mNumberOfPrimaryTracks]/F" );
	tree->Branch( "Tracks.mDedx", tracks.mDedx, "Tracks.mDedx[Event.mNumberOfPrimaryTracks]/F" );
	tree->Branch( "Tracks.mBTofBeta", tracks.mBTofBeta, "Tracks.mBTofBeta[Event.mNumberOfPrimaryTracks]/F" );
	tree->Branch( "Tracks.mBTofYLocal", tracks.mBTofYLocal, "Tracks.mBTofYLocal[Event.mNumberOfPrimaryTracks]/F" );
	tree->Branch( "Tracks.mBTofZLocal", tracks.mBTofZLocal, "Tracks.mBTofZLocal[Event.mNumberOfPrimaryTracks]/F" );
	tree->Branch( "Tracks.mGDca", tracks.mGDca, "Tracks.mGDca[Event.mNumberOfPrimaryTracks]/F" );
	tree->Branch( "Tracks.mCharge", tracks.mCharge, "Tracks.mCharge[Event.mNumberOfPrimaryTracks]/B" );

	tree->Branch( "Tracks.mNHitsFit", tracks.mNHitsFit, "Tracks.mNHitsFit[Event.mNumberOfPrimaryTracks]/I" );
	tree->Branch( "Tracks.mNHitsMax", tracks.mNHitsMax, "Tracks.mNHitsMax[Event.mNumberOfPrimaryTracks]/I" );
	tree->Branch( "Tracks.mNHitsDedx", tracks.mNHitsDedx, "Tracks.mNHitsDedx[Event.mNumberOfPrimaryTracks]/I" );

	tree->Branch( "Tracks.mPMomentum.mX1", tracks.mPMomentumX1, "Tracks.mPMomentum.mX1[Event.mNumberOfPrimaryTracks]/F" );
	tree->Branch( "Tracks.mPMomentum.mX2", tracks.mPMomentumX2, "Tracks.mPMomentum.mX2[Event.mNumberOfPrimaryTracks]/F" );
	tree->Branch( "Tracks.mPMomentum.mX3", tracks.mPMomentumX3, "Tracks.mPMomentum.mX3[Event.mNumberOfPrimaryTracks]/F" );
	tree->SetAutoSave( cfg->getInt( nodePath + "output.tree:autoSaveSize", 250000000 ) );
	tree->SetMaxTreeSize( cfg->getInt( nodePath + "output.tree:splitSize", 250000000 ) );

}


void FemtoDstMaker::analyzeEvent(){

	event.mNumberOfPrimaryTracks = 0;
	event.mNBTOFMatchedTracks = nTofMatchedTracks;
	InclusiveSpectra::analyzeEvent();
	event.mTriggerWord = pico->triggerWord();
	event.mRunId = pico->runId();
	event.mPrimaryVertexX1 = pico->vX();
	event.mPrimaryVertexX2 = pico->vY();
	event.mPrimaryVertexX3 = pico->vZ();
	event.mRefMult = pico->refMult();

	tree->Fill();
}

void FemtoDstMaker::analyzeTrack( Int_t iTrack ) {


	tracks.mPMomentumX1[ event.mNumberOfPrimaryTracks ] = pico->trackPx( iTrack );
	tracks.mPMomentumX2[ event.mNumberOfPrimaryTracks ] = pico->trackPy( iTrack );
	tracks.mPMomentumX3[ event.mNumberOfPrimaryTracks ] = pico->trackPz( iTrack );

	tracks.mCharge[ event.mNumberOfPrimaryTracks ] = (Short_t)pico->trackCharge( iTrack );
	tracks.mEta[ event.mNumberOfPrimaryTracks ] = pico->trackEta( iTrack );
	tracks.mPt[ event.mNumberOfPrimaryTracks ] = pico->trackPt( iTrack );
	
	tracks.mDedx[ event.mNumberOfPrimaryTracks ] = pico->trackDedx( iTrack );
	tracks.mBTofBeta[ event.mNumberOfPrimaryTracks ] = pico->trackBeta( iTrack );
	
	tracks.mBTofYLocal[ event.mNumberOfPrimaryTracks ] = pico->trackYLocal( iTrack );
	tracks.mBTofZLocal[ event.mNumberOfPrimaryTracks ] = pico->trackZLocal( iTrack );
	
	tracks.mGDca[ event.mNumberOfPrimaryTracks ] = pico->trackDca( iTrack );

	tracks.mNHitsFit[ event.mNumberOfPrimaryTracks ] = pico->trackNHitsFit( iTrack );
	tracks.mNHitsDedx[ event.mNumberOfPrimaryTracks ] = pico->trackNHitsDedx( iTrack );
	tracks.mNHitsMax[ event.mNumberOfPrimaryTracks ] = pico->trackNHitsPossible( iTrack );


	event.mNumberOfPrimaryTracks ++;


}


void FemtoDstMaker::postEventLoop(){
	InclusiveSpectra::postEventLoop();

	TFile * cFile = tree->GetCurrentFile();
	cFile->Write();
	cFile->Close();

}





