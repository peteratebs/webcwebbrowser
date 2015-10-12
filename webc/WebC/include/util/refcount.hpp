/*
|  REFCOUNT.HPP -
|
|  EBS -
|
|
|  Copyright EBS Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/

#ifndef __REFCOUNT_HPP__
#define __REFCOUNT_HPP__

class ReferenceCounted
{
 private:
	int mReferenceCount;

 public:
	ReferenceCounted (void);
	virtual ~ReferenceCounted (void);

 	virtual void claim   (void);
 	virtual void recycle (void) = 0;
// 	virtual void recycle (const char* file, int line);
 	virtual void release (void);
};

#endif /* __REFCOUNT_HPP__ */
