#ifndef REGISTERCONTROL_H
#define REGISTERCONTROL_H

#include <QObject>

#include "3rd/vprotect/VP_License_Sdk.h"

class RegisterControl : public QObject {
	Q_OBJECT

public:
	RegisterControl(QObject *parent);
	~RegisterControl();

	static RegisterControl* Instance();

	bool IsRegister();

private:
	QString id_;
	QString username_;
	int left_count_;
	int used_count_;

	static RegisterControl* instance_;
};

#endif // REGISTERCONTROL_H
