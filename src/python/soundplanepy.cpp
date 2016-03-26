

#include "TouchTrackerListener.h"

#include <boost/python.hpp>

BOOST_PYTHON_MODULE(libsoundplane)
{
    using namespace boost::python;
    
	class_<TouchTrackerListener>("TouchTrackerListener");
		
}


