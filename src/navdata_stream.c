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
#include <sys/types.h>
#include <netdb.h>

#include <RT_ARDrone/navdata_stream.h>


void* navdata_threadfct( void* data ) ;
int32_t shift_byte(int32_t lsb, int32_t mlsb, int32_t mmsb, int32_t msb) ;


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
		
	// Send one packet to enter bootstrap mode
	
	sendto(	stream->socket, 
		buf, strlen(buf), 
		0, 
		(struct sockaddr *) &(stream->addr), sizeof(stream->addr) );

	while(1) {
		
		// Zeros out the array
	
		bzero(&(nav_data), 2048);
	
		// Receives data from drone.  Hangs until received

		int len = sizeof( stream->addr ) ;

		ret = recvfrom( stream->socket, 
			      	nav_data, 2048, 
				0,
			        (struct sockaddr *) &(stream->addr), &len  );

		// Received navdata ... 

		// Stores the initial information from the drone
		
		uint32_t header   = shift_byte( nav_data[0], nav_data[1],  nav_data[2], nav_data[3]);
		uint32_t state    = shift_byte( nav_data[4], nav_data[5],  nav_data[6], nav_data[7]);
		uint32_t sequence = shift_byte( nav_data[8], nav_data[9],  nav_data[10], nav_data[11]);
		uint32_t vision	  = shift_byte( nav_data[12],nav_data[13], nav_data[14], nav_data[15]);

		printf ( "%d --- header = %d, state = %d, sequence = %d, vision = %d\n", ret, header, state, sequence, vision ) ;

		int option_index = 16;		// Options start at byte 16

		while ( option_index+8 < ret )
		{
			int32_t option_tag = nav_data[option_index];
			option_index++;
			option_tag |= (int16_t) nav_data[option_index] << 8;
			option_index++;

			int32_t option_size = nav_data[option_index];
			option_index++;
			option_size |= (int16_t) nav_data[option_index] << 8;
			option_index++;

			printf("Have option tag: %d with size: %d at index: %d \n", option_tag, option_size, option_index-4);

			if ( ( option_tag == 0 ) && ( option_size > 0 ) ) { 	// this is a navdemo option 



			} else {

				option_index += (option_size + 1);		// Move on to the next option block
			}
			
		}


		// Wait for a while

		usleep(25000) ;

	}

}

int32_t shift_byte(int32_t lsb, int32_t mlsb, int32_t mmsb, int32_t msb)
{
	int32_t tmp = 0;

	tmp = lsb;
	tmp |= mlsb << 8;
	tmp |= mmsb << 16;
	tmp |= msb << 24;

	return tmp ;
}
