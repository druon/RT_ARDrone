#ifndef GLX_WINDOW_H
#define GLX_WINDOW_H

#include <GL/gl.h>
#include <GL/glx.h>

#include <X11/extensions/xf86vmode.h>

#include <stdint.h>


/* attributes for a single buffered visual in RGBA format with at least
 * 4 bits per color and a 16 bit depth buffer */

static int attrListSgl[] = { GLX_RGBA, GLX_RED_SIZE, 4,
	GLX_GREEN_SIZE, 4,
	GLX_BLUE_SIZE, 4,
	GLX_DEPTH_SIZE, 16,
	None
};

/* attributes for a double buffered visual in RGBA format with at least 
 4 bits per color and a 16 bit depth buffer */

static int attrListDbl[] = { GLX_RGBA, GLX_DOUBLEBUFFER,
	GLX_RED_SIZE, 4,
	GLX_GREEN_SIZE, 4,
	GLX_BLUE_SIZE, 4,
	GLX_DEPTH_SIZE, 16,
	None
};


typedef struct  {

		Display* dpy_;				// window stuff
		int screen_;
		Window win_;
		GLXContext ctx_;
		XSetWindowAttributes attr_;

		Bool fullScreen_;
		Bool doubleBuffered_;
		
		XF86VidModeModeInfo deskMode_;
		unsigned int depth_;

		int winWidth_, winHeight_;		// window size

		char	font_[255];			// font used for info display
		GLuint 	fontBase_;			// liste des fontes

		unsigned char* RGB_img ;
		int RGB_img_width ;
		int RGB_img_height ;
} GLWindow ;


GLWindow* GLWindow_new ( unsigned char* title, int width, int height, int fullscreen );

void GLWindow_del ( GLWindow* ) ;

void GLWindow_draw_rgb ( GLWindow* win, unsigned char* img ) ;
		
void GLWindow_swap_buffers(GLWindow* win ) ;
int  GLWindow_next_event ( GLWindow* win, XEvent *event ) ;
void GLWindow_process_events( GLWindow* win, XEvent event ) ;


#endif
