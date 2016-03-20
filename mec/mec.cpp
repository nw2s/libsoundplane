#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#include <pthread.h>

#include <osc/OscOutboundPacketStream.h>
#include <osc/OscReceivedElements.h>
#include <osc/OscPacketListener.h>
#include <ip/UdpSocket.h>


#include "mec.h"

pthread_cond_t  waitCond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t waitMtx = PTHREAD_MUTEX_INITIALIZER;

volatile int keepRunning = 1;

void exitHandler() {
    std::cerr  << "exit handler called" << std::endl;
}

void intHandler(int sig) {
    // only called in main thread
    if (sig == SIGINT) {
        std::cerr  << "int handler called" << std::endl;
        keepRunning = 0;
        pthread_cond_broadcast(&waitCond);
    }
}



int main(int ac, char **av)
{
    atexit(exitHandler);

    // block sigint from other threads
    sigset_t sigset, oldset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGINT);
    pthread_sigmask(SIG_BLOCK, &sigset, &oldset);

    int rc =0;

    std::cout   << "mec initialise " << std::endl;
    pthread_t command_thread;
    rc = pthread_create(&command_thread, NULL, osc_command_proc, NULL);
    if (rc) {
        std::cerr << "unabled to create osc thread" << rc << std::endl;
        exit(-1);
    }
    usleep(1000);
    
#if 0
    pthread_t eigen_thread;
    rc = pthread_create(&eigen_thread, NULL, eigenharp_proc, NULL);
    if (rc) {
        std::cerr << "unabled to create eigen thread" << rc << std::endl;
        exit(-1);
    }
    usleep(1000);
#endif 
    
    pthread_t soundplane_thread;
    rc = pthread_create(&soundplane_thread, NULL, soundplane_proc, NULL);
    if (rc) {
        std::cerr << "unabled to create soundplane thread" << rc << std::endl;
        exit(-1);
    }
    usleep(1000);

    // Install the signal handler for SIGINT.
    struct sigaction s;
    s.sa_handler = intHandler;
    sigemptyset(&s.sa_mask);
    s.sa_flags = 0;
    sigaction(SIGINT, &s, NULL);

    // Restore the old signal mask only for this thread.
    pthread_sigmask(SIG_SETMASK, &oldset, NULL);

    pthread_mutex_lock(&waitMtx);

    std::cout   << "mec running " << std::endl;
    while(keepRunning) {
        pthread_cond_wait(&waitCond, &waitMtx);
    }
    pthread_mutex_unlock(&waitMtx);
    
    // really we should join threads where to do a nice exit
    std::cout   << "mec stopping " << std::endl;
    sleep(5);
    std::cout   << "mec exit " << std::endl;


    pthread_cond_destroy(&waitCond);
    pthread_mutex_destroy(&waitMtx);
    exit(0);
}
