#ifndef ADDPROGRAMDLG_H
#define ADDPROGRAMDLG_H

#include <QtGui/QDialog>
#include "ui_AddProgramDlg.h"

class AddProgramDlg : public QDialog
{
    Q_OBJECT
public:
    AddProgramDlg(QWidget *parent = 0, Qt::WFlags flags = 0);
    ~AddProgramDlg();

    public slots:
        void OnSelectProgramButton();


public:
    Ui::AddProgramDlg ui;
};


#endif // ADDPROGRAMDLG_H