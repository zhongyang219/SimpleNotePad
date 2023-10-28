#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ribbonframewindow.h"
#include "common.h"
#include "Scintilla.h"

#include "tools/SyntaxHighlight.h"
#include "dialog/findreplacedialog.h"

class ScintillaEditor;

class MainWindow : public RibbonFrameWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    bool OpenFile(const QString& filePath);
    bool SaveFile(const QString& filePath, CodeType code);
    bool OnFileSave();
    bool OnFileSaveAs();
    bool SaveInquiry(const QString& info = NULL, int* prtn = nullptr);	//询问用户是否保存，参数为提示信息(用户点击了取消后返回false，否则返回true。rtn：对话框的返回值)

    void SetTitle();				//设置窗口标题
    void UpdateStatusBarInfo();
    void UpdateActionState();

    void SetEditorSyntaxHight();
    void SetSyntaxHight(const CLanguage& lan);

private slots:
    void OnNotify(SCNotification *pscn);

private:
    ScintillaEditor* m_edit;
    QString m_filePath;
    CodeType m_code{};
    CSyntaxHighlight m_syntax_highlight;
    int m_cur_lan_index{ -1 };      //当前选择的语言索引
    bool m_monitor_mode{ false };
    FindReplaceDialog m_findReplaceDlg;

protected:
    //数据当前行号的位数自动设置行号的宽度
    void UpdateLineNumberWidth(bool update = false);

    // RibbonFrameWindow interface
protected:
    virtual bool OnCommand(const QString &strCmd, bool checked) override;
    virtual void OnItemChanged(const QString &strId, int index, const QString &text) override;

    // QWidget interface
protected:
    virtual void closeEvent(QCloseEvent *event) override;
};
#endif // MAINWINDOW_H
