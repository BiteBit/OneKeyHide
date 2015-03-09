#include "hider.h"

#include <QDebug>
#include <QTimer>
#include <QWidget>

#include "psapi.h"

#include "util/util.h"

WindowHash Hider::windows_info_;

QStringList blacklist = 
	QStringList() << "" << "Default IME" << "MSCTFIME UI" << "MS_WebcheckMonitor" << "DDE Server Window" << "MediaContextNotificationWindow"
	<< G2U("���ָʾ��") << "Network Flyout" << "ms_sqlce_se_notify_wndproc" << "CiceroUIWndFrame"
	<< "TSVNCacheWindow" << "CiceroUIWndFrame" << G2U("TaskEng - ����ƻ������������") << "VATreeParent" << "GDI+ Window"
	<< "SystemResourceNotifyWindow" << "Windows Push Notifications Platform" << "BluetoothNotificationAreaIconWindowClass" << "Task Host Window" << "DWM Notification Window"
	<< "Program Manager" << "VAStatMsgWnd" << G2U("�����л�") << "OneKeyHide";

QStringList whitelist = QStringList() << "";

Hider::Hider(Delegate* delegate)
	: QObject(NULL),
	enable_enum_window_(false),
	delegate_(delegate) {
	volumer_.SetVolume(0);
	volumer_.SetMuteEx(true);
}

Hider::~Hider() {
}

void Hider::StartEnumWindows() {
	delegate_->OnEnumStart();
	windows_info_.clear();
	::EnumWindows((WNDENUMPROC)MyEnumWindowsProc, 0);
	delegate_->OnEnumFinish(windows_info_);
	if (enable_enum_window_)
		QTimer::singleShot(1000, this, SLOT(StartEnumWindows()));
}

void Hider::SlotShortCutActivated() {
	auto sender = (QxtGlobalShortcut*)this->sender();
	auto it = actions_.find(sender);
	if (it != actions_.end()) {
		OneKeyShowHide(it->visible_list);
		OneKeySwitch(it->switch_list);
	}
}

bool EnablePrivilege(HANDLE hToken, TCHAR* szPrivName)
{
	TOKEN_PRIVILEGES tkp;

	//�޸Ľ���Ȩ��
	LookupPrivilegeValue(NULL, szPrivName, &tkp.Privileges[0].Luid);
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	//֪ͨϵͳ�޸Ľ���Ȩ��  
	AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof tkp, NULL, NULL);

	return((GetLastError() == ERROR_SUCCESS));
}

bool CALLBACK Hider::MyEnumWindowsProc(HWND hwnd, LPARAM lParam) {
	if (IsWindow(hwnd) && IsWindowVisible(hwnd)) {
		auto it = windows_info_.find(hwnd);
		if (it == windows_info_.end()) {
			Window window;
			TCHAR lpWinTitle[256];
			memset(lpWinTitle, 0, sizeof(lpWinTitle));
			::GetWindowText(hwnd, lpWinTitle, sizeof(lpWinTitle) - 1);
			window.title = QString::fromStdWString(lpWinTitle).trimmed();
			window.hwnd = hwnd;
			if (blacklist.contains(window.title))
				return true;

			HANDLE hToken;
			OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken);
			EnablePrivilege(hToken, SE_DEBUG_NAME);

			DWORD process_id = 0;
			::GetWindowThreadProcessId(hwnd, &process_id);
			HANDLE handle = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, process_id);
			window.process_handle = handle;
			window.process_id = process_id;

			TCHAR exe_file_path[MAX_PATH + 1];
			memset(exe_file_path, 0, sizeof(exe_file_path));
			GetModuleFileNameEx(handle, NULL, exe_file_path, sizeof(exe_file_path));
			window.exe_path = QString::fromStdWString(exe_file_path);

			//qDebug() << hwnd << window.title << window.exe_path;
			if (window.process_handle && window.process_id)
				windows_info_.insert(hwnd, window);	
		} else {
			TCHAR lpWinTitle[256];
			memset(lpWinTitle, 0, sizeof(lpWinTitle));
			::GetWindowText(hwnd, lpWinTitle, sizeof(lpWinTitle) - 1);
			it->title = QString::fromStdWString(lpWinTitle).trimmed();
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
	auto short_cut = new QxtGlobalShortcut(key_seq);
	connect(short_cut, SIGNAL(activated()), this, SLOT(SlotShortCutActivated()));
	actions_.insert(short_cut, rule);
	delegate_->OnNewRuleAdd(rule);
	for (auto& it : rule.visible_list) {
		auto iter = windows_info_.find(it.hwnd);
		if (iter != windows_info_.end())
			iter->setted = true;
	}
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
		ShowWindow(it.hwnd, SW_NORMAL);
		BringWindowToTop(it.hwnd);
		it.visible = true;
	}
}

void Hider::SetVisible(HWND hwnd, bool visible) {
	auto it = windows_info_.find(hwnd);
	if (it != windows_info_.end())
		it->visible = visible;
}
