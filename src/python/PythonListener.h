
// SoundplanePython: a python binding for the Madrona Soundplane
// Copyright (c) 2016 Thomas Scott Wilson http://nw2s.net
// Distributed under the MIT license

#include "../SoundplaneDriver.h"
#include "../SoundplaneModelA.h"
#include "../MLSignal.h"
#include "../TouchTracker.h"


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


std::unique_ptr<SoundplaneDriver> create(SoundplaneDriverListener *listener);
