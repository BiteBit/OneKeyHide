#ifndef NEWDIALOG_H
#define NEWDIALOG_H

#include <QWidget>
#include "ui_newdialog.h"

#include "core/hider.h"

class NewDialog : public QWidget {
	Q_OBJECT

public:
	NewDialog(Hider* hider, QWidget *parent = 0);
	~NewDialog();

	void Exec();
	
	void ShowWin(QTableWidget* list_widget = NULL);
private slots:
	void on_pushButtonCancel_clicked();

	void on_pushButtonOk_clicked();

	void on_pushButtonRefresh_clicked();

	void on_pushButtonTest_clicked();

	void SlotSwitchCheckChanged(int);

	void SlotKeySeqChanged(const QKeySequence&);
private:
	void InitTable(QTableWidget* table);

	void ShowSwitchWidget(bool show);

	bool Add();

	void Reset();

	WindowList CheckedWindow(const QTableWidget* list);
private:
	Ui::NewDialog ui;

	Hider* hider_;

	//QKeySequence max_key_seq_;
	//QKeySequence min_key_seq_;
	QKeySequence visible_key_seq_;
};

#endif // NEWDIALOG_H
