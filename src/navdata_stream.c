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

#include "shift_byte.h"

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

	pthread_mutex_init( &(tmp->mutex), NULL ) ;
	
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
	uint32_t option_id ;
	uint32_t option_size ;

	uint32_t temp ;


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

		// printf ( "%d --- header = %d, state = %d, sequence = %d, vision = %d\n", ret, header, state, sequence, vision ) ;

		int option_index = 16;		// Options start at byte 16

		do {
			option_id = nav_data[option_index];
			option_id |=  nav_data[option_index+1] << 8;

			option_size = nav_data[option_index+2];
			option_size |=  nav_data[option_index+3] << 8;

//			printf("Have option tag: %x with size: %d at index: %d \n", option_id, option_size, option_index);

			if ( (option_id ==0) && (option_size>0) && (option_index==16) ) {

				uint32_t state   = shift_byte( nav_data[option_index+4], nav_data[option_index+5], nav_data[option_index+6], nav_data[option_index+7]);
				uint32_t bat     = shift_byte( nav_data[option_index+8], nav_data[option_index+9], nav_data[option_index+10], nav_data[option_index+11]);
				
				temp = shift_byte( nav_data[option_index+12], nav_data[option_index+13], nav_data[option_index+14], nav_data[option_index+15]);
				float theta = ( *(float*)(&temp) ) / 1000 ;

				temp = shift_byte( nav_data[option_index+16], nav_data[option_index+17], nav_data[option_index+18], nav_data[option_index+19]);
				float phi = ( *(float*)(&temp) ) / 1000 ;

				temp = shift_byte( nav_data[option_index+20], nav_data[option_index+21], nav_data[option_index+22], nav_data[option_index+23]);
				float psi = ( *(float*)(&temp) ) / 1000 ;
				
				int32_t altitude = shift_byte( nav_data[option_index+24], nav_data[option_index+25], nav_data[option_index+26], nav_data[option_index+27]);
				
				temp = shift_byte( nav_data[option_index+28], nav_data[option_index+29], nav_data[option_index+30], nav_data[option_index+31]);
				float vx = ( *(float*)(&temp) ) / 1000 ;
				
				temp = shift_byte( nav_data[option_index+32], nav_data[option_index+33], nav_data[option_index+34], nav_data[option_index+35]);
				float vy = ( *(float*)(&temp) ) / 1000  ;


				temp = shift_byte( nav_data[option_index+36], nav_data[option_index+37], nav_data[option_index+38], nav_data[option_index+39]);
				float vz = ( *(float*)(&temp) ) / 1000 ;

//				printf("%d %d -- %f %f %f - %f %f %f --- %d \n", state, bat, theta, phi, psi, vx, vy, vz, altitude ) ;

				// int32_t     num_frames               Octets 40->43 Not used
				// matrix33_t  detection_camera_rot;    Octets 43->47 Deprecated !!!
				// vector31_t  detection_camera_trans;  Octets 48->51 Deprecated !!!
				// uint32_t    detection_tag_index;     Octets 52->55 Deprecated !!!
  				// uint32_t    detection_camera_type;   Octets 56->59 Deprecated !!!
  				// matrix33_t  drone_camera_rot;        Octets 60->63 Deprecated !!!
				// vector31_t  drone_camera_trans;      Octets 63->66 Deprecated !!!

				
				pthread_mutex_lock( &(stream->mutex)  );

				stream->current_navdata.altitude = altitude ;
				stream->current_navdata.bat = bat ;
				stream->current_navdata.phi = phi ;
				stream->current_navdata.psi = psi ;
				stream->current_navdata.state = state ;
				stream->current_navdata.theta = theta ;
				stream->current_navdata.vx = vx ;
				stream->current_navdata.vy = vy ;
				stream->current_navdata.vz = vz ;

				pthread_mutex_unlock( &(stream->mutex)  );

			}			

			option_index += 4 + option_size ; 

		} while ( option_id != 0xffff ) ;

		
		// Wait for a while

		usleep(25000) ;

	}

}

void NavDataStream_get_navdata( NavDataStream* stream, NavData* data ) {

	pthread_mutex_lock( &(stream->mutex)  );

	data->altitude = stream->current_navdata.altitude ;
	data->bat = stream->current_navdata.bat ;
	data->phi = stream->current_navdata.phi ;
	data->psi = stream->current_navdata.psi ;
	data->state = stream->current_navdata.state ;
	data->theta = stream->current_navdata.theta ;
	data->vx = stream->current_navdata.vx ;
	data->vy = stream->current_navdata.vy ;
	data->vz = stream->current_navdata.vz ;

	pthread_mutex_unlock( &(stream->mutex)  );

}


