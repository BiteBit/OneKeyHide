#include "hider.h"

#include <QDebug>
#include <QTimer>
#include <QWidget>

#include "psapi.h"

#include "util/util.h"

WindowHash Hider::windows_info_;

QStringList blacklist = 
	QStringList() << "" << "Default IME" << "MSCTFIME UI" << "MS_WebcheckMonitor" << "DDE Server Window" << "MediaContextNotificationWindow"
	<< G2U("电池指示器") << "Network Flyout" << "ms_sqlce_se_notify_wndproc" << "CiceroUIWndFrame"
	<< "TSVNCacheWindow" << "CiceroUIWndFrame" << G2U("TaskEng - 任务计划程序引擎进程") << "VATreeParent" << "GDI+ Window"
	<< "SystemResourceNotifyWindow" << "Windows Push Notifications Platform" << "BluetoothNotificationAreaIconWindowClass" << "Task Host Window" << "DWM Notification Window"
	<< "Program Manager" << "VAStatMsgWnd" << G2U("任务切换") << "OneKeyHide";

QStringList whitelist = QStringList() << "";

Hider::Hider(Delegate* delegate)
	: QObject(NULL),
	delegate_(delegate) {
	volumer_.SetVolume(0);
	volumer_.SetMuteEx(true);
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

bool EnablePrivilege(HANDLE hToken, TCHAR* szPrivName)
{
	TOKEN_PRIVILEGES tkp;

	//修改进程权限
	LookupPrivilegeValue(NULL, szPrivName, &tkp.Privileges[0].Luid);
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	//通知系统修改进程权限  
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
			if (/*!window.exe_path.isEmpty() && */window.process_handle && !window.title.isEmpty() && window.process_id)
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

BOOL MuteProcessVolume()
{
	//1..屏蔽进程mp3等声音 
	HMODULE	hDsound = LoadLibraryA("DSound.dll");
	if (!hDsound)
		return FALSE;

	LPVOID lpDirectSoundCreate = GetProcAddress(hDsound, "DirectSoundCreate");
	if (!lpDirectSoundCreate)
		return FALSE;

	DWORD NewProtect = PAGE_EXECUTE_READWRITE;
	DWORD OldProtect = 0;
	if (!VirtualProtect(lpDirectSoundCreate, 1, NewProtect, &OldProtect)) {
		FreeLibrary(hDsound);
		return FALSE;
	}

	//拷贝内存
	u_char szBuf[] = { 0xC2, 0x0C, 0x00 };
	MoveMemory(lpDirectSoundCreate, szBuf, 3);

	//恢复  
	if (!VirtualProtect(lpDirectSoundCreate, 1, OldProtect, &NewProtect)) {
		FreeLibrary(hDsound);
		return FALSE;
	}

	//释放DSound.dll 
	FreeLibrary(hDsound);


	//2..屏蔽进程Flash等声音  
	HMODULE hWinmm = LoadLibraryA("winmm.dll");
	if (!hWinmm)
		return FALSE;

	//midiStreamOpen:为输出,打开一个MIDI流
	LPVOID lpmidiStreamOpen = GetProcAddress(hWinmm, "midiStreamOpen");
	if (!lpmidiStreamOpen)
		return FALSE;

	NewProtect = PAGE_EXECUTE_READWRITE;
	if (!VirtualProtect(lpmidiStreamOpen, 1, NewProtect, &OldProtect))
		FreeLibrary(hWinmm);
		return FALSE;

	//拷贝内存
	u_char szBuf2[] = { 0xC2, 0x04, 0x00 };
	MoveMemory(lpmidiStreamOpen, szBuf2, 3);

	//恢复  
	if (!VirtualProtect(lpmidiStreamOpen, 1, OldProtect, &NewProtect)) {
		FreeLibrary(hWinmm);
		return FALSE;
	}

	//waveOutWrite:向指定的波形输出设备发送一个数据块
	LPVOID lpwaveOutWrite = GetProcAddress(hWinmm, "waveOutWrite");
	if (!lpwaveOutWrite)
		return FALSE;

	NewProtect = PAGE_EXECUTE_READWRITE;
	if (!VirtualProtect(lpwaveOutWrite, 1, NewProtect, &OldProtect)) {
		FreeLibrary(hWinmm);
		return FALSE;
	}

	//拷贝内存
	u_char szBuf3[] = { 0xC2, 0x0C, 0x00 };
	MoveMemory(lpwaveOutWrite, szBuf3, 3);

	//恢复  
	if (!VirtualProtect(lpwaveOutWrite, 1, OldProtect, &NewProtect)) {
		FreeLibrary(hWinmm);
		return FALSE;
	}

	//释放winmm.dll 
	FreeLibrary(hWinmm);

	return TRUE;
}
