#ifndef ONEKEYHIDE_H
#define ONEKEYHIDE_H

#include <QtWidgets/QDialog>
#include "ui_onekeyhide.h"

#include "core/hider.h"

#include "base-ui/supportframess.h"
#include "ui/newdialog.h"

class OneKeyHide : public SupportFrameless<QDialog>, public Hider::Delegate {
	Q_OBJECT

public:
	OneKeyHide(QWidget *parent = 0);
	~OneKeyHide();

	virtual void OnEnumStart() override;

	virtual void OnEnumFinish(const QHash<HWND, Window>&) override;
public slots:
	void on_pushButtonNewHide_clicked();

	void on_pushButtonNewSwitch_clicked();

	void on_pushButtonSettings_clicked();

	void on_pushButtonHelp_clicked();

	void on_pushButtonClose_clicked();

	void on_pushButtonMax_clicked();

	void on_pushButtonMin_clicked();
private:
	void UpdateList(const QHash<HWND, Window>&);

	bool nativeEvent(const QByteArray &eventType, void *message, long *result);
private:
	Ui::OneKeyHideClass ui;

	Hider* hider_;

	NewDialog new_dialog_;
};

#endif // ONEKEYHIDE_H
