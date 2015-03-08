#ifndef WINDOW_H
#define WINDOW_H

#include <QHash>
#include <QObject>
#include <QKeySequence>
#include <Windows.h>

struct Window {
	Window() :
		hwnd(NULL),
		process_id(0),
		visible(true),
		voice_follow_vision(false),
		setted(false) {};
	HWND hwnd;
	HANDLE process_handle;
	int process_id;
	bool visible;
	bool voice_follow_vision;
	bool setted;
	QString title;
	QString show_hide_hotkey;
	QString exe_path;
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
