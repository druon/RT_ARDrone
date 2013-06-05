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
#include <netdb.h>

#include <RT_ARDrone/video_stream.h>
#include <RT_ARDrone/video_pave.h>

void* video_threadfct( void* data ) ;

VideoStream* VideoStream_new( const char* ip_addr ) {

	VideoStream* tmp ;
	tmp = ( VideoStream* ) malloc ( sizeof( VideoStream ) ) ;

	tmp->host = ( struct hostent* ) gethostbyname((char*) ip_addr ) ;
	tmp->socket = socket( AF_INET, SOCK_STREAM, 0 );
	
	if ( tmp->socket == -1) {
		fprintf(stderr, "[%s] Error creating socket for Video stream \n", __FILE__ ) ;
		exit( EXIT_FAILURE) ;
	}

	tmp->addr.sin_family = AF_INET;
	tmp->addr.sin_port = htons( VIDEO_PORT );
	tmp->addr.sin_addr = *( (struct in_addr *) tmp->host->h_addr );	// Sets drone address
	
	bzero( &(tmp->addr.sin_zero), 8 ) ;	// zeros the array

	pthread_mutex_init( &(tmp->mutex), NULL ) ;
	
	return tmp ;
}


void VideoStream_free ( VideoStream* stream ) {

	free ( stream ) ;

}


void VideoStream_connect( VideoStream* stream ) {

	printf("[ RT_ARDrone ] Starting Video thread ... \n" ) ;
	stream->thread = pthread_create( &(stream->thread), NULL, video_threadfct, (void*) stream ) ;
}


void* video_threadfct( void* data ) {

	int i ;
	int ret ;
	uint8_t buffer[50000] ;
	Pave* header ;
	VideoStream* stream ;

	stream = ( VideoStream* ) data ;

	if ( connect( stream->socket,( struct sockaddr *) &(stream->addr), sizeof( struct sockaddr) ) != 0 ) {
    		fprintf(stderr, "[ %s ] ERROR : cannot connect to video stream \n", __FILE__ );
        	exit( EXIT_FAILURE ) ;	
	}

		ret = recv( stream->socket, buffer, sizeof(buffer), 0) ;

		header = buffer ;

		printf("Received %d bytes --- %c %c %c %c \n", ret, header->signature[0], 
								    header->signature[1],
								    header->signature[2],
								    header->signature[3] ) ; 
		
		unsigned char* charheader ;
		charheader = &(header->header_size) ;

		int headersize = shift_byte( charheader[0], charheader[1], 0, 0 ) ; 
		
		unsigned char *charload ;
		charload = &(header->payload_size ) ;

		int payload = shift_byte( charload[0], charload[1], charload[2], charload[3] ) ;

		printf(" Payload: %d \n", payload ) ;
		printf(" HeaderSize : %d \n", headersize ) ;

			if ( ret < 68 + payload + 4 ) {


			printf(" --- %c %c %c %c \n", ret, buffer[ 68 + payload + 1 ], 
							   buffer[ 68 + payload + 2 ],
							   buffer[ 68 + payload + 3 ],
							   buffer[ 68 + payload + 4 ] ) ; 
			}

}
