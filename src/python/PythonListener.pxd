
# SoundplanePython: a python binding for the Madrona Soundplane
# Copyright (c) 2016 Thomas Scott Wilson http://nw2s.net
# Distributed under the MIT license


cdef extern from "PythonListener.h": 
	
	cdef cppclass TouchTrackerListener:
		
		TouchTrackerListener()
		
	