/* *******************************************************************
	xinterface.c  :  C interface to X routines for WINDOWTOOLS

	Bjorn Kirkerud: Institute for Informatics,
	                University of Oslo
 
 ***************** Integrated version v97.4 **************************
 
- July 1997:	
	- changed xcopyarea
	- removed old 'gray' bitmaps 
====================================================================
- Vaucher mods may 1997

*** Changed ALARM interface to stop ALARM from being misinterpreted 
    as DestroyNotices after a Destroy Event
    
 - alarmfunc(sig)  added line to set Event TYPE:
 - xclientmessage changed to xalarmmessage 
    
- Vaucher mods april 1997
====================================================================
*** for X procedures requiring String & Length we now pass LENGTH
    from Simula avoiding adding "!0!" at end.
   - XTextWidth
   - XDrawString
   - XDrawImageString
   
*** reintroduced the Fontfile routines
 *********************************************************************  */

#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include "Xpm/xpm.h"

#define max_points 2000

#define TRUE 1
#define FALSE 0

#define rast_width 4
#define rast_height 4

static char rast0_bits[] = {0x00, 0x00, 0x00, 0x00};
static char rast1_bits[] = {0x01, 0x00, 0x00, 0x00};
static char rast2_bits[] = {0x01, 0x00, 0x04, 0x00};
static char rast3_bits[] = {0x05, 0x00, 0x04, 0x00};
static char rast4_bits[] = {0x05, 0x00, 0x05, 0x00};
static char rast5_bits[] = {0x05, 0x02, 0x05, 0x00};
static char rast6_bits[] = {0x05, 0x02, 0x05, 0x08};
static char rast7_bits[] = {0x05, 0x0a, 0x05, 0x08};
static char rast8_bits[] = {0x05, 0x0a, 0x05, 0x0a};
static char rast9_bits[] = {0x07, 0x0a, 0x05, 0x0a};
static char rast10_bits[] = {0x07, 0x0a, 0x0d, 0x0a};
static char rast11_bits[] = {0x0f, 0x0a, 0x0d, 0x0a};
static char rast12_bits[] = {0x0f, 0x0a, 0x0f, 0x0a};
static char rast13_bits[] = {0x0f, 0x0b, 0x0f, 0x0a};
static char rast14_bits[] = {0x0f, 0x0b, 0x0f, 0x0e};
static char rast15_bits[] = {0x0f, 0x0f, 0x0f, 0x0e};
static char rast16_bits[] = {0x0f, 0x0f, 0x0f, 0x0f};

static char *rastptr[] = {
  rast16_bits, rast15_bits, rast14_bits, rast13_bits, rast12_bits,
  rast11_bits, rast10_bits, rast9_bits, rast8_bits, rast7_bits,
  rast6_bits, rast5_bits, rast4_bits, rast3_bits, rast2_bits,
  rast1_bits, rast0_bits
};

/* ------------- replaced by rasters above --------------
   ------------------------------------------------------ */

#define gray12_width 4
#define gray12_height 4
static char gray12_bits[] = {
   0x08, 0x00, 0x02, 0x00};

#define gray25_width 4
#define gray25_height 2
static char gray25_bits[] = {
   0x01, 0x04};

#define gray50_width 2
#define gray50_height 2
static char gray50_bits[] = {
   0x01, 0x02};

#define gray75_width 4
#define gray75_height 2
static char gray75_bits[] = {
   0x0e, 0x0b};

#define gray87_width 4
#define gray87_height 4
static char gray87_bits[] = {
   0x07, 0x0f, 0x0d, 0x0f};

typedef struct{
   int type;
   unsigned long serial;
   Bool send_event;
   Display *display;
   Window window;
   Window root;
   Window subwindow;
   Time time;
} XTimeEvent;

typedef struct{
   int type;
   unsigned long serial;
   Bool send_event;
   Display *display;
   Window window;
   Window root;
   Window subwindow;
   Time time;
   int x, y;
} XCoordEvent;

/* Global variables */

	Display    *dpy = NULL;            /* X server connection */
	XPoint      points[max_points];
	
	int         screen;         /* Default screen */
	int         depth;          /* Default depth */
	Visual     *visual;         /* Default visual */
	Colormap    colormap;       /* Default colormap */ 
	XFontStruct *fontstruct;    /* Font descriptor */
	XGCValues   gcv;            /* Struct for creating GC */
	XEvent      event;          /* Event received */
	XAnyEvent   anyevent;       /* Event received */
	XSizeHints  xsh;            /* Size hints for window manager */
	char       *geomSpec;       /* Window geometry string */
	XSetWindowAttributes xswa;  /* Temporary Set Window Attribute struct */
	Window      alarmwindow;
	
	XColor      defaultcolors[256];  /* Defaultcolors when colormap is full */
	char        clientcminstalled = FALSE;
	
	int         xshapesupport = 0, xshape_event, xshape_error;
	
/******************************************************
 *
 *  0: General utility functions:
 *
 ******************************************************/

void sleepus(n_usecs)
int n_usecs;
{
  /* Sleep n_usecs microseconds.
     (Not accurate for values below about 10000.) */

  struct timeval tval;
  
  tval.tv_sec  = n_usecs / 1000000;
  tval.tv_usec = n_usecs % 1000000;
  select(0, NULL, NULL, NULL, &tval);
}


/*******************************************************
 *
 *  1: Procedure for opening the display: 
 *
 *******************************************************/

Display *xopendisplay(dname) char *dname;
{
  dpy = XOpenDisplay(NULL);
  if ( dpy == (Display *) NULL ) {
      
    }
  else {
    /*    XSynchronize (dpy, True); */
    /* Synchronous behaviour (nice when debugging) */
    screen = DefaultScreen(dpy);
    depth = DefaultDepth(dpy, screen);
    colormap = DefaultColormap(dpy, screen);
    
/*    xshapesupport = XShapeQueryExtension (dpy, &xshape_event, &xshape_error);
*/
  }
  return dpy;
}


/*******************************************************
 *
 *  2: Procedures for handling windows:
 *
 *******************************************************/

Window xrootwindow()
{
   return DefaultRootWindow(dpy);
}

Window xcreatesimplewindow(parentID, x, y, width, height, 
                           borderwidth, borderpix, backpix) 
    Window parentID; int x, y, width, height, borderwidth, borderpix, backpix;
{
     return XCreateSimpleWindow(dpy, parentID,
     			      x, y, width, height,
     			      borderwidth, borderpix, backpix);
}


Window xcreatewindow(parentID, window_name)
    Window parentID; char *window_name;
{
  int WindowID;
  WindowID = XCreateSimpleWindow(dpy, parentID,
     			      0, 0, 1, 1, 1, 
     			      BlackPixel(dpy, screen),
     			      WhitePixel(dpy, screen));
  XSetStandardProperties(dpy, 
                         WindowID, window_name, 
                         window_name, 0, 
                         None, 0, &xsh);
  return WindowID;
}


Window xcreatepopup(parentID, window_name)
    Window parentID; char *window_name;
{
  int WindowID;
  WindowID = XCreateSimpleWindow(dpy, parentID,
     			      0, 0, 1, 1, 1, 
     			      BlackPixel(dpy, screen),
     			      WhitePixel(dpy, screen));
  XSetStandardProperties(dpy, 
                         WindowID, window_name, 
                         window_name, 0, 
                         None, 0, &xsh);
   xswa.override_redirect = TRUE;
   XChangeWindowAttributes(dpy, WindowID, CWOverrideRedirect, &xswa);

  return WindowID;
}


void xsetstandardproperties(windowID, window_name, icon_name, icon_pixmap)
    Window windowID; char *window_name, *icon_name; Pixmap icon_pixmap;
{
    XSetStandardProperties(dpy, 
                           windowID, window_name, 
                           icon_name, icon_pixmap, 
                           None, 0, &xsh);
}

void xmapwindow(WindowNumber)
    int WindowNumber;
{
    XMapWindow(dpy, WindowNumber);
    XFlush(dpy);
}

void xunmapwindow(WindowNumber)
    int WindowNumber;
{
    XUnmapWindow(dpy, WindowNumber);
    XFlush(dpy);
}

void xclearwindow(WindowNumber)
    int WindowNumber;
{
    XClearWindow(dpy, WindowNumber);
    XFlush(dpy);
}

void xdestroywindow(WindowNumber)
    int WindowNumber;
{
    XDestroyWindow(dpy, WindowNumber);
    XFlush(dpy);
}

void xresizewindow(WindowNumber, width, height)
    int WindowNumber, width, height;
{
    XResizeWindow(dpy, WindowNumber, width, height);
    XFlush(dpy);
}

void xmovewindow(WindowNumber, x, y)
    int WindowNumber, x, y;
{
    XMoveWindow(dpy, WindowNumber, x, y);
    XFlush(dpy);
}

void xraisewindow(WindowNumber)
    int WindowNumber;
{
    XRaiseWindow(dpy, WindowNumber);
    XFlush(dpy);
}

void xsetwindowborderwidth(WindowNumber, bw)
    int WindowNumber, bw;
{
    XSetWindowBorderWidth(dpy, WindowNumber, bw);
    XFlush(dpy);
}

/*******************************************************
 *
 *  3: Procedures for drawing:  
 *
 *******************************************************/

void xdrawstring(WindowNumber, gc1, x, y, t, len)
   int WindowNumber, gc1, x, y; char t[]; int len;
{
   XDrawString(dpy, WindowNumber, (GC)gc1, x, y, t, len);
   XFlush(dpy);
}

void xdrawimagestring(WindowNumber, gc1, x, y, t, len)
   int WindowNumber, gc1, x, y; char t[]; int len;
{
   XDrawImageString(dpy, WindowNumber, (GC)gc1, x, y, t, len);
   XFlush(dpy);
}

void xdrawpoint(WindowNumber, gc, x, y) 
   int WindowNumber, gc, x, y;
{
   XDrawPoint(dpy, WindowNumber, (GC)gc, x, y);
   XFlush(dpy);
}

xdrawpoints(WindowNumber, gc, x, y, n, mode) 
   int WindowNumber, gc, x[], y[], n, mode;
{
  int i, dmode;
  if (n > max_points) n = max_points;
  for( i = 0; i < n+1; i++)
    {
      points[i].x = x[i];
      points[i].y = y[i];
    }
   if (mode > 0) dmode = CoordModeOrigin;
   else          dmode = CoordModePrevious;
   XDrawPoints(dpy, WindowNumber, (GC)gc, points, n, dmode);
   XFlush(dpy);
}

void xfillpolygon(WindowNumber, gc, x, y, n, mode) 
   int WindowNumber, gc, x[], y[], n, mode;
{
  int i, dmode;
  if (n > max_points) n = max_points;
  for( i = 0; i < n+1; i++)
    {
      points[i].x = x[i];
      points[i].y = y[i];
    }
   if (mode > 0) dmode = CoordModeOrigin;
   else          dmode = CoordModePrevious;
   XFillPolygon(dpy, WindowNumber, (GC)gc, points, n, Complex, dmode);
   XFlush(dpy);
}

void xfillrectangle(WindowNumber, gc, x, y, width, height) 
   int WindowNumber, gc, x, y, width, height;
{
   XFillRectangle(dpy, WindowNumber, (GC)gc, x, y, width, height);
   XFlush(dpy);
}

void xcleararea(WindowNumber, x, y, width, height) 
   int WindowNumber, x, y; unsigned int width, height;
{
   XClearArea(dpy, WindowNumber, x, y, width, height, False);
   XFlush(dpy);
}

void xdrawline(WindowNumber, gc, x1, y1, x2, y2) 
   int WindowNumber, gc, x1, y1, x2, y2;
{
   XDrawLine(dpy, WindowNumber, (GC)gc, x1, y1, x2, y2);
   XFlush(dpy);
}

void xrootline(gc, x1, y1, x2, y2) GC gc; int x1, y1, x2, y2;
{
   XDrawLine(dpy, DefaultRootWindow(dpy), gc, x1, y1, x2, y2);
   XFlush(dpy);
}                    /* from Oslo 3.4 version (june 1997)  */

void xdrawrectangle(WindowNumber, gc, x, y, width, height) 
   int WindowNumber, gc, x, y, width, height;
{
   XDrawRectangle(dpy, WindowNumber, (GC)gc, x, y, width, height);
   XFlush(dpy);
}

void xdrawarc(WindowNumber, gc, x, y, width, height, angle1, angle2) 
   int WindowNumber, gc, x, y, width, height, angle1, angle2;
{
   XDrawArc(dpy, WindowNumber, (GC)gc, x, y, width, height, angle1, angle2);
   XFlush(dpy);
}

void xfillarc(WindowNumber, gc, x, y, width, height, angle1, angle2) 
   int WindowNumber, gc, x, y, width, height, angle1, angle2;
{
   XFillArc(dpy, WindowNumber, (GC)gc, x, y, width, height, angle1, angle2);
   XFlush(dpy);
}



/*******************************************************
 *
 *  4: Procedures for handling fonts:
 *
 *******************************************************/

int xfontid(font_struct) XFontStruct *font_struct; 
{
    return font_struct->fid;
}

int xfontheight(font_struct) XFontStruct *font_struct; 
{
    return (font_struct->max_bounds.ascent + font_struct->max_bounds.descent); 
}

int xfontascent(font_struct) XFontStruct *font_struct; 
{
    return (font_struct->max_bounds.ascent);
}

int xfontdescent(font_struct) XFontStruct *font_struct; 
{
    return (font_struct->max_bounds.descent);
}

int xfontmaxwidth(font_struct) XFontStruct *font_struct; 
{
    return (font_struct->max_bounds.width);
}

int xfontminwidth(font_struct) XFontStruct *font_struct; 
{
    return (font_struct->min_bounds.width);
}

int xfontlbearing(font_struct) XFontStruct *font_struct; 
{
    return (font_struct->max_bounds.lbearing);
}

int xfontrbearing(font_struct) XFontStruct *font_struct; 
{
    return (font_struct->max_bounds.rbearing);
}

int xfontminchar(font_struct) XFontStruct *font_struct; 
{
    return (font_struct->min_char_or_byte2);
}

int xfontmaxchar(font_struct) XFontStruct *font_struct; 
{
    return (font_struct->max_char_or_byte2);
}

int xcharlbearing(font_struct, rank) XFontStruct *font_struct; int rank; 
{
    return (font_struct->per_char[rank].lbearing);
}

int xcharrbearing(font_struct, rank) XFontStruct *font_struct; int rank; 
{
    return (font_struct->per_char[rank].rbearing);
}

int xcharwidth(font_struct, rank) XFontStruct *font_struct; int rank; 
{
    return (font_struct->per_char[rank].width);
}

int xcharascent(font_struct, rank) XFontStruct *font_struct; int rank; 
{
    return (font_struct->per_char[rank].ascent);
}

int xchardescent(font_struct, rank) XFontStruct *font_struct; int rank; 
{
    return (font_struct->per_char[rank].descent);
}

XFontStruct *xloadqueryfont(fn)
    char fn[];
{
    return XLoadQueryFont(dpy, fn);
}

void xdestroyfont(fn) XFontStruct *fn;   /*CDL June 15, 1995*/
{
  XFreeFont(dpy,fn);
}

int xtextwidth(font_struct, t, Len)
    XFontStruct *font_struct; char t[]; int Len;
{
    return XTextWidth(font_struct, t, Len);
}

/*****************************************/
/*        Handling of font lists         */
/* to find all fonts available on system */
/*****************************************/

static char** fontlist = 0;
static int fontpos = 0;
static int fontlen = 0;

int getfontlist(maxnames)
int maxnames;
{
  int len;
  fontlist = XListFonts(dpy, "*", maxnames, &len);
  fontpos = 0;
  fontlen = len;
  return len;
}

xfreefontnames()
{
  if (fontlist)
    {
      XFreeFontNames(fontlist);
      fontlist = 0;
    }
}

char* nextinfontlist()
{
  if (fontpos < fontlen)
    return fontlist[fontpos++];
  else
    {
      xfreefontnames();
      return "";
    }
}

/******** end of font list addition ************/


Cursor xarrowcursor()    
{
  return XCreateFontCursor(dpy,XC_top_left_arrow);
}

Cursor xxtermcursor()
{
  return XCreateFontCursor(dpy, XC_xterm);
}

void xsetcursor(WindowID,CursorID) int WindowID,CursorID;
{
  XDefineCursor(dpy, WindowID, CursorID); 
}

/*******************************************************
 *
 *  5: Procedures for synchronization:
 *
 *******************************************************/

xflush()  {  XFlush(dpy);        }
xsync()   {  XSync(dpy, False);  }

int xfd()    /* the fd for the socket used by X for events  */
{
	return ConnectionNumber(dpy);
}


/*******************************************************
 *
 *  6: Procedures for event-handling:
 *
 *******************************************************/


void xselectinput(WindowNumber, EventMask)
    int WindowNumber, EventMask;
{
    XSelectInput(dpy, WindowNumber, EventMask);
}


XEvent *xeventpointer()
{
   return &event;
}


int xpending()
{
   return XPending(dpy);
}

void xnextevent(event) XEvent *event; 
{
   XNextEvent(dpy, event);
}

Bool xchecktypedeventwindow(event) XAnyEvent *event; 
{
   return XCheckTypedWindowEvent(dpy, event->window, event->type,
(XEvent*)event);
}

int xeventtype(event) XEvent *event; 
{
   return event->type;
}

int xeventtime(event) XTimeEvent *event; 
{
   return event->time;
}

int xeventx(event) XCoordEvent *event; 
{
   return event->x;
}

int xeventy(event) XCoordEvent *event; 
{
   return event->y;
}

int xeventbutton(event) XButtonEvent *event; 
{
   return event->button;
}

int xeventkeycode(event) XKeyEvent *event; 
{
   return event->keycode;
}

Window xeventwindow(event) XAnyEvent *event; 
{
   return event->window;
}

/*******************************************************
 *  6a: Alarm event-handling:
 *******************************************************/

void alarmfunc(sig) int sig;
{
  event.type = ClientMessage;
  event.xclient.window = alarmwindow;
  event.xclient.message_type = 6; /* Magic numbers, just to distinguish */
  event.xclient.format = 32;      /* from other clientmessages */
  XSendEvent(dpy, alarmwindow, 0, ExposureMask, &event);
  XFlush(dpy);
}

int xalarmclock(WindowNumber, time) int WindowNumber, time;
{
   alarmwindow = WindowNumber;
   signal(SIGALRM, alarmfunc);
   return alarm(time);
}

Bool xalarmmessage(event) XEvent *event;
{
  return (event->xany.send_event
	  && event->xclient.message_type == 6);
}


/*******************************************************
 *
 *  7: Procedures to get various masks etc.:
 *
 *******************************************************/

int blackpixel()	{ return BlackPixel(dpy, screen);}
int whitepixel()	{ return WhitePixel(dpy, screen);}

int keypressmask()  	{ return KeyPressMask;}
int buttonpressmask()	{ return ButtonPressMask;}
int buttonreleasemask()	{ return ButtonReleaseMask;}
int enterwindowmask()	{ return EnterWindowMask;}
int leavewindowmask()	{ return LeaveWindowMask;}

int pointermotionmask()	{ return PointerMotionMask; }
int structurenotifymask()	{ return StructureNotifyMask; }
int exposuremask()  	{ return ExposureMask; }

int keypress()   	{ return KeyPress; }
int buttonpress()	{ return ButtonPress; }
int buttonrelease()	{ return ButtonRelease; }

int enternotify()	{ return EnterNotify; }
int leavenotify()	{ return LeaveNotify; }
int destroynotify()	{ return DestroyNotify; }
int configurenotify()	{ return ConfigureNotify; }
int motionnotify()	{ return MotionNotify; }
int mapnotify() 	{ return MapNotify; }
int expose()    	{ return Expose; }
int clientmessage()	{ return ClientMessage; }



/*******************************************************
 *
 *  8: Various procedures:  
 *
 *******************************************************/

int xscreendepth()
{
  return depth;
}

void xwarppointer(WindowNumber, x, y)
   int WindowNumber, x, y;
{
   XWarpPointer(dpy, None, WindowNumber, 0, 0, 0, 0, x, y);
   XFlush(dpy);
}

Status xgetgeometry(d, root, x, y, width, height, borderwidth, depth)
   Window d; Window *root; int *x, *y;
   unsigned int *width, *height, *borderwidth, *depth;
{
  return XGetGeometry(dpy, d, root, x, y, width, height, borderwidth, depth);
}

Status xqueryparent(d,root,parent)
  Window d;Window *root,*parent;
{
	Window *children;  
	unsigned int nchilds;
	Status s;

	s = XQueryTree(dpy,d,root,parent,&children,&nchilds);
	XFree(children);
	return s;
}

Bool xquerypointer(w, root, child, rootx, rooty, winx, winy, kbuttons)
   Window w; Window *root, *child; int *rootx, *rooty, *winx, *winy;
   unsigned int *kbuttons;
{
  return XQueryPointer(dpy, w, root, child, rootx, rooty, winx, winy,
                       kbuttons);
}

unsigned int xdisplaywidth()
{
  return DisplayWidth(dpy, screen);
}

unsigned int xdisplayheight()
{
  return DisplayHeight(dpy, screen);
}

unsigned int xdisplaywidthmm()
{
  return DisplayWidthMM(dpy, screen);
}

unsigned int xdisplayheightmm()
{
  return DisplayHeightMM(dpy, screen);
}

/* JPH */
void xcopyarea(srcWindowID, dstWindowID, GCID, srcx, srcy, width, height, dstx, dsty) 
    int srcWindowID, dstWindowID, GCID, srcx, srcy, width, height, dstx, dsty;
{
  XCopyArea(dpy, srcWindowID, dstWindowID, (GC)GCID, 
            srcx, srcy, width, height, dstx, dsty); 
   XFlush(dpy);
}

int xlookupstring(event, keybuffer, bufferlength, keysym, status)
  XKeyEvent *event;
  char *keybuffer;
  int bufferlength;
  KeySym *keysym;
  XComposeStatus *status;
{
  return XLookupString(event, keybuffer, bufferlength, keysym, status);
}


void xsetstipple(gc, stipple) GC gc; Pixmap stipple;
{
    XSetFillStyle(dpy, gc, FillOpaqueStippled);
    XSetStipple(dpy, gc, stipple);
}

void xsetnostipple(gc) GC gc;
{
    XSetFillStyle(dpy, gc, FillSolid);
}

void xsetwindowbackground(WindowNumber, pix)
   int WindowNumber, pix;
{
    XSetWindowBackground(dpy, WindowNumber, pix);
}


void xsetwindowbackgroundpixmap(WindowNumber, pix)
    int WindowNumber, pix;
{
     XSetWindowBackgroundPixmap(dpy, WindowNumber, pix);
}

/*******************************************************
 *
 *  Gray PIXmaps &BITmaps
 *
 *******************************************************/

Pixmap xrast ()
/* Assume that this function is called exactly 17 times for each xdisplay */
{
  static char **last = rastptr;
  return XCreateBitmapFromData (dpy, DefaultRootWindow (dpy), 
                              *(last++), rast_width, rast_height);
}


Pixmap xgray12pixmap(windowID) 
     Window windowID;
{
    return XCreatePixmapFromBitmapData(dpy, windowID, 
                        gray12_bits, gray12_width, gray12_height,
			BlackPixel(dpy, screen),
			WhitePixel(dpy, screen),
		        DefaultDepth(dpy, screen)); 
}

Pixmap xgray25pixmap(windowID) 
     Window windowID;
{
    return XCreatePixmapFromBitmapData(dpy, windowID, 
                        gray25_bits, gray25_width, gray25_height,
			BlackPixel(dpy, screen),
			WhitePixel(dpy, screen),
		        DefaultDepth(dpy, screen)); 
}

Pixmap xgray50pixmap(windowID) 
     Window windowID;
{
    return XCreatePixmapFromBitmapData(dpy, windowID, 
                        gray50_bits, gray50_width, gray50_height,
			BlackPixel(dpy, screen),
			WhitePixel(dpy, screen),
		        DefaultDepth(dpy, screen)); 
}

Pixmap xgray75pixmap(windowID) 
     Window windowID;
{
    return XCreatePixmapFromBitmapData(dpy, windowID, 
                        gray75_bits, gray75_width, gray75_height,
			BlackPixel(dpy, screen),
			WhitePixel(dpy, screen),
		        DefaultDepth(dpy, screen)); 
}

Pixmap xgray87pixmap(windowID) 
     Window windowID;
{
    return XCreatePixmapFromBitmapData(dpy, windowID, 
                        gray87_bits, gray87_width, gray87_height,
			BlackPixel(dpy, screen),
			WhitePixel(dpy, screen),
		        DefaultDepth(dpy, screen)); 
}

Pixmap xgetpixmap(windowID, pixmap_name) 
     Window windowID; char *pixmap_name;
{
  int gray;
  if (sscanf(pixmap_name,"gray%i",&gray)==0) return -1;
  
  if (gray <= 0) return BlackPixel(dpy, screen);
  if (gray<(12+25)/2) return xgray12pixmap(windowID);
  if (gray<(25+50)/2) return xgray25pixmap(windowID);
  if (gray<(50+75)/2) return xgray50pixmap(windowID);
  if (gray<(75+87)/2) return xgray75pixmap(windowID);
  if (gray<100) return xgray87pixmap(windowID);
  return WhitePixel(dpy, screen);
}



Pixmap xgray12(windowID) Window windowID;
{
    return XCreateBitmapFromData(dpy, windowID, 
                        gray12_bits, gray12_width, gray12_height); 
}

Pixmap xgray25(windowID) Window windowID;
{
    return XCreateBitmapFromData(dpy, windowID, 
                        gray25_bits, gray25_width, gray25_height); 
}

Pixmap xgray50(windowID) Window windowID;
{
    return XCreateBitmapFromData(dpy, windowID, 
                        gray50_bits, gray50_width, gray50_height); 
}

Pixmap xgray75(windowID) Window windowID;
{
    return XCreateBitmapFromData(dpy, windowID, 
                        gray75_bits, gray75_width, gray75_height); 
}

Pixmap xgray87(windowID) Window windowID;
{
    return XCreateBitmapFromData(dpy, windowID, 
                        gray87_bits, gray87_width, gray87_height); 
}

Pixmap xgetbitmap(windowID, bitmap_name)  
     Window windowID; char *bitmap_name;
{
  int gray;
  if (sscanf(bitmap_name,"gray%i",&gray)==0) return -1;
  if (gray <= 0) return BlackPixel(dpy, screen);
  if (gray<(12+25)/2) return xgray12(windowID);
  if (gray<(25+50)/2) return xgray25(windowID);
  if (gray<(50+75)/2) return xgray50(windowID);
  if (gray<(75+87)/2) return xgray75(windowID);
  if (gray<100) return xgray87(windowID);
  return WhitePixel(dpy, screen);;
}



int xreadbitmapfile(windowID, bitmap_file, 
                    bitmap_width, bitmap_height,bitmap,x_hot,y_hot)
    Window windowID; char *bitmap_file; 
    unsigned int *bitmap_width, *bitmap_height;
    Pixmap *bitmap;
    int  *x_hot, *y_hot; 
{
    int xr;

    xr = XReadBitmapFile(dpy, windowID, bitmap_file, 
                        bitmap_width, bitmap_height, bitmap,  x_hot, y_hot); 
    return xr;
}


void xsaveunder(windowID) Window windowID; 
{
   xswa.save_under = TRUE;
   XChangeWindowAttributes(dpy, windowID, CWSaveUnder, &xswa);
}

void xbackingstore(windowID) Window windowID; 
{
   xswa.backing_store = Always;
   XChangeWindowAttributes(dpy, windowID, CWBackingStore, &xswa);
}

GC xcreategc(wn) int wn;    
{
    gcv.foreground = BlackPixel(dpy, screen);
    gcv.background = WhitePixel(dpy, screen);
    gcv.graphics_exposures = FALSE;
    gcv.fill_style = FillOpaqueStippled;
    return XCreateGC(dpy, wn,
		     GCForeground | GCBackground | GCGraphicsExposures | GCFillStyle,
		     &gcv);
}


void xdestroygc(gc) GC gc;  
{
  XFreeGC(dpy,gc);
}

void xfree(ID) int ID;
{
  XFree((void *)ID);
}

void xcopygc(sc,dst) GC sc,dst;
{
  XCopyGC(dpy,sc,GCFunction|GCForeground|GCBackground|GCLineWidth|
	  GCFillStyle|GCStipple|GCFont,dst);
}

void xinvertgc(gc) GC gc;
{
   XSetForeground(dpy, gc, WhitePixel(dpy, screen));
   XSetFunction(dpy,gc,GXxor);
}

/* 
void xinvertgc(gc) GC gc;
{
  XSetFunction(dpy,gc,GXinvert); 
}  -------------------------------- */

void xxorgc(gc, d) GC gc; int d;
{
  XSetFunction(dpy,gc,GXxor);
  if (xscreendepth () == 1) {
    XSetForeground (dpy, gc, 1);
    XSetBackground (dpy, gc, 0);
  }
}

void xdrawinsubwindows (gc) GC gc;
{
  XSetSubwindowMode (dpy, gc, IncludeInferiors);
}

void xclipbysubwindows (gc) GC gc;
{
  XSetSubwindowMode (dpy, gc, ClipByChildren);
}

int xdefaultvisual()
{
  visual = DefaultVisual(dpy, screen);
  if (visual->class == PseudoColor)
    {
      return 1;
    }
  else
  if (visual->class == StaticGray)
    {
      return 2;
    }
  else
    {
      return 0;
    }
}

void xsetblackonwhite(gc) GC gc;
{
    XSetForeground(dpy, gc, BlackPixel(dpy, screen));
    XSetBackground(dpy, gc, WhitePixel(dpy, screen));
    XSetFillStyle(dpy, gc, FillSolid);
    XSetFunction(dpy,gc,GXcopy);
}

void xsetwhiteonblack(gc) GC gc;
{
    XSetForeground(dpy, gc, WhitePixel(dpy, screen));
    XSetBackground(dpy, gc, BlackPixel(dpy, screen));
    XSetFillStyle(dpy, gc, FillSolid);
    XSetFunction(dpy,gc,GXcopy);
}

void xsetforeground(gc, foreground) GC gc; unsigned long foreground;
{
    XSetForeground(dpy, gc, foreground);
}


void xsetbackground(gc, background) GC gc; unsigned long background;
{
    XSetBackground(dpy, gc, background);
}


void xsetlinewidth(gc, linewidth) GC gc; unsigned int linewidth;
{
    XSetLineAttributes(dpy, gc, linewidth, LineSolid, CapButt, JoinMiter);
}

void xsetfont(gc, font) GC gc; Font font;
{
    XSetFont(dpy, gc, font);
}

void xstorebytes(bytes, nbytes) char bytes[]; int nbytes;
{
  XStoreBytes(dpy, bytes, nbytes);
}

char *xfetchbytes(nbytes) int *nbytes;
{
  char *x;
  x = XFetchBytes(dpy, nbytes); 
  return x != NULL ? x : "";
}

int xgrabpointer(grab_window) 
          Window grab_window;
{

  return XGrabPointer(dpy,grab_window,False,
        ButtonPressMask|ButtonReleaseMask|
	EnterWindowMask|LeaveWindowMask|
        PointerMotionMask|ExposureMask| FocusChangeMask ,
        GrabModeAsync,GrabModeAsync,None,None,CurrentTime);
}

void xungrabpointer()
{
  XUngrabPointer(dpy,CurrentTime);
}

/* Color functions */
/* Return 24-bit RGB-value for colorname, -1 if invalid name */

int xparsecolor(spec) char *spec;
{
  XColor retdata;
  int st = XParseColor (dpy, colormap, spec, &retdata);
  unsigned short r, g, b;

  r = retdata.red>>8; g = retdata.green>>8; b = retdata.blue>>8;
/*
  printf ("Color: %s, (%d, %d, %d)\n",
	  spec, r, g, b); 
*/

  return st ? r*256*256+g*256+b: -1;
}

/* Allocate RO-pixel for RGB-value, return pixel-value */
int xalloccolor (RGB) int RGB;
{
  XColor color;
  int i;

  color.red = ((RGB>>16)&0xFF)<<8;
  color.green = ((RGB>>8)&0xFF)<<8;
  color.blue = (RGB&0xFF)<<8;

/*  printf ("Allocating (%d, %d, %d) - pixel %d",
	  color.red, color.green, color.blue, color.pixel); */

  if (XAllocColor (dpy, colormap, &color)) return color.pixel;
  else {
    if (!clientcminstalled) {
      for (i = 0; i < 256; i++) defaultcolors[i].pixel = i;
      XQueryColors (dpy, colormap, defaultcolors, 256);
      clientcminstalled = TRUE;
    }
    return -1;
  }
  
}

/* Get RGB-values of pixel */
int xquerycolor(pix) int pix;
{
  XColor color;
  int r, g, b;

  if (clientcminstalled) {
    r = defaultcolors[pix].red>>8;
    g = defaultcolors[pix].green>>8;
    b = defaultcolors[pix].blue>>8;
  }
  else {
    color.pixel = pix;
    XQueryColor (dpy, colormap, &color);
    r = color.red>>8; g = color.green>>8; b = color.blue>>8;
  }
  return r*256*256+g*256+b;
}

int xallocnamedcolor(cName) char * cName;
{
  XColor color, exact;
  if (XAllocNamedColor (dpy, colormap, cName, &exact, &color)) return 
color.pixel;
  else return -1;
}



/****************** PixMaps & BitMaps ******************/

/* Allocates bitmaps as well as pixmaps */

int xcreatepixmap (w, h, d) int w, h, d;
{
  return XCreatePixmap (dpy, DefaultRootWindow (dpy), w, h, d);
}

void xfreepixmap (pm) Pixmap pm;
{
  if (pm != None) XFreePixmap (dpy, pm);
}


void xreadpixmap (t, d, p, m, w, h)
  char *t; Drawable d; Pixmap *p, *m; int *w, *h;
{
  XpmAttributes attrs;
  attrs.visual = visual;
  attrs.colormap = colormap;
  attrs.depth = depth;
  attrs.closeness = 65535;
  attrs.valuemask = /*XpmVisual | XpmColormap | XpmDepth |*/ XpmCloseness;

  XpmReadFileToPixmap (dpy, d, t, p, m, &attrs);
  if (*p) {
    *w = attrs.width;;
    *h = attrs.height;
  }
  else printf ("Could not load picture %s\n", t);
}

void xreadbitmap (t, d, p, w, h) char *t; Drawable d; Pixmap *p; int *w, *h;
{
  int dummy;
  unsigned int width, height;
  if (XReadBitmapFile (dpy, d, t, &width, &height, p, &dummy, &dummy) == BitmapSuccess) {
    *w = (int) width;
    *h = (int) height;
  }
  else {
    *p = None;
    printf ("Could not load bitmap %s\n", t);
  }
}

void xsetclipmask (g, p) GC g; Pixmap p;
{
  XSetClipMask (dpy, g, p);
}

void xsetcliporigin (g, x, y) GC g; int x, y;
{
  XSetClipOrigin (dpy, g, x, y);
}

void xcopyplane(srcWindowID, dstWindowID, GCID, srcx, srcy, width, height, dstx, dsty) 
    Drawable srcWindowID, dstWindowID; GC GCID;
    int srcx, srcy, width, height, dstx, dsty;
{
  XCopyPlane (dpy, srcWindowID, dstWindowID, GCID, srcx, srcy, width, height,
	      dstx, dsty, 1);
}

/* -------------------- Oslo extra stuff ---------------- 

void xshapecombinemask (w, m) Window w; Pixmap m;
{
  XShapeCombineMask (dpy, w, ShapeBounding, 0, 0, m, ShapeSet);
}

void xgetdefault (progname, option, value) char *progname, *option, *value;
{
  char *c;

  for (c = progname; (*c) != ' ' && (*c) != '\0' && c-progname < 200-1; c++);
  (*c) = '\0';
  if ((c = strrchr (progname, '/'))) progname = c+1;
  c = XGetDefault (dpy, progname, option);
  if (c) strncpy (value, c, 200);
}


---------------------------------------------------------- */


/* Does not use any Xlib-calls, may be moved later */
int xxorcombinepixels (a, b) int a, b;
{
  return a^b;
}


