
# Soundplaned sample python client
# Copyright (c) 2016 Thomas Scott Wilson http://nw2s.net
# Distributed under the MIT license

import socket
import sys
import os
import time
import struct

from ctypes import *


SERVER_ADDRESS = '/tmp/soundplane'
TOUCH_SIZE = 28

# Define a class that can be overlaid onto the struct we read
class Touch(Structure):
    
    _fields_ = [
        
        ("index", c_int),
        ("x", c_float),
        ("y", c_float),
        ("z", c_float),
        ("dx", c_float),
        ("age", c_float),
        ("dt", c_float)
    ]

def initialize_socket():
    
    # Create a UDS socket
    sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)

    # Make sure the socket is there
    # TODO: 
    
    # If it's not, keep looking, hang up after a while
    # TODO:

    # Connect the socket to the port where the server is listening
    server_address = '/tmp/soundplane'

    try:
        
        sock.connect(SERVER_ADDRESS)
    
    except socket.error, msg:
    
        print >>sys.stderr, msg

        exit(-1)
        
    return sock

    
# Read a frame at a time. This happens a lot, so be nice!    
def read_frame(sock):    

    # Create a mutable buffer to store the struct
    buff = create_string_buffer(TOUCH_SIZE)
    
    # Read the struct
    sock.recv_into(buff, 28)

    # Return to the main program
    return Touch.from_buffer(buff)



# MAIN    
# MAIN    
# MAIN    

sock = initialize_socket()
start = time.time()
touches = []

while 1:
    
    touches.append(None)
    touches.append(None)
    touches.append(None)
    touches.append(None)
    
    touch = read_frame(sock)
    
    touches[touch.index] = touch
    
    if (time.time() - start) > 1.0:
        
        if touches[0] is not None: print touches[0].index, touches[0].x, touches[0].y, touches[0].z, touches[0].dx, touches[0].age, touches[0].dt
        if touches[1] is not None: print touches[1].index, touches[1].x, touches[1].y, touches[1].z, touches[1].dx, touches[1].age, touches[1].dt
        if touches[2] is not None: print touches[2].index, touches[2].x, touches[2].y, touches[2].z, touches[2].dx, touches[2].age, touches[2].dt
        if touches[3] is not None: print touches[3].index, touches[3].x, touches[3].y, touches[3].z, touches[3].dx, touches[3].age, touches[3].dt
    
        start = time.time()
    


