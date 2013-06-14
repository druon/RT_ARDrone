#include <stdio.h>
#include <RT_ARDrone/RT_ARDrone.h>

void SaveFrame( RGB24Image* img, int iFrame) {

	FILE *pFile;
	char szFilename[32];
  	int  y;
  
  // Open file
  
  	sprintf(szFilename, "frame%d.ppm", iFrame);
  
  	pFile=fopen(szFilename, "wb");
  	if(pFile==NULL)
    		return;
  
  // Write header
  	
	fprintf(pFile, "P6\n%d %d\n255\n", img->width, img->height);
  
  // Write pixel data
  
  	for(y=0; y<img->height; y++)
		fwrite(&(img->pixels[y*3*img->width]), 1, img->width*3, pFile ) ;
	
  // Close file
  
  	fclose(pFile);
}


int main ( int argc, char** argv, char** envv ) {

	int iFrame ;
	ARDrone* bob ;
	RGB24Image* img ;

	img = RGB24Image_new(640,480) ;
	bob = ARDrone_new( "192.168.1.1" ) ;

	ARDrone_connect( bob ) ;
		
	ARDrone_trim ( bob ) ;
	
	iFrame = 0 ;
	
	ARDrone_zap_camera ( bob, 1 ) ;

	sleep(2) ;

	for ( iFrame=0; iFrame<100; iFrame++ ) {

		ARDrone_get_RGB24Image ( bob, img ) ;
		SaveFrame( img, iFrame ) ;

	}

	ARDrone_zap_camera ( bob, 2 ) ;

	sleep(2) ;

	for ( iFrame=100; iFrame<200; iFrame++ ) {

		ARDrone_get_RGB24Image ( bob, img ) ;
		SaveFrame( img, iFrame ) ;

	}

	ARDrone_free( bob ) ;


	return 0 ;

}
