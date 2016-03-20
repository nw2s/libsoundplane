#include <iostream>
#include <unistd.h>
#include <string.h>

#include <pthread.h>

#include <osc/OscOutboundPacketStream.h>
#include <osc/OscReceivedElements.h>
#include <osc/OscPacketListener.h>
#include <ip/UdpSocket.h>


#include <iostream>
#include <chrono>
#include <thread>
#include <unistd.h>

#include <iomanip>
#include <string.h>

#include <SoundplaneDriver.h>
#include <SoundplaneModelA.h>
#include <MLSignal.h>
#include <TouchTracker.h>

#include "mec.h"

#define OUT_ADDRESS "127.0.0.1"
#define OUT_PORT 9002
#define OUTPUT_BUFFER_SIZE 1024

#define TOUCH_THROTTLE 4

const int kMaxTouch = 8;

class TouchProcesssor : public SoundplaneDriverListener
{
public:
    TouchProcesssor() :
    mTracker(kSoundplaneWidth,kSoundplaneHeight),
    mSurface(kSoundplaneWidth, kSoundplaneHeight),
    mCalibration(kSoundplaneWidth, kSoundplaneHeight),
    mTest(kSoundplaneWidth, kSoundplaneHeight),
    transmitSocket( IpEndpointName( OUT_ADDRESS, OUT_PORT ) )
    {
        mTracker.setSampleRate(kSoundplaneSampleRate);
        mTouchFrame.setDims(kTouchWidth, kSoundplaneMaxTouches);
        mTracker.setMaxTouches(kMaxTouch);
        mTracker.setLopass(100);
        //mTracker.setThresh(0.005000);
        mTracker.setThresh(0.005000);
        mTracker.setZScale(0.700000);
        mTracker.setForceCurve(0.250000);
        mTracker.setTemplateThresh(0.279104);
        mTracker.setBackgroundFilter(0.050000);
        mTracker.setQuantize(1);
        mTracker.setRotate(0);
        //mTracker.setNormalizeMap(sig)
    }
    
    virtual void deviceStateChanged(SoundplaneDriver& driver, MLSoundplaneState s) override {
        std::cout << "Device state changed: " << s << std::endl;
        }
    void sendTouches(int frameCount) {
            int count = 0;
            osc::OutboundPacketStream op( buffer, OUTPUT_BUFFER_SIZE );
            op << osc::BeginBundleImmediate;
            for(int i=0; i<kMaxTouch; ++i)
            {
                bool active = mTouchFrame(ageColumn,i) > 0 ;
                
                if(     active != touchActive[i]
                    ||  (active && (frameCount % TOUCH_THROTTLE) ==0 )) {

                        touchActive[i]= active;
                        count++;
                        op << osc::BeginMessage( "/tb/touch" )
                        << i
                        << mTouchFrame(xColumn, i)
                        << mTouchFrame(yColumn, i)
                        << mTouchFrame(zColumn, i)
                        << mTouchFrame(noteColumn, i)
                        << osc::EndMessage;
                        
                        //cout << mTouchFrame(dzColumn, i);
                        //cout << mTouchFrame(ageColumn, i;
                        //cout << mTouchFrame(dtColumn, i);
                        //cout << mTouchFrame(reservedColumn, i);
                }
            }
            op << osc::EndBundle;
            if(count > 0)  
            {
                transmitSocket.Send( op.Data(), op.Size() );
            }
        }
        
        virtual void receivedFrame(SoundplaneDriver& driver, const float* data, int size) override {
            if (!mHasCalibration)
            {
                memcpy(mCalibration.getBuffer(), data, sizeof(float) * size);
                mHasCalibration = true;
                mTracker.setCalibration(mCalibration);
                //std::cout << "calibration\n";
                //mCalibration.dumpASCII(std::cout);
                //std::cout << "============\n";
                
                //            mTracker.setDefaultNormalizeMap(); // ?
                for(int i=0;i<kMaxTouch;i++) touchActive[i] = false;
            }
            else {
                memcpy(mSurface.getBuffer(), data, sizeof(float) * size);
                mTest = mSurface;
                mTest.subtract(mCalibration);
                mTracker.setInputSignal(&mTest);
                mTracker.setOutputSignal(&mTouchFrame);
                mTracker.process(1);
                sendTouches(mFrameCounter);
                mFrameCounter = (mFrameCounter + 1) % 1000;
            }
        }
        
    private:
        int mFrameCounter = 0;
        bool mHasCalibration = false;
        MLSignal mTest;
        MLSignal mTouchFrame;
        MLSignal mSurface;
        MLSignal mCalibration;
        TouchTracker mTracker;
        
        UdpTransmitSocket transmitSocket;
        char buffer[OUTPUT_BUFFER_SIZE];
        bool touchActive[kMaxTouch];
};

void *soundplane_proc(void *)
{
    pthread_mutex_lock(&waitMtx);
    std::cout  << "soundplane_proc start" << std::endl;
    TouchProcesssor listener;
    auto pDriver = SoundplaneDriver::create(&listener);
    int searchCount = 5;
    
    while(pDriver->getDeviceState() == 0 && searchCount > 0) 
    {
        sleep(1);
        searchCount--;
    }
    
    if(searchCount>0) {
        while(keepRunning) {
            pthread_cond_wait(&waitCond, &waitMtx);
        }
    }
    delete pDriver.release();
    
    std::cout  << "soundplane_proc stop" << std::endl;
    pthread_mutex_unlock(&waitMtx);
    pthread_exit(NULL);
}

