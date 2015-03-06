#include "newdialog.h"

NewDialog::NewDialog(QWidget *parent)
	: FramelessDialog(parent) {
	ui.setupUi(this);
}

NewDialog::~NewDialog() {

}

void NewDialog::SetHideMode(bool is_hide) {
	is_hide_mode_ = is_hide;
	SetTitle(is_hide_mode_ ? "New Hide" : "New Switch");
}
