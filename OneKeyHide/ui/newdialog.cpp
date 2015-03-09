#include "newdialog.h"

#include <assert.h>
#include <QDebug>
#include <QFileIconProvider>

#include "util/util.h"

NewDialog::NewDialog(Hider* hider, QWidget *parent)
	: QWidget(parent),
	hider_(hider) {
	ui.setupUi(this);
	InitTable(ui.tableWidgetSwitching);
	InitTable(ui.tableWidgetOption);


	connect(ui.checkBoxSwitchWhenHide, SIGNAL(stateChanged(int)), this, SLOT(SlotSwitchCheckChanged(int)));
	connect(ui.keySequenceEditVisible, SIGNAL(keySequenceChanged(QKeySequence)), this, SLOT(SlotKeySeqChanged(QKeySequence)));
	ShowSwitchWidget(false);
}

NewDialog::~NewDialog() {
}

void NewDialog::InitTable(QTableWidget* table) {
	table->setColumnWidth(0, 18);
	table->setColumnWidth(1, 45);
	table->setColumnWidth(2, 300);
	table->setColumnWidth(3, 285);
}

void NewDialog::Exec() {
	ShowWin(ui.tableWidgetOption);
}

void NewDialog::on_pushButtonCancel_clicked() {
}

void NewDialog::on_pushButtonOk_clicked() {
	if (Add())
		Reset();
}

void NewDialog::Reset() {
	ui.checkBoxOpenVoiceWhenShow->setChecked(true);
	ui.checkBoxSwitchWhenHide->setChecked(false);
	ui.keySequenceEditVisible->clear();
	ui.tableWidgetOption->clearContents();
	ui.tableWidgetSwitching->clearContents();
}

bool NewDialog::Add() {
	auto option_checked_windows = CheckedWindow(ui.tableWidgetOption);
	auto switch_checked_windows = CheckedWindow(ui.tableWidgetSwitching);

	if (option_checked_windows.length() && !visible_key_seq_.toString().isEmpty()) {
		Rule rule;
		rule.visible_list = option_checked_windows;
		if (ui.checkBoxSwitchWhenHide->isChecked())
			rule.switch_list = switch_checked_windows;

		hider_->AddRule(visible_key_seq_, rule);
		return true;
	} else {
		MsgBoxA(this, "警告", "未选择窗口或者未设置显隐快捷键");
		return false;
	}
}

WindowList NewDialog::CheckedWindow(const QTableWidget* list) {
	WindowList infos;
	for (int i = 0; i < list->rowCount(); ++i) {
		auto current_item = list->item(i, 0);
		if (current_item->checkState() != Qt::Checked)
			continue;

		auto it_data = current_item->data(Qt::UserRole + 1);
		HWND hwnd = (HWND)((WId)(it_data.toInt()));

		Window win = hider_->FindByHwnd(hwnd);
		win.show_hide_hotkey = visible_key_seq_.toString(QKeySequence::PortableText);
		win.voice_follow_vision = ui.checkBoxOpenVoiceWhenShow->isChecked();
		if (win.hwnd)
			infos.push_back(win);
	}
	return infos;
}

void NewDialog::on_pushButtonTest_clicked() {
	auto checked_wins = CheckedWindow(ui.tableWidgetOption);
	for (auto& it : checked_wins) {
		ShowWindow(it.hwnd, it.visible ? SW_HIDE : SW_SHOW);
		hider_->SetVisible(it.hwnd, !it.visible);
	}

	if (ui.checkBoxSwitchWhenHide->isChecked()) {
		auto switch_checked_wins = CheckedWindow(ui.tableWidgetSwitching);
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
	visible_key_seq_ = key_seq;
}

void NewDialog::ShowWin(int mode) {
	if (mode == 0)
		ShowWin(ui.tableWidgetOption);
	else
		ShowWin(ui.tableWidgetSwitching);
}

void NewDialog::ShowWin(QTableWidget* table_widget) {
	if (!table_widget)
		table_widget = ui.tableWidgetOption;

	QList<HWND> optional_select_windows;
	for (int i = 0; i < table_widget->rowCount(); ++i) {
		if (table_widget->item(i, 0)->checkState() != Qt::Checked)
			continue;

		auto id = table_widget->item(i, 0)->data(Qt::UserRole + 1).toInt();
		optional_select_windows.append((HWND)(WId)id);
	}

	table_widget->clearContents();
	int index = 0;
	for (const auto& it : hider_->WindowsInfo()) {
		if (it.setted)
			continue;
		table_widget->setRowCount(index + 1);

		auto check_item = new QTableWidgetItem;
		auto pid_item = new QTableWidgetItem;
		auto title_item = new QTableWidgetItem;
		auto path_item = new QTableWidgetItem;

		check_item->setData(Qt::UserRole + 1, QVariant(WId(it.hwnd)));
		check_item->setCheckState(optional_select_windows.contains(it.hwnd) ? Qt::Checked : Qt::Unchecked);
		pid_item->setText(QString::number(it.process_id));
		title_item->setText(it.title);
		title_item->setToolTip(it.title);
		QFileIconProvider icon_provider;
		title_item->setIcon(icon_provider.icon(QFileInfo(it.exe_path)));
		path_item->setText(it.exe_path);
		path_item->setToolTip(it.exe_path);

		table_widget->setItem(index, 0, check_item);
		table_widget->setItem(index, 1, pid_item);
		table_widget->setItem(index, 2, title_item);
		table_widget->setItem(index, 3, path_item);

		index++;
	}
}

void NewDialog::ShowSwitchWidget(bool show) {
	ui.groupBoxSwitchWindow->setVisible(show);
	ui.labelArrow->setVisible(show);
	if (show)
		ShowWin(ui.tableWidgetSwitching);
}