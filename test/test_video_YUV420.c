#include <stdio.h>
#include <RT_ARDrone/RT_ARDrone.h>

void SaveFrame( YUV420Image* img, int iFrame) {

	FILE *pFile;
	char szFilename[32];
  	int  x,y;
  
  // Open file
  
  	sprintf(szFilename, "frame%d.ppm", iFrame);
  
  	pFile=fopen(szFilename, "wb");
  	if(pFile==NULL)
    		return;
  
  // Write header
  	
	fprintf(pFile, "P5\n%d %d\n255\n", img->width, img->height);
  
  // Write pixel data
  
	for (y=0; y<img->height; y++ ) {
		for (x=0; x<img->width; x++ ) {

			fprintf(pFile, "%d ", img->pixels[0] ) ;

		}

		fprintf(pFile, "\n" ) ;
	}

  // Close file
  
  	fclose(pFile);
}


int main ( int argc, char** argv, char** envv ) {

	int iFrame ;
	ARDrone* bob ;
	YUV420Image* img ;

	img = YUV420Image_new(640,480) ;
	bob = ARDrone_new( "192.168.1.1" ) ;

	ARDrone_connect( bob ) ;
		
	ARDrone_trim ( bob ) ;
	
	iFrame = 0 ;

	while(1) {

		ARDrone_get_YUV420Image ( bob, img ) ;
		
		SaveFrame( img, iFrame ) ;
		iFrame++ ;

		usleep( 50000 ) ;
	}

	ARDrone_free( bob ) ;


	return 0 ;

}
