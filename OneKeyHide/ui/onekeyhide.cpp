#include "onekeyhide.h"

#include <QTableWidgetItem>
#include <QPixmap>
#include <QLabel>
#include <QHBoxLayout>
#include <QDebug>
#include <QFileIconProvider>

#include "ui/settingswidget.h"
#include "util/util.h"

const int kShadowWidth = 5;

OneKeyHide::OneKeyHide(QWidget *parent)
	: SupportFrameless<QDialog>(parent, 5, true) {
	ui.setupUi(this);
	setAttribute(Qt::WA_TranslucentBackground);
	
	InitTable(ui.tableWidgetHiding);
	
	hider_ = new Hider(this);
	new_dialog_ = new NewDialog(hider_);
	SetWidgetToWidget(ui.pageCreate, new_dialog_);

	auto setting_widget = new SettingsWidget(this);
	SetWidgetToWidget(ui.pageSettings, setting_widget);

	ui.pushButtonHome->click();
}

OneKeyHide::~OneKeyHide() {
}

void OneKeyHide::SetWidgetToWidget(QWidget* parent, QWidget* child) {
	auto* layout = new QHBoxLayout;
	layout->setMargin(0);
	layout->addWidget(child);
	parent->setLayout(layout);
}

void OneKeyHide::InitTable(QTableWidget* table) {
	table->setColumnWidth(0, 200);
	table->setColumnWidth(1, 40);
	table->setColumnWidth(2, 100);
	table->setColumnWidth(3, 60);
	table->setColumnWidth(4, 200);
	table->setColumnWidth(5, 280);
}

void OneKeyHide::AddRow(QTableWidget* table, bool is_hide, const Rule& rule) {
	for (const auto& it : is_hide ? rule.visible_list : rule.switch_list) {
		auto index = table->rowCount();
		table->setRowCount(index + 1);

		auto title_item = new QTableWidgetItem;
		title_item->setData(Qt::UserRole + 1, QVariant((WId)it.hwnd));
		title_item->setText(it.title);
		title_item->setToolTip(it.title);

		auto visible_item = new QTableWidgetItem;
		visible_item->setText(it.visible ? G2U("ÏÔÊ¾") : G2U("Òþ²Ø"));

		auto show_hide_hotkey_item = new QTableWidgetItem;
		show_hide_hotkey_item->setText(it.show_hide_hotkey);

		auto voice_follow_vision = new QTableWidgetItem;
		voice_follow_vision->setText(it.voice_follow_vision ? G2U("ÊÇ") : G2U("·ñ"));

		auto switch_to_item = new QTableWidgetItem;
		QString switch_window_list_str;
		for (const auto& iter: rule.switch_list)
			switch_window_list_str += iter.title;
		switch_to_item->setText(switch_window_list_str);
		switch_to_item->setToolTip(switch_window_list_str);

		auto path_item = new QTableWidgetItem;
		path_item->setText(it.exe_path);
		path_item->setToolTip(it.exe_path);
		
		QFileIconProvider icon_provider;
		title_item->setIcon(icon_provider.icon(QFileInfo(it.exe_path)));
		table->setItem(index, 0, title_item);
		table->setItem(index, 1, visible_item);
		table->setItem(index, 2, show_hide_hotkey_item);
		table->setItem(index, 3, voice_follow_vision);
		table->setItem(index, 4, switch_to_item);
		table->setItem(index, 5, path_item);
	}
}

void OneKeyHide::on_pushButtonHome_clicked() {
	ui.stackedWidget->setCurrentWidget(ui.pageHome);
	hider_->EnableEnum(false);
}

void OneKeyHide::on_pushButtonNewHide_clicked() {
	hider_->EnableEnum(true);
	hider_->StartEnumWindows();
	ui.stackedWidget->setCurrentWidget(ui.pageCreate);
	new_dialog_->Exec();
}

void OneKeyHide::on_pushButtonSettings_clicked() {
	ui.stackedWidget->setCurrentWidget(ui.pageSettings);
	hider_->EnableEnum(false);
}

void OneKeyHide::on_pushButtonHelp_clicked() {
	ui.stackedWidget->setCurrentWidget(ui.pageHelp);
	hider_->EnableEnum(false);
}

void OneKeyHide::on_pushButtonClose_clicked() {
	this->close();

}

void OneKeyHide::on_pushButtonMax_clicked() {
	if (isMaximized())
		showNormal();
	else
		showMaximized();
}

void OneKeyHide::on_pushButtonMin_clicked() {
	this->showMinimized();
}

void OneKeyHide::SlotVisibleActivated() {
	if (isVisible())
		hide();
	else
		show();
}

void OneKeyHide::SlotOnekeyActivated() {
}

void OneKeyHide::OnEnumStart() {

}

void OneKeyHide::OnEnumFinish(const WindowHash& infos) {
	if (new_dialog_) {
		new_dialog_->ShowWin(0);
		new_dialog_->ShowWin(1);
	}
}

void OneKeyHide::OnNewRuleAdd(const Rule&  rule) {
	AddRow(ui.tableWidgetHiding, true, rule);
	ui.pushButtonHome->click();
}

void OneKeyHide::OnNewRuleUpdate(const Rule& rule) {
}

void OneKeyHide::OnWindowVisibleChanged(HWND hwnd, bool visible) {
	for (int i = 0; i < ui.tableWidgetHiding->rowCount(); ++i) {
		auto data = ui.tableWidgetHiding->item(i, 0)->data(Qt::UserRole + 1);
		HWND current_hwnd = (HWND)(WId)(data.toInt());
		if (current_hwnd == hwnd)
			ui.tableWidgetHiding->item(i, 1)->setText(visible ? G2U("ÏÔÊ¾") : G2U("Òþ²Ø"));
	}
}

bool OneKeyHide::nativeEvent(const QByteArray& eventType, void* message, long* result) {
	MSG* msg = (MSG*)message;
	switch (msg->message) {
	case WM_NCHITTEST: {
		int xPos = GET_X_LPARAM(msg->lParam) - this->frameGeometry().x();
		int yPos = GET_Y_LPARAM(msg->lParam) - this->frameGeometry().y();
		if (QRect(0, 0, this->width() - 150, 15).contains(xPos, yPos) || QRect(ui.pushButtonNewHide->width() * 4, 30, this->width(), 60).contains(xPos, yPos))
			*result = HTCAPTION;
		else
			return SupportFrameless::nativeEvent(eventType, message, result);
	}
		break;
	}

	return *result != 0;
}