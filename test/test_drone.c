#include <stdio.h>
#include <RT_ARDrone/RT_ARDrone.h>

int main ( int argc, char** argv, char** envv ) {

	int i ;
	ARDrone* bob ;
	NavData  data ;

	bob = ARDrone_new( "192.168.1.1" ) ;

	ARDrone_connect( bob ) ;
		
	ARDrone_trim ( bob ) ;
	
	sleep(5) ;

	while(1){

		ARDrone_get_navdata( bob, &data ) ;
		printf( "alt %d - bat %d - RTL %f %f %f - Vitesse %f %f %f \n", data.altitude, data.bat, data.theta, data.phi, data.psi, data.vx, data.vy, data.vz ) ;
	}


	ARDrone_free( bob ) ;


	return 0 ;

}
