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

void* video_threadfct( void* data ) ;
void  process_frame( VideoStream*, uint8_t* data, int data_size ) ;

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

	// Init H264 Decoder
	
	printf("[ RT_ARDrone ] Initializing H264 decoder ... \n" ) ;
	
	avcodec_register_all() ;

	stream->pCodecH264 = avcodec_find_decoder( CODEC_ID_H264 ) ;

	if ( stream->pCodecH264 == NULL ) {
		printf("ERROR\n") ;
		exit(0) ;
	}


	stream->pCodecCtxH264 = avcodec_alloc_context3( stream->pCodecH264 ) ;

	if ( stream->pCodecCtxH264 == NULL ) {
		printf("ERROR\n") ;
		exit(0) ;
	}

	stream->pCodecCtxH264->pix_fmt = PIX_FMT_YUV420P ;
	stream->pCodecCtxH264->skip_frame = AVDISCARD_DEFAULT ;
	stream->pCodecCtxH264->error_concealment = FF_EC_GUESS_MVS | FF_EC_DEBLOCK ;
	stream->pCodecCtxH264->skip_loop_filter = AVDISCARD_DEFAULT ;
	stream->pCodecCtxH264->workaround_bugs = FF_BUG_AUTODETECT ;
	stream->pCodecCtxH264->coder_type = AVMEDIA_TYPE_VIDEO ;
	stream->pCodecCtxH264->codec_id = CODEC_ID_H264 ;
	stream->pCodecCtxH264->skip_idct = AVDISCARD_DEFAULT ;

	av_dict_set(&(stream->opts), "b", "2.5M", 0);

	if ( avcodec_open2( stream->pCodecCtxH264, stream->pCodecH264, &(stream->opts) ) < 0 ) {
		printf("ERROR opening H264 codec \n") ;
		exit(0) ;
	}

	stream->picture = avcodec_alloc_frame() ;

	stream->RGBPicture = avcodec_alloc_frame() ;

	av_init_packet(&stream->packet) ;
	
	stream->bool_new_picture = 0 ;
	
	// Start thread

	printf("[ RT_ARDrone ] - Starting Video thread ... \n" ) ;
	
	stream->thread = pthread_create( &(stream->thread), NULL, video_threadfct, (void*) stream ) ;


}


void* video_threadfct( void* data ) {

	int size ;
	int ret ;
	
	uint8_t buffer[50000] ;

	VideoStream* stream ;

	stream = ( VideoStream* ) data ;

	
	if ( connect( stream->socket,( struct sockaddr *) &(stream->addr), sizeof( struct sockaddr) ) != 0 ) {
    		fprintf(stderr, "[ %s ] ERROR : cannot connect to video stream \n", __FILE__ );
        	exit( EXIT_FAILURE ) ;	
	}

	size = 0 ;

	while (1) {

		// First, receive PaVE Header

		size += recv( stream->socket, &(buffer[size]), 50000-size, 0) ;

		if ( size > 68 ) {		

			// Find Payload size

			int header_size = shift_byte( buffer[6], buffer[7], 0, 0 ) ; 
	
			if ( header_size != 68 ) {
				fprintf(stderr,"ERROR : wrong PAVE HEADER size \n" ) ;
				exit ( EXIT_FAILURE ) ;
			}

			int payload_size = shift_byte( buffer[8], buffer[9],buffer[10] , buffer[11] ) ;

			// Then, the rest of the frame
			

			if ( size > header_size + payload_size ) {

				stream->pCodecCtxH264->width  = shift_byte( buffer[12], buffer[13], 0, 0 ) ;
				stream->pCodecCtxH264->height = shift_byte( buffer[14], buffer[15],0, 0 ) ;

				printf(" processing %d %d \n", stream->pCodecCtxH264->width, stream->pCodecCtxH264->height ) ;

				process_frame( stream, &(buffer[header_size]), payload_size ) ;
				
				size -= header_size + payload_size ;
				memmove( buffer, &(buffer[header_size+payload_size]), size ) ;
			
			}

		}
	
	}
}


void  process_frame( VideoStream* stream,  uint8_t* data, int data_size ) {

	int got_picture ;

	stream->packet.data = data ;
	stream->packet.size = data_size ;
	
	pthread_mutex_lock( &(stream->mutex) ) ;
	
		avcodec_decode_video2( stream->pCodecCtxH264, stream->picture, &got_picture, &(stream->packet) ) ;
	
		stream->bool_new_picture = 1 ;	

	pthread_mutex_unlock( &(stream->mutex) ) ;


}



void VideoStream_getRGB24Image( VideoStream* stream, RGB24Image* image ) {


	pthread_mutex_lock( &(stream->mutex) ) ;

	if ( stream->bool_new_picture == 1 ) {

		int numBytes=avpicture_get_size( PIX_FMT_RGB24, stream->pCodecCtxH264->width, stream->pCodecCtxH264->height ) ;

		if ( ( image->width != stream->pCodecCtxH264->width ) || (image->height != stream->pCodecCtxH264->height ) )
			RGB24Image_resize( image, stream->pCodecCtxH264->width, stream->pCodecCtxH264->height ) ;

	 
		avpicture_fill((AVPicture *) stream->RGBPicture, image->pixels, PIX_FMT_RGB24, stream->pCodecCtxH264->width, stream->pCodecCtxH264->height);
  

		stream->sws_ctx = sws_getContext ( stream->pCodecCtxH264->width,
						   stream->pCodecCtxH264->height,
						   stream->pCodecCtxH264->pix_fmt,
						   stream->pCodecCtxH264->width,
						   stream->pCodecCtxH264->height,
						   PIX_FMT_RGB24,
						   SWS_BILINEAR,
						   NULL, NULL, NULL );
 

        	sws_scale ( stream->sws_ctx,
	        	    (uint8_t const * const *) stream->picture->data,
	        	    	stream->picture->linesize,
        	    	    	0,
            		    	stream->pCodecCtxH264->height,
            	    		stream->RGBPicture->data,
	   			stream->RGBPicture->linesize );

		stream->bool_new_picture = 0 ;
	
	} ;

	pthread_mutex_unlock( &(stream->mutex) ) ;

}



void VideoStream_getYUV420Image( VideoStream*, YUV420Image* ) ;


