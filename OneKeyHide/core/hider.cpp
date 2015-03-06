#include "hider.h"

#include <QDebug>
#include <QTimer>

QHash<HWND, Window> Hider::windows_info_;

Hider::Hider(Delegate* delegate)
	: QObject(NULL),
	delegate_(delegate) {
	StartEnumWindows();
}

Hider::~Hider() {
}

void Hider::StartEnumWindows() {
	delegate_->OnEnumStart();
	::EnumWindows((WNDENUMPROC)MyEnumWindowsProc, 0);
	delegate_->OnEnumFinish(windows_info_);

	//QTimer::singleShot(1000, this, SLOT(StartEnumWindows()));
}

bool CALLBACK Hider::MyEnumWindowsProc(HWND hwnd, LPARAM lParam) {
	//if (IsWindow(hwnd)) {
		auto it = windows_info_.find(hwnd);
		if (it == windows_info_.end()) {
			Window window;
			TCHAR lpWinTitle[256];
			memset(lpWinTitle, 0, sizeof(lpWinTitle));
			::GetWindowText(hwnd, lpWinTitle, sizeof(lpWinTitle) - 1);
			window.name = QString::fromStdWString(lpWinTitle).trimmed();
			if (window.name.isEmpty())
				return true;
			//if (window.name.contains("Chrome"))
			//	ShowWindow(hwnd, SW_SHOW);
			//	//ShowWindow(hwnd, SW_HIDE);

			qDebug() << window.name;
			windows_info_.insert(hwnd, window);
		} else {
		}
	//}
	return true;
}

void Hider::OneKeyShowHide() {

}

void Hider::SetShowHideHotkey(HWND hwnd, const QString& key) {
	auto it = windows_info_.find(hwnd);
	if (it != windows_info_.end()) {
		it->show_hide_hotkey = key;
	}
}

void Hider::SetMaxHotkey(HWND hwnd, const QString& key) {
	auto it = windows_info_.find(hwnd);
	if (it != windows_info_.end()) {
		it->show_hide_hotkey = key;
	}
}

void Hider::SetMinHotkey(HWND hwnd, const QString& key) {
	auto it = windows_info_.find(hwnd);
	if (it != windows_info_.end()) {
		it->show_hide_hotkey = key;
	}
}