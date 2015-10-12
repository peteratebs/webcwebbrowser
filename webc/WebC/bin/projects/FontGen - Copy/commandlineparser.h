// CommandLineParser.h: StandardCommandLineParser et al
/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2000-2001, DevelopMentor.
// All rights reserved. No warranties extended.
// You may use them without royalty (and without support) in your compiled
// code. You may not redistribute the source code.
// Inspired by Josh Gray.
/////////////////////////////////////////////////////////////////////////////
// Features:
// -Parsing command line args from a single string or argv/argc
// -Simple flags and flag/arg pairs
// -Typed args, both params and flags
// -Building usages on the fly
// -Command line args can be read via @argfile.
// -Automatically generates the banner logo from the VERSIONINFO resource.
// -Case insensitive flag comparisions by default, case sensitive as an option.
/////////////////////////////////////////////////////////////////////////////
// Limitations:
// -Requires flags to be space separated, e.g.
//  /efg is a single flag, even if e, f and g are defined and efg isn't
// -Requires flag and value to be space separated, e.g.
//  /fconfig.sys is treated as a single flag, even if f is defined to take
//  a value and fconfig.sys isn't defined
// -Need a good way for FileNameValue to communicate a file not found error
//  if we give it a MustExist property. No need to make the calling program
//  handle that usage error when we handle all the others. Specializing
//  ValueArg would be a slippery slope (what about MultiValueArg?) *plus*
//  Argument::ConsuleValue doesn't get to participate in the error reporting
//  anyway. Exceptions are probably the way to go, since we're relying on
//  the standard library so much anyway...
// -Need better formatting to pad flag/param names that are larger than
//  16 characters and to support multi-line descriptions.
/////////////////////////////////////////////////////////////////////////////
// History:
//  8/17/01:
//  -Tim Tabor [tltabor@earthlink.net] contributed the ability to be able to
//   set a default value for a FileNameValue. He also pointed out a const bug.
//   Thanks, Tim!
//
//  5/2/01:
//  -Paul Westcott [paul_westcott@hotmail.com] added a class 
//   RestrictedValueArg class which allows a list of values to be specified.
//
//  4/4/01:
//  -Chris Tavares [ctavares@develop.com] fixed a nasty memory leak that the
//   compiler mistakenly reported as a warning. Thanks, Chris!
//
//  3/29/01:
//  -Added support for case sensitivity by default, case insensitive as an option.
//
//  3/25/01:
//  -Unicode support from Johan Nilsson (johan.nilsson@esrange.ssc.se).
//  -Unicode arg file support, also from Johan.
//  -Bug fix (you guess it, Johan...) to avoid pushing the leading exe name if
//   there are no command line args.
//
//  12/22/00:
//  -Bug fixes
//
//  10/16/00:
//  -Tim Tabor [tltabor@earthlink.net] pointed out IMAGE_NT_SIGNATURE to me.
//  -Changed SimpleFlag to FlagArg.
//  -Fixed some usage formatting problems.
//  -Added some typedefs.
//  -Removed /nologo from StandardCommandLineParser (it's not used)
//
//  10/15/00:
//  -Initial release
/////////////////////////////////////////////////////////////////////////////
// Usage:
/*
#include "CommandLineParser.h"

struct TestCommandLine : public StandardCommandLineParser
{
public:
    // Create flags and params
    FlagArg                         debug;
    ValueArg<int>                   minX;
    ValueArg<double>                intensity;
    ValueArg<tstring>               foo;
    MultiValueArg<tstring>          i;
    MultiValueArg<FileNameValue>    files;

public:
    TestCommandLine()
        // Set names and descriptions for usage
    :   debug(__T("debug"), __T("Turn on debugger.")),
        minX(__T("MinX"), __T("Minimum x value.")),
        intensity(__T("Intensity"), __T("Intensity value.")),
        foo(__T("foo"), __T("Not the bar.")),
        i(__T("i"), __T("Include directories.")),
        files(__T("files"), __T("File names."))
    {
        // Add flags matched by name, e.g. /foo
        AddFlag(debug);

        AddFlag(minX);
        minX.SetDefaultValue(10);

        AddFlag(intensity);
        //intensity.SetDefaultValue(1.1);   // No default means this arg is required

        AddFlag(i);
        AddFlag(foo);
        foo.SetDefaultValue(__T("bar"));

        // Add params matched by position, e.g. foo
        AddParam(files);
    }
};

void _tmain(int argc, LPCTSTR* argv)
{
    // Parse the command line
    TestCommandLine cl;
    cl.Parse(argc, argv);

    // Will show usage or version if there's an error, /?|h|help or /v|version
    if( !cl.Continue() ) return;

    // Harvest values
    std::cout << "debug: " << cl.debug << std::endl;
    std::cout << "minX: " << cl.minX << std::endl;
    std::cout << "intensity: " << cl.intensity << std::endl;
    std::cout << "foo: " << cl.foo.Value() << std::endl;

    const std::vector<tstring>& includes = cl.i;
    std::cout << "includes:" << std::endl;
    for(std::vector<tstring>::const_iterator incl = includes.begin(); incl != includes.end(); ++incl)
        std::cout << "    '" << *incl << "'" << std::endl;

    const std::vector<FileNameValue>& params = cl.files.Values();
    std::cout << "nameValuePairs:" << std::endl;
    for(std::vector<FileNameValue>::const_iterator p = params.begin(); p != params.end(); ++p)
        std::cout << "    '" << *p << "'" << (p->Exists() ? " (exists)" : " (doesn't exist)") << std::endl;
}
*/

#pragma once
#ifndef INC_COMMANDLINEPARSER
#define INC_COMMANDLINEPARSER

#pragma warning(disable:4786) // identifier was truncated to '255' characters in the debug information
#pragma warning(disable : 4311)

#include <windows.h>
#include <crtdbg.h> // _ASSERTE
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>	// replace_if
#include <functional>	// bind2nd, equal_to
#include <shellapi.h>   // SHGetFileInfo
#include "versionInfo.h"
#pragma comment(lib, "shell32.lib")

#ifndef __TSTRING_H__
#include <tchar.h>
#include <string>
#include <sstream>
typedef std::basic_string<TCHAR> tstring;
typedef std::basic_stringstream<TCHAR> tstringstream;
#endif

// Unicode console output
#ifdef _UNICODE
std::wostream& tcout = std::wcout;
#else
std::ostream& tcout = std::cout;
#endif

//
// NOTE: We are using this because I couldn't figure out how to
//       overload operator>> for FileNameValue.
template <typename T>
bool ReadFromString(const tstring& s, T* pValue)
{
    tstringstream   ss(s);
    ss >> *pValue;
    return true;
}

template <>
bool ReadFromString(const tstring& s, tstring* pValue)
{
    // We need this specialization so that we the
    // entire string, including spaces. Otherwise,
    // operator>>(istream, s) will just take up to
    // the first space.
    *pValue = s;
    return true;
}

class Argument
{
public:
    Argument(const tstring& sName, const tstring& sDescription = __T(""))
    :   m_bFound(false),
        m_bCaseSensitive(false),
        m_sDescription(sDescription)
    {
        _ASSERTE(!sName.empty());
        AddAlternate(sName);
    }

    void AddAlternate(const tstring& sName)
    {
        _ASSERTE(!Matches(sName));
        m_rgNames.push_back(sName);
    }

    tstring Name() const
    {
        _ASSERTE(!m_rgNames.empty());
        return m_rgNames[0];
    }

    tstring sDescription() const
    {
        return m_sDescription;
    }

    virtual ~Argument()
    {
    }

    bool Found() const
    {
        return m_bFound;
    }

    void SetFound(bool bFound = true)
    {
        m_bFound = bFound;
    }

    bool Matches(const tstring& sName) const
    {
        // Choose comparitor based on case sensitivity needs
        int (*cmp)(LPCTSTR, LPCTSTR) = (m_bCaseSensitive ? _tcscmp : _tcsicmp);

        for( std::vector<tstring>::const_iterator it = m_rgNames.begin(); it != m_rgNames.end(); ++it )
        {
            if( cmp(it->c_str(), sName.c_str()) == 0 ) return true;
        }

        return false;
    }

    void SetCaseSensitive(bool bCaseSensitive = true)
    {
        m_bCaseSensitive = bCaseSensitive;
    }

public:
    virtual bool IsValid() const =0;
    virtual bool ExpectsValues() const =0;
    virtual bool WantsAValue() const =0;
    virtual bool ConsumeValue(const tstring& s) =0;
    virtual tstring Usage(const tstring sPrefix) const =0;
    virtual tstring LongUsage(const tstring sPrefix) const =0;

protected:
    tstring UsageHelper(const tstring& sPrefix = __T("/"), bool bOptional = true, const tstring& sValueName = __T("")) const
    {
        tstring sUsage;
        if( bOptional ) sUsage += __T("[");
        sUsage += sPrefix;

        // If there's no prefix, assume a param instead of a flag,
        // and wrap the name (great for names with spaces)
        bool    bWrapName = sPrefix.size() == 0;
        if( bWrapName && !bOptional ) sUsage += __T("<");

        tstring sPipe;
        for( std::vector<tstring>::const_iterator it = m_rgNames.begin(); it != m_rgNames.end(); ++it )
        {
            sUsage += sPipe + *it;
            sPipe = __T("|");
        }

        if( !sValueName.empty() ) sUsage += __T(" <") + sValueName + __T(">");
        if( bWrapName && !bOptional ) sUsage += __T(">");
        if( bOptional ) sUsage += __T("]");
        return sUsage;
    }

    tstring LongUsageHelper(const tstring& sPrefix, const tstring& sValueName = __T("")) const
    {
        if( m_sDescription.empty() ) return __T("");

        bool    bFlag = !sPrefix.empty();
        tstring sUsage;
        sUsage += sPrefix;
        sUsage += Name();
        if( bFlag && !sValueName.empty() ) sUsage += __T(" <") + sValueName + __T(">");

        // Line all descriptions up on two tabs
        if( sUsage.size() < 8 ) sUsage += __T('\t');
        sUsage += __T('\t');
        sUsage += m_sDescription;

        return sUsage;
    }

protected:
    bool                    m_bFound:1;
    bool                    m_bCaseSensitive:1;
    std::vector<tstring>    m_rgNames;
    tstring                 m_sDescription;
};

class FlagArg : public Argument
{
public:
    FlagArg(const tstring& sName, const tstring& sDescription = __T(""))
    :   Argument(sName, sDescription)
    {
    }

    virtual bool IsValid() const
    {
        return true;
    }

    virtual bool ExpectsValues() const
    {
        return false;
    }

    virtual bool WantsAValue() const
    {
        _ASSERTE(false && "FlagArgs don't consume values");
        return false;
    }

    virtual bool ConsumeValue(const tstring& sValue)
    {
        _ASSERTE(false && "FlagArgs don't consume values");
        return false;
    }

    virtual tstring Usage(const tstring sPrefix) const
    {
        return UsageHelper(sPrefix, true, __T(""));
    }

    virtual tstring LongUsage(const tstring sPrefix) const
    {
        return LongUsageHelper(sPrefix);
    }

    operator bool() const
    {
        return Found();
    }
};

template <typename T>
class ValueArg : public Argument
{
public:
    ValueArg(const tstring& sName, const tstring& sDescription = __T(""), const tstring& sValueName = __T("value"))
    :   Argument(sName, sDescription), m_sValueName(sValueName), m_bHasDefault(false)
    {
    }

    void SetDefaultValue(const T& value)
    {
        m_value = value;
        m_valueDefault = value;
        m_bHasDefault = true;
    }

    virtual bool IsValid() const
    {
        return m_bHasDefault || Found();
    }

    virtual bool ExpectsValues() const
    {
        return true;
    }

    virtual bool WantsAValue() const
    {
        return !Found();
    }

    virtual bool ConsumeValue(const tstring& s)
    {
        return ReadFromString(s, &m_value);
    }

    virtual tstring Usage(const tstring sPrefix) const
    {
        bool    bFlag = sPrefix.size() != 0;
        return UsageHelper(sPrefix, m_bHasDefault, bFlag ? m_sValueName : __T(""));
    }

    virtual tstring LongUsage(const tstring sPrefix) const
    {
        return LongUsageHelper(sPrefix, m_sValueName);
    }

    operator const T&() const
    {
        return m_value;
    }

    const T& Value() const
    {
        return m_value;
    }

protected:
    tstring m_sValueName;
    bool    m_bHasDefault;
    T       m_valueDefault;
    T       m_value;
};

template <typename T>
class RestrictedValueArg : public ValueArg<T>
{
protected:
    typedef std::pair<T, tstring>           RestrictedValue;
    typedef std::vector<RestrictedValue>    RestrictedValues;

public:
    RestrictedValueArg(const tstring& sName, const tstring& sDescription = __T(""), const tstring& sValueName = __T("value"))
    :   ValueArg<T>(sName, sDescription, sValueName),
        m_bRestrictedValueDescriptions(false)
    {
    }

    virtual bool IsValid() const
    {
        RestrictedValues::const_iterator it;
        const RestrictedValues::const_iterator last = m_rgValues.end();
        for( it = m_rgValues.begin(); it != last; ++it )
        {
            if( it->first == m_value ) return true;
        }

        return false;
    }

    virtual tstring Usage(const tstring& sPrefix) const
    {
        tstring sUsage = ValueArg<T>::Usage(sPrefix);

        if( !m_rgValues.empty() )
        {
            bool            bFirst = true;
            tstringstream   ssValues;

            RestrictedValues::const_iterator        it;
            const RestrictedValues::const_iterator  end = m_rgValues.end();
            for( it = m_rgValues.begin(); it != end; ++it )
            {
                if( bFirst) bFirst = false;
                else ssValues << _T('|');

                ssValues << it->first;
            }

            const tstring       sValue(_T("<value>"));
            tstring::size_type  pos = sUsage.find(sValue);
            if( tstring::npos != pos )
            {
                sUsage.replace(sUsage.begin() + pos, sUsage.begin() + pos + sValue.size(), ssValues.str());
            }
            // else something is wrong; UsageHelper() must have changed
        }

        return sUsage;
    }

    virtual tstring LongUsage(const tstring sPrefix) const
    {
        tstring sUsage = ValueArg<T>::LongUsage(sPrefix);

        if( m_bRestrictedValueDescriptions )
        {
            sUsage += _T(" values are:");

            RestrictedValues::const_iterator        it;
            const RestrictedValues::const_iterator  end = m_rgValues.end();
            for( it = m_rgValues.begin(); it != end; ++it )
            {
                sUsage += _T("\n");

                const size_t    LEFT_PAD = 3;
                const size_t    TOTAL_WITH_PADDING = 15;
                tstring         sValue(LEFT_PAD, _T(' '));
                sValue += it->first;

                if( sValue.size() < TOTAL_WITH_PADDING )
                {
                    sValue.resize(TOTAL_WITH_PADDING, _T(' ')); 
                }

                tstringstream   ssValues;
                ssValues << _T("\t\t") << sValue << _T(" ") << it->second;

                sUsage += ssValues.str();
            }
        }

        return sUsage;
    }

    void AddValue(const T& value, const tstring& sDescription = tstring())
    {
        if( sDescription.empty() ) m_bRestrictedValueDescriptions = true;
        m_rgValues.push_back(RestrictedValue(value, sDescription));
    }

protected:
    RestrictedValues    m_rgValues;
    bool                m_bRestrictedValueDescriptions;
};

template <typename T, size_t nMinValues = 0, size_t nMaxValues = size_t(-1)>
class MultiValueArg : public Argument
{
public:
    MultiValueArg(const tstring& sName, const tstring& sDescription = __T(""), const tstring& sValueName = __T("value"))
    :   Argument(sName, sDescription), m_sValueName(sValueName)
    {
    }

    virtual bool IsValid() const
    {
        return m_rgValues.size() >= nMinValues;
    }

    virtual bool ExpectsValues() const
    {
        return true;
    }

    virtual bool WantsAValue() const
    {
        return m_rgValues.size() < nMaxValues;
    }

    virtual bool ConsumeValue(const tstring& s)
    {
        T       value;
        bool    b = ReadFromString(s, &value);
        if( !b ) return b;

        m_rgValues.push_back(value);
        return true;
    }

    virtual tstring Usage(const tstring sPrefix) const
    {
        bool    bFlag = sPrefix.size() != 0;
        return UsageHelper(sPrefix, true, bFlag ? __T("value") : __T("")) + __T("...");
    }

    virtual tstring LongUsage(const tstring sPrefix) const
    {
        return LongUsageHelper(sPrefix, m_sValueName);
    }

    size_t size() const
    {
        return m_rgValues.size();
    }

    operator const std::vector<T>&() const
    {
        return m_rgValues;
    }

    const std::vector<T>& Values() const
    {
        return m_rgValues;
    }

    const T& operator[](size_t i) const
    {
        _ASSERTE(i < m_rgValues.size());
        return m_rgValues[i];
    }

    const T& Value(size_t i) const
    {
        _ASSERTE(i < m_rgValues.size());
        return m_rgValues[i];
    }

protected:
    tstring         m_sValueName;
    std::vector<T>  m_rgValues;
};

// Resolves to full path name based on current working directory.
// For use with [Multi]ValueArg.
class FileNameValue : public tstring
{
public:
	FileNameValue() {}
	FileNameValue(const tstring& sFileName) : tstring(sFileName) {}
	FileNameValue(LPCTSTR pszFileName) : tstring(pszFileName) {}

    bool Exists() const
    {
        DWORD dwAttrib = GetFileAttributes(c_str());
        return (dwAttrib != -1) && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
    }
};


inline
bool ReadFromString(const tstring& s, FileNameValue* pValue)
{
    _ASSERTE(pValue);

    // Resolve to full path name at current working directory
    TCHAR   szPath[MAX_PATH+1];
    GetFullPathName(s.c_str(), MAX_PATH, szPath, 0);

    pValue->assign(szPath);
    return true;
}

// Splits left=right into left and right.
// For use with [Multi]ValueArg.
class PairValue
{
public:
    tstring Left() const
    {
        return m_sLeft;
    }

    tstring Right() const
    {
        return m_sRight;
    }

    friend
    bool ReadFromString(const tstring& s, PairValue* pValue);

private:
    tstring m_sLeft;
    tstring m_sRight;
};

inline
bool ReadFromString(const tstring& s, PairValue* pValue)
{
    _ASSERTE(pValue);

    // Split from left=right to left and right
    size_t  equals = s.find(__T('='));
    pValue->m_sLeft = s.substr(0, equals);
    if( equals != tstring::npos ) pValue->m_sRight = s.substr(equals + 1);
    else pValue->m_sRight.erase();

    return true;
}

class CommandLineParser
{
public:
    CommandLineParser(bool bAllowArgFile = true)
        : m_bAllowArgFile(bAllowArgFile)
    {
    }

    void AddFlag(Argument& arg)
    {
        _ASSERTE(!m_rgArgInfos.FindFlag(arg.Name()) && "Can't add two flags with the same name");
        m_rgArgInfos.push_back(ArgInfo(ARG_FLAG, &arg));
    }

    void AddParam(Argument& arg)
    {
        m_rgArgInfos.push_back(ArgInfo(ARG_PARAM, &arg));
    }

    tstring Usage(const tstring& sErr = __T("")) const
    {
        tstringstream   ssUsage;

        // Logo
        tstring sLogo = Logo();
        if( sLogo.size() )
        {
            ssUsage << Logo() << std::endl;
        }

        // Error string
        if( !sErr.empty() )
        {
            ssUsage << sErr << std::endl << std::endl;
        }
        else if( m_sError.size() )
        {
            ssUsage << m_sError << std::endl << std::endl;
        }

        // Short (name and default value only)
        ssUsage << __T("Usage: ") << ModuleName();

        if( m_bAllowArgFile ) ssUsage << __T(" ") << __T("[@argfile]");

        for( ArgInfoContainer::const_iterator it = m_rgArgInfos.begin(); it != m_rgArgInfos.end(); ++it )
        {
            _ASSERTE((it->type == ARG_FLAG) || (it->type == ARG_PARAM));
            _ASSERTE(it->pArg);
            ssUsage << __T(" ") << it->pArg->Usage(it->type == ARG_FLAG ? __T("/") : __T(""));
        }

        ssUsage << std::endl;
        ssUsage << std::endl;

        // Long (name and sDescription)
        if( m_bAllowArgFile ) ssUsage << __T("@argfile\tRead arguments from a file.") << std::endl;

        for( ArgInfoContainer::const_iterator it = m_rgArgInfos.begin(); it != m_rgArgInfos.end(); ++it )
        {
            _ASSERTE((it->type == ARG_FLAG) || (it->type == ARG_PARAM));
            _ASSERTE(it->pArg);
            tstring sLongUsage = it->pArg->LongUsage(it->type == ARG_FLAG ? __T("/") : __T(""));
            if( sLongUsage.size() ) ssUsage << sLongUsage << std::endl;
        }

        return ssUsage.str();
    }

    bool Parse(int argc, const TCHAR** argv, bool bValidate = true, bool bLeadingExe = true)
    {
        std::vector<tstring> Params;
        for(int i = (bLeadingExe ? 1 : 0); i < argc; ++i) Params.push_back(argv[i]);
        return Parse(Params, bValidate);
    }

    bool Parse(const tstring & CommandLine, bool bValidate = true, bool bLeadingExe = false)
    {
        std::vector<tstring> Params;
        tstring CurParam;

        bool bInQuotes = false;
        for(tstring::const_iterator i = CommandLine.begin(); i != CommandLine.end(); ++i)
        {
            if (*i == __T('"'))
            {
                bInQuotes = bInQuotes ? false : true;
                continue;
            }

            if (IsWhiteSpace(*i) && !bInQuotes)
            {
                if (!CurParam.empty())
                {
                    if( bLeadingExe ) bLeadingExe = false;
                    else Params.push_back(CurParam);
                    CurParam = __T("");
                }
                continue;
            }

            CurParam += *i;
        }

        // Check for bLeadingExe to avoid pushing module file
        // name if no parameters were supplied.
        if( !CurParam.empty() && !bLeadingExe )
        {
            Params.push_back(CurParam);
        }

        return Parse(Params, bValidate);
    }

    // Assumes exe name *not* in the first in the list
    bool Parse(const std::vector<tstring>& rgsArgs, bool bValidate = true)
    {
        ClearError();

        for(std::vector<tstring>::const_iterator psArg = rgsArgs.begin(); psArg != rgsArgs.end(); ++psArg)
        {
#ifdef _DEBUG
            OutputDebugString(__T("Processing arg: "));
            OutputDebugString(psArg->c_str());
            OutputDebugString(__T("\n"));
#endif
            Argument*   pArg = 0;
            bool        bIsFlag = false;

            // It's a flag
            if( (psArg->size() > 1) && (psArg->at(0) == __T('/') || psArg->at(0) == __T('-')) )
            {
                // Find the argument by name
                pArg = m_rgArgInfos.FindFlag(psArg->substr(1));
                bIsFlag = true;
            }
            // It's a file name to process parameters from
            else if( psArg->size() > 1 && psArg->at(0) == __T('@') && m_bAllowArgFile )
            {
                if( !ParseFromFile(psArg->substr(1)) ) return false;
                continue;
            }
            // It's a parameter
            else
            {
                // Find the argument by offset
                pArg = m_rgArgInfos.GetNextParam();
            }
        
            if( !pArg )
            {
                m_sError = __T("Unrecognized argument '") + *psArg + __T("'.");
                return false;
            }

            _ASSERTE(pArg);

            // Argument with a value, e.g. /foo bar
            if( pArg->ExpectsValues() )
            {
                if( bIsFlag && (++psArg == rgsArgs.end()) )
                {
                    m_sError = __T("Argument '") + pArg->Name() + __T("' expects a parameter.");
                    return false;
                }
                
                if( !pArg->WantsAValue() )
                {
                    m_sError = __T("Argument '") + pArg->Name() + __T("' cannot accept another parameter: '") + *psArg + __T("'.");
                    return false;
                }
                
                if( !pArg->ConsumeValue(*psArg) )
                {
                    m_sError = __T("Argument '") + pArg->Name() + __T("' cannot accept parameter: '") + *psArg + __T("'.");
                    return false;
                }
            }
            // Argument w/o a value, e.g. /foo
            else
            {
                if( pArg->Found() )
                {
                    m_sError = __T("Argument '") + pArg->Name() + __T("' already present.");
                    return false;
                }
            }
            
            pArg->SetFound(true);
        }
        
        // Check for missing required arguments
        for( ArgInfoContainer::iterator it = m_rgArgInfos.begin(); it != m_rgArgInfos.end(); ++it )
        {
            _ASSERTE(it->pArg);

            if( !it->pArg->IsValid() )
            {
                m_sError = __T("Expected argument '") + it->pArg->Name() + __T("'.");
                return false;
            }
        }

        return true;
    }

    bool ParseFromFile(const tstring& sFileName)
    {
        _ASSERT(sFileName.size());

        // Check if file exists
        DWORD   dwAttrib = GetFileAttributes(sFileName.c_str());
        bool    bFileExists = (dwAttrib != -1) && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY);

        if( !bFileExists )
        {
            m_sError = __T("");
            m_sError += __T("File not found: ");
            m_sError += sFileName;
            return false;
        }

        // Point current directory at the input file
        TCHAR   szFileDir[MAX_PATH+1];
        LPTSTR  pszFilePart = 0;
        GetFullPathName(sFileName.c_str(), MAX_PATH, szFileDir, &pszFilePart);
        tstring sFullPath = szFileDir;
        _ASSERTE(pszFilePart);
        *pszFilePart = 0;

        // Set (and auto-reset) current working directory
        // We do this so that any file names read out of the file
        // can be relative to the file, not where we're running
        // the app from. We're using the system managed working directory
        // as a "context" for the individual values, i.e. FileNameValue,
        // to be able to compute a complete file name. This is potentially
        // dangerous as the cwd is set per process, not per thread, but
        // since command lines are typically processed before threads are
        // fired off, we should be safe. It saves us from having to pass
        // a virtual cwd to all values as they're parsed.
        class CurrentDir
        {
        public:
            CurrentDir(LPCTSTR pszNewDir)
            {
                GetCurrentDirectory(MAX_PATH, m_szOldDir);
                _ASSERT(pszNewDir);
                BOOL    b = SetCurrentDirectory(pszNewDir);
                _ASSERTE(b);
            }

            ~CurrentDir()
            {
                SetCurrentDirectory(m_szOldDir);
            }

        private:
            TCHAR   m_szOldDir[MAX_PATH+1];
        };

        CurrentDir  cd(szFileDir);

        // Read in the contents of the file
        tstring tContents;
        if (!ReadFile(sFullPath, tContents)) return false;

        // Parse the contents of the file like a string
        return Parse(tContents, false, false);
    }

    void ClearError()
    {
        m_sError.erase();
    }

    virtual tstring Logo() const
    {
        CVersionInfo        vi;
        if( FAILED(vi.LoadVersion()) ) return __T("");

        LPCTSTR pszDesc = vi.QueryValueString(__T("FileDescription"));
        LPCTSTR pszVersion = vi.QueryValueString(__T("FileVersion"));
        LPCTSTR pszCopyright = vi.QueryValueString(__T("LegalCopyright"));

        tstring         sDesc = (pszDesc ? pszDesc : __T(""));
        tstring         sVersion = (pszVersion ? pszVersion : __T(""));
        tstring         sCopyright = (pszCopyright ? pszCopyright : __T(""));
        tstringstream   ss;

        sCopyright = SearchAndReplace(sCopyright, __T("\x0a9"), __T("(c)"));
        sCopyright = SearchAndReplace(sCopyright, __T("\x0ae"), __T("(r)"));

        if( sCopyright[sCopyright.size() - 1] != __T('.') ) sCopyright += __T('.');

        // e.g.
        // Microsoft MIDL Compiler Version 5.01.0164
        // Copyright (c) 1991-1997, Microsoft, Inc. All rights reserved.
        ss  << sDesc << __T(" version ") << sVersion << std::endl
            << sCopyright << __T(" All rights reserved.") << std::endl;

        return ss.str();
    }

    static
    bool FileExists(const tstring& sFileName) 
    { 
        DWORD dwAttrib = GetFileAttributes(sFileName.c_str());
        return (dwAttrib != -1) && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
    }

    static
    tstring ModuleName()
    {
        TCHAR   szPath[MAX_PATH+1];
        GetModuleFileName(0, szPath, MAX_PATH);
        TCHAR   szFile[_MAX_FNAME+1];
        _tsplitpath(szPath, 0, 0, szFile, 0);
        return szFile;
    }

    static
    bool IsWhiteSpace(TCHAR ch)
    {
        return ch == __T(' ') || ch == __T('\t') || ch == __T('\n') || ch == __T('\r');
    }

    static
    tstring SearchAndReplace(
        tstring         s,
        const tstring&  sFind,
        const tstring&  sReplace,
        bool            bGlobal = true)
    {
        _ASSERTE(sFind.size());
        const size_t    cchFind = sFind.size();

        size_t pos = 0;
        while( (pos = s.find(sFind, pos)) != tstring::npos )
        {
            s.replace(pos, cchFind, sReplace);
            if( !bGlobal ) break;
            pos += cchFind;
        }

        return s;
    }

public:
    enum ARG_TYPE { ARG_FLAG, ARG_PARAM };

private:
    // Just some simple helpers used in ReadFile() implementation.
    template<typename invalid_t, invalid_t invalid_value>
    class SimpleHandleT
    {
    private:
        HANDLE m_h;
        SimpleHandleT& operator=(const SimpleHandleT&);
    
    public:
        SimpleHandleT(const HANDLE& h) : m_h(h) {}

        ~SimpleHandleT()
        {
            if( invalid_value != reinterpret_cast<invalid_t>(m_h) )
            {
                ::CloseHandle(m_h);
            }
        }
    
        bool operator!() const
        {
            return (invalid_value == reinterpret_cast<invalid_t>(m_h));
        }
    
        operator HANDLE() const
        { return m_h; }
    };
    
    typedef SimpleHandleT<int, -1> FileHandle; // -1 == INVALID_HANDLE_VALUE
    typedef SimpleHandleT<HANDLE, NULL> Handle;
    
    template<class From, class To>
    inline static void ConvertStringData(const From* pFrom, size_t nSize, std::basic_string<To>& sTo) //throw (std::exception)
    { sTo.assign(pFrom, nSize); }
    
    template<>
    inline static void ConvertStringData(const char* pFrom, size_t nSize, std::basic_string<wchar_t>& sTo) //throw (std::exception)
    {
        sTo.empty();
        
        int nLen = MultiByteToWideChar(GetACP(), 0, pFrom, (int)nSize, 0, 0);
        std::vector< wchar_t > buf( nLen );
        wchar_t *pBuf = &buf[ 0 ];
        
        int nCvt = MultiByteToWideChar(GetACP(), 0, pFrom, (int)nSize, pBuf, nLen);
        _ASSERTE(nCvt == nLen);
        
        sTo.assign(pBuf, nCvt);
    }
    
    template<>
    inline static void ConvertStringData(const wchar_t* pFrom, size_t nSize, std::basic_string<char>& sTo) //throw (std::exception)
    {
        sTo.empty();
        
        int nLen = WideCharToMultiByte(GetACP(), 0, pFrom, (int)nSize, 0, 0, 0, 0);
        std::vector< char > buf( nLen );
        char *pBuf = &buf[ 0 ];
        
        int nCvt = WideCharToMultiByte(GetACP(), 0, pFrom, (int)nSize, pBuf, nLen, 0, 0);
        _ASSERTE(nCvt == nLen);
        
        sTo.assign(pBuf, nCvt);
    }
    
protected:
    // Helper to read file in either unicode or ansi
    // NOTE: We're missing formatting of error codes to text (e.g.
    //       FormatMessage(...) and incorporating these to m_sError on failure.
    bool ReadFile(const tstring& sFullPath, tstring& sContents)
    {
        // Clean up
        ClearError();
        sContents = _T("");
    
        FileHandle hFile = CreateFile(sFullPath.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    
        if( !hFile )
        {
            _ASSERTE(!"Failed to open parameter file");
            m_sError = _T("Failed to open file ");
            m_sError += sFullPath;
        
            return false;
        }
    
        BY_HANDLE_FILE_INFORMATION bhfi;
        ::ZeroMemory(&bhfi, sizeof(bhfi));
    
        if( GetFileInformationByHandle(hFile, &bhfi) == 0 )
        {
            _ASSERTE(!"Failed to get file information");
            m_sError = _T("Failed to get file information for ");
            m_sError += sFullPath;
        
            return false;
        }
    
        // A few special cases
        if( bhfi.nFileSizeHigh != 0 )
        {
            _ASSERTE(!"Ridiculously large parameter file");
            m_sError = _T("File ");
            m_sError += sFullPath;
            m_sError += _T(" is too large");
        
            return false;
        }
        else if( bhfi.nFileSizeLow == 0 )
        {
            // Empty file (but should be accepted anyway)
            return true;
        }
    
        size_t nDataSize = bhfi.nFileSizeLow;
    
        Handle hFileMapping = CreateFileMapping(hFile, 0, PAGE_WRITECOPY, 0, (DWORD)nDataSize, 0);
        if( !hFileMapping )
        {
            _ASSERTE(!"Failed to create file mapping");
            m_sError = _T("Failed to create file mapping on file ");
            m_sError += sFullPath;
        
            return false;
        }
    
        LPVOID lpView = MapViewOfFile(hFileMapping, FILE_MAP_COPY, 0, 0, 0);
    
        if( !lpView )
        {
            _ASSERTE(!"Failed to map view of file");
            m_sError = _T("Failed to map view of file ");
            m_sError += sFullPath;
        
            return false;
        }
    
        class MapViewCleanup
        {
        public:
            explicit MapViewCleanup(LPVOID lpv)
                : m_lpView(lpv)
            {}

            ~MapViewCleanup()
            { ::UnmapViewOfFile(m_lpView); }
        
        private:
            LPVOID m_lpView;
        } mapCleanup(lpView);
    
        // Try to determine if unicode
        LPVOID  lpData = lpView;
        bool    bUnicode = false;
    
        if( nDataSize > sizeof(WORD) )
        {
            WORD wMaybeUnicodeMarker = *(reinterpret_cast<LPWORD>(lpView));
        
            if( wMaybeUnicodeMarker == 0xFEFF )
            {
                _ASSERTE(0 == (nDataSize%sizeof(wchar_t)));
                lpData = &(reinterpret_cast<LPWORD>(lpView)[1]);
                bUnicode = true;
            }
            else if( wMaybeUnicodeMarker == 0xFFFE )
            {
                _ASSERTE(!"Big-endian unicode not supported");
                m_sError = _T("File ");
                m_sError += sFullPath;
                m_sError += _T(" is in big-endian unicode format");
            
                return false;
            }
        }
    
        // Convert as necessary
        try
        {
            if( bUnicode ) ConvertStringData(reinterpret_cast<LPCWSTR>(lpData), (nDataSize/sizeof(wchar_t))-1, sContents);
            else ConvertStringData(reinterpret_cast<LPCSTR>(lpData), nDataSize/sizeof(char), sContents);
        }
        catch( std::exception& e )
        {
            _ASSERTE(e.what());
            e;  // Avoid "unreferenced local variable"
        
            m_sError = _T("Failed to convert character data from file ");
            m_sError += sFullPath;
            m_sError += _T(".");
        
            return false;
        }
    
        // Replace CRs & LFs
        std::replace_if(sContents.begin(),
                        sContents.end(),
                        std::bind2nd(std::equal_to<TCHAR>(), _T('\r')),
                        _T(' '));
        std::replace_if(sContents.begin(),
                        sContents.end(),
                        std::bind2nd(std::equal_to<TCHAR>(), _T('\n')),
                        _T(' '));
    
        return true;
    }

protected:
    struct ArgInfo
    {
        ARG_TYPE    type;
        Argument*   pArg;

        ArgInfo(ARG_TYPE _type, Argument* _pArg) : type(_type), pArg(_pArg) {}
    };

    friend class ArgInfoContainer;

    class ArgInfoContainer : public std::vector<ArgInfo>
    {
    public:
        Argument* FindFlag(const tstring& sName) const
        {
            for( const_iterator it = begin(); it != end(); ++it )
            {
                _ASSERTE(it->pArg);
                if( (it->type == ARG_FLAG) &&
                    (it->pArg->Matches(sName)) )
                {
                    return it->pArg;
                }
            }

            return 0;
        }

        Argument* GetNextParam() const
        {
            // Give each argument all the values it wants
            // before moving onto the next one
            for( const_iterator it = begin(); it != end(); ++it )
            {
                if( it->type == ARG_PARAM )
                {
                    _ASSERTE(it->pArg);
                    Argument*   pArg = it->pArg;
                    if( pArg->ExpectsValues() && pArg->WantsAValue() )
                    {
                        return pArg;
                    }
                }
            }

            return 0;
        }
    };

    ArgInfoContainer    m_rgArgInfos;
    tstring             m_sError;
    bool                m_bAllowArgFile:1;
};

class StandardCommandLineParser : public CommandLineParser
{
public:
    StandardCommandLineParser(bool bAllowArgFile = true)
    :   CommandLineParser(bAllowArgFile),
        help(__T("?"), __T("Show usage.")),
        version(__T("v"), __T("Show version."))
    {
        AddFlag(help);
        help.AddAlternate(__T("h"));
        help.AddAlternate(__T("help"));

        AddFlag(version);
        version.AddAlternate(__T("version"));
    }

    static
    bool IsConsole()
    {
        TCHAR       szPath[MAX_PATH+1];
        GetModuleFileName(0, szPath, MAX_PATH);

        SHFILEINFO  sfi = { 0 };
        DWORD       dw = (DWORD) SHGetFileInfo(szPath, 0, &sfi, sizeof(sfi), SHGFI_EXETYPE);

        return (LOWORD(dw) == IMAGE_NT_SIGNATURE) && (HIWORD(dw) == 0);
    }

    void Show(const tstring& s) const
    {
        if( IsConsole() )
        {
            // Always send usage to stdout so it's easy to capture the output
            tcout << s.c_str() << std::endl;
        }
        else
        {
            MessageBox(0, s.c_str(), ModuleName().c_str(), MB_SETFOREGROUND);
        }
    }

    bool Continue()
    {
        if( version )
        {
            Show(Logo());
            return false;
        }

        if( m_sError.size() || help )
        {
            if( help ) ClearError();
            Show(Usage());
            return false;
        }

        return true;
    }

    bool ParseAndContinue(int argc, const TCHAR* argv[])
    {
        Parse(argc, argv);
        return Continue();
    }

    bool ParseAndContinue(int argc, TCHAR* argv[])
    {
        // To avoid C2664
        Parse(argc, (const TCHAR**)argv);
        return Continue();
    }

    bool ParseAndContinue(LPCTSTR psz)
    {
        Parse(psz);
        return Continue();
    }

public:
    FlagArg  help;
    FlagArg  version;
};

// Commonly used types
typedef ValueArg<tstring> StringArg;
typedef ValueArg<long> LongArg;
typedef ValueArg<double> DoubleArg;

#endif // INC_COMMANDLINEPARSER

// put these back for the rest of the world
#pragma warning(default:4786)
#pragma warning(default : 4311)
