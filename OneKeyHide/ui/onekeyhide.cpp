#include "onekeyhide.h"

#include <QTableWidgetItem>
#include <QPixmap>
#include <QLabel>
#include <QHBoxLayout>
#include <QDebug>
#include <QFileIconProvider>
#include <QContextMenuEvent>
#include <QMovie>

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

	help_layout_ = new QVBoxLayout;
	help_layout_->setSpacing(20);
	ui.scrollAreaWidgetContents->setLayout(help_layout_);
	auto boss_link = new QLabel(G2U("<html><head/><body><p align='center'><a href='http://shop117029376.taobao.com/'><span style=' font-size:28pt; font-weigth:100pt; text-decoration: underline; color:#ff0000;'>老板不在官方旗舰店</span></a></p></body></html>"));
	boss_link->setOpenExternalLinks(true);
	help_layout_->addWidget(boss_link);
	help_layout_->addSpacerItem(new QSpacerItem(20, 40));
	AddHelp(G2U("1.新建隐藏窗口"), ":/OneKeyHide/Resources/help/new_hide.gif");
	AddHelp(G2U("2.新建切换窗口"), ":/OneKeyHide/Resources/help/new_switch.gif");
	AddHelp(G2U("3.一键隐藏【老板不在】"), ":/OneKeyHide/Resources/help/hide.gif");
	AddHelp(G2U("4.修改一条窗口切换规则"), ":/OneKeyHide/Resources/help/modify.gif");
	AddHelp(G2U("5.删除一条窗口切换规则"), ":/OneKeyHide/Resources/help/delete.gif");
	AddHelp(G2U("6.检测升级更新"), ":/OneKeyHide/Resources/help/update.gif");
	AddHelp(G2U("7.安装程序"), ":/OneKeyHide/Resources/help/install.gif");
	AddHelp(G2U("8.卸载程序"), ":/OneKeyHide/Resources/help/uninstall.gif");
	help_layout_->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Expanding));
	
	auto delete_act = new QAction(G2U("删除"), &menu_);
	connect(delete_act, SIGNAL(triggered()), this, SLOT(SlotDelete()));
	menu_.addAction(delete_act);

	ui.tableWidgetHiding->installEventFilter(this);

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

bool OneKeyHide::eventFilter(QObject* obj, QEvent* e) {
	if (obj == ui.tableWidgetHiding && e->type() == QEvent::ContextMenu) {
		QContextMenuEvent* menu_event = (QContextMenuEvent*)e;
		menu_.move(menu_event->globalPos());
		menu_.exec();
		return true;
	}

	return QDialog::eventFilter(obj, e);
}

void OneKeyHide::AddHelp(const QString& title, const QString& gif_path) {
	auto layout = new QVBoxLayout;
	QLabel* title_label = new QLabel(title);
	title_label->setAlignment(Qt::AlignCenter);
	title_label->setFixedHeight(30);
	title_label->setStyleSheet("color:red");
	title_label->setFont(QFont("", 20, 30));

	QMovie* gif = new QMovie(gif_path);
	QLabel* gif_label = new QLabel;
	gif_label->setAlignment(Qt::AlignCenter);
	gif_label->setMovie(gif);
	gif->start();

	auto spacer_label = new QLabel;
	spacer_label->setFixedHeight(10);
	spacer_label->setStyleSheet("background-color: rgb(0, 200, 0);");

	layout->addWidget(title_label);
	layout->addWidget(gif_label);
	layout->addWidget(spacer_label);
	help_layout_->addLayout(layout);
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
		visible_item->setText(it.visible ? G2U("显示") : G2U("隐藏"));

		auto show_hide_hotkey_item = new QTableWidgetItem;
		show_hide_hotkey_item->setText(it.show_hide_hotkey);

		auto voice_follow_vision = new QTableWidgetItem;
		voice_follow_vision->setText(it.voice_follow_vision ? G2U("是") : G2U("否"));

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
	hider_->OneKeyAllHide();
	SlotVisibleActivated();
}

void OneKeyHide::SlotDelete() {
	int current = ui.tableWidgetHiding->currentRow();
	if (current)
		return;

	HWND hwnd = (HWND)(WId)(ui.tableWidgetHiding->item(current, 0)->data(Qt::UserRole + 1).toInt());
	hider_->SetUnsetted(hwnd);
	ui.tableWidgetHiding->removeRow(current);
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
			ui.tableWidgetHiding->item(i, 1)->setText(visible ? G2U("显示") : G2U("隐藏"));
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