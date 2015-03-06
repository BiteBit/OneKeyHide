#include "onekeyhide.h"

#include <QTableWidgetItem>
#include <QPixmap>
#include <QLabel>

const int kShadowWidth = 5;

OneKeyHide::OneKeyHide(QWidget *parent)
	: SupportFrameless<QDialog>(parent) {
	ui.setupUi(this);
	setAttribute(Qt::WA_TranslucentBackground);

	//ui.tableWidget->setColumnCount(6);
	//ui.tableWidget->setColumnWidth(0, 300);
	//ui.tableWidget->setColumnWidth(1, 60);
	//ui.tableWidget->setColumnWidth(2, 100);
	//ui.tableWidget->setColumnWidth(3, 100);
	//ui.tableWidget->setColumnWidth(4, 100);
	//ui.tableWidget->setColumnWidth(5, 100);

	hider_ = new Hider(this);
}

OneKeyHide::~OneKeyHide() {
}

void OneKeyHide::on_pushButtonNewHide_clicked() {
	new_dialog_.SetHideMode(true);
	new_dialog_.exec();
}

void OneKeyHide::on_pushButtonNewSwitch_clicked() {
	new_dialog_.SetHideMode(false);
	new_dialog_.exec();
}

void OneKeyHide::on_pushButtonSettings_clicked() {
}

void OneKeyHide::on_pushButtonHelp_clicked() {
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

void OneKeyHide::OnEnumFinish(const QHash<HWND, Window>& infos) {
	UpdateList(infos);
}

void OneKeyHide::UpdateList(const QHash<HWND, Window>& windows) {
	for (const auto& it : windows) {
		//auto index = ui.tableWidget->rowCount();
		//ui.tableWidget->setRowCount(index + 1);

		//auto name_item = new QTableWidgetItem;
		//name_item->setText(it.name);
		//
		//auto visible_item = new QTableWidgetItem;
		//visible_item->setText(it.visible ? tr("visible") : tr("invisible"));

		//auto min_hotkey_item = new QTableWidgetItem;
		//min_hotkey_item->setText(it.min_hotkey);

		//auto max_hotkey_item = new QTableWidgetItem;
		//max_hotkey_item->setText(it.max_hotkey);

		//auto show_hide_hotkey_item = new QTableWidgetItem;
		//show_hide_hotkey_item->setText(it.show_hide_hotkey);

		//ui.tableWidget->setItem(index, 0, name_item);
		//ui.tableWidget->setItem(index, 1, visible_item);
		//ui.tableWidget->setItem(index, 2, min_hotkey_item);
		//ui.tableWidget->setItem(index, 4, max_hotkey_item);
		//ui.tableWidget->setItem(index, 5, show_hide_hotkey_item);
	}
}

bool OneKeyHide::nativeEvent(const QByteArray& eventType, void* message, long* result) {
	MSG* msg = (MSG*)message;
	switch (msg->message) {
	case WM_NCHITTEST: {
		int xPos = GET_X_LPARAM(msg->lParam) - this->frameGeometry().x();
		int yPos = GET_Y_LPARAM(msg->lParam) - this->frameGeometry().y();

		if (QRect(0, 0, this->width() - 150, 15).contains(xPos, yPos)        //标题栏不包括最大化最小化按钮，上边
			|| QRect(ui.pushButtonNewHide->width() * 4, 30, this->width(), 60).contains(xPos, yPos)    //按钮栏右边 logo等空白
			|| QRect(0, 0, 10, this->height()).contains(xPos, yPos)         //左边
			|| QRect(0, this->height() - kShadowWidth, this->width(), this->height()).contains(xPos, yPos)    //下边
			|| QRect(this->width() - kShadowWidth, 0, this->width(), this->height()).contains(xPos, yPos))    //右边
			*result = HTCAPTION;

		if (xPos > shadow_width_ - 2 && xPos < shadow_width_ + 2)
			*result = HTLEFT;
		else if (xPos >(this->width() - shadow_width_ - 2) && xPos < (this->width()))
			*result = HTRIGHT;
		else if (yPos > shadow_width_ - 2 && yPos < shadow_width_ + 2)
			*result = HTTOP;
		else if (yPos >(this->height() - shadow_width_ - 2) && yPos < (this->height() - shadow_width_ + 2))
			*result = HTBOTTOM;
		else if (xPos > shadow_width_ - 2 && xPos < shadow_width_ + 2 && yPos > 18 && yPos < shadow_width_ + 2)
			*result = HTTOPLEFT;
		else if (xPos >(this->width() - shadow_width_ - 2) && xPos < (this->width() - shadow_width_ + 2) && yPos > shadow_width_ - 2 && yPos < shadow_width_ + 2)
			*result = HTTOPRIGHT;
		else if (xPos > shadow_width_ - 2 && xPos < shadow_width_ + 2 && yPos >(this->height() - shadow_width_ - 2) && yPos < (this->height() - shadow_width_ - 2))
			*result = HTBOTTOMLEFT;
		else if (xPos >(this->width() - shadow_width_ - 2) && xPos < (this->width() - shadow_width_ + 2) && yPos >(this->height() - shadow_width_ - 2) && yPos < (this->height() - shadow_width_ + 2))
			*result = HTBOTTOMRIGHT;
	}
		break;
	}

	return *result != 0;
}