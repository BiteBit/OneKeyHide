#ifndef HIDER_H
#define HIDER_H

#include "windowinfo.h"

#include <QObject>
#include <QHash>

#include <Windows.h>

class Hider : public QObject {
	Q_OBJECT

public:
	class Delegate {
	public:
		virtual void OnEnumStart() {}
		virtual void OnEnumFinish(const QHash<HWND, Window>&) {}
	};

	Hider(Delegate* delegate);
	~Hider();

	static bool CALLBACK MyEnumWindowsProc(HWND hwnd, LPARAM lParam);

	void OneKeyShowHide();

	void SetShowHideHotkey(HWND, const QString&);

	void SetMaxHotkey(HWND, const QString&);

	void SetMinHotkey(HWND, const QString&);
private slots:
	void StartEnumWindows();

private:
	static QHash<HWND, Window> windows_info_;

	Delegate* delegate_;
};

#endif // HIDER_H
