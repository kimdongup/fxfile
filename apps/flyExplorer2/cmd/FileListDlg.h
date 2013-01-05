//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_FILE_LIST_DLG_H__
#define __FX_FILE_LIST_DLG_H__
#pragma once

#include "TitleDlg.h"

namespace fxb
{
class FileList;
} // namespace fxb

class DlgState;

class FileListDlg : public TitleDlg
{
    typedef TitleDlg super;

public:
    FileListDlg(void);
    virtual ~FileListDlg(void);

public:
    void addPath(const xpr_tchar_t *aPath);
    void setTextFile(const xpr_tchar_t *aTextFile);

protected:
    void enableWindow(xpr_bool_t aEnable = XPR_TRUE);

protected:
    typedef std::deque<std::tstring> PathDeque;
    PathDeque mPathDeque;

    std::tstring mTextFile;

    fxb::FileList *mFileList;

protected:
    DlgState *mDlgState;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnPathBrowse(void);
    afx_msg void OnOK(void);
    afx_msg LRESULT OnFinalize(WPARAM wParam, LPARAM lParam);

public:
    virtual xpr_bool_t DestroyWindow();
};

#endif // __FX_FILE_LIST_DLG_H__