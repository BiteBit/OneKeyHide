#include "settingswidget.h"

Settings* gSettings = Settings::Instance();

SettingsWidget::SettingsWidget(OneKeyHide *parent)
	: one_key_(parent) {
	ui.setupUi(this);
	boss_shortcut_ = new QxtGlobalShortcut();
	onekey_shortcut_ = new QxtGlobalShortcut();
	connect(boss_shortcut_, SIGNAL(activated()), one_key_, SLOT(SlotVisibleActivated()));
	connect(onekey_shortcut_, SIGNAL(activated()), one_key_, SLOT(SlotOnekeyActivated()));

	gSettings->SetDelegate(this);
	gSettings->LoadFromFile("./config.cfg");
}

SettingsWidget::~SettingsWidget() {
}

void SettingsWidget::OnLoadFinish(const QJsonObject& obj) {
	auto boss = QKeySequence(obj["key_boss"].toString());
	auto one_key = QKeySequence(obj["key_one_key_hide"].toString());

	ui.keySequenceEditBoss->setKeySequence(boss);
	ui.keySequenceEditOneKey->setKeySequence(one_key);
	boss_shortcut_->setShortcut(boss);
	onekey_shortcut_->setShortcut(one_key);
}

void SettingsWidget::OnSaveFinish(const QJsonObject& doc) {

}

void SettingsWidget::on_pushButtonSettingsReset_clicked() {
	gSettings->Reset();
	gSettings->SaveToFile("./config.cfg");
}

void SettingsWidget::on_pushButtonSettingsSave_clicked() {
	boss_shortcut_->setShortcut(ui.keySequenceEditBoss->keySequence());
	onekey_shortcut_->setShortcut(ui.keySequenceEditOneKey->keySequence());

	gSettings->Set("key_boss", ui.keySequenceEditBoss->keySequence().toString());
	gSettings->Set("key_one_key_hide", ui.keySequenceEditOneKey->keySequence().toString());
	gSettings->SaveToFile("./config.cfg");
}
