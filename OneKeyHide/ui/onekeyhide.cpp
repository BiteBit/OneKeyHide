#include "onekeyhide.h"

#include <QTableWidgetItem>
#include <QPixmap>
#include <QLabel>
#include <QHBoxLayout>

const int kShadowWidth = 5;

OneKeyHide::OneKeyHide(QWidget *parent)
	: SupportFrameless<QDialog>(parent, 5, true) {
	ui.setupUi(this);
	setAttribute(Qt::WA_TranslucentBackground);
	
	InitTable(ui.tableWidgetHiding);
	
	hider_ = new Hider(this);
	new_dialog_ = new NewDialog(hider_);

	SetWidgetToWidget(ui.pageCreate, new_dialog_);
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
	table->setColumnWidth(0, 300);
	table->setColumnWidth(1, 60);
	table->setColumnWidth(2, 100);
	table->setColumnWidth(3, 100);
	table->setColumnWidth(4, 100);
}

void OneKeyHide::AddRow(QTableWidget* table, bool is_hide, const Rule& rule) {
	for (const auto& it : is_hide ? rule.visible_list : rule.switch_list) {
		auto index = table->rowCount();
		table->setRowCount(index + 1);

		auto name_item = new QTableWidgetItem;
		name_item->setData(Qt::UserRole + 1, QVariant((WId)it.hwnd));
		name_item->setText(it.name);

		auto visible_item = new QTableWidgetItem;
		visible_item->setText(it.visible ? QString::fromLocal8Bit("ÏÔÊ¾") : QString::fromLocal8Bit("Òþ²Ø"));

		auto min_hotkey_item = new QTableWidgetItem;
		min_hotkey_item->setText(it.min_hotkey);

		auto max_hotkey_item = new QTableWidgetItem;
		max_hotkey_item->setText(it.max_hotkey);

		auto show_hide_hotkey_item = new QTableWidgetItem;
		show_hide_hotkey_item->setText(it.show_hide_hotkey);

		auto voice_follow_vision = new QTableWidgetItem;
		voice_follow_vision->setText(it.voice_follow_vision ? QString::fromLocal8Bit("ÊÇ") : QString::fromLocal8Bit("·ñ"));

		table->setItem(index, 0, name_item);
		table->setItem(index, 1, visible_item);
		table->setItem(index, 2, max_hotkey_item);
		table->setItem(index, 3, min_hotkey_item);
		table->setItem(index, 4, show_hide_hotkey_item);
		table->setItem(index, 5, voice_follow_vision);
	}
}

void OneKeyHide::on_pushButtonHome_clicked() {
	ui.stackedWidget->setCurrentWidget(ui.pageHome);
}

void OneKeyHide::on_pushButtonNewHide_clicked() {
	ui.stackedWidget->setCurrentWidget(ui.pageCreate);
	new_dialog_->Exec();
}

void OneKeyHide::on_pushButtonSettings_clicked() {
	ui.stackedWidget->setCurrentWidget(ui.pageSettings);
}

void OneKeyHide::on_pushButtonHelp_clicked() {
	ui.stackedWidget->setCurrentWidget(ui.pageHelp);
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

void OneKeyHide::OnEnumStart() {

}

void OneKeyHide::OnEnumFinish(const WindowHash& infos) {
	if (new_dialog_)
		new_dialog_->ShowWin();
	UpdateList(infos);
}

void OneKeyHide::OnNewRuleAdd(const Rule&  rule) {
	AddRow(ui.tableWidgetHiding, true, rule);
}

void OneKeyHide::OnNewRuleUpdate(const Rule& rule) {
}

void OneKeyHide::OnWindowVisibleChanged(HWND hwnd, bool visible) {
	for (int i = 0; i < ui.tableWidgetHiding->rowCount(); ++i) {
		auto data = ui.tableWidgetHiding->item(i, 0)->data(Qt::UserRole + 1);
		HWND current_hwnd = (HWND)(WId)(data.toInt());
		if (current_hwnd == hwnd) {
			ui.tableWidgetHiding->item(i, 1)->setText(visible ? QString::fromLocal8Bit("ÏÔÊ¾") : QString::fromLocal8Bit("Òþ²Ø"));
			break;
		}
	}
}

void OneKeyHide::UpdateList(const WindowHash& windows) {
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