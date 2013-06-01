// ****************************************************************************
// ***
// ***
// *** RT_Ardrone - A basic library for interfacing the AR.Drone quadricopter
// ***
// *** Copyright (C)   Sebastien Druon ( sebastien.druon@univ-montp2.fr )
// ***
// *** This program is free software: you can redistribute it and/or modify
// *** it under the terms of the GNU General Public License as published by
// *** the Free Software Foundation, either version 3 of the License, or
// *** (at your option) any later version.
// ***
// *** This program is distributed in the hope that it will be useful,
// *** but WITHOUT ANY WARRANTY; without even the implied warranty of
// *** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// *** GNU General Public License for more details.
// *** 
// *** You should have received a copy of the GNU General Public License
// *** along with this program.  If not, see <http://www.gnu.org/licenses/>.
// ***
// ***
// ***
// ****************************************************************************

#include <stdio.h>
#include <stdlib.h>

#include <RT_ARDrone/RT_ARDrone.h>


ARDrone* ARDrone_new( const char* ip_addr ) {

	ARDrone* tmp ;

	tmp = (ARDrone*) malloc ( sizeof( ARDrone ) ) ;

	tmp->at_stream = ATStream_new( ip_addr ) ;
	tmp->ctrl_stream = ControlStream_new ( ip_addr ) ; 
	tmp->navdata_stream = NavDataStream_new( ip_addr ) ;
	tmp->video_stream = VideoStream_new( ip_addr ) ;
	
	return tmp ;

}


void ARDrone_free( ARDrone* drone ) {

	ATStream_free( drone->at_stream ) ;
	ControlStream_free( drone->ctrl_stream ) ;
	NavDataStream_free( drone->navdata_stream ) ;
	VideoStream_free( drone->video_stream ) ;

	free( drone ) ;
}

void ARDrone_connect( ARDrone* drone ) {

	NavDataStream_connect( drone->navdata_stream ) ;
	ATStream_connect( drone->at_stream ) ;	

}

