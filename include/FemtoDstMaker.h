#ifndef FEMTO_DST_MAKER_H
#define FEMTO_DST_MAKER_H

#include "InclusiveSpectra.h"

#include "FemtoEvent.h"
#include "FemtoTrack.h"


class FemtoDstMaker : public InclusiveSpectra
{
protected:

	FemtoEvent event;
	FemtoTrack tracks;

	TTree * tree;
	TFile * output;

public:
	FemtoDstMaker( XmlConfig * config, string np, string fileList ="", string jobPrefix ="");
	~FemtoDstMaker();

protected:
	virtual void analyzeEvent();
	virtual void analyzeTrack( Int_t iTrack );
	virtual void preEventLoop();
	virtual void postEventLoop();
	
};

#endif