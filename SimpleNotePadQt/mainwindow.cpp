#include "mainwindow.h"
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QFontComboBox>
#include <QFontDialog>
#include "mainframedefine.h"
#include "scintillaeditor.h"
#include "tools/EditorHelper.h"
#include "tools/FindReplaceTools.h"
#include "SciLexer.h"

MainWindow::MainWindow(QWidget *parent)
    : RibbonFrameWindow(parent, "://res/MainFrame.xml", true)
{
    InitUi();
    QIcon appIcon("://res/logo.ico");
    setWindowIcon(appIcon);
    SetItemIcon("AppAbout", appIcon);
    resize(DPI(800), DPI(520));

    m_edit = new ScintillaEditor;
    SetDefaultWidget(m_edit);
    m_edit->setStyleSheet("QAbstractScrollArea{border:none;}");

    SetItemEnable(CMD_EditCopy, false);
    SetItemEnable(CMD_EditCut, false);
    SetItemEnable(CMD_EditUndo, false);
    SetItemEnable(CMD_EditRedo, false);
    SetItemEnable(CMD_FileSave, false);

    connect(m_edit, SIGNAL(notify(SCNotification*)), this, SLOT(OnNotify(SCNotification*)));

    //加载语法高亮设置
    m_syntax_highlight.LoadFromFile(":/res/lang.xml");


#ifdef QT_DEBUG
    OpenFile("D:\\temp\\Console\\LogTools.cpp");
#endif
}

MainWindow::~MainWindow()
{
}

bool MainWindow::OpenFile(const QString &filePath)
{
    if (!filePath.isEmpty())
    {
        m_filePath = filePath;
        m_edit->SetSavePoint();
        SetTitle();
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly))
        {
            QByteArray fileData = file.readAll();
            m_code = Common::JudgeCode(fileData);
            QString text = Common::BytesToString(fileData, m_code);
            m_edit->SetText(text);
            m_edit->EmptyUndoBuffer();
            ScintillaEditor::eEolMode eol_mode = ScintillaEditor::JudgeEolMode(text);
            m_edit->SetEolMode(eol_mode);

            //文件打开后为编辑设置语法高亮
            SetEditorSyntaxHight();

            UpdateStatusBarInfo();										//更新状态栏
            file.close();
            return true;
        }
    }
    //打开失败
    m_filePath.clear();
    return false;
}

bool MainWindow::SaveFile(const QString &filePath, CodeType code)
{
    if (!filePath.isEmpty())
    {
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly))
        {
            QString text;
            m_edit->GetText(text);
            QByteArray fileData = Common::StringToBytes(text, m_code);
            file.write(fileData);

            m_edit->SetSavePoint();
            UpdateStatusBarInfo();		//保存后刷新状态栏
            SetTitle();

            return true;
        }

    }
    return false;
}

bool MainWindow::OnFileSave()
{
    if (m_edit->IsModified())		//只有在已更改过之后才保存
    {
        //已经打开过一个文件时就直接保存，还没有打开一个文件就弹出“另存为”对话框
        if (!m_filePath.isEmpty())
        {
            if (SaveFile(m_filePath, m_code/*, m_code_page*/))
                return true;
            else
                return OnFileSaveAs();		//文件无法保存时弹出“另存为”对话框
        }
        else
        {
            return OnFileSaveAs();
        }
    }
    return false;
}

bool MainWindow::OnFileSaveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, QString(), m_filePath, u8"文件文件 (*.txt);;所有文件 (*.*)");
    if (!fileName.isEmpty())
    {
        if (SaveFile(fileName, m_code))
        {
             m_filePath = fileName;	//另存为后，当前文件名为保存的文件名
             SetTitle();
        }
        else
        {
            QMessageBox::warning(this, QString(), u8"文件保存失败！");
        }
    }
    return false;
}

bool MainWindow::SaveInquiry(const QString &info, int *prtn)
{
    if (m_edit->IsModified())
    {
        show();
        activateWindow();

        QString text;
        if (info == NULL)
        {
            if (m_filePath.isEmpty())
                text = u8"无标题 中的内容已更改，是否保存？";
            else
                text = QString(u8"“%1”文件中的内容已更改，是否保存？").arg(m_filePath);
        }
        else
        {
            text = info;
        }

        int rtn = QMessageBox::question(this, QString(), text);
        if (prtn != nullptr)
            *prtn = rtn;
        switch (rtn)
        {
        case QMessageBox::Yes:
            return OnFileSave();
        case QMessageBox::No:
            m_edit->SetSavePoint();
            SetTitle();
            break;
        default:
            return false;
        }
    }
    return true;
}

void MainWindow::SetTitle()
{
    QString str_title;
    if (m_edit->IsModified())
        str_title += '*';
    if (!m_filePath.isEmpty())
        str_title += m_filePath;
    else
        str_title += u8"无标题";
    str_title += " - ";
    str_title += qApp->applicationName();

    if (m_monitor_mode)
        str_title += " (监视模式)";

#ifdef _DEBUG
    str_title += " (Debug)";
#endif
    setWindowTitle(str_title);

}

void MainWindow::UpdateStatusBarInfo()
{

}

void MainWindow::UpdateActionState()
{
    SetItemEnable(CMD_EditUndo, m_edit->CanUndo());
    SetItemEnable(CMD_EditRedo, m_edit->CanRedo());
    SetItemEnable(CMD_EditCopy, !m_edit->IsSelectionEmpty());
    SetItemEnable(CMD_EditCut, !m_edit->IsSelectionEmpty());
    SetItemEnable(CMD_EditPaste, m_edit->CanPaste());
    SetItemEnable(CMD_FileSave, m_edit->IsModified());
}

void MainWindow::SetEditorSyntaxHight()
{
    CLanguage lan;
    //如果文件的前5个字符是“<?xml”则认为是xml格式
    if (m_edit->GetText(0, 5).startsWith("<?xml"))
        lan = m_syntax_highlight.FindLanguageByName("XML");
    if (lan.m_name.isEmpty())
    {
        QFileInfo helper(m_filePath);
        lan = m_syntax_highlight.FindLanguageByFileName(helper.fileName());
        if (lan.m_name.isEmpty()) //如果根据当前文件的扩展名无法在默认语言列表中找到对应的语言，则在自定义语言列表中查找
        {
            QString language_name = g_config.GetLanguageSettings().FindLanguageByFileName(helper.fileName());
            lan = m_syntax_highlight.FindLanguageByName(language_name);
        }
    }
    SetSyntaxHight(lan);
}

void MainWindow::SetSyntaxHight(const CLanguage &lan)
{
    m_edit->SetLexerNormalText();
    if (!lan.m_name.isEmpty())
    {
        //设置语法解析器
        m_edit->SetLexer(lan.m_id);
        //设置关键字
        for (auto iter = lan.m_keywords_list.begin(); iter != lan.m_keywords_list.end(); ++iter)
        {
            m_edit->SetKeywords(iter.key(), iter.value().toUtf8().constData());
        }
        //设置样式
        int style_index = 0;
        for (const auto& syntax_style : lan.m_syntax_list)
        {
            //查找是自定义样式是否存在
            CUserLanguage user_lan = g_config.GetLanguageSettings().GetLanguage(lan.m_name);
            if (user_lan.IsStyleExist(style_index))     //自定义的样式存在，使用自定义的样式
            {
                CUserLanguage::SyntaxStyle user_style = user_lan.GetStyle(style_index);
                m_edit->SetSyntaxColor(syntax_style.id, user_style.color);
                m_edit->SetSyntaxFontStyle(syntax_style.id, user_style.bold, user_style.italic);
            }
            else        //否则使用默认的样式
            {
                m_edit->SetSyntaxColor(syntax_style.id, syntax_style.color);
                m_edit->SetSyntaxFontStyle(syntax_style.id, syntax_style.bold, syntax_style.italic);
            }
            style_index++;
        }
        m_cur_lan_index = m_syntax_highlight.IndexOf(lan.m_name);
    }
    else
    {
        m_cur_lan_index = -1;
    }
    m_edit->SetFold();
    UpdateStatusBarInfo();
}

void MainWindow::OnNotify(SCNotification *notification)
{
    //响应编辑器文本变化
    if (notification->nmhdr.code == SCN_MODIFIED && m_edit->IsEditChangeNotificationEnable())
    {
        unsigned int marsk = (SC_MOD_DELETETEXT | SC_MOD_INSERTTEXT | SC_PERFORMED_UNDO | SC_PERFORMED_REDO);
        if ((notification->modificationType & marsk) != 0)
        {
            //TODO:
//                m_text_changed = true;

            UpdateStatusBarInfo();		//保存后刷新状态栏
            SetTitle();
        }
        //当删除了字符时
        if (notification->modificationType == (SC_MOD_DELETETEXT | SC_PERFORMED_USER)
            || notification->modificationType == (SC_MOD_DELETETEXT | SC_PERFORMED_USER | SC_STARTACTION))
        {
            char ch{};
            if (notification->text != nullptr)
                ch = notification->text[0];
            CEditorHelper helper(m_edit);
            if (g_config.GetEditSettings().brackets_auto_comp)
                helper.BracketsAutoDelete(ch);

            //显示自动完成列表
            if (g_config.GetEditSettings().show_auto_comp_list)
            {
                const CLanguage& language = m_syntax_highlight.GetLanguage(m_cur_lan_index);
                helper.AutoShowCompList(language);
            }
        }
    }
    else if (notification->nmhdr.code == SCN_ZOOM)
    {
            g_config.MenuSettings().zoom = m_edit->GetZoom();
            //TODO:
//            UpdateStatusBarInfo();
//            UpdateMenuSettingsToAllProcess();
            UpdateLineNumberWidth();
    }
    else if (notification->nmhdr.code == SCN_COPY)
    {
        //TODO:
//            std::wstring str = CCommon::StrToUnicode(notification->text, CodeType::UTF8_NO_BOM);
//            AddItemToClipboardHistory(str);
//            AddClipboardDataToAllProcess(str);
    }
    else if (notification->nmhdr.code == SCN_MARGINCLICK)
    {
        const int line_number = m_edit->send(SCI_LINEFROMPOSITION, notification->position);
        m_edit->send(SCI_TOGGLEFOLD, line_number);
    }
    else if (notification->nmhdr.code == SCN_UPDATEUI)
    {
        //垂直滚动条位置变化
        if ((notification->updated & SC_UPDATE_V_SCROLL) != 0)
        {
            UpdateLineNumberWidth();
        }
        //选择区域变化
        if ((notification->updated & SC_UPDATE_SELECTION) != 0)
        {
            //标记文档中与选中部分相同单词
            if (g_config.GetEditSettings().mark_same_words)
            {
                //先清除标记
                m_edit->ClearAllMark(ScintillaEditor::MarkStyle::SELECTION_MARK);
                //获取选中部分文本
                QString selected_text = m_edit->GetSelectedText();
                if (!selected_text.isEmpty() && Common::IsStringIdentifier(selected_text))
                {
                    //标记相同单词
                    if (FindReplaceTools::MarkSameWord(selected_text, ScintillaEditor::MarkStyle::SELECTION_MARK, m_edit))
                    {
                        //如果成功标记，则将已标记单词填充到查找替换对话框中的查找文本框中
                        //TODO:
//                            m_find_replace_dlg.SetFindString(selected_text_wcs.c_str());
                    }
                }
            }

            //标记匹配的括号
            CEditorHelper helper(m_edit);
            if (g_config.GetEditSettings().mark_matched_brackets)
                helper.MarkMatchedBrackets();

            //标记匹配的html标记
            if (g_config.GetEditSettings().mark_matched_html_mark)
            {
                const CLanguage& language = m_syntax_highlight.GetLanguage(m_cur_lan_index);
                if (language.m_id == SCLEX_XML || language.m_id == SCLEX_HTML)
                    helper.MarkMatchedHtmlMarks();
            }

            //TODO:
//                m_find_replace_dlg.EnableControl();
//                UpdateStatusBarInfo();
        }
        UpdateActionState();
    }
    else if (notification->nmhdr.code == SCN_CHARADDED)
    {
        if (notification->characterSource == SC_CHARACTERSOURCE_DIRECT_INPUT)
        {
            char ch = static_cast<char>(notification->ch);
            CEditorHelper helper(m_edit);
            //括号自动完成
            if (g_config.GetEditSettings().brackets_auto_comp)
                helper.BracketsAutoComp(ch);

            const CLanguage& language = m_syntax_highlight.GetLanguage(m_cur_lan_index);
            //HTML标记自动完成
            if (g_config.GetEditSettings().html_mark_auto_comp)
            {
                if (language.m_id == SCLEX_XML || language.m_id == SCLEX_HTML)
                {
                    helper.HtmlMarkAutoComp(ch);
                }
            }

            //显示自动完成列表
            if (g_config.GetEditSettings().show_auto_comp_list)
                helper.AutoShowCompList(language);
        }
    }

}

void MainWindow::UpdateLineNumberWidth(bool update)
{
    int current_line = m_edit->GetFirstVisibleLine();
    int leng_height = m_edit->GetLineHeight();
    int line_per_page = m_edit->height() / leng_height;
    int line_number = current_line + line_per_page;
    static int last_line_number{ -1 };
    if (update || last_line_number != line_number)
    {
        int width = m_edit->GetTextWidth(std::to_string(line_number)) + DPI(8);
        m_edit->SetLineNumberWidth(width);
        m_edit->ShowLineNumber(g_config.MenuSettings().show_line_number);
    }
    last_line_number = line_number;
}

bool MainWindow::OnCommand(const QString &strCmd, bool checked)
{
    if (strCmd == "AppAbout")
    {
        QMessageBox::about(this, QString(u8"关于 %1").arg(qApp->applicationName()), QString(u8"%1 %2\r\n这是一个Office风格的纯文本编辑器。\r\nCopyright(C) 2023 by ZhongYang").arg(qApp->applicationName()).arg(qApp->applicationVersion()));
        return true;
    }
    else if (strCmd == CMD_FileOpen)
    {
        //打开新文件前询问用户是否保存
        if (!SaveInquiry())
            return true;

        QString fileName = QFileDialog::getOpenFileName(this, QString(), QString(), u8"文件文件 (*.txt);;所有文件 (*.*)");
        if (!fileName.isEmpty())
        {
            if (!OpenFile(fileName))
            {
                QMessageBox::warning(this, QString(), u8"文件打开失败！");
            }
        }
    }
    else if (strCmd == CMD_FileSave)
    {
        OnFileSave();
    }
    else if (strCmd == CMD_FileSaveAs)
    {
        OnFileSaveAs();
    }
    else if (strCmd == CMD_EditUndo)
    {
        m_edit->Undo();
    }
    else if (strCmd == CMD_EditRedo)
    {
        m_edit->Redo();
    }
    else if (strCmd == CMD_EditCopy)
    {
        m_edit->Copy();
    }
    else if (strCmd == CMD_EditCut)
    {
        m_edit->Cut();
    }
    else if (strCmd == CMD_EditPaste)
    {
        m_edit->Paste();
    }
    else if (strCmd == CMD_FormatFont)
    {
        QFont font(g_config.GetEditSettings().font_name, g_config.GetEditSettings().font_size);
        QFontDialog dlg(font, this);
        if (dlg.exec() == QDialog::Accepted)
        {
            QFont curFont = dlg.currentFont();
            m_edit->SetFontFace(curFont.family());
            m_edit->SetFontSize(curFont.pointSize());
            UpdateLineNumberWidth(true);
            //设置字体后重新设置一下语法高亮，以解决字体设置无法立即生效的问题
            SetSyntaxHight(m_syntax_highlight.GetLanguage(m_cur_lan_index));

            auto editSettings = g_config.GetEditSettings();
            editSettings.font_name = curFont.family();
            editSettings.font_size = curFont.pointSize();
            g_config.SetEditSettings(editSettings);
        }
    }
    else if (strCmd == CMD_EditFind)
    {
        m_findReplaceDlg.show();
    }


    return RibbonFrameWindow::OnCommand(strCmd, checked);
}

void MainWindow::OnItemChanged(const QString &strId, int index, const QString &text)
{
}



void MainWindow::closeEvent(QCloseEvent *event)
{
    //询问是否保存
    if (!SaveInquiry())
        return;

    RibbonFrameWindow::closeEvent(event);
}
