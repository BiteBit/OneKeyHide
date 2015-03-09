#ifndef SETTINGS_H
#define SETTINGS_H

#include <QJsonObject>

class Settings {
public:
	Settings();
	~Settings();

	class Delegate {
	public:
		virtual void OnSaveFinish(const QJsonObject&) = 0;
		virtual void OnLoadFinish(const QJsonObject&) = 0;
	};

	static Settings* Instance();

	void SetDelegate(Delegate* delegate) { delegate_ = delegate; }

	void Reset();
	bool LoadFromFile(const QString& file);
	bool SaveToFile(const QString& file);

	void Set(const QString& key, const QString& value);
	void Set(const QString& key, int value);
	void Set(const QString& key, QJsonValue& value);
private:

private:
	static Settings* instance_;

	Delegate* delegate_;

	QJsonObject settings_obj_;
};

#endif // SETTINGS_H
