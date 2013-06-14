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

#include <RT_ARDrone/at_stream.h>


void* at_threadfct( void* data ) ;


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

	pthread_mutex_init( &(tmp->mutex), NULL ) ;

	tmp->seq = 1 ;

	return tmp ;
}



void ATStream_free( ATStream* stream ) {

	free( stream ) ;

}

void ATStream_connect( ATStream* stream ) {

	unsigned char msg[1024] ;

	// Set max altitude
	
	//printf("[ AT ] Setting Max altitude ... \n" ) ;
	
	//sprintf( msg, "AT*CONFIG=%d,\"control:altitude_max\",\"%d\"\r", stream->seq, 2500 ); //set max alt @ 2.5m
	//sendto( stream->socket, msg, strlen(msg), 0, (struct sockaddr *) &(stream->addr), sizeof(struct sockaddr)); 
	//stream->seq++;

	// Set max angle

	//printf("[ AT ] Setting Max Euler angle ...\n") ; 
	//sprintf( msg, "AT*CONFIG=%d,\"control:euler_angle_max\",\"%f\"\r", stream->seq, 0.52f);// set max rotation velocity
	//sendto( stream->socket, msg, strlen(msg), 0, (struct sockaddr *) &(stream->addr), sizeof(struct sockaddr)); 
	//stream->seq++;

	// Requesting NavData
	
	printf("[ AT ] Requesting Navdata ... \n" ) ;
	sprintf( msg, "AT*CONFIG=%d,\"general:navdata_demo\",\"TRUE\"\r", stream->seq ) ;
	sendto( stream->socket, msg, strlen(msg), 0, (struct sockaddr *) &(stream->addr), sizeof(struct sockaddr));
	stream->seq++ ;

	// Starting AT thread

	printf("[ RT_ARDrone ] Starting AT thread ... \n" ) ;

	sprintf( stream->at_command, "AT*PCMD="  );
	sprintf( stream->at_argument,"0,0,0,0,0" );

	stream->thread = pthread_create( &(stream->thread), NULL, at_threadfct, (void*) stream ) ;

}


void* at_threadfct( void* data ) {

	int ret ;
	ATStream* stream ;
	unsigned char msg[1024] ;



	stream = (ATStream*) data ;

	while(1) {


		pthread_mutex_lock( &(stream->mutex)  );


			// Send a watchdog reset

//			sprintf( msg, "AT*COMWDG=%d\r", stream->seq); 
//			sendto( stream->socket, msg, strlen(msg), 0, (struct sockaddr *)&(stream->addr), sizeof(struct sockaddr)); 
//			stream->seq++;	// Increment the command seq number	

			sprintf( msg, "%s%d,%s\r", stream->at_command, stream->seq, stream->at_argument );	
			sendto( stream->socket, msg, strlen(msg), 0, (struct sockaddr *) &(stream->addr), sizeof(struct sockaddr));
			stream->seq++ ;

		pthread_mutex_unlock( &(stream->mutex) ) ;


		// Wait before next loop

		usleep( 25000 ) ;
	}

}


void ATStream_trim( ATStream* stream ) {

	char command[160] ;

	printf("[ AT ] Flat Trimming\n");			
	
	pthread_mutex_lock( &(stream->mutex) ) ;

	sprintf( command, "AT*FTRIM=%d,\r", stream->seq ) ;
	
	sendto( stream->socket, 
		command, strlen(command), 0, 
		(struct sockaddr *) &(stream->addr), sizeof(struct sockaddr) );
	
	stream->seq++ ;

	usleep(50000);

	pthread_mutex_unlock( &(stream->mutex) ) ;

}


void ATStream_takeoff( ATStream* stream ) {

	char command[160] ;

	printf("[ AT ] Taking Off ... \n");			
	
	pthread_mutex_lock( &(stream->mutex) ) ;

	sprintf( command, "AT*REF=%d,290718208\r", stream->seq ) ;
	
	sendto( stream->socket, 
		command, strlen(command), 0, 
		(struct sockaddr *) &(stream->addr), sizeof(struct sockaddr) );
	
	stream->seq++ ;

	pthread_mutex_unlock( &(stream->mutex) ) ;


}


void ATStream_land( ATStream* stream ) {

	char command[160] ;

	printf("[ AT ] Landing ... \n");			
	
	pthread_mutex_lock( &(stream->mutex) ) ;

	sprintf( command, "AT*REF=%d,290717696\r", stream->seq ) ;
	
	sendto( stream->socket, 
		command, strlen(command), 0, 
		(struct sockaddr *) &(stream->addr), sizeof(struct sockaddr) );
	
	stream->seq++ ;

	pthread_mutex_unlock( &(stream->mutex) ) ;


}

void ATStream_reset_defaults( ATStream* stream ) {

	char command[160] ;

	printf("[ AT ] Resetting defaults ... \n");			
	
	pthread_mutex_lock( &(stream->mutex) ) ;
	sprintf( command, "AT*REF=%d,290717952\r", stream->seq ) ;
	
	sendto( stream->socket, 
		command, strlen(command), 0, 
		(struct sockaddr *) &(stream->addr), sizeof(struct sockaddr) );
	
	stream->seq++ ;

	pthread_mutex_unlock( &(stream->mutex) ) ;



}

void ATStream_move( ATStream* stream, uint32_t flag, float roll, float pitch, float yaw, float gaz ) {

	pthread_mutex_lock( &(stream->mutex) ) ;

	sprintf( stream->at_command,"AT*PCMD=" ) ;
	sprintf( stream->at_argument,"%d,%d,%d,%d,%d", flag, 
							*(int*)(&roll),
							*(int*)(&pitch),
							*(int*)(&gaz),
							*(int*)(&yaw) ) ;

	pthread_mutex_unlock( &(stream->mutex) ) ;

}


void ATStream_zap_camera( ATStream* stream, int numcam ) {

	char command[160] ;

	printf("[ AT ] Zapping ... \n");			
	
	pthread_mutex_lock( &(stream->mutex) ) ;

	sprintf( command, "AT*CONFIG=%d,\"video:video_channel\",\"%d\"\r", stream->seq, numcam ) ;
	
	sendto( stream->socket, 
		command, strlen(command), 0, 
		(struct sockaddr *) &(stream->addr), sizeof(struct sockaddr) );

	usleep( 20000 ) ;

	stream->seq++ ;

	pthread_mutex_unlock( &(stream->mutex) ) ;


}























