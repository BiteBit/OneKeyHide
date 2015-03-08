#include "volume.h"
#include <QDebug>
#include <Endpointvolume.h>

#pragma comment(lib, "Winmm.lib")

template <class T>
inline void SafeRelease(T &p)
{
	if (p != NULL)
	{
		p->Release();
		p = NULL;
	}
}

//void Mixer() {
//	UINT count = mixerGetNumDevs();
//	for (int i = 0; i < count; ++i) {
//		MIXERCAPS mixer_caps;
//		mixerGetDevCaps(i, &mixer_caps, sizeof(mixer_caps));
//		int xx = 0;
//	}
//
//	HMIXER mix;
//	mixerOpen(&mix, 0, 0, 0, 0);
//	HMIXEROBJ hmxobj;
//	MIXERCONTROLDETAILS_LISTTEXT detail;
//	mixerGetControlDetails(hmxobj, (MIXERCONTROLDETAILS*)(&detail), MIXER_GETCONTROLDETAILSF_LISTTEXT);
//	QString str = QString::fromStdWString(detail.szName);
//}

Volumer::Volumer()
{
	m_pEnumerator = NULL;
	m_bMute = FALSE;
	Init();
}

Volumer::~Volumer()
{
	Uninit();
}

HRESULT Volumer::Init()
{
	HRESULT hr = E_FAIL;
	CoInitialize( NULL  );
	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL,
		CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&m_pEnumerator);
	if (FAILED(hr))
		return hr;

	IMMDeviceCollection* collections = NULL;
	m_pEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &collections);
	
	UINT deviceCount = 0;
	hr = collections->GetCount(&deviceCount);

	for (UINT DeviceIndex = 0; DeviceIndex < deviceCount; DeviceIndex++) {
		IMMDevice* device = NULL;
		collections->Item(DeviceIndex, &device);
		IAudioEndpointVolume* volumer = NULL;
		device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&volumer);
		//volumer->SetMute(true, &__uuidof(IAudioEndpointVolume));
	}

	return hr;
}

HRESULT Volumer::Uninit()
{
	SafeRelease(m_pEnumerator);
	return S_OK;
}

HRESULT Volumer::SetVolume(float fVolume)
{
	HRESULT hr = E_FAIL;

	return hr;
}

HRESULT Volumer::GetVolume(float *pfVolume)
{
	HRESULT hr = E_FAIL;

	return hr;
}

HRESULT Volumer::SetMuteEx(BOOL bMute)
{
	if (m_bMute == bMute)
		return S_OK;

	HRESULT hr = E_FAIL;

	IMMDevice* pDevice;
	hr = m_pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
	if (FAILED(hr))
		return hr;

	IAudioSessionManager2* pasm = NULL;
	hr = pDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, NULL, (void**)&pasm);
	if (FAILED(hr))
		return hr;

	IAudioSessionEnumerator* audio_session_enumerator;
	if(SUCCEEDED(pasm->GetSessionEnumerator(&audio_session_enumerator)))
	{
		int count;
		
		if(SUCCEEDED(audio_session_enumerator->GetCount(&count)))
		{
			for (int i = 0; i < count; i++)  
			{
				IAudioSessionControl* audio_session_control;
				IAudioSessionControl2* audio_session_control2;

				if(SUCCEEDED(audio_session_enumerator->GetSession(i, &audio_session_control)))
				{
					if(SUCCEEDED(audio_session_control->QueryInterface(__uuidof(IAudioSessionControl2), (void**)&audio_session_control2)))
					{
						DWORD processid;
						
						if(SUCCEEDED(audio_session_control2->GetProcessId(&processid)))
						{
							if (processid == GetCurrentProcessId())
							{
								ISimpleAudioVolume* pSAV;
								hr = audio_session_control2->QueryInterface(__uuidof(ISimpleAudioVolume), (void**) &pSAV);
								if (SUCCEEDED(hr))
								{
									hr = pSAV->SetMute(bMute, NULL);
									if (SUCCEEDED(hr))
									{
										m_bMute = bMute;
									}
									pSAV->Release();
								}
							}
							audio_session_control->Release();
							audio_session_control2->Release();
						}
					}
				}
			}
			audio_session_enumerator->Release();
		}
	} 

	pasm->Release();

	return hr;
}

BOOL Volumer::GetMuteEx()
{
	return m_bMute;
}