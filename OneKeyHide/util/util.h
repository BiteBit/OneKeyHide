#ifndef ONEKEYHIDE_UTIL_H_
#define ONEKEYHIDE_UTIL_H_

#include <QMessageBox>

#define G2U(str) QString::fromLocal8Bit(str)

#define MsgBox(parent, title, content) \
	QMessageBox msgBox;\
	msgBox.setTextFormat(Qt::TextFormat::RichText);\
	msgBox.setText(G2U(content));\
	msgBox.setWindowTitle(G2U(title));\
	msgBox.setStandardButtons(QMessageBox::Ok);\
	msgBox.exec();\

#endif // ONEKEYHIDE_UTIL_H_