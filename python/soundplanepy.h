
// SoundplanePython: a python binding for the Madrona Soundplane
// Copyright (c) 2016 Thomas Scott Wilson http://nw2s.net
// Distributed under the MIT license

#ifndef SOUNDPLANEPY_H
#define SOUNDPLANEPY_H

#include "../src/SoundplaneDriver.h"
#include "../src/SoundplaneModelA.h"
#include "../src/MLSignal.h"
#include "../src/TouchTracker.h"


class TouchTrackerListener : public SoundplaneDriverListener
{

	public:
	
	    TouchTrackerListener();

	    virtual void deviceStateChanged(SoundplaneDriver& driver, MLSoundplaneState s) override; 
	    virtual void receivedFrame(SoundplaneDriver& driver, const float* data, int size) override;
		
		void dumpTouches();
		

	private:

	    int mFrameCounter = 0;
	    bool mHasCalibration = false;
	    MLSignal mTest;
	    MLSignal mTouchFrame;
	    MLSignal mSurface;
	    MLSignal mCalibration;
	    TouchTracker mTracker;
};


SoundplaneDriver* create(SoundplaneDriverListener *listener);

#endif