//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "file_scrap_group_dlg.h"

#include "file_scrap.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cmd
{
FileScrapGroupDlg::FileScrapGroupDlg(void)
    : CDialog(IDD_FILE_SCRAP_GROUP, XPR_NULL)
    , mSelGroupId(FileScrap::InvalidGroupId)
{
}

FileScrapGroupDlg::~FileScrapGroupDlg(void)
{
}

void FileScrapGroupDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST, mListBox);
}

BEGIN_MESSAGE_MAP(FileScrapGroupDlg, CDialog)
    ON_LBN_DBLCLK(IDC_LIST, OnDblclkGrpList)
    ON_BN_CLICKED(IDOK, OnOK)
END_MESSAGE_MAP()

xpr_bool_t FileScrapGroupDlg::OnInitDialog(void) 
{
    CDialog::OnInitDialog();

    if (mTitle.empty() == XPR_FALSE)
        SetWindowText(mTitle.c_str());

    FileScrap &sFileScrap = FileScrap::instance();

    xpr_sint_t sIndex;
    xpr_size_t i, sCount;
    xpr::string sText;
    FileScrap::Group *sGroup;

    sCount = sFileScrap.getGroupCount();
    for (i = 0; i < sCount; ++i)
    {
        sGroup = sFileScrap.getGroup(i);
        if (XPR_IS_NULL(sGroup))
            continue;

        sText = sGroup->mGroupName;
        if (sText == kFileScrapNoGroupName)
            sText = gApp.loadString(XPR_STRING_LITERAL("popup.file_scrap_group.no_group"));

        if (sGroup->mGroupId == sFileScrap.getCurGroupId())
        {
            sText += XPR_STRING_LITERAL(" (");
            sText += gApp.loadString(XPR_STRING_LITERAL("popup.file_scrap_group.default"));
            sText += XPR_STRING_LITERAL(")");
        }

        sIndex = mListBox.AddString(sText.c_str());
        mListBox.SetItemData(sIndex, (DWORD_PTR)sGroup);

        if (sGroup->mGroupId == sFileScrap.getCurGroupId())
            mListBox.SetCurSel(sIndex);
    }

    SetWindowText(gApp.loadString(XPR_STRING_LITERAL("popup.file_scrap_group.title")));

    return XPR_TRUE;
}

void FileScrapGroupDlg::setTitle(const xpr::string &aTitle)
{
    mTitle = aTitle;
}

xpr_uint_t FileScrapGroupDlg::getSelGroupId(void)
{
    return mSelGroupId;
}

void FileScrapGroupDlg::OnDblclkGrpList(void) 
{
    CPoint sPoint;
    GetCursorPos(&sPoint);

    mListBox.ScreenToClient(&sPoint);

    xpr_bool_t sOutside = XPR_TRUE;
    mListBox.ItemFromPoint(sPoint, sOutside);

    if (XPR_IS_TRUE(sOutside))
        return;

    OnOK();
}

void FileScrapGroupDlg::OnOK(void)
{
    xpr_sint_t sIndex = mListBox.GetCurSel();
    if (sIndex == CB_ERR)
        return;

    FileScrap::Group *sGroup = (FileScrap::Group *)mListBox.GetItemData(sIndex);
    if (XPR_IS_NULL(sGroup))
        return;

    mSelGroupId = sGroup->mGroupId;

    CDialog::OnOK();
}
} // namespace cmd
} // namespace fxfile
