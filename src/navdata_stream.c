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
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>

#include <RT_ARDrone/navdata_stream.h>


void* navdata_threadfct( void* data ) ;


NavDataStream* NavDataStream_new( const char* ip_addr ) {

	int ret ;
	NavDataStream* tmp ;
	
	tmp = (NavDataStream*) malloc ( sizeof( NavDataStream ) ) ;

	tmp->socket = socket(AF_INET, SOCK_DGRAM, 0);

	if ( tmp->socket == -1) {
		fprintf(stderr, "[%s] Error creating Navdata socket\n", __FILE__ ) ;
		exit( EXIT_FAILURE ) ;
	}

	tmp->host = (struct hostent *) gethostbyname((char *) ip_addr ); 
	
	tmp->addr.sin_family = AF_INET;
	tmp->addr.sin_addr.s_addr = htonl( INADDR_ANY ) ;	
	tmp->addr.sin_port = htons( NAVDATA_PORT );	
	
	bzero(&(tmp->addr.sin_zero),8);	

	ret = bind( tmp->socket, (struct sockaddr *) &(tmp->addr), sizeof(tmp->addr) );

	return tmp ;
}


void NavDataStream_free ( NavDataStream* stream ) {

	free( stream );
}

void NavDataStream_connect( NavDataStream* stream ) {

	printf("[ RT_ARDrone ] Starting NavData thread ... \n" ) ;

	stream->thread = pthread_create( &(stream->thread), NULL, navdata_threadfct, (void*) stream ) ;

}


void* navdata_threadfct( void* data ) {

	int ret ;
	int length ;
	NavDataStream* stream ;
	
	stream = (NavDataStream*) data ;

	char buf[] = {0x01, 0x00, 0x00, 0x00};
	unsigned char nav_data[2048];

	stream->addr.sin_addr = *((struct in_addr *) stream->host->h_addr); 
		
	sendto(	stream->socket, 
		buf, strlen(buf), 
		0, 
		(struct sockaddr *) &(stream->addr), sizeof(stream->addr) );


	while(1) {
	
		// Send a message to trigger navdata

		sendto(	stream->socket, 
			buf, strlen(buf), 
			0, 
			(struct sockaddr *) &(stream->addr), sizeof(stream->addr) );


		// Zeros out the array
	
		bzero(&(nav_data), 2048);
	
		// Receives data from drone.  Hangs until received

		int len = sizeof( stream->addr ) ;

		ret = recvfrom( stream->socket, 
			      	nav_data, 2048, 
				0,
			        (struct sockaddr *) &(stream->addr), &len  );

		// Received navdata ... 

		printf("%d bytes received\n", ret);

		// Wait for a while

		usleep(25000) ;

	}

}



