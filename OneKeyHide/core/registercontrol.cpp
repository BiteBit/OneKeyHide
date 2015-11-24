#include "registercontrol.h"

#include <QMessageBox>

//#define ENABLE_VPROTECT

#ifdef ENABLE_VPROTECT
#pragma comment(lib, "VP_License_Sdk")
#endif

RegisterControl* RegisterControl::instance_ = NULL;

RegisterControl::RegisterControl(QObject *parent)
	: QObject(parent) {

#ifdef ENABLE_VPROTECT
	TCHAR id[256] = { 0 };
	DWORD len = VP_Sdk_GetHardWareId(VP_SDK_INDEX_GETHARDWAREIDW, id);

	TCHAR username[256] = { 0 };
	len = VP_Sdk_GetUserName(VP_SDK_INDEX_GETUSERNAMEW, username);;

	TCHAR key[1024] = { 0 };
	len = VP_Sdk_SaveKey(VP_SDK_INDEX_SAVEKEYW, key);

	left_count_ = VP_Sdk_GetLeftCount();
	used_count_ = VP_Sdk_GetUsedCount();

	auto reg_left_day = VP_Sdk_GetRegLeftDay();
	auto trial_left_day = VP_Sdk_GetTrialLeftDay();

	auto state = VP_Sdk_GetLicenseState();
	auto is_register = VP_Sdk_IsRegister();

	//BOOL VP_Sdk_CheckLicenseInMem(DWORD Sdk_Index, BYTE *PMemKey, DWORD KeySize);
	auto valid_day = VP_Sdk_GetVaildDay();

	auto str = QString("%1-%2-%3-%4-%5-%6-%7-%8").arg(QString::fromStdWString(id)).arg(QString::fromStdWString(username)).
		arg(QString::fromStdWString(key)).arg(left_count_).arg(used_count_).arg(reg_left_day).arg(trial_left_day).arg(state).arg(is_register).arg(valid_day);
	QMessageBox::information(NULL, "", str);
#endif
}

RegisterControl::~RegisterControl() {
}

RegisterControl* RegisterControl::Instance() {
	if (!instance_)
		instance_ = new RegisterControl(NULL);
	return instance_;
}

bool RegisterControl::IsRegister() {
#ifdef ENABLE_VPROTECT
	return VP_Sdk_IsRegister() == LICENSE_STATE_REGMODE;
#else
	return true;
#endif
}