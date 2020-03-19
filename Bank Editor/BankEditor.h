#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_BankEditor.h"
#include "stdafx.h"

class BankEditor : public QMainWindow
{
	Q_OBJECT

public:
	BankEditor(QWidget *parent = Q_NULLPTR);
	~BankEditor() { if (m_bank) delete m_bank; }

private slots:
	void sActionOpen();
	void sActionGenerate();
	void sActionClose();
	void sActionQuit();
	void sActionAutoPlay(bool checked);
	void sActionExport();
	void sActionClear();
	void sActionAbout();

	void sTimer();

	void sClicked(int row, int column);
	void sActivated(int row, int column);

private:
	Ui::BankEditorClass ui;

	BankFile *m_bank = nullptr;
	QTimer *m_timer;
	QSlider* m_slider = new QSlider(Qt::Orientation::Horizontal, this);

	bool m_autoplay = true;

	void setupTable();
	void clearTable();
};
