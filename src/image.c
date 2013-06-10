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

#include <RT_ARDrone/image.h>


RGB24Image* RGB24Image_new( int width, int height ) {

	RGB24Image* tmp ;
	tmp = (RGB24Image*) malloc ( sizeof ( RGB24Image ) ) ;
	tmp->height = 480 ;
	tmp->width = 640 ;
	tmp->pixels = ( unsigned char*) malloc ( tmp->height * tmp->width * 3 ) ;

	return tmp ;


}


void RGB24Image_del( RGB24Image* image ) {

	free ( image->pixels ) ;
	free ( image ) ;


}

void RGB24Image_resize( RGB24Image* image,  int width, int height ) {

	image->height = height ;
	image->width = width ;
	free ( image->pixels ) ;

	image->pixels = (unsigned char*) malloc ( height * width * 3 ) ;

}






