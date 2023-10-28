#ifndef FINDREPLACEDIALOG_H
#define FINDREPLACEDIALOG_H

#include <QDialog>
#include "tools/FindReplaceTools.h"

namespace Ui {
class FindReplaceDialog;
}

class FindReplaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FindReplaceDialog(QWidget *parent = nullptr);
    ~FindReplaceDialog();

    FindOption GetOptions() const;

signals:
    void findPrevious();
    void findNext();
    void replace();
    void replaceAll();
    void replaceSelection();
    void markAllClear();

private slots:
    void on_findPreviousBtn_clicked();

    void on_findNextBtn_clicked();

    void on_replaceBtn_clicked();

    void on_replaceAllBtn_clicked();

    void on_replaceSelectionBtn_clicked();


private:
    Ui::FindReplaceDialog *ui;
};

#endif // FINDREPLACEDIALOG_H
