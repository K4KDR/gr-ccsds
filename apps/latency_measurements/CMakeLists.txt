# Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

include(GrPython)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON) 
set(Boost_USE_STATIC_RUNTIME OFF)

find_package( Boost REQUIRED COMPONENTS system thread )

find_package( TICP )

if ( LIBTICP_FOUND )

	MESSAGE( STATUS "TICP library found, building latency testing applications" )
	
	add_executable (ccsds_latency_sender ccsds_latency_sender.cpp)
	target_link_libraries(ccsds_latency_sender ticp ${Boost_SYSTEM_LIBRARY} ${Boost_THREAD_LIBRARY} pthread)
	
	add_executable (ccsds_latency_receiver ccsds_latency_receiver.cpp)
	target_link_libraries(ccsds_latency_receiver ticp ${Boost_SYSTEM_LIBRARY} ${Boost_THREAD_LIBRARY} pthread)
	
	install(
		TARGETS
			ccsds_latency_sender
			ccsds_latency_receiver
		DESTINATION bin
	)

else()
	MESSAGE( STATUS "optional TICP library NOT found, NOT building (optional) latency testing applications" )
endif()
