#ifndef IMAGE_H
#define IMAGE_H

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



typedef struct {

	int width  ;
	int height ;
	
	unsigned char* pixels ;


} RGB24Image ;



typedef struct {

	int width ;
	int height ;

	unsigned char* pixels ;

} YUV420Image ;




RGB24Image* RGB24Image_new( int width, int height ) ;
void RGB24Image_del( RGB24Image* ) ;
void RGB24Image_resize( RGB24Image*, int width, int height ) ;


YUV420Image* YUV420Image_new( int width, int height ) ;
void YUV420Image_del( YUV420Image* ) ;
void YUV420Image_resize( YUV420Image*, int width, int height ) ;

#endif
