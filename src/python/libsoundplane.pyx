# SoundplanePython: a python binding for the Madrona Soundplane
# Copyright (c) 2016 Thomas Scott Wilson http://nw2s.net
# Distributed under the MIT license

cimport PythonListener

cdef class TouchTrackerListener:
	
	cdef PythonListener.TouchTrackerListener *wrapped
	
	def __cinit__(self):
		
		# Try to create an instance of the wrapped class.
		self.wrapped = new PythonListener.TouchTrackerListener()

		# If we couldn't create one, we're out of memory.
		if self.wrapped is NULL:

			raise MemoryError()	
	


cdef __cinit__():
	
	pass
	

def initlibsoundplane():
	
	pass
