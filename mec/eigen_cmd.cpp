#include <eigenfreed/eigenfreed.h>

#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#include <pthread.h>

#include <osc/OscOutboundPacketStream.h>
#include <ip/UdpSocket.h>

#include "mec.h"


#define OUT_ADDRESS "127.0.0.1"
#define OUT_PORT 9001
#define OUTPUT_BUFFER_SIZE 1024


class EigenharpOSCCallback: public  EigenApi::Callback
{
public:
    EigenharpOSCCallback(EigenApi::Eigenharp& eh) :  
    	transmitSocket( IpEndpointName( OUT_ADDRESS, OUT_PORT ) )
    {
    }
    
    virtual void device(const char* dev, DeviceType dt, int rows, int cols, int ribbons, int pedals)
    {
        osc::OutboundPacketStream op( buffer, OUTPUT_BUFFER_SIZE );
	    op << osc::BeginBundleImmediate
        << osc::BeginMessage( "/tb/device" )
        << dev << (int) dt << rows << cols << ribbons << pedals  
        << osc::EndMessage
        << osc::EndBundle;
	    transmitSocket.Send( op.Data(), op.Size() );
    }
    
    virtual void key(const char* dev, unsigned long long t, unsigned course, unsigned key, bool a, unsigned p, int r, int y)
    {
        osc::OutboundPacketStream op( buffer, OUTPUT_BUFFER_SIZE );
	    op << osc::BeginBundleImmediate
        << osc::BeginMessage( "/tb/key" )
        << dev << (int) t << (int) course << (int) key << a << (int) p << r << y    
        << osc::EndMessage
        << osc::EndBundle;
	    transmitSocket.Send( op.Data(), op.Size() );
    }
    
    virtual void breath(const char* dev, unsigned long long t, unsigned val)
    {
    }
    
    virtual void strip(const char* dev, unsigned long long t, unsigned strip, unsigned val)
    {
    }
    
    virtual void pedal(const char* dev, unsigned long long t, unsigned pedal, unsigned val)
    {
    }
    
private:
	UdpTransmitSocket transmitSocket;
	char buffer[OUTPUT_BUFFER_SIZE];
};

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
void getWaitTime(struct timespec& ts, int t) {
    clock_serv_t cclock;
    mach_timespec_t mts;
    host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    ts.tv_sec = mts.tv_sec;
    ts.tv_nsec = mts.tv_nsec;
    t += (ts.tv_nsec/1000);
    ts.tv_nsec = 0;
    ts.tv_sec += t/1000000;
    ts.tv_nsec += 1000*(t%1000000);
}
#else 
void getWaitTime(struct timespec& ts, int t) {
    clock_gettime(CLOCK_REALTIME, &ts);

    t += (ts.tv_nsec/1000);
    ts.tv_nsec = 0;
    ts.tv_sec += t/1000000;
    ts.tv_nsec += 1000*(t%1000000);
}
#endif




void *eigenharp_proc(void *)
{
    std::cout  << "eigenharp_proc start" << std::endl;
    EigenApi::Eigenharp myD("../eigenharp/resources/");
    myD.addCallback(new EigenharpOSCCallback(myD));
    if(myD.create())
    {
        if(myD.start()) 
        {
            pthread_mutex_lock(&waitMtx);
            while (keepRunning)
            {
                myD.poll(1000);
                struct timespec ts;
                getWaitTime(ts,1000);
                pthread_cond_timedwait(&waitCond, &waitMtx,&ts);
            }
            pthread_mutex_unlock(&waitMtx);
        }
        else 
        {
            std::cout  << "eigenharp_proc eigenapi start failed" << std::endl;
        }
    }
    // important destructor is called, else 10.11 mac will panic
    myD.destroy();
    std::cout  << "eigenharp_proc stop" << std::endl;
    pthread_exit(NULL);
}


