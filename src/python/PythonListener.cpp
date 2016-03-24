
// SoundplanePython: a python binding for the Madrona Soundplane
// Copyright (c) 2016 Thomas Scott Wilson http://nw2s.net
// Distributed under the MIT license

#include <iostream>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <signal.h>

#include <iomanip>
#include <string.h>

#include "PythonListener.h"

#include "../SoundplaneDriver.h"
#include "../SoundplaneModelA.h"
#include "../MLSignal.h"
#include "../TouchTracker.h"

const int kMaxTouch = 4;

TouchTrackerListener::TouchTrackerListener() :  mTracker(kSoundplaneWidth,kSoundplaneHeight), mSurface(kSoundplaneWidth, kSoundplaneHeight), mCalibration(kSoundplaneWidth, kSoundplaneHeight), mTest(kSoundplaneWidth, kSoundplaneHeight)
{
    mTracker.setSampleRate(kSoundplaneSampleRate);
    mTouchFrame.setDims(kTouchWidth, kSoundplaneMaxTouches);
    mTracker.setMaxTouches(kMaxTouch);
    mTracker.setLopass(100);
    mTracker.setThresh(0.005000);
    mTracker.setZScale(0.700000);
    mTracker.setForceCurve(0.250000);
    mTracker.setTemplateThresh(0.279104);
    mTracker.setBackgroundFilter(0.050000);
    mTracker.setQuantize(1);
    mTracker.setRotate(0);
    //mTracker.setNormalizeMap(sig)
}

void TouchTrackerListener::deviceStateChanged(SoundplaneDriver& driver, MLSoundplaneState s) 
{
	//TODO: syslog all std::couts
    std::cout << "Device state changed: " << s << std::endl;
}
    
void TouchTrackerListener::dumpTouches() 
{
    std::cout << std::fixed << std::setw(6) << std::setprecision(4);

    for(int i=0; i<kMaxTouch; ++i)
    {
        std::cout << " x:" << mTouchFrame(xColumn, i);
        std::cout << " y:" << mTouchFrame(yColumn, i);
        std::cout << " z:" << mTouchFrame(zColumn, i);
        //cout << mTouchFrame(dzColumn, i);
        //cout << mTouchFrame(ageColumn, i;
        //cout << mTouchFrame(dtColumn, i);
        std::cout << " n:" << mTouchFrame(noteColumn, i);
        //cout << mTouchFrame(reservedColumn, i);
        std::cout << std::endl;
    }
}

void TouchTrackerListener::receivedFrame(SoundplaneDriver& driver, const float* data, int size) 
{
    if (!mHasCalibration)
    {
        memcpy(mCalibration.getBuffer(), data, sizeof(float) * size);
        mHasCalibration = true;
        mTracker.setCalibration(mCalibration);
        std::cout << "calibration\n";
        mCalibration.dumpASCII(std::cout);
        std::cout << "============\n";
	
		// mTracker.setDefaultNormalizeMap(); // ?
    }
    else 
    {
        memcpy(mSurface.getBuffer(), data, sizeof(float) * size);

        mTest = mSurface;
        mTest.subtract(mCalibration);
        mTracker.setInputSignal(&mTest);
        mTracker.setOutputSignal(&mTouchFrame);
        mTracker.process(1);

        if (mFrameCounter == 0)  
		{
            mTest.scale(100.f);
            mTest.flipVertical();
            mTest.dumpASCII(std::cout);
            mTest.dump(std::cout);
            dumpTouches();
            std::cout << "\n";
        }

    }

    mFrameCounter = (mFrameCounter + 1) % 1000;
}


std::unique_ptr<SoundplaneDriver> create(SoundplaneDriverListener *listener)
{
	return SoundplaneDriver::create(listener);
}

