//
// Calibrate.cpp - WebC GUI Example
//
//
//  C code needed for the Calibrate page of the Spectrometer Demo.
//
// Public functions:
//
// CalibrateBodycb()
//  Called by SpectrometerDemoBodycb() when the document is loaded
//  	Assigns event handlers to the buttons in the html source with ids "DoneCalibration" and "CancelCalibration".
//  	Calls StartCalibration to set up graphics and timers to perform simulation.
//
// Internal functions:
//
//	StartCalibration 	 			- Set up graphics and set up a timer to perform simulation.
//	ContinueCalibration  			- Perform one sequence of the simulation.
//	CalibrateRearmTimer  			- Interface to webC timer API.
//	CalibrateDrawRealtime			- Animated simulation of data collect.
//	CalibrateDrawThermometerView 	- Animated Progress bar.
//
//  CalibrateAcceptHandler()	 -  C event handler assigned to the accept button.
//  CalibrateCancelHandler()	 -  C event handler assigned to the cancel button.
//
// Calls webC api functions:
// 	webc_DocFindElement,webc_ElementSetTagEventHandler,webc_BrowserQueueTimedEvent,webc_ImageGraphBeginDraw
// 	webc_ImageGraphSetPenColor,webc_ImageGraphDrawLine,webc_ImageGraphEndDraw,webc_ImageGraphFillRect
// 	webc_ElementSetStyleASCII


#include "htmlsdk.h"
#include "rtpstr.h"
#include "rtpprint.h"

extern int DemoModeEnabled;
int DemoCountdown;
void MainWindowHome(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc);

static void StartCalibration(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem);
static int ContinueCalibration(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem);
static void AbortCalibration(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem);
static void	SetCalibrationHandlers(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem);
static char *CalibrateStatusImage(int per_cent_complete);
static void CalibrateDrawRealtime(HELEMENT_HANDLE ImageElement, int per_cent_complete);
static void CalibrateDrawThermometerView(HELEMENT_HANDLE ImageElement, int per_cent_complete);

void PauseSlideShow(void);
void ResumeSlideShow(void);

HTMLEventStatus CalibrateBodycb(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEvent* event,char * param)
{
    switch (event->type)
    {
    case HTML_EVENT_LOAD:
    	DemoCountdown = 0;
		SetCalibrationHandlers(hbrowser, hdoc, helem);
		StartCalibration(hbrowser, hdoc, helem);
        break;
    case HTML_EVENT_UNLOAD:
		AbortCalibration(hbrowser, hdoc, helem);
        break;
    case HTML_EVENT_TIMER:
		ContinueCalibration(hbrowser, hdoc, helem);
        break;
    default:
            break;
    }
    return (HTML_EVENT_STATUS_CONTINUE);
}


static HTMLEventStatus CalibrateCancel(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEvent* event,char * param);
static HTMLEventStatus CalibrateFinish(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEvent* event,char * param);

static void	SetCalibrationHandlers(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem)
{
HELEMENT_HANDLE h;
	h = webc_DocFindElement (hdoc, "DoneCalibration", 0, HTML_ELEMENT_ANY, 0);
	webc_ElementSetTagEventHandler (h, CalibrateFinish);
	h = webc_DocFindElement (hdoc, "CancelCalibration", 0, HTML_ELEMENT_ANY, 0);
	webc_ElementSetTagEventHandler (h, CalibrateCancel);
}


static void UpdateCalibrationScreen(HBROWSER_HANDLE hbrowser, HELEMENT_HANDLE helem, HDOC_HANDLE hdoc, int per_cent_complete);
static void CalibrateRearmTimer(HBROWSER_HANDLE hbrowser, HELEMENT_HANDLE helem, char *data);
static void UpdateCalibrationDone(HBROWSER_HANDLE hbrowser, HELEMENT_HANDLE helem, HDOC_HANDLE hdoc);

static int timer_running = 0;
int per_cent_complete = 0;



static void StartCalibration(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem)
{
	 timer_running = 1;
	 per_cent_complete = 0;
	 PauseSlideShow();
	 UpdateCalibrationScreen(hbrowser, helem, hdoc, per_cent_complete);
     CalibrateRearmTimer(hbrowser, helem, 0);
}
// ContinueCalibration is attached to a timer and called every 40 milisonds.
static int ContinueCalibration(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem)
{	// Attached to a time by Called every 40 milisonds.
	if (timer_running)					// timer_running == 0 means we cancelled.
	{
		if (DemoCountdown)
		{
			if (!--DemoCountdown)
			{
				MainWindowHome(hbrowser, hdoc);
				timer_running = 0;
				return -1;
			}
			else
				CalibrateRearmTimer(hbrowser, helem, 0);
			return 0;
		}
		per_cent_complete += 2;
		if (per_cent_complete > 100)
		{ // Stop the animation
			ResumeSlideShow();
			UpdateCalibrationDone(hbrowser,helem, hdoc);
			if (DemoModeEnabled)
			{
				DemoCountdown = 40;
				CalibrateRearmTimer(hbrowser, helem, 0);
				timer_running = 1;
			}
			else
				timer_running = 0;
		}
		else
		{ // Continue the animation and restart the timer
			UpdateCalibrationScreen(hbrowser, helem, hdoc, per_cent_complete);
			CalibrateRearmTimer(hbrowser, helem, 0);
		}
	}
	return 0;
}
static void AbortCalibration(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem)
{
	 timer_running = 0;
}

// Draw the simulated data collect and the progress bar.
static void UpdateCalibrationScreen(HBROWSER_HANDLE hbrowser, HELEMENT_HANDLE helem, HDOC_HANDLE hdoc, int per_cent_complete)
{
HELEMENT_HANDLE h;
	h = webc_DocFindElement (hdoc, "CalibrateStatusCanvas", 0, HTML_ELEMENT_ANY, 0);
	if (h)
		CalibrateDrawRealtime(h, per_cent_complete);
	h = webc_DocFindElement (hdoc, "CalibrateStatus", 0, HTML_ELEMENT_ANY, 0);
	if (h)
		CalibrateDrawThermometerView(h, per_cent_complete);
}

#define CALIBRATEUPDATEPERIOD 40

// Schedule sending and HTML_EVENT_TIMER to the body 40 miliseconds from now.
static void CalibrateRearmTimer(HBROWSER_HANDLE hbrowser, HELEMENT_HANDLE helem, char *data)
{
HTMLEvent event;
	rtp_memset(&event, 0, sizeof(event));
	event.type = HTML_EVENT_TIMER;
	event.elem = event.target = event.srcElement = helem;
	webc_BrowserQueueTimedEvent (hbrowser,helem,&event,CALIBRATEUPDATEPERIOD);
}

// Some external that synthesise simulated signals.
extern "C" {
void InstrumentSignal(unsigned char* c, int Height, int Width);
void sinNoise(unsigned char *c, int decay, int frequency , int amp, int Height, int Width);
void triple_sin_noise(char * trip, int Height, int Width);
void rand_noise(unsigned char* c, char min, char max, int num);
void noisy_signal(unsigned char* output, int Height, int Width);
}

#define SIGNALWIDTH 300
#define SIGNALHEIGHT 100

static unsigned char NoiseBuffer[SIGNALWIDTH];
static unsigned char SignalBuffer[SIGNALWIDTH];
static void PlotSignalBuffer(HELEMENT_HANDLE ImageElement,unsigned char *SignalBuffer, int yorigin, int n_to_display);

#define FillColor 0x00F5C476 // Blueish
#define FillColor2 0x00E2FA9B
#define ThermometerColor 0x0051DB1A
#define NoiseColor 0x00FA4002 // Redish
#define LineColor  0xff0519F5
// #define BackColor  0x26F0B3
#define BackColor  0xffDCDEDC // 0xffffff

// Draw the main animation.
static void CalibrateDrawRealtime(HELEMENT_HANDLE ImageElement, int per_cent_complete)
{
int i, n_to_display, max_noise, min_noise;
static int CaptureBuffer=0;
	//webc_ImageGraphBeginDraw(ImageElement);
	//	webc_ImageGraphSetPenColor(ImageElement, 0x000000ff);
	//	webc_ImageGraphFillRect(ImageElement, 30, 30, 50, 50);
	//webc_ImageGraphEndDraw(ImageElement);
//return;
	webc_ImageGraphBeginDraw(ImageElement);
	//if (1 || per_cent_complete == 0)
	{
		webc_ImageGraphSetPenColor(ImageElement, BackColor);
		webc_ImageGraphFillRect(ImageElement, 0, 0, 320, 240);
	}
	// Numbers to control the simulation
	min_noise = 0;
	{
	double	dd = (double) per_cent_complete;
	dd /= 100.0;
	dd *= (double) SIGNALWIDTH;
	n_to_display = (int) dd;
	dd = (double) per_cent_complete;
	dd /= 100.0;
	dd *= (double) 25;
	max_noise = 25 - (int) dd;
	if (max_noise < 1)
		max_noise = 1;
	}
	if (n_to_display < 1)
		return;
	// If just starting generate a simulated signal scaled to a 300 by 100 pixel array
	if (CaptureBuffer==0)
		noisy_signal(SignalBuffer, SIGNALHEIGHT, SIGNALWIDTH);
	// Display a portion of the simulated signal.
	webc_ImageGraphSetPenColor(ImageElement, LineColor);
	for (i = 0; i < n_to_display-1; i++)
	{
		int x0,y0,x1,y1;
		x0 = i;
		x1 = i+1;
		y0 = 100-SignalBuffer[i];
		y1 = 100-SignalBuffer[i+1];
		webc_ImageGraphDrawLine(ImageElement, x0, y0, x1, y1);
	}
	// Display a portion of the simulated signal.
	webc_ImageGraphSetPenColor(ImageElement, FillColor);
	for (i = 0; i < n_to_display-1; i+=4)
	{
		int x0,y0,x1,y1;
		x0 = i;
		x1 = i;
		y0 = 125;
		y1 = 104-SignalBuffer[i];
		webc_ImageGraphDrawLine(ImageElement, x0, y0, x1, y1);
		webc_ImageGraphDrawLine(ImageElement, x0+1, y0, x1+1, y1);
	}
		// Display a portion of the simulated signal.
	webc_ImageGraphSetPenColor(ImageElement, FillColor2);
	for (i = 2; i < n_to_display-1; i+=4)
	{
		int x0,y0,x1,y1;
		x0 = i;
		x1 = i;
		y0 = 125;
		y1 = 104-SignalBuffer[i];
		webc_ImageGraphDrawLine(ImageElement, x0, y0, x1, y1);
		webc_ImageGraphDrawLine(ImageElement, x0+1, y0, x1+1, y1);
	}
	// Plot a screen full of simulated noise.
	// First undraw the previous noise.
	// Now generate and plot new noise. Save it so we can erase it cleanly.
	rand_noise( NoiseBuffer, min_noise, max_noise, SIGNALWIDTH);
	webc_ImageGraphSetPenColor(ImageElement, NoiseColor);
	if (per_cent_complete == 100)
		PlotSignalBuffer(ImageElement, NoiseBuffer, 10, SIGNALWIDTH);
	else
		PlotSignalBuffer(ImageElement, NoiseBuffer, 30, SIGNALWIDTH);

	webc_ImageGraphEndDraw(ImageElement);
	if (per_cent_complete==100)
		CaptureBuffer=0;
	else
		CaptureBuffer=1;

}
// Plot an array of points.
static void PlotSignalBuffer(HELEMENT_HANDLE ImageElement,unsigned char *SignalBuffer, int yorigin, int n_to_display)
{
	int i;
	for (i = 0; i < n_to_display-1; i++)
	{
		int x0,y0,x1,y1;
		x0 = i;
		x1 = i+1;
		y0 = yorigin-SignalBuffer[i];
		y1 = yorigin-SignalBuffer[i+1];
		webc_ImageGraphDrawLine(ImageElement, x0, y0, x1, y1);
	}
}


// Plot a "thermometer" status bar.
// Draws a horizontal bar proportional to the percentage completed.
static void CalibrateDrawThermometerView(HELEMENT_HANDLE ImageElement, int per_cent_complete)
{
int x0,y0,x1,y1;
static int CaptureBuffer=0;

	x0 = 12 + per_cent_complete*3;
	x1 = 12 + (per_cent_complete*3)+6;
	y0 = 20;
	y1 = 10;

	if (x1 > SIGNALWIDTH)
		return;
	webc_ImageGraphBeginDraw(ImageElement);
	webc_ImageGraphSetPenColor(ImageElement, ThermometerColor);
	webc_ImageGraphFillRect(ImageElement, 18, 10, (per_cent_complete*3+6), 10);
	// webc_ImageGraphDrawLine(ImageElement, x0-1, y0, x1-1, y1);
	// webc_ImageGraphDrawLine(ImageElement, x0, y0, x1, y1);
	// webc_ImageGraphDrawLine(ImageElement, x0+1, y0, x1+2, y1);
	webc_ImageGraphEndDraw(ImageElement);

}
// Change the buttons at the bottom of the display when the simulated data collect is done.
static void UpdateCalibrationDone(HBROWSER_HANDLE hbrowser, HELEMENT_HANDLE helem, HDOC_HANDLE hdoc)
{
HELEMENT_HANDLE h;
	h = webc_DocFindElement (hdoc, "CancelCalibration", 0, HTML_ELEMENT_ANY, 0);
	if (h)
		webc_ElementSetStyleASCII(h, "visibility:hidden;", true);
	h = webc_DocFindElement (hdoc, "RealTimeCalibration", 0, HTML_ELEMENT_ANY, 0);
	if (h)
		webc_ElementSetStyleASCII(h, "visibility:hidden;", true);
	h = webc_DocFindElement (hdoc, "DoneCalibration", 0, HTML_ELEMENT_ANY, 0);
	if (h)
		webc_ElementSetStyleASCII(h, "visibility:visible;", true);
	h = webc_DocFindElement (hdoc, "PlotCalibration", 0, HTML_ELEMENT_ANY, 0);
	if (h)
		webc_ElementSetStyleASCII(h, "visibility:visible;", true);
}
void AnimateImageEvent(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEventType eventType);


static HTMLEventStatus CalibrateCancel(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEvent* event,char * param)
{
	AnimateImageEvent(hbrowser, hdoc, helem, event->type);

    if (event->type == HTML_EVENT_CLICK)
	{

		AbortCalibration(hbrowser, hdoc, helem);
		MainWindowHome(hbrowser, hdoc);
	    return (HTML_EVENT_STATUS_DONE);

    }
	return (HTML_EVENT_STATUS_CONTINUE);

}
static HTMLEventStatus CalibrateFinish(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEvent* event,char * param)
{
	AnimateImageEvent(hbrowser, hdoc, helem, event->type);

    if (event->type == HTML_EVENT_CLICK)
	{
		MainWindowHome(hbrowser, hdoc);
	    return (HTML_EVENT_STATUS_DONE);
    }
	return (HTML_EVENT_STATUS_CONTINUE);

}
