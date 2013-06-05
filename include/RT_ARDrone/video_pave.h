#ifndef VIDEO_PAVE_H
#define VIDEO_PAVE_H


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


typedef struct {

	uint8_t		signature[4] ;		// Shouldbe 'P' 'a' 'V' 'E'
	uint8_t		version ;
	uint8_t		video_codec ;
	uint16_t	header_size ;
	uint32_t	payload_size ;
	uint16_t	encoded_stream_width ;
	uint16_t	encoded_stream_height ;
	uint16_t	display_width ;
	uint16_t	display_height ;
	uint32_t	frame_number ;
	uint32_t	timestamp ;
	uint8_t		total_chunks ;
	uint8_t		chunk_index ;
	uint8_t		frame_type ;
	uint8_t		control ;
	uint32_t	stream_byte_position_lw ;
	uint32_t	stream_byte_position_uw ;
	uint16_t	stream_id ;
	uint8_t		total_slices ;
	uint8_t		slice_index ;
	uint8_t		header1_size ;
	uint8_t		header2_size ;
	uint8_t		reserved2[2] ;
	uint32_t	advertized_size ;
	uint8_t		reserved3[12] ;


} Pave ;



#endif


