#ifndef RT_ARDRONE_H
#define RT_ARDRONE_H


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

#include <RT_ARDrone/navdata.h>

#include <RT_ARDrone/control_stream.h>
#include <RT_ARDrone/at_stream.h>
#include <RT_ARDrone/navdata_stream.h>
#include <RT_ARDrone/video_stream.h>

typedef struct {

	ATStream*	at_stream   ;
	NavDataStream*	navdata_stream ;
	VideoStream*	video_stream ;
	ControlStream*	ctrl_stream ;	

} ARDrone ;


ARDrone* ARDrone_new( const char* ip_addr ) ;
void ARDrone_free ( ARDrone* drone ) ;

void ARDrone_connect( ARDrone* drone ) ;
void ARDrone_disconnect( ARDrone* drone ) ;

void ARDrone_trim( ARDrone* drone ) ;
void ARDrone_takeoff( ARDrone* drone ) ;
void ARDrone_land( ARDrone* drone ) ;
void ARDrone_move( ARDrone*, float roll, float pitch, float yaw, float gaz ) ; 

void ARDrone_reset_defaults( ARDrone* ) ;


void ARDrone_get_navdata( ARDrone*, NavData* ) ;

#endif
