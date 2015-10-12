#ifndef __REF_HPP_INCLUDED__
#define __REF_HPP_INCLUDED__

#ifdef __cplusplus

class Referable;

class ObjectAlias
{
private:
	unsigned long miUseCount;
	Referable *mpObject;

public:
	ObjectAlias(Referable *ptr, unsigned long count);

	static ObjectAlias *AllocObject(Referable *ptr, unsigned long count);
	static void FreeObject(ObjectAlias *ref);
	
	void Set(Referable *ptr);
	Referable *Get(void);
	void Claim(void);
	void Release(void);	
};

class Referable
{
private:
	ObjectAlias * mpAlias;

public:
	Referable (void);
	virtual ~Referable (void);
	
	ObjectAlias * ClaimReference (void);	
};

template<class T> class ObjectReference
{
private:
	ObjectAlias *mpAlias;

public:
	ObjectReference(void);
	ObjectReference(T *obj);
	~ObjectReference(void);	

	T *Get(void);
	void Set(T *obj);
};

#endif

#endif // __REF_HPP_INCLUDED__
