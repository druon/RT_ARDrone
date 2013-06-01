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
#include <strings.h>
#include <sys/socket.h>
#include <netdb.h>

#include <RT_ARDrone/at_stream.h>


ATStream* ATStream_new ( const char* ip_addr ) {

	ATStream* tmp ;
	tmp = (ATStream*) malloc ( sizeof (ATStream) ) ;
	
	tmp->host= (struct hostent *) gethostbyname((char *) ip_addr );
	tmp->socket = socket(AF_INET, SOCK_DGRAM, 0);

	if ( tmp->socket == -1) {
		fprintf(stderr, "[%s] Error creating socket for AT commands\n", __FILE__ ) ;
		exit( EXIT_FAILURE) ;
	}

	tmp->addr.sin_family = AF_INET;
	tmp->addr.sin_port = htons( AT_PORT );
	tmp->addr.sin_addr = *( (struct in_addr *) tmp->host->h_addr );	// Sets drone address
	
	bzero( &(tmp->addr.sin_zero), 8 ) ;	// zeros the array

	tmp->seq = 1 ;

	return tmp ;
}



void ATStream_free( ATStream* stream ) {

	free( stream ) ;

}

void ATStream_connect() {

	// Set max altitude

	// sprintf(_drone.send_data, "AT*CONFIG=%d,\"control:altitude_max\",\"%d\"\r", _drone.seq, 2500); //set max alt @ 2.5m
	// sendto(_drone.at_sock, _drone.send_data, strlen(_drone.send_data), 0, (struct sockaddr *)&_drone.at_addr, sizeof(struct sockaddr)); 
	// _drone.seq++;

	// Set max angle

	// sprintf(_drone.send_data, "AT*CONFIG=%d,\"control:euler_angle_max\",\"%f\"\r", _drone.seq, 0.52f);// set max rotation velocity
	// sendto(_drone.at_sock, _drone.send_data, strlen(_drone.send_data), 0, (struct sockaddr *)&_drone.at_addr, sizeof(struct sockaddr)); 
	// _drone.seq++;

}


