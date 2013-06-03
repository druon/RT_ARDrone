#include <stdio.h>
#include <RT_ARDrone/RT_ARDrone.h>

int main ( int argc, char** argv, char** envv ) {

	ARDrone* bob ;

	bob = ARDrone_new( "192.168.1.1" ) ;

	ARDrone_connect( bob ) ;

		
	ARDrone_trim ( bob ) ;
	
	sleep(2) ;

	ARDrone_reset_defaults( bob ) ;


	ARDrone_free( bob ) ;


	return 0 ;

}
