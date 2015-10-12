#ifndef __WTSLIDER_HPP__
#define __WTSLIDER_HPP__

#include "delement.hpp"
#include "listener.hpp"

#define MINIMUM_SLIDER_SIZE 16
#define MAXIMUM_SLIDER_SIZE 10000 // No maximum
enum WebSliderState
{
	STATE_NORMAL = 0,
	STATE_PULL_UP,
	STATE_PULL_DOWN,
	STATE_DRAGGING
};

enum SliderPointType
{
	POINT_INSIDE = 0,           // inside the slider thing
	POINT_BEFORE,               // pull up
	POINT_AFTER,                // pull down
	POINT_ALONGSIDE,            // alongside the main axis
	POINT_NONE                  // outside the widget
};

class WebSlider : public DisplayElement
{
protected:
	long position;
	long range;
	long window;
	long step;
	long initialDragPosition;
	Listener * listener;


	WebSliderState state;

	void BeginDrag (DISPLAY_INT init);
	void EndDrag (void);
	void BeginPullUp (void);
	void BeginPullDown (void);
	void EndPullUp (void);
	void EndPullDown (void);
	void ClampPosition(void);
	virtual void GetTotalRange(DISPLAY_INT* slideMin, DISPLAY_INT* slideMax) = 0;

	virtual void DrawThisOnly(DISPLAY_INT x, DISPLAY_INT y, WebGraphics * gc);
	virtual void DrawSlider(WebRect* sliderRect, WebGraphics* gc);
	virtual DISPLAYElementType Type (void) { return DISPLAY_ELEMENT_WEBSLIDER; }

	WebColor backgroundColor,sliderColor, sliderHi, sliderLo;

public:
	WebSlider ();
	virtual ~WebSlider ();

	void SetPosition(long i);
	void SetRange(long r);
	void SetWindow(long w);
	void SetStep(long s);

	long GetPosition(void);

	long GetRange(void);
	virtual long GetWindow(void);
	long GetStep(void);

	void Up (void);
	void Down (void);
	void PageUp(void);
	void PageDown(void);
	void SetListener(Listener * l);
	WebSliderState GetSliderState (void) { return state; }

	virtual void GetSliderRange(DISPLAY_INT* start, DISPLAY_INT* end);

};

class WebVSlider : public WebSlider
{
protected:
	virtual void GetTotalRange(DISPLAY_INT* slideMin, DISPLAY_INT* slideMax);

public:
	WebVSlider ();
	virtual ~WebVSlider ();

	HTMLEventStatus Event (HTMLEvent *e);
	void DrawThisOnly (DISPLAY_INT x, DISPLAY_INT y, WebGraphics *gc);
	SliderPointType ClassifyPoint(DISPLAY_INT x, DISPLAY_INT y);
	void DragTo (DISPLAY_INT pos);
	long GetWindow(void);


};

class WebHSlider : public WebSlider
{
protected:
	virtual void GetTotalRange(DISPLAY_INT* slideMin, DISPLAY_INT* slideMax);

public:
	WebHSlider ();
	virtual ~WebHSlider ();

	HTMLEventStatus Event (HTMLEvent *e);
	void DrawThisOnly (DISPLAY_INT x, DISPLAY_INT y, WebGraphics *gc);
	SliderPointType ClassifyPoint(DISPLAY_INT x, DISPLAY_INT y);
	void DragTo (DISPLAY_INT pos);
	long GetWindow(void);


};

#endif // __WTSLIDER_HPP__
