#ifndef WINDOW_H
#define WINDOW_H

#include <QHash>
#include <QObject>
#include <QKeySequence>
#include <Windows.h>

struct Window {
	Window() :
		hwnd(NULL),
		visible(true),
		voice_follow_vision(false) {};
	HWND hwnd;
	bool visible;
	bool voice_follow_vision;
	QString name;
	QString show_hide_hotkey;
	QString max_hotkey;
	QString min_hotkey;
};
typedef QList<Window> WindowList;
typedef QHash<HWND, Window> WindowHash;

struct Rule {
	WindowList visible_list;
	WindowList switch_list;
	QKeySequence key_seq;
};
typedef QHash<QKeySequence, Rule> Rules;
#endif // WINDOW_H
