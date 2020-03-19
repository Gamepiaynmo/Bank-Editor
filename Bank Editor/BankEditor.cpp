#include "stdafx.h"
#include "BankEditor.h"
#include "AboutDialog.h"

BankEditor::BankEditor(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	ui.mainToolBar->insertWidget(ui.action_Generate, m_slider);
	m_slider->setMaximumWidth(200);
	m_slider->setMinimum(1);
	m_slider->setMaximum(100);
	m_slider->setValue(100);
	m_slider->setToolTip("生成 Bank 的质量");

	connect(ui.action_Open, SIGNAL(triggered()), this, SLOT(sActionOpen()));
	connect(ui.action_Generate, SIGNAL(triggered()), this, SLOT(sActionGenerate()));
	connect(ui.action_Close, SIGNAL(triggered()), this, SLOT(sActionClose()));
	connect(ui.action_Quit, SIGNAL(triggered()), this, SLOT(sActionQuit()));
	connect(ui.action_AutoPlay, SIGNAL(toggled(bool)), this, SLOT(sActionAutoPlay(bool)));
	connect(ui.action_Export, SIGNAL(triggered()), this, SLOT(sActionExport()));
	connect(ui.action_Clear, SIGNAL(triggered()), this, SLOT(sActionClear()));
	connect(ui.action_About, SIGNAL(triggered()), this, SLOT(sActionAbout()));

	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(sTimer()));
	timer->start(50);

	connect(ui.table_bank, SIGNAL(cellClicked(int, int)), this, SLOT(sClicked(int, int)));
	connect(ui.table_bank, SIGNAL(cellActivated(int, int)), this, SLOT(sActivated(int, int)));
	ui.table_bank->setEditTriggers(QAbstractItemView::NoEditTriggers);

	ui.table_bank->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	ui.table_bank->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
}

void BankEditor::sActionOpen() {
	if (m_bank)
		sActionClose();

	if (m_bank == nullptr) {
		QString filename = QFileDialog::getOpenFileName(this, "选择 Bank 文件", "", "Bank 音源文件 (*.bank)");
		if (!filename.isEmpty()) {
			m_bank = new BankFile();
			try {
				m_bank->importFromFile(QFile(filename));
			} catch (QString error) {
				QMessageBox::critical(this, "错误", error);
				delete m_bank;
				m_bank = nullptr;
			}
		}
	}

	if (m_bank)
		setupTable();
}

void BankEditor::sActionGenerate() {
	if (m_bank) {
		QString filename = QFileDialog::getSaveFileName(this, "生成 Bank 保存至", "", "Bank 音源文件 (*.bank)");
		if (!filename.isEmpty()) {
			try {
				m_bank->exportToFile(QFile(filename), m_slider->value());
				QMessageBox::information(this, "生成 Bank", "生成完毕");
			} catch (QString error) {
				QMessageBox::critical(this, "错误", error);
			}
		}
	}
}

void BankEditor::sActionClose() {
	if (m_bank) {
		delete m_bank;
		m_bank = nullptr;
		clearTable();
	}
}

void BankEditor::sActionQuit() {
	if (m_bank) {
		delete m_bank;
		m_bank = nullptr;
	}
	qApp->quit();
}

void BankEditor::sActionAutoPlay(bool checked) {
	m_autoplay = checked;
}

void BankEditor::sActionExport() {
	if (m_bank) {
		QString dirname = QFileDialog::getExistingDirectory(this, "选择导出目录", "");
		if (!dirname.isEmpty()) {
			QDir dir(dirname);
			QSet<int> selectedrows;
			for (QTableWidgetItem* item : ui.table_bank->selectedItems())
				selectedrows.insert(item->row());
			try {
				for (int index : selectedrows)
					m_bank->exportSound(dir, index);
			} catch (QString error) {
				QMessageBox::critical(this, "错误", error);
			}
		}
	}
}

void BankEditor::sActionClear() {
	if (m_bank) {
		for (QTableWidgetItem* item : ui.table_bank->selectedItems()) {
			try {
				m_bank->clearReplaceFile(item->row());

				ui.table_bank->item(item->row(), 2)->setText("");
				ui.table_bank->item(item->row(), 3)->setText("");
			} catch (QString error) {
				QMessageBox::critical(this, "错误", error);
			}
		}
	}
}

void BankEditor::sActionAbout() {
	AboutDialog(this).exec();
}

void BankEditor::setupTable() {
	for (int i = 0; i < m_bank->getSubsoundCount(); i++) {
		BankSound* sound = m_bank->getSubsound(i);

		int row = ui.table_bank->rowCount();
		ui.table_bank->insertRow(row);
		ui.table_bank->setRowHeight(row, 24);

		QTableWidgetItem* item = new QTableWidgetItem();
		item->setText(sound->getName());
//		item->setTextAlignment(Qt::AlignRight);
		ui.table_bank->setItem(row, 0, item);

		item = new QTableWidgetItem();
		item->setText(QString("%1 ms").arg(sound->getLengthMs()));
		item->setTextAlignment(Qt::AlignRight);
		ui.table_bank->setItem(row, 1, item);

		item = new QTableWidgetItem();
		ui.table_bank->setItem(row, 2, item);

		item = new QTableWidgetItem();
		item->setTextAlignment(Qt::AlignRight);
		ui.table_bank->setItem(row, 3, item);
	}
}

void BankEditor::clearTable() {
	ui.table_bank->clearContents();
	ui.table_bank->setRowCount(0);
}

void BankEditor::sTimer() {
	try {
		if (m_bank) m_bank->updateFmod();
	} catch (QString error) {
		QMessageBox::critical(this, "错误", error);
		qApp->quit();
	}
}

void BankEditor::sClicked(int row, int column) {
	if (m_bank && m_autoplay) {
		try {
			if (column == 0) {
				m_bank->playSound(row);
			} else if (column == 2) {
				m_bank->playReplaceSound(row);
			}
		} catch (QString error) {
			QMessageBox::critical(this, "错误", error);
		}
	}
}

void BankEditor::sActivated(int row, int column) {
	if (column == 2) {
		QString filename = QFileDialog::getOpenFileName(this, "选择音频文件", "", "音频文件 (*.wav *.ogg *.mp3)");
		if (!filename.isEmpty()) {
			try {
				m_bank->setReplaceFile(row, filename);
			} catch (QString error) {
				QMessageBox::critical(this, "错误", error);
			}
			ui.table_bank->item(row, 2)->setText(filename);
			ui.table_bank->item(row, 3)->setText(QString("%1 ms")
				.arg(m_bank->getSubsound(row)->getNewLengthMs()));
		}
	}
}