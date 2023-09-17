#ifndef MAINFRAMEDEFINE_H
#define MAINFRAMEDEFINE_H

#define CMD_FileOpen "FileOpen"
#define CMD_FileSave "FileSave"
#define CMD_SaveAs "SaveAs"
#define CMD_EditPaste "EditPaste"
#define CMD_EditCut "EditCut"
#define CMD_EditCopy "EditCopy"
#define CMD_EditUndo "EditUndo"
#define CMD_EditRedo "EditRedo"
#define CMD_FontComboBox "FontComboBox"
#define CMD_FontPointSizeComboBox "FontPointSizeComboBox"
#define CMD_Bold "Bold"
#define CMD_Italic "Italic"
#define CMD_UnderLine "UnderLine"
#define CMD_Strickout "Strickout"
#define CMD_FontGroup "FontGroup"

#include <QScreen>
#include <QGuiApplication>

//将一个像素值根据当前屏幕DPI设置进行等比放大
inline int DPI(int x)
{
    return QGuiApplication::primaryScreen()->logicalDotsPerInch() * x / 96;
}


#endif // MAINFRAMEDEFINE_H
