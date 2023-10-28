#include "findreplacedialog.h"
#include "ui_findreplacedialog.h"

FindReplaceDialog::FindReplaceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FindReplaceDialog)
{
    ui->setupUi(this);
}

FindReplaceDialog::~FindReplaceDialog()
{
    delete ui;
}

FindOption FindReplaceDialog::GetOptions() const
{
    FindOption options;
    options.find_str = ui->findContentCombo->currentText();
    options.replace_str = ui->replaceCombo->currentText();
    options.match_whole_word = ui->matchWholeWordCheck->isChecked();
    options.match_case = ui->matchCaseCheck->isChecked();
    options.find_loop = ui->findRepeatCheck->isChecked();
    if (ui->modeNormalRadio)
    return options;
}

void FindReplaceDialog::on_findPreviousBtn_clicked()
{
    emit findPrevious();
}

void FindReplaceDialog::on_findNextBtn_clicked()
{
    emit findNext();
}

void FindReplaceDialog::on_replaceBtn_clicked()
{
    emit replace();
}

void FindReplaceDialog::on_replaceAllBtn_clicked()
{
    emit replaceAll();
}

void FindReplaceDialog::on_replaceSelectionBtn_clicked()
{
    emit replaceSelection();
}
