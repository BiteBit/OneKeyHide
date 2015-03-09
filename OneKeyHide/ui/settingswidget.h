#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>
#include "ui_settingswidget.h"

#include "ui/onekeyhide.h"
#include "core/settings.h"
#include "3rd/qxtglobalshortcut/qxtglobalshortcut.h"

class SettingsWidget : public QWidget, public Settings::Delegate {
	Q_OBJECT

public:
	SettingsWidget(OneKeyHide *parent);
	~SettingsWidget();

	virtual void OnLoadFinish(const QJsonObject& doc) override;

	virtual void OnSaveFinish(const QJsonObject&) override;

	void Save();

private slots:
	void on_pushButtonSettingsReset_clicked();

	void on_pushButtonSettingsSave_clicked();

private:
	Ui::SettingsWidget ui;

	OneKeyHide* one_key_;
	QxtGlobalShortcut* boss_shortcut_;
	QxtGlobalShortcut* onekey_shortcut_;
};

#endif // SETTINGSWIDGET_H
