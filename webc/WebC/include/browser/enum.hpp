#ifndef __ENUMERATOR_INCLUDED__
#define __ENUMERATOR_INCLUDED__

class Enumerator
{
public:
	virtual unsigned char * Next (void) = 0;
	virtual void Dispose (void) = 0;
};

#endif // __ENUMERATOR_INCLUDED__
