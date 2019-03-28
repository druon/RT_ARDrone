/* This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */


#include <stdlib.h>
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>
#include <string.h>
#include <X11/extensions/xf86vmode.h>
#include <X11/keysym.h>

#include "glwindow.h"

#define LINES 		28
#define LINE_HEIGHT	17
#define LINE_OFFSET	12

GLWindow* GLWindow_new (unsigned char* title, int width, int height, int fullscreen) {
	
	XVisualInfo *vi;
	Colormap cmap;
	int dpyWidth, dpyHeight;
	int i;
	int glxMajorVersion, glxMinorVersion;
	int vidModeMajorVersion, vidModeMinorVersion;
	XF86VidModeModeInfo **modes;
	int modeNum;
	int bestMode;
	Atom wmDelete;
	Window winDummy;
	unsigned int borderDummy;


	GLWindow* tmp = (GLWindow*) malloc ( sizeof(GLWindow) ) ;
	
	tmp->RGB_img = (unsigned char*) malloc ( width * height * 3 ) ;
	tmp->RGB_img_width = width ;
	tmp->RGB_img_height = height ;

	tmp->fullScreen_ = fullscreen;

	/* set best mode to current */
	bestMode = 0;

	/* get a connection */
	tmp->dpy_ = XOpenDisplay(0);
	tmp->screen_ = DefaultScreen(tmp->dpy_);
	XF86VidModeQueryVersion(tmp->dpy_, &vidModeMajorVersion, &vidModeMinorVersion);
	XF86VidModeGetAllModeLines(tmp->dpy_, tmp->screen_, &modeNum, &modes);

	/* save desktop-resolution before switching modes */
	tmp->deskMode_ = *modes[0];

	/* look for mode with requested resolution */
	for (i = 0; i < modeNum; i++) {
		if ((modes[i]->hdisplay == width) && (modes[i]->vdisplay == height)) {
			bestMode = i;
		}
	}

	/* get an appropriate visual */
	vi = glXChooseVisual(tmp->dpy_, tmp->screen_, attrListDbl);
	if (vi == NULL) {
		vi = glXChooseVisual(tmp->dpy_, tmp->screen_, attrListSgl);
		tmp->doubleBuffered_ = False;
		printf("Only Singlebuffered Visual!" ) ;
	} else {
		tmp->doubleBuffered_ = True;
		printf("Got doubleBuffered_ Visual!" ) ;
	}
	glXQueryVersion(tmp->dpy_, &glxMajorVersion, &glxMinorVersion);

	/* create a GLX context */
	tmp->ctx_ = glXCreateContext(tmp->dpy_, vi, 0, GL_TRUE);

	/* create a color map */
	cmap = XCreateColormap(tmp->dpy_, RootWindow(tmp->dpy_, vi->screen), vi->visual,
			AllocNone);
	tmp->attr_.colormap = cmap;
	tmp->attr_.border_pixel = 0;
	if (tmp->fullScreen_) {
		XF86VidModeSwitchToMode(tmp->dpy_, tmp->screen_, modes[bestMode]);
		XF86VidModeSetViewPort(tmp->dpy_, tmp->screen_, 0, 0);
		dpyWidth = modes[bestMode]->hdisplay;
		dpyHeight = modes[bestMode]->vdisplay;
		printf("Resolution %d x %d \n", dpyWidth, dpyHeight) ;
		XFree(modes);

		/* create a fullscreen window */
		tmp->attr_.override_redirect = True;
		tmp->attr_.event_mask = ExposureMask | KeyPressMask | ButtonPressMask
				| StructureNotifyMask;

		tmp->winWidth_ = dpyWidth;
		tmp->winHeight_ = dpyHeight;

		tmp->win_ = XCreateWindow(tmp->dpy_, RootWindow(tmp->dpy_, vi->screen), 0, 0,
				dpyWidth, dpyHeight, 0, vi->depth, InputOutput, vi->visual,
				CWBorderPixel | CWColormap | CWEventMask | CWOverrideRedirect,
				&(tmp->attr_));

		XWarpPointer(tmp->dpy_, None, tmp->win_, 0, 0, 0, 0, 0, 0);
		XMapRaised(tmp->dpy_, tmp->win_);
		XGrabKeyboard(tmp->dpy_, tmp->win_, True, GrabModeAsync, GrabModeAsync,
				CurrentTime);
		XGrabPointer(tmp->dpy_, tmp->win_, True, ButtonPressMask, GrabModeAsync,
				GrabModeAsync, tmp->win_, None, CurrentTime);
	} else {
		/* create a window in window mode */
		tmp->attr_.event_mask = ExposureMask | KeyPressMask | ButtonPressMask
				| StructureNotifyMask;
		tmp->winWidth_ = width;
		tmp->winHeight_ = height;
		tmp->win_ = XCreateWindow(tmp->dpy_, RootWindow(tmp->dpy_, vi->screen), 0, 0, width,
				height, 0, vi->depth, InputOutput, vi->visual, CWBorderPixel
						| CWColormap | CWEventMask, &(tmp->attr_));
		/* only set window title and handle wm_delete_events if in windowed mode */
		wmDelete = XInternAtom(tmp->dpy_, "WM_DELETE_WINDOW", True);
		XSetWMProtocols(tmp->dpy_, tmp->win_, &wmDelete, 1);
		XSetStandardProperties(tmp->dpy_, tmp->win_, title, title, None,
				NULL, 0, NULL);
		XMapRaised(tmp->dpy_, tmp->win_);
	}
	/* connect the glx-context to the window */

	glXMakeCurrent(tmp->dpy_, tmp->win_, tmp->ctx_);

	int x, y;
	XGetGeometry(tmp->dpy_, tmp->win_, &winDummy, &x, &y, (unsigned int *) &width,
			(unsigned int *) &height, &borderDummy, &(tmp->depth_));


	if (glXIsDirect(tmp->dpy_, tmp->ctx_))
		printf("Congrats, you have Direct Rendering!\n") ;
	else
		printf("Sorry, no Direct Rendering possible!\n");

	/* Init fonts */

	sprintf(tmp->font_, "fixed");

	tmp->fontBase_ = glGenLists(256);

	if (!glIsList(tmp->fontBase_)) {
		printf("ERROR : Out of display lists. - Exiting.\n") ;
		exit(0);
	}

	XFontStruct* font_info;
	int first, last;

	font_info = XLoadQueryFont(tmp->dpy_, tmp->font_);

	if (!font_info) {
		printf("ERROR: XLoadQueryFont() failed - Exiting.\n");
		exit(0);
	}

	first = font_info->min_char_or_byte2;
	last = font_info->max_char_or_byte2;

	glXUseXFont(font_info->fid, first, last - first + 1, tmp->fontBase_ + first);

	return tmp ;
}

void GLWindow_del( GLWindow* win ) {
	if (win->ctx_) {
		if (!glXMakeCurrent(win->dpy_, None, NULL)) {
			printf("ERROR: Could not release drawing context.\n" ) ;
		}
		glXDestroyContext(win->dpy_, win->ctx_);
		win->ctx_ = NULL;
	}
	/* switch back to original desktop resolution if we were in fullScreen */
	if (win->fullScreen_) {
		XF86VidModeSwitchToMode(win->dpy_, win->screen_, &(win->deskMode_));
		XF86VidModeSetViewPort(win->dpy_, win->screen_, 0, 0);
	}
	XCloseDisplay ( win->dpy_);

}

void GLWindow_process_events( GLWindow* win, XEvent event ) {

	switch (event.type) {

		case Expose:
			if (event.xexpose.count != 0)
			break;
				GLWindow_draw_rgb( win, win->RGB_img ) ;
			break;

		case ConfigureNotify:

			if ((event.xconfigure.width != win->winWidth_)
					|| (event.xconfigure.height != win->winHeight_)) {

				win->winWidth_ = event.xconfigure.width;
				win->winHeight_ = event.xconfigure.height;

			}
			break;

		case ClientMessage:
			if (*XGetAtomName(win->dpy_, event.xclient.message_type)
					== *"WM_PROTOCOLS") {
				printf( "Exiting sanely...\n" ) ;
			}
			break;

		default:
			break;
	}

}

int GLWindow_next_event( GLWindow* win, XEvent *event ) {

	if ( XPending( win->dpy_ ) == 0 ) 
		return 0 ;
		
	XNextEvent( win->dpy_, event);
		return 1 ;
	
}


void GLWindow_draw_rgb( GLWindow* win, unsigned char *img ) {

	memcpy( win->RGB_img, img, win->RGB_img_height*win->RGB_img_width*3*sizeof(unsigned char) ) ;	

	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);

	if (win->winHeight_ == 0) /* Prevent A Divide By Zero If The Window Is Too Small */
		win->winHeight_ = 1;

	glViewport(0, 0, win->winWidth_, win->winHeight_);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(-0.5, (double) win->RGB_img_width - 0.5, (double) win->RGB_img_height - 0.5, -0.5,
			-1.0, 1.0);

	glPixelZoom( (double) win->winWidth_  / (double) win->RGB_img_width, 
	            -(double) win->winHeight_ / (double) win->RGB_img_height ) ;

	glClear(GL_COLOR_BUFFER_BIT);

	glFlush();

	// Draw the camera picture

	glRasterPos2d(-0.5, -0.5);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, win->RGB_img_width * sizeof(unsigned char ));

	glDrawPixels( win->RGB_img_width, win->RGB_img_height, GL_RGB, GL_UNSIGNED_BYTE, win->RGB_img );		

	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

	glClear(GL_DEPTH_BUFFER_BIT);

}


void GLWindow_swap_buffers(GLWindow* win) {

	// Swap buffers if needed


	if (win->doubleBuffered_) {
		glXSwapBuffers(win->dpy_, win->win_);
	}

}

