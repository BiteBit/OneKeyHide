#include "settings.h"

#include <QFile>
#include <QJsonDocument>

bool ReadFile(const QString& path, QByteArray& content) {
	QFile file(path);
	if (!file.open(QIODevice::ReadOnly))
		return false;

	content = file.readAll();
	file.close();
	return true;
}

bool SaveFile(const QString& path, const QByteArray& content) {
	QFile file(path);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
		return false;

	file.write(content);
	file.close();
	return true;
}

Settings* Settings::instance_ = NULL;

Settings::Settings() :
	delegate_(0) {
	Reset();
}

Settings::~Settings() {
}

Settings* Settings::Instance() {
	if (!instance_)
		instance_ = new Settings;
	return instance_;
}

void Settings::Reset() {
	settings_obj_["key_boss"] = "Ctrl+Shift+Home";
	settings_obj_["key_one_key_hide"] = "Ctrl+Shift+End";
}

bool Settings::LoadFromFile(const QString& file) {
	do 
	{
		QByteArray content;
		if (!ReadFile(file, content))
			break;

		settings_obj_ = QJsonDocument::fromJson(content).object();

		if (delegate_)
			delegate_->OnLoadFinish(settings_obj_);
		return true;

	} while (false);

	if (delegate_)
		delegate_->OnLoadFinish(settings_obj_);
	return false;
}

bool Settings::SaveToFile(const QString& file) {
	do 
	{
		if (!SaveFile(file, QJsonDocument(settings_obj_).toJson(QJsonDocument::Indented)))
			break;

		if (delegate_)
			delegate_->OnSaveFinish(settings_obj_);

		return true;
	} while (false);

	if (delegate_)
		delegate_->OnSaveFinish(settings_obj_);
	return false;
}

void Settings::Set(const QString& key, const QString& value) {
	settings_obj_[key] = value;
}

void Settings::Set(const QString& key, int value) {
	settings_obj_[key] = value;
}

void Settings::Set(const QString& key, QJsonValue& value) {
	settings_obj_[key] = value;
}