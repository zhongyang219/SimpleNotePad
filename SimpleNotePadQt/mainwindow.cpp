#include "mainwindow.h"
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QFontComboBox>
#include <QFontDialog>
#include "mainframedefine.h"
#include "ScintillaEditBase.h"

MainWindow::MainWindow(QWidget *parent)
    : RibbonFrameWindow(parent, "://res/MainFrame.xml", true)
{
    InitUi();
    QIcon appIcon("://res/logo.ico");
    setWindowIcon(appIcon);
    SetItemIcon("AppAbout", appIcon);
    resize(DPI(800), DPI(520));

    textEdit = new ScintillaEditBase;
    SetDefaultWidget(textEdit);
    textEdit->setStyleSheet("QAbstractScrollArea{border:none;}");

    SetItemText(CMD_FontPointSizeComboBox, "9");
    QWidget* pFontSizeCombo = _GetWidget(CMD_FontPointSizeComboBox);
    if (pFontSizeCombo != nullptr)
        pFontSizeCombo->setMinimumWidth(DPI(50));
    QFontComboBox* pFontCombo = qobject_cast<QFontComboBox*>(_GetWidget(CMD_FontComboBox));
    if (pFontCombo != nullptr)
        pFontCombo->setEditable(false);

    SetItemEnable(CMD_EditCopy, false);
    SetItemEnable(CMD_EditCut, false);
    SetItemEnable(CMD_EditUndo, false);
    SetItemEnable(CMD_EditRedo, false);

    connect(textEdit, SIGNAL(textChanged()), this, SLOT(documentWasModified()));


#ifdef QT_DEBUG
//    OpenFile("D:\\Temp\\2.html");
#endif
}

MainWindow::~MainWindow()
{
}

void MainWindow::documentWasModified()
{
//    setWindowModified(textEdit->isModified());
}


//void MainWindow::OnCopyAvailable(bool yes)
//{
//    SetItemEnable(CMD_EditCopy, yes);
//    SetItemEnable(CMD_EditCut, yes);
//}

//void MainWindow::OnUndoAvailable(bool available)
//{
//    SetItemEnable(CMD_EditUndo, available);
//}

//void MainWindow::OnRedoAvailable(bool available)
//{
//    SetItemEnable(CMD_EditRedo, available);
//}

bool MainWindow::OnCommand(const QString &strCmd, bool checked)
{
    if (strCmd == "AppAbout")
    {
        QMessageBox::about(this, QString(u8"关于 %1").arg(qApp->applicationName()), QString(u8"%1 %2\r\n这是一个Office风格的富文本编辑器示例程序。\r\nCopyright(C) 2023 by ZhongYang").arg(qApp->applicationName()).arg(qApp->applicationVersion()));
        return true;
    }
    else if (strCmd == "FileOpen")
    {
        QString fileName = QFileDialog::getOpenFileName(this, QString(), QString(), u8"所有支持的文件 (*.txt *.md *.html *.htm);;HTML文件 (*.html *.htm);;Markdown文件 (*.md);;文件文件 (*.txt)");
        if (!fileName.isEmpty())
        {
//            if (!OpenFile(fileName))
//            {
//                QMessageBox::warning(this, QString(), u8"文件打开失败！");
//            }
        }
    }
    else if (strCmd == CMD_FileSave)
    {
//        if (!SaveFile(m_filePath))
//        {
//            QMessageBox::warning(this, QString(), u8"文件保存失败！");
//        }
    }
    else if (strCmd == CMD_SaveAs)
    {
        QString fileName = QFileDialog::getSaveFileName(this, QString(), curFile, u8"HTML文件 (*.html);;Markdown文件 (*.md);;文件文件 (*.txt)");
        if (!fileName.isEmpty())
        {
//            if (!SaveFile(fileName))
//            {
//                QMessageBox::warning(this, QString(), u8"文件保存失败！");
//            }
        }
    }
    else if (strCmd == CMD_EditUndo)
    {
//        m_edit.undo();
    }
    else if (strCmd == CMD_EditRedo)
    {
//        m_edit.redo();
    }
    else if (strCmd == CMD_EditCopy)
    {
//        m_edit.copy();
    }
    else if (strCmd == CMD_EditCut)
    {
//        m_edit.cut();
    }
    else if (strCmd == CMD_EditPaste)
    {
//        m_edit.paste();
    }
    else if (strCmd == CMD_Bold || strCmd == CMD_Italic || strCmd == CMD_UnderLine || strCmd == CMD_Strickout)
    {
//        ApplyFont();
    }
    else if (strCmd == CMD_FontGroup)
    {
//        QFontDialog dlg(m_edit.currentFont(), this);
//        if (dlg.exec() == QDialog::Accepted)
//            m_edit.setCurrentFont(dlg.font());
    }

    return RibbonFrameWindow::OnCommand(strCmd, checked);
}

void MainWindow::OnItemChanged(const QString &strId, int index, const QString &text)
{
    if (strId == CMD_FontPointSizeComboBox)
    {
//        ApplyFont();
    }
}

QWidget *MainWindow::CreateUserWidget(const QString &strId, QWidget *pParent)
{
    if (strId == CMD_FontComboBox)
    {
        QFontComboBox* pFontCombo = new QFontComboBox(pParent);
        connect(pFontCombo, SIGNAL(currentFontChanged(const QFont&)), this, SLOT(OnCurrentFontChanged(const QFont&)));
        pFontCombo->setMaximumWidth(DPI(100));
        return pFontCombo;
    }
    return nullptr;
}

