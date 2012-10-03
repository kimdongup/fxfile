//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "fxb_file_split.h"

#include "fxb_crc_checksum.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxb
{
#define DEFAULT_BUFFER_SIZE (16 * 1024)

FileSplit::FileSplit(void)
    : mHwnd(XPR_NULL), mMsg(0)
    , mStatus(StatusNone)
    , mSplitedCount(0)
    , mFlags(0)
    , mUnitSize(0i64)
    , mBufferSize(DEFAULT_BUFFER_SIZE)
{
}

FileSplit::~FileSplit(void)
{
}

void FileSplit::setOwner(HWND aHwnd, xpr_uint_t aMsg)
{
    mHwnd = aHwnd;
    mMsg  = aMsg;
}

void FileSplit::setPath(const xpr_tchar_t *aPath, const xpr_tchar_t *aDestDir)
{
    if (XPR_IS_NULL(aPath) || XPR_IS_NULL(aDestDir))
        return;

    mPath = aPath;
    mDestDir = aDestDir;

    if (mDestDir[mDestDir.length()-1] == XPR_STRING_LITERAL('\\'))
        mDestDir.erase(mDestDir.length()-1);
}

xpr_uint_t FileSplit::getFlags(void)
{
    return mFlags;
}

void FileSplit::setFlags(xpr_uint_t aFlags)
{
    mFlags = aFlags;
}

void FileSplit::setSplitSize(xpr_sint64_t aUnitSize)
{
    mUnitSize = aUnitSize;
}

void FileSplit::setBufferSize(xpr_size_t aBufferSize)
{
    mBufferSize = aBufferSize;
}

xpr_bool_t FileSplit::OnPreEntry(void)
{
    if (mUnitSize <= 10)
        mUnitSize = 10;

    if (IsExistFile(mPath) == XPR_FALSE)
    {
        setStatus(StatusFileNotExist);
        return XPR_FALSE;
    }

    if (IsExistFile(mDestDir) == XPR_FALSE)
    {
        setStatus(StatusDestDirNotExist);
        return XPR_FALSE;
    }

    mSplitedCount = 0;
    setStatus(StatusSplitting);

    return XPR_TRUE;
}

unsigned FileSplit::OnEntryProc(void)
{
    xpr_char_t *sCrcCode = XPR_NULL;
    if (XPR_TEST_BITS(mFlags, FlagCrcFile))
    {
        sCrcCode = new xpr_char_t[0xff];
        sCrcCode[0] = '\0';

        getFileCrcSfv(mPath.c_str(), sCrcCode);
    }

    Status sStatus = StatusNone;

    FILE *sFile = _tfopen(mPath.c_str(), XPR_STRING_LITERAL("rb"));
    if (XPR_IS_NULL(sFile))
    {
        sStatus = StatusNotReadable;
    }
    else
    {
        xpr_uint64_t sFileSize = GetFileSize(mPath);

        xpr_size_t sRead;
        xpr_byte_t *sBuffer;
        FILE *sTargetFile;
        std::tstring sFileName;
        xpr_char_t sFileNameA[XPR_MAX_PATH + 1];
        xpr_size_t sOutputBytes;

        xpr_sint_t sIndex;
        xpr_tchar_t sPath[XPR_MAX_PATH + 1];

        xpr_sint64_t i;
        xpr_sint64_t sCount;
        xpr_sint64_t sWritedFileSize;
        xpr_size_t sSize;
        xpr_size_t sWritedBuffer;
        xpr_size_t sWrite;

        sTargetFile = XPR_NULL;
        sBuffer = new xpr_byte_t[mBufferSize];
        sFileName = mPath.substr(mPath.rfind(XPR_STRING_LITERAL('\\'))+1);
        sIndex = 1;

        sOutputBytes = XPR_MAX_PATH * sizeof(xpr_char_t);
        XPR_MBS_TO_TCS(sFileName.c_str(), sFileName.length() * sizeof(xpr_tchar_t), sFileNameA, &sOutputBytes);
        sFileNameA[sOutputBytes / sizeof(xpr_char_t)] = 0;

        while (IsStop() == XPR_FALSE)
        {
            sRead = fread(sBuffer, 1, mBufferSize, sFile);
            if (sRead == 0)
                break;

            sWritedBuffer = 0;

            do
            {
                if (XPR_IS_NULL(sTargetFile))
                    sWritedFileSize = 0;

                if (XPR_IS_NULL(sTargetFile))
                {
                    _stprintf(sPath, XPR_STRING_LITERAL("%s\\%s.%03d"), mDestDir.c_str(), sFileName.c_str(), sIndex++);
                    sTargetFile = _tfopen(sPath, XPR_STRING_LITERAL("wb"));

                    if (XPR_IS_NOT_NULL(sTargetFile))
                    {
                        CsLocker sLocker(mCs);
                        mSplitedCount++;
                    }
                }

                if (XPR_IS_NULL(sTargetFile))
                    break;

                sSize = sRead - sWritedBuffer;
                if (sSize > mUnitSize)
                    sSize = (xpr_size_t)mUnitSize;

                sCount = (xpr_sint64_t)sSize / mUnitSize;
                if ((xpr_sint64_t)sSize % mUnitSize)
                    sCount++;

                for (i = 0; i < sCount; ++i)
                {
                    if ((mUnitSize - sWritedFileSize) < sSize)
                        sSize = (xpr_size_t)(mUnitSize - sWritedFileSize);

                    sWrite = fwrite(sBuffer+sWritedBuffer, 1, sSize, sTargetFile);

                    sWritedBuffer += sWrite;
                    sWritedFileSize += sWrite;
                    if (sWritedFileSize == mUnitSize)
                        break;

                    if (sWritedBuffer == sRead)
                        break;
                }

                if (sWritedFileSize == mUnitSize)
                {
                    fclose(sTargetFile);
                    sTargetFile = XPR_NULL;
                }

                if (sWritedBuffer == sRead)
                    break;
            }
            while (IsStop() == XPR_FALSE);
        }

        if (XPR_IS_NOT_NULL(sTargetFile))
        {
            fclose(sTargetFile);
            sTargetFile = XPR_NULL;
        }

        fclose(sFile);

        if (IsStop() == XPR_FALSE && XPR_IS_NOT_NULL(sCrcCode))
        {
            xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
            _stprintf(sPath, XPR_STRING_LITERAL("%s\\%s.crc"), mDestDir.c_str(), sFileName.c_str());

            sFile = _tfopen(sPath, XPR_STRING_LITERAL("wt"));
            if (XPR_IS_NOT_NULL(sFile))
            {
                fprintf(sFile, "filename=%s\n", sFileNameA);
                fprintf(sFile, "size=%I64u\n", sFileSize);
                fprintf(sFile, "crc32=%s\n", sCrcCode);
                fclose(sFile);
            }
        }

        if (IsStop() == XPR_FALSE && XPR_TEST_BITS(mFlags, FlagBatFile))
        {
            xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
            _stprintf(sPath, XPR_STRING_LITERAL("%s\\%s.bat"), mDestDir.c_str(), sFileName.c_str());

            sFile = _tfopen(sPath, XPR_STRING_LITERAL("wt"));
            if (XPR_IS_NOT_NULL(sFile))
            {
                const xpr_char_t *sTempFile = "fxCombine.tmp";

                fprintf(sFile, "@echo off\n");
                fprintf(sFile, "echo.\n");
                fprintf(sFile, "echo Created by flyExplorer (http://www.flychk.com)\n");
                fprintf(sFile, "echo.\n");
                fprintf(sFile, "echo File Combine...\n");
                fprintf(sFile, "echo.\n");
                fprintf(sFile, "copy /b \"%s.001\" \"%s\"\n", sFileNameA, sTempFile);

                xpr_size_t j;
                for (j = 2; j <= mSplitedCount; ++j)
                    fprintf(sFile, "copy /b \"%s\" + \"%s.%03d\" \"%s\"\n", sTempFile, sFileNameA, j, sTempFile);

                fprintf(sFile, "ren \"%s\" \"%s\"\n", sTempFile, sFileNameA);
                fprintf(sFile, "echo Done...\n");

                fclose(sFile);
            }
        }

        if (IsStop() == XPR_FALSE && XPR_TEST_BITS(mFlags, FlagDelOrgFile))
        {
            ::DeleteFile(mPath.c_str());
        }

        XPR_SAFE_DELETE_ARRAY(sBuffer);
        XPR_SAFE_DELETE_ARRAY(sCrcCode);

        sStatus = IsStop() ? StatusStopped : StatusSplitCompleted;
    }

    setStatus(sStatus);

    ::PostMessage(mHwnd, mMsg, (WPARAM)XPR_NULL, (LPARAM)XPR_NULL);

    return 0;
}

FileSplit::Status FileSplit::getStatus(xpr_size_t *aSplitedCount)
{
    CsLocker sLocker(mCs);

    if (XPR_IS_NOT_NULL(aSplitedCount)) *aSplitedCount = mSplitedCount;

    return mStatus;
}

void FileSplit::setStatus(Status aStatus)
{
    CsLocker sLocker(mCs);
    mStatus = aStatus;
}
} // namespace fxb