#include "stdafx.h"
#include "AboutDialog.h"

AboutDialog::AboutDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
}

AboutDialog::~AboutDialog()
{
}
