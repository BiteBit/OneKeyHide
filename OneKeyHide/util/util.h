#ifndef ONEKEYHIDE_UTIL_H_
#define ONEKEYHIDE_UTIL_H_

#include <QMessageBox>

#define G2U(str) QString::fromLocal8Bit(str)

#define MsgBoxA(parent, title, content) \
	QMessageBox::information(parent, G2U(title), G2U(content));\

#endif // ONEKEYHIDE_UTIL_H_