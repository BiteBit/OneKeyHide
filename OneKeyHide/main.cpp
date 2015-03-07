#include "ui/onekeyhide.h"
#include <QtWidgets/QApplication>
#include <QFile>
#include <QMessageBox>

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
		QMessageBox::information(NULL, "Error", "load skin file error!");
	}
}
