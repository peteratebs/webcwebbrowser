// VersionInfo.h: CVersionInfo interface
/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2000, DevelopMentor.
// All rights reserved. No warranties extended.
// You may use them without royalty (and without support) in your compiled
// code. You may not redistribute the source code.
/////////////////////////////////////////////////////////////////////////////

#ifndef __VERSIONINFO_H__
#define __VERSIONINFO_H__

#include <tchar.h>

#pragma comment(lib, "version.lib")
#pragma comment(lib, "user32.lib")  // wsprintf

#if defined(__AFXWIN_H__)

// MFC
#define GetModuleInstance() AfxGetResourceHandle()
#define GetResourceInstance() AfxGetResourceHandle()

#elif defined(__ATLBASE_H__)

// ATL
#define GetModuleInstance() _Module.GetModuleInstance()
#define GetResourceInstance() _Module.GetResourceInstance()

#else

// Raw Win32
#define GetModuleInstance() GetModuleHandle(0)
#define GetResourceInstance() GetModuleHandle(0)

#endif

class CVersionInfo
{
public:
    CVersionInfo(LPCTSTR pszTrans = __T("040904B0"))
    :   m_pVerInfo(0)
    {
        _ASSERTE(_tcslen(pszTrans) < sizeof(m_szTrans)/sizeof(*m_szTrans));
        _tcscpy(m_szTrans, pszTrans);
    }

    HRESULT LoadVersion(HINSTANCE hinst = GetResourceInstance())
    {
        TCHAR   szModule[MAX_PATH+1];
        if( !GetModuleFileName(hinst, szModule, MAX_PATH) ) return HRESULT_FROM_WIN32(GetLastError());
        return LoadVersion(szModule);
    }

    HRESULT LoadVersion(LPCTSTR pszModule)
    {
        _ASSERTE(pszModule && *pszModule);

        Clear();

        DWORD   nVerInfo = 0;
        DWORD   nVerInfoSize = GetFileVersionInfoSize(const_cast<LPTSTR>(pszModule), &nVerInfo);
        if( !nVerInfoSize ) return E_FAIL;

        m_pVerInfo = new BYTE[nVerInfoSize];
        if( !m_pVerInfo ) return E_OUTOFMEMORY;

        if( !GetFileVersionInfo(const_cast<LPTSTR>(pszModule), nVerInfo, nVerInfoSize, m_pVerInfo) )
        {
            return HRESULT_FROM_WIN32(GetLastError());
        }

        return S_OK;
    }

    void Clear()
    {
        delete[] m_pVerInfo;
        m_pVerInfo = 0;
    }
    
    ~CVersionInfo()
    {
        Clear();
    }
    
    LPCTSTR QueryValueString(LPCTSTR pszVerInfo) const
    {
        _ASSERTE(pszVerInfo && *pszVerInfo);
        _ASSERTE(m_pVerInfo);
        
        LPTSTR  pszBuf = 0;
        UINT    nBufSize = 0;
        TCHAR   szQuery[128];
        wsprintf(szQuery, __T("\\StringFileInfo\\%s\\%s"), m_szTrans, pszVerInfo);

        if( VerQueryValue(m_pVerInfo, szQuery, (void**)&pszBuf, &nBufSize) )
        {
            return pszBuf;
        }

        return 0;
    }

    operator bool()
    {
        return m_pVerInfo != 0;
    }
    
private:
    TCHAR   m_szTrans[16];
    void*   m_pVerInfo;
};

#undef GetModuleInstance
#undef GetResourceInstance

#endif  // __VERSIONINFO_H__
