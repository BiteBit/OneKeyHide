#include "ui/onekeyhide.h"
#include <QtWidgets/QApplication>
#include <QFile>
#include <QMessageBox>

#include "core/registercontrol.h"
#include "util/util.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	app.setWindowIcon(QIcon(":/OneKeyHide/Resources/logo.ico"));

	if (!RegisterControl::Instance()->IsRegister()) {
		MsgBox(NULL, "错误", "<p>软件未注册，请联系老板不在购买许可！</p> <a href='shop117029376.taobao.com'>shop117029376.taobao.com</a>");
		return 0;
	}

	QFile file(":/OneKeyHide/Resources/default_zh.qss");
	if (file.open(QIODevice::ReadOnly)) {
		OneKeyHide w;
		w.show();
		app.setStyleSheet(file.readAll());
		return app.exec();
	} else {
		MsgBox(NULL, "错误", "加载皮肤文件错误，请联系卖家！");
	}
}
