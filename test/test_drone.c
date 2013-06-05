#include <stdio.h>
#include <RT_ARDrone/RT_ARDrone.h>

int main ( int argc, char** argv, char** envv ) {

	int i ;
	ARDrone* bob ;
	NavData  data ;

	bob = ARDrone_new( "192.168.1.1" ) ;

	ARDrone_connect( bob ) ;
		
	ARDrone_trim ( bob ) ;
	
	sleep(2) ;

	for ( i=0; i<15; i++ ) {

		sleep(1) ;
		ARDrone_get_navdata( bob, &data ) ;
		printf( "state %d - bat %d \n", data.state, data.bat ) ;
	}

	ARDrone_free( bob ) ;


	return 0 ;

}
