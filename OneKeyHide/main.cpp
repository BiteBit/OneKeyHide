#include "ui/onekeyhide.h"
#include <QtWidgets/QApplication>
#include <QFile>
#include <QMessageBox>

#include "util/util.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	app.setWindowIcon(QIcon(":/OneKeyHide/Resources/logo.ico"));

	QFile file(":/OneKeyHide/Resources/default_zh.qss");
	if (file.open(QIODevice::ReadOnly)) {
		OneKeyHide w;
		w.show();
		app.setStyleSheet(file.readAll());
		return app.exec();
	} else {
		MsgBoxA(NULL, "错误", "加载皮肤文件错误，请联系卖家！");
	}
}
