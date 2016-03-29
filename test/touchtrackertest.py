
import time
import libsoundplane


def deviceStateChanged(state):
    
    print "Device state: ", state
    


libsoundplane.onDeviceStateChange(deviceStateChanged)
libsoundplane.initialize()

while 1:
    
    time.sleep(1.0)

