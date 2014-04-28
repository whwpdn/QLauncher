#include "AddProgramDlg.h"

#include <QFileDialog>

//---------------------------------------------------------------------------------------------
AddProgramDlg::AddProgramDlg(QWidget *parent, Qt::WFlags flags)
: QDialog(parent, flags)
{ 
    ui.setupUi(this);

    QObject::connect(ui.SelectProgramButton, SIGNAL(clicked()), this, SLOT(OnSelectProgramButton()));
}
//---------------------------------------------------------------------------------------------
AddProgramDlg::~AddProgramDlg()
{

}
//---------------------------------------------------------------------------------------------
void AddProgramDlg::OnSelectProgramButton()
{
    QString strProgramName = QFileDialog::getOpenFileName(this);

    ui.ProgramLineEdit->setText(strProgramName);
}
//---------------------------------------------------------------------------------------------