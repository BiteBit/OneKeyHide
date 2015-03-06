#ifndef WINDOW_H
#define WINDOW_H

#include <QObject>

#include <Windows.h>

struct Window {
	HWND hwnd;
	bool visible;
	QString name;
	QString show_hide_hotkey;
	QString max_hotkey;
	QString min_hotkey;
};

#endif // WINDOW_H
