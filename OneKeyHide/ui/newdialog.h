#ifndef NEWDIALOG_H
#define NEWDIALOG_H

#include <QWidget>
#include "ui_newdialog.h"

#include "base-ui/frameless_dialog.h"

class NewDialog : public FramelessDialog
{
	Q_OBJECT

public:
	NewDialog(QWidget *parent = 0);
	~NewDialog();

	void SetHideMode(bool is_hide);
private:
	Ui::NewDialog ui;

	bool is_hide_mode_;
};

#endif // NEWDIALOG_H
