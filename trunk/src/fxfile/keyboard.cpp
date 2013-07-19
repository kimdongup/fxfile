//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "keyboard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxfile
{
CString GetKeyName(xpr_uint_t aVirtKey)
{
    xpr_slong_t sScan = MapVirtualKey(aVirtKey, 0) << 16;

    // if it's an extended key, add the extended flag
    if ((VK_PRIOR <= aVirtKey && aVirtKey <= VK_HELP) || (aVirtKey == VK_DIVIDE))
        sScan |= 0x01000000L;

    CString sKeyName;
    xpr_sint_t sBufferLen = 64;
    xpr_sint_t sLen;

    do
    {
        sBufferLen *= 2;
        xpr_tchar_t *sStr = sKeyName.GetBufferSetLength(sBufferLen);
        sLen = ::GetKeyNameText(sScan, sStr, sBufferLen + 1);
        sKeyName.ReleaseBuffer(sLen);
    }
    while (sLen == sBufferLen);

    return sKeyName;
}

void ConvertStringToFormat(const xpr_tchar_t *aString, xpr_tchar_t *aFormat)
{
    if (XPR_IS_NULL(aString) || XPR_IS_NULL(aFormat))
        return;

    xpr_sint_t i, j;
    xpr_tchar_t sChar;
    xpr_sint_t sDivLen = (xpr_sint_t)_tcslen(aString);

    for (i = 0, j = 0; i < sDivLen; ++i)
    {
        sChar = aString[i];
        if (aString[i] == '\\')
        {
            switch (aString[i+1])
            {
            case 'r': sChar = '\r'; i++; break;
            case 'n': sChar = '\n'; i++; break;
            case 't': sChar = '\t'; i++; break;
            default: continue;
            }
        }

        aFormat[j++] = sChar;
    }

    aFormat[j] = '\0';
}

// & -> &&
void ConvertDoubleAmpersand(xpr_tchar_t *aText, xpr_tchar_t *aTemp)
{
    if (XPR_IS_NULL(aText) || XPR_IS_NULL(aTemp))
        return;

    xpr_tchar_t *sPrefix = aText;
    while (true)
    {
        sPrefix = _tcschr(sPrefix, '&');
        if (XPR_IS_NULL(sPrefix))
            break;

        _tcscpy(aTemp, sPrefix+1);
        sPrefix[1] = '&';
        _tcscpy(sPrefix+2, aTemp);
        sPrefix += 2;
    }
}
} // namespace fxfile