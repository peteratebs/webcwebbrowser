/*
|  jsParamUtils. - utilities for parsing JavaScript parameters
|
|  EBS - WebC
|
|  Copyright EBS Inc. , 2009
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
|
|  Module description:
|   [tbd]
*/

#include "jscntxt.h"
#include "jhutils.h"

class jsVarUtil
{
	public:
	jsVarUtil(JSContext *cx=0, jsval vp=0)
	{
		m_cx = cx;
		m_vp = vp;
		m_jsStr = 0;
	}

	~jsVarUtil()
	{
		if(m_jsStr)
		{
			JS_RemoveRoot(m_cx, &m_jsStr);
		}
	}

	void setVal(jsval vp)
	{
		m_vp = vp;
	}

	WEBC_CHAR *toString()
	{
		if (JSVAL_IS_STRING(m_vp))
		{
			m_jsStr = JSVAL_TO_STRING(m_vp);
		}
		else
		{
			m_jsStr = JS_ValueToString(m_cx, m_vp);
		}

		JS_AddRoot(m_cx, &m_jsStr);

		return WEBC_JS_STRING_TO_WEBC_STRING(m_jsStr);
	}

	WEBC_BOOL toBool()
	{
		JSBool temp=0;
		if (JSVAL_IS_BOOLEAN(m_vp))
		{
			temp = JSVAL_TO_BOOLEAN(m_vp);
		}

		return ((temp == JS_TRUE) ? WEBC_TRUE : WEBC_FALSE);
	}

	protected:
		JSContext *m_cx;
		jsval m_vp;
		JSString *m_jsStr;
};
