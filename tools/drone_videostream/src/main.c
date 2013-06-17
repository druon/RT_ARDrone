#include <stdio.h>
#include <stdlib.h>

#include <RT_ARDrone/RT_ARDrone.h>

#include "glwindow.h"

int main() {

	GLWindow*   win ; 
	ARDrone*    bob ;
	RGB24Image* img ;

	img = RGB24Image_new(640,360) ;
	bob = ARDrone_new( "192.168.1.1" ) ;

	ARDrone_connect( bob ) ;
		
	ARDrone_zap_camera ( bob, 2 ) ;

	sleep(2) ;

	win = GLWindow_new ( "glview", 640, 360, 0 ) ;  

	while (1) {


		ARDrone_get_RGB24Image ( bob, img ) ;

		GLWindow_draw_rgb(win,img->pixels );
		
		GLWindow_swap_buffers(win);
		
		XEvent event;

		while ( GLWindow_next_event( win, &event ) ) {

			GLWindow_process_events( win, event );
		
		}

	}
	
	return 0 ;

}
