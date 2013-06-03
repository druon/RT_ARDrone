#ifndef NAVDATA_STREAM_H
#define NAVDATA_STREAM_H


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

#include <netinet/in.h>
#include <pthread.h>


#define NAVDATA_PORT	5554


typedef struct {

	int 			socket ;
	struct sockaddr_in 	addr  ;		
	struct hostent* 	host;

	pthread_t 		thread ;			




} NavDataStream ;

NavDataStream* NavDataStream_new( const char* ip_addr ) ;
void NavDataStream_free ( NavDataStream* ) ;

void NavDataStream_connect( NavDataStream* stream ) ;

#endif






