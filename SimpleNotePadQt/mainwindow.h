#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ribbonframewindow.h"

class ScintillaEditBase;

class MainWindow : public RibbonFrameWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:

private slots:
    void documentWasModified();

private:
    ScintillaEditBase *textEdit;
    QString curFile;

    // RibbonFrameWindow interface
protected:
    virtual bool OnCommand(const QString &strCmd, bool checked) override;
    virtual void OnItemChanged(const QString &strId, int index, const QString &text) override;
    virtual QWidget *CreateUserWidget(const QString &strId, QWidget *pParent) override;
};
#endif // MAINWINDOW_H
