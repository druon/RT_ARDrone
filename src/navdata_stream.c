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

	unsigned int length ;
	int n ;
	NavDataStream* tmp ;
	
	tmp = (NavDataStream*) malloc ( sizeof( NavDataStream ) ) ;

	tmp->recv_sock = socket(AF_INET, SOCK_DGRAM, 0);
	tmp->send_sock = socket(AF_INET, SOCK_DGRAM, 0);

	if ( tmp->recv_sock == -1) {
		fprintf(stderr, "[%s] Error creating Navdata receiving socket\n", __FILE__ ) ;
		exit( EXIT_FAILURE ) ;
	}

	if ( tmp->send_sock == -1) {
		fprintf(stderr, "[%s] Error creating Navdata sending socket\n", __FILE__ ) ;
		perror("socket");
		exit(1);
	}

	tmp->addr.sin_family = AF_INET;
	tmp->addr.sin_port = htons( NAVDATA_PORT );	
	tmp->addr.sin_addr.s_addr = INADDR_ANY;	
	bzero(&(tmp->addr.sin_zero),8);		

	tmp->send_addr.sin_family = AF_INET;	
	tmp->send_addr.sin_port = htons( NAVDATA_PORT ) ;
	tmp->send_host= (struct hostent *) gethostbyname((char *) ip_addr ); 
	tmp->send_addr.sin_addr = *((struct in_addr *) tmp->send_host->h_addr); 
	bzero(&(tmp->send_addr.sin_zero),8);	

	length = sizeof(struct sockaddr_in);
	n = bind( tmp->recv_sock, (struct sockaddr *) &tmp->addr, length);

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

	length = sizeof( struct sockaddr_in ) ;

	while(1) {
		
		sendto(	stream->send_sock, 
			buf, 
			strlen(buf), 
			0, 
			(struct sockaddr *) &(stream->send_addr), 
			sizeof(struct sockaddr_in) );

		// Zeros out the array
	
		bzero(&(nav_data), 2048);
	
		// Receives data from drone.  Hangs until received
	
		ret = recvfrom( stream->recv_sock, 
			      	nav_data, 2048, 0,
			        (struct sockaddr *) &(stream->recv_addr), &length );

		// Received navdata ... 

		printf("%d bytes received\n", ret);

		// Wait for a while

		usleep(25000) ;

	}

}



