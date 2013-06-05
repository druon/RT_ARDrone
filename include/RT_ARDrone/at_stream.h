#ifndef AT_STREAM_H
#define AT_STREAM_H


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

#include <pthread.h>
#include <stdint.h>
#include <netinet/in.h>

#define AT_PORT	5556

typedef struct {

	int 			socket ;
	struct hostent* 	host ;
	struct sockaddr_in 	addr;

	int			seq ;		// Sequence number for AT commands

	pthread_t 		thread ;			
	pthread_mutex_t		mutex ;		// This mutex protects the access to the
						//  at_commmand and at_argument fields

	char at_command[80] ;			// Currently executed AT command
	char at_argument[80] ;		

} ATStream ;


ATStream* ATStream_new( const char* ip_addr ) ;
void      ATStream_free( ATStream* ) ;

void ATStream_connect( ATStream* ) ; 


void ATStream_trim( ATStream* stream ) ;
void ATStream_takeoff( ATStream* stream ) ;
void ATStream_land( ATStream* stream ) ;
void ATStream_move( ATStream* stream, uint32_t flag, float roll, float pitch, float yaw, float gaz ) ; 

void ATStream_reset_defaults( ATStream* stream ) ;

#endif






