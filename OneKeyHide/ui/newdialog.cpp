#include "newdialog.h"

#include <assert.h>
#include <QDebug>

NewDialog::NewDialog(Hider* hider, QWidget *parent)
	: QWidget(parent),
	hider_(hider) {
	ui.setupUi(this);
	connect(ui.checkBoxSwitchWhenHide, SIGNAL(stateChanged(int)), this, SLOT(SlotSwitchCheckChanged(int)));
	connect(ui.keySequenceEditVisible, SIGNAL(keySequenceChanged(QKeySequence)), this, SLOT(SlotKeySeqChanged(QKeySequence)));
	connect(ui.keySequenceEditMax, SIGNAL(keySequenceChanged(QKeySequence)), this, SLOT(SlotKeySeqChanged(QKeySequence)));
	connect(ui.keySequenceEditMin, SIGNAL(keySequenceChanged(QKeySequence)), this, SLOT(SlotKeySeqChanged(QKeySequence)));
	ShowSwitchWidget(false);
}

NewDialog::~NewDialog() {
}

void NewDialog::Exec() {
	ShowWin(ui.listWidgetOptionalWindows);
}

void NewDialog::on_pushButtonCancel_clicked() {
	//this->close();
}

void NewDialog::on_pushButtonOk_clicked() {
	Add();
	Reset();
	//close();
}

void NewDialog::Reset() {
	ui.checkBoxOpenVoiceWhenShow->setChecked(true);
	ui.checkBoxSwitchWhenHide->setChecked(false);
	ui.keySequenceEditMax->clear();
	ui.keySequenceEditMin->clear();
	ui.keySequenceEditVisible->clear();
	ui.listWidgetOptionalWindows->clear();
	ui.listWidgetSwitchWindows->clear();
}

void NewDialog::Add() {
	auto option_checked_windows = CheckedWindow(ui.listWidgetOptionalWindows);
	auto switch_checked_windows = CheckedWindow(ui.listWidgetSwitchWindows);

	if (option_checked_windows.length() && !visible_key_seq_.toString().isEmpty()) {
		Rule rule;
		rule.visible_list = option_checked_windows;
		if (ui.checkBoxSwitchWhenHide->isChecked())
			rule.switch_list = switch_checked_windows;

		hider_->AddRule(visible_key_seq_, rule);
	}
}

WindowList NewDialog::CheckedWindow(const QListWidget* list) {
	WindowList infos;
	for (int i = 0; i < list->count(); ++i) {
		auto current_item = list->item(i);
		if (current_item->checkState() != Qt::Checked)
			continue;

		auto it_data = current_item->data(Qt::UserRole + 1);
		HWND hwnd = (HWND)((WId)(it_data.toInt()));

		Window win = hider_->FindByHwnd(hwnd);
		win.show_hide_hotkey = visible_key_seq_.toString(QKeySequence::PortableText);
		win.max_hotkey = max_key_seq_.toString(QKeySequence::PortableText);
		win.min_hotkey = min_key_seq_.toString(QKeySequence::PortableText);
		win.voice_follow_vision = ui.checkBoxOpenVoiceWhenShow->isChecked();
		if (win.hwnd)
			infos.push_back(win);
	}
	return infos;
}

void NewDialog::on_pushButtonRefresh_clicked() {
	hider_->StartEnumWindows();
}

void NewDialog::on_pushButtonTest_clicked() {
	auto checked_wins = CheckedWindow(ui.listWidgetOptionalWindows);
	for (auto& it : checked_wins) {
		ShowWindow(it.hwnd, it.visible ? SW_HIDE : SW_SHOW);
		hider_->SetVisible(it.hwnd, !it.visible);
	}

	if (ui.checkBoxSwitchWhenHide->isChecked()) {
		auto switch_checked_wins = CheckedWindow(ui.listWidgetSwitchWindows);
		for (auto& it : switch_checked_wins) {
			ShowWindow(it.hwnd, SW_SHOW);
			hider_->SetVisible(it.hwnd, !it.visible);
			BringWindowToTop(it.hwnd);
		}
	}
}

void NewDialog::SlotSwitchCheckChanged(int state) {
	ShowSwitchWidget(state);
}

void NewDialog::SlotKeySeqChanged(const QKeySequence& key_seq) {
	if (sender() == ui.keySequenceEditMax)
		max_key_seq_ = key_seq;
	else if (sender() == ui.keySequenceEditMin)
		min_key_seq_ = key_seq;
	else if (sender() == ui.keySequenceEditVisible)
		visible_key_seq_ = key_seq;
	else
		assert(0);
}

void NewDialog::ShowWin(QListWidget* list_widget) {
	if (!list_widget)
		list_widget = ui.listWidgetOptionalWindows;

	list_widget->clear();
	for (const auto& it : hider_->WindowsInfo()) {
		auto item = new QListWidgetItem;
		item->setData(Qt::UserRole + 1, QVariant(WId(it.hwnd)));
		item->setCheckState(Qt::Unchecked);
		item->setText(it.name);
		list_widget->addItem(item);
	}
}

void NewDialog::ShowSwitchWidget(bool show) {
	ui.groupBoxSwitchWindow->setVisible(show);
	ui.labelArrow->setVisible(show);
	if (show)
		ShowWin(ui.listWidgetSwitchWindows);
}