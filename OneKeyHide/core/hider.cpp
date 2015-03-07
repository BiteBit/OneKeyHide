#include "hider.h"

#include <QDebug>
#include <QTimer>

#include <QWidget>

WindowHash Hider::windows_info_;

QStringList blacklist = 
	QStringList() << "" << "Default IME" << "MSCTFIME UI" << "MS_WebcheckMonitor" << "DDE Server Window" << "MediaContextNotificationWindow"
	<< QString::fromLocal8Bit("电池指示器") << "Network Flyout" << "ms_sqlce_se_notify_wndproc" << "CiceroUIWndFrame"
	<< "TSVNCacheWindow" << "CiceroUIWndFrame" << QString::fromLocal8Bit("TaskEng - 任务计划程序引擎进程") << "VATreeParent" << "GDI+ Window"
	<< "SystemResourceNotifyWindow" << "Windows Push Notifications Platform" << "BluetoothNotificationAreaIconWindowClass" << "Task Host Window" << "DWM Notification Window"
	<< "Program Manager" << "VAStatMsgWnd" << QString::fromLocal8Bit("任务切换");

QStringList whitelist = QStringList() << "";

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
}

void Hider::SlotShortCutActivated() {
	auto sender = (QxtGlobalShortcut*)this->sender();
	auto it = actions_.find(sender);
	if (it != actions_.end()) {
		OneKeyShowHide(it->visible_list);
		OneKeySwitch(it->switch_list);
	}
}

bool CALLBACK Hider::MyEnumWindowsProc(HWND hwnd, LPARAM lParam) {
	if (IsWindow(hwnd)) {
		auto it = windows_info_.find(hwnd);
		if (it == windows_info_.end()) {
			Window window;
			TCHAR lpWinTitle[256];
			memset(lpWinTitle, 0, sizeof(lpWinTitle));
			::GetWindowText(hwnd, lpWinTitle, sizeof(lpWinTitle) - 1);
			window.name = QString::fromStdWString(lpWinTitle).trimmed();
			window.hwnd = hwnd;
			if (blacklist.contains(window.name))
				return true;
			
			qDebug() << "--------------------------------------------";
			qDebug() << hwnd << window.name;

			if (window.name.contains("Chrome")) {
				WId id = WId(hwnd);
				QWidget* widget = QWidget::find((WId)hwnd);
				if (widget)
					widget->hide();
			}

			windows_info_.insert(hwnd, window);
		} else {
		}
	}
	return true;
}

Window Hider::FindByHwnd(HWND hwnd) {
	auto it = windows_info_.find(hwnd);
	if (it != windows_info_.end())
		return it.value();
	
	return Window();
}

void Hider::AddRule(const QKeySequence& key_seq, const Rule& rule) {
	//rules_.insert(key_seq, rule);
	auto short_cut = new QxtGlobalShortcut(key_seq);
	connect(short_cut, SIGNAL(activated()), this, SLOT(SlotShortCutActivated()));
	actions_.insert(short_cut, rule);
	delegate_->OnNewRuleAdd(rule);
}

void Hider::OneKeyShowHide(WindowList& list) {
	for (auto& it : list) {
		ShowWindow(it.hwnd, it.visible ? SW_HIDE : SW_SHOW);
		delegate_->OnWindowVisibleChanged(it.hwnd, !it.visible);
		it.visible = !it.visible;
	}
}

void Hider::OneKeySwitch(WindowList& list) {
	for (auto& it : list) {
		ShowWindow(it.hwnd, SW_SHOW);
		it.visible = true;
	}
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

void Hider::SetVisible(HWND hwnd, bool visible) {
	auto it = windows_info_.find(hwnd);
	if (it != windows_info_.end())
		it->visible = visible;
}