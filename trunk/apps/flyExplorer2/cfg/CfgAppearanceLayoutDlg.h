//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_CFG_APPEARANCE_LAYOUT_DLG_H__
#define __FX_CFG_APPEARANCE_LAYOUT_DLG_H__
#pragma once

#include "CfgDlg.h"

class CfgAppearanceLayoutDlg : public CfgDlg
{
    typedef CfgDlg super;

public:
    CfgAppearanceLayoutDlg(void);

protected:
    // from CfgDlg
    virtual void onInit(Option::Config &aConfig);
    virtual void onApply(Option::Config &aConfig);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnDestroy(void);
    afx_msg void OnInfoBar(void);
    afx_msg void OnDriveBar(void);
    afx_msg void OnPathBar(void);
};

#endif // __FX_CFG_APPEARANCE_LAYOUT_DLG_H__