# SoundplanePython: a python binding for the Madrona Soundplane
# Copyright (c) 2016 Thomas Scott Wilson http://nw2s.net
# Distributed under the MIT license

# cimport libsoundplanex

# A place to store the driver
#cdef SoundplaneDriver* driver


# cdef class TouchTrackerListener:
#
# 	cdef libsoundplane.TouchTrackerListener *wrapped
#
# 	def __cinit__(self):
#
# 		# Try to create an instance of the wrapped class.
# 		self.wrapped = new libsoundplane.TouchTrackerListener()
#
# 		# If we couldn't create one, we're out of memory.
# 		if self.wrapped is NULL:
#
# 			raise MemoryError()


# cdef init():
#
# 	driver = new SoundplaneDriver()


cdef extern from "soundplanepy.h": 
	
	cdef cppclass TouchTrackerListener:
		
		TouchTrackerListener()



cdef TouchTrackerListener* listener

def init():
	
	listener = new TouchTrackerListener()
	
	if listener is NULL:
		
		raise MemoryError()

		


cdef __cinit__():
	
	pass
	

def initlibsoundplane():
	
	pass
