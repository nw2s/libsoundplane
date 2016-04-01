
// Soundplaned socket server
// Copyright (c) 2016 Thomas Scott Wilson http://nw2s.net
// Distributed under the MIT license


#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>


#include <iostream>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <signal.h>

#include <iomanip>
#include <string.h>

#include <syslog.h>

#include "../src/SoundplaneDriver.h"
#include "../src/SoundplaneModelA.h"
#include "../src/MLSignal.h"
#include "../src/TouchTracker.h"

namespace 
{
	const int kMaxTouch = 4;

	static bool noteOn[kMaxTouch];

	static volatile int keepRunning = 1;
		
	/* 28 bytes */
	struct touchdata
	{
		int index;
		float x;
		float y;
		float z;
		float dz;
		float age;
		float dt;
	};


	class TouchTrackerServer : public SoundplaneDriverListener
	{
		public:
		
		    TouchTrackerServer(int socketfd) :  
				mTracker(kSoundplaneWidth,kSoundplaneHeight), 
				mSurface(kSoundplaneWidth, kSoundplaneHeight), 
				mCalibration(kSoundplaneWidth, kSoundplaneHeight), 
				mTest(kSoundplaneWidth, kSoundplaneHeight), 
				socketfd(socketfd)
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

		    virtual void deviceStateChanged(SoundplaneDriver& driver, MLSoundplaneState s) override 
			{
				syslog(LOG_INFO, "Device state changed: %d\n", s);
		    }
	    
		    virtual void receivedFrame(SoundplaneDriver& driver, const float* data, int size) override 
			{
		        if (!mHasCalibration)
		        {
		            memcpy(mCalibration.getBuffer(), data, sizeof(float) * size);
		            mHasCalibration = true;
		            mTracker.setCalibration(mCalibration);
					syslog(LOG_INFO, "Set calibration\n");					
		        }
		        else 
		        {
		            memcpy(mSurface.getBuffer(), data, sizeof(float) * size);

		            mTest = mSurface;
		            mTest.subtract(mCalibration);
		            mTracker.setInputSignal(&mTest);
		            mTracker.setOutputSignal(&mTouchFrame);
		            mTracker.process(1);


				    for (int i = 0; i < kMaxTouch; ++i)
			        {
						//TODO: Check for empty touches
						
						touchdata d;
						
						d.index = i;
						d.x = mTouchFrame(xColumn, i);
						d.y = mTouchFrame(yColumn, i);
						d.z = mTouchFrame(zColumn, i);
						d.dz = mTouchFrame(dzColumn, i);
						d.age = mTouchFrame(ageColumn, i);
						d.dt = mTouchFrame(dtColumn, i);
						
						int b = write(socketfd, &d, sizeof(d));
						
						if (b < sizeof(d))
						{
							syslog(LOG_ERR, "Error - short write");
						}
					}
		        }
		    }

		private:
			
			int socketfd;
		    bool mHasCalibration = false;
		    MLSignal mTest;
		    MLSignal mTouchFrame;
		    MLSignal mSurface;
		    MLSignal mCalibration;
		    TouchTracker mTracker;
	};




	void intHandler(int value) 
	{
	    std::cerr  << "int handler called";
    	
		if(keepRunning == 0) 
		{
	        sleep(1);
	        exit(-1);
	    }

	    keepRunning = 0;
	}


	/* Initializes a socket and waits for an incoming connection. Returns the file descriptor soon as someone is listening */
	int initSocket()
	{
		const char *socket_path = "/tmp/soundplane";

		struct sockaddr_un addr;

		int fd;
		int cl;
		int rc;

		/* Create the socket */
		if ((fd = socket(AF_LOCAL, SOCK_STREAM, 0)) == -1) 
		{
			syslog(LOG_ERR, "Socket error\n");
			exit(-1);
		}

		/* Initiaize the address */
		memset(&addr, 0, sizeof(addr));
		addr.sun_family = AF_UNIX;
	
		strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);

		/* Remove it if it's there */
		unlink(socket_path);

		/* Bind to the address */
		if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) 
		{
			syslog(LOG_ERR, "Socket error (bind)\n");
			exit(-1);
		}

		/* Indicate that we'll be listening */
		if (listen(fd, 1) == -1) 
		{
			syslog(LOG_ERR, "Socket error (listen)\n");
			exit(-1);
		}

		/* Wait for a connection */
		if ( (cl = accept(fd, NULL, NULL)) == -1)
		{
			syslog(LOG_ERR, "Socket error (accept): %d\n", cl);
			exit(-1);
		}
		
		return cl;
	}
	
	void cleanupSocket(int fd)
	{
		close(fd);
	
		unlink("/tmp/soundplane");
	}
}  

      
int main(int argc, const char * argv[]) 
{
	/* Open up the logs */
	openlog ("soundplaned", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
	syslog(LOG_INFO, "Initializing soundplane server\n");	
	
	/* Initialize local state */
	for (int i = 0; i < kMaxTouch; i++)
	{
		noteOn[i] = false;
	}
	
	/* Set up the interrupt handler so we can exit cleanly */
    signal(SIGINT, intHandler);
	signal(SIGPIPE, intHandler);

	/* Listen on a socket until someone connects */
	int socketfd = initSocket();

	/* Now that we know we have a listener, start looking for a soundplane*/
    TouchTrackerServer listener(socketfd);

    auto driver = SoundplaneDriver::create(&listener);

    syslog(LOG_INFO, "Initial device state: %d\n", driver->getDeviceState());

    while(keepRunning) 
	{
        sleep(1);
    }
    
	delete driver.release();

	cleanupSocket(socketfd);

    return 0;
}
