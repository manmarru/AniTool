#include "../Public/SoundManager.h"

CSoundManager::CSoundManager() : m_pSystem(nullptr)
{

}

CSoundManager::~CSoundManager()
{
}
	
CSoundManager* CSoundManager::Create()
{
	CSoundManager* pSoundManager = new CSoundManager();
	pSoundManager->Initialize();
	return pSoundManager;
}

void CSoundManager::Initialize()
{
	{
		System_Create(&m_pSystem);

		m_pSystem->init(70, FMOD_INIT_NORMAL, NULL);
	}
	FMOD::ChannelGroup* masterChannelGroup = nullptr;
	for (int i = 30; i < 70; ++i)
	{
		m_availableChannels.push_back(i);
	}
}

void CSoundManager::Update()
{
	m_pSystem->update();
	CheckChannels();
}

void CSoundManager::Free()
{
	__super::Free();

	// m_mapSound�� ��� ���带 ����
	for (auto iter : m_mapSound)
	{
		iter.second->release();  // FMOD::Sound ��ü ����
		delete[] iter.first;     // TCHAR* �޸� ����
	}
	m_mapSound.clear();

	// FMOD �ý��� ��ü ���� (�ߺ� ȣ�� ����)
	if (m_pSystem)
	{
		m_pSystem->release();
		m_pSystem = nullptr;
	}
}

HRESULT CSoundManager::PlayDefault(TCHAR* _TagSound, SOUND_CHANNEL _Channel, _float _fVolume, _bool _bLoop)
{
	auto iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)->bool
		{
			return !lstrcmp(_TagSound, iter.first);
		});
	if (iter == m_mapSound.end())
		return E_FAIL;

	m_pSystem->playSound(iter->second, 0, FALSE, &m_pChannel[_Channel]);// ä�α׷� 0���� �ص� �ȴ�.(�ɼ���)
	_bLoop ? m_pChannel[_Channel]->setMode(FMOD_DEFAULT) : m_pChannel[_Channel]->setMode(FMOD_DEFAULT | FMOD_LOOP_OFF);
	m_pChannel[_Channel]->setVolume(_fVolume);

	return S_OK;
}

HRESULT CSoundManager::PlayBGM(const TCHAR* _TagSound, _float _fVolume, _bool _bLoop)
{
#pragma region ���� ���� ã��
	auto iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)->bool
		{
			return !lstrcmp(_TagSound, iter.first);
		});
	if (iter == m_mapSound.end())
		return E_FAIL;
#pragma endregion

	m_pSystem->playSound(iter->second, 0, FALSE, &m_pChannel[CHANNEL_BGM]);// ä�α׷� 0���� �ص� �ȴ�.(�ɼ���)
	_bLoop ? m_pChannel[CHANNEL_BGM]->setMode(FMOD_DEFAULT | FMOD_LOOP_NORMAL) : m_pChannel[CHANNEL_BGM]->setMode(FMOD_DEFAULT | FMOD_LOOP_OFF);
	
	m_pChannel[CHANNEL_BGM]->setVolume(_fVolume);
	return S_OK;
}

HRESULT CSoundManager::Play3D(TCHAR* _TagSound, _int _Channel, _float _fVolume, _float3 _vPos, _bool _bLoop)
{
	auto iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)->bool
		{
			return !lstrcmp(_TagSound, iter.first);
		});
	if (iter == m_mapSound.end())
		return E_FAIL;

	m_pSystem->playSound(iter->second, 0, FALSE, &m_pChannel[_Channel]);// ä�α׷� 0���� �ص� �ȴ�.(�ɼ���)
	_bLoop ? m_pChannel[_Channel]->setMode(FMOD_3D | FMOD_LOOP_NORMAL) : m_pChannel[_Channel]->setMode(FMOD_3D | FMOD_LOOP_OFF);
	m_pChannel[_Channel]->setVolume(_fVolume);

	FMOD_VECTOR vSoundPos = { _vPos.x, _vPos.y, _vPos.z };
	m_pChannel[_Channel]->set3DAttributes(&vSoundPos, 0);

	return S_OK;
}

HRESULT CSoundManager::Play3DRemain(TCHAR* _TagSound, _float _fVolume, _float3 _vPos, _bool _bLoop)
{
	// ���� �±׸� ã�� �ش� ���带 �����ɴϴ�.
	auto iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)->bool
		{
			return !lstrcmp(_TagSound, iter.first);
		});
	if (iter == m_mapSound.end())
		return E_FAIL;

	// ��� ������ ä���� �ִ��� Ȯ���մϴ�.
	if (!m_availableChannels.empty())
	{
		int channelIndex = m_availableChannels.back(); // ��� ������ ������ ä���� ������
		m_availableChannels.pop_back(); // Ǯ���� ����

		FMOD::Channel* pChannel = m_pChannel[channelIndex];

		// ���带 ����մϴ�.
		m_pSystem->playSound(iter->second, 0, FALSE, &pChannel);
		_bLoop ? pChannel->setMode(FMOD_3D | FMOD_LOOP_NORMAL) : pChannel->setMode(FMOD_3D | FMOD_LOOP_OFF);
		pChannel->setVolume(_fVolume);

		FMOD_VECTOR vSoundPos = { _vPos.x, _vPos.y, _vPos.z };
		pChannel->set3DAttributes(&vSoundPos, 0);

		return S_OK; // ���������� ���
	}

	return E_FAIL; // ��� ������ ä���� ����
}

HRESULT CSoundManager::Play_NonStack(TCHAR* _TagSound, int _Channel, _float _fVolume, _bool _bLoop)
{
	bool bIsPlaying;
	m_pChannel[_Channel]->isPlaying(&bIsPlaying);
	if (bIsPlaying)
		return S_OK;

#pragma region ���� ���� ã��
	auto iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)->bool
		{
			return !lstrcmp(_TagSound, iter.first);
		});
	if (iter == m_mapSound.end())
		return E_FAIL;
#pragma endregion

	m_pSystem->playSound(iter->second, 0, FALSE, &m_pChannel[_Channel]); //ä�α׷� 0���� �ص� ��(�ɼǰ���)
	_bLoop ? m_pChannel[_Channel]->setMode(FMOD_DEFAULT | FMOD_LOOP_NORMAL) : m_pChannel[_Channel]->setMode(FMOD_DEFAULT);

	m_pChannel[_Channel]->setVolume(_fVolume);
	return S_OK;
}

void CSoundManager::StopSound(SOUND_CHANNEL _Channel)
{
	m_pChannel[_Channel]->stop();
}

void CSoundManager::MuteAll()
{
	for (int i = 0; i < CHANNEL_END; ++i)
	{
		SetChannelVolume((SOUND_CHANNEL)i, 0.f);
	}
}

void CSoundManager::StopSoundALL()
{
	for (int i = 0; i < CHANNEL_END; ++i)
	{
		StopSound((SOUND_CHANNEL)i);
	}
}

void CSoundManager::SetChannelVolume(SOUND_CHANNEL _Channel, _float _fVolume)
{
	m_pChannel[_Channel]->setVolume(_fVolume);

	m_pSystem->update();
}

HRESULT CSoundManager::Set_Listener(_float3 _vListenerPos, _float3 _v_vListenerForward)
{
	FMOD_VECTOR ListenerPos = { _vListenerPos.x, _vListenerPos.y, _vListenerPos.z };
	FMOD_VECTOR ListenerForward = { _v_vListenerForward.x, _v_vListenerForward.y, _v_vListenerForward.z };
	FMOD_VECTOR ListenerUp = { 0.f, 1.f, 0.f };
	FMOD_RESULT result = m_pSystem->set3DListenerAttributes(0, &ListenerPos, 0, &ListenerForward, &ListenerUp);

	return FMOD_OK == result ? S_OK : E_FAIL;
}

HRESULT CSoundManager::Set_SoundPos(_int _iChannel, _float3 _vPos)
{
	FMOD_VECTOR fmodPos = { _vPos.x, _vPos.y, _vPos.z };
	FMOD_VECTOR fmodnull = { 0.f, 0.f, 0.f };
	FMOD_RESULT result = m_pChannel[_iChannel]->set3DAttributes(&fmodPos, &fmodnull);
	
	return FMOD_OK == result ? S_OK : E_FAIL;
}

void CSoundManager::CheckChannels()
{
	for (int i = 30; i < 70; ++i)
	{
		bool isPlaying;
		m_pChannel[i]->isPlaying(&isPlaying);
		if (!isPlaying && std::find(m_availableChannels.begin(), m_availableChannels.end(), i) == m_availableChannels.end())
		{
			m_availableChannels.push_back(i); // Ǯ�� �ٽ� ä�� �߰�
		}
	}
}

HRESULT CSoundManager::Add_Sound(const char* _fullpath, const TCHAR* _filename, const TCHAR* _path)
{
	auto iter = m_mapSound.find(_filename);
	if (m_mapSound.end() != iter)
		return E_FAIL;

	_finddata_t fd;
	long handle = _findfirst(_fullpath, &fd);
	if (handle == -1)
		return E_FAIL;

	//���� �߰�

	FMOD::Sound* pSound;
	m_pSystem->createSound(_fullpath, FMOD_CREATESAMPLE, 0, &pSound);

	m_mapSound.emplace(_filename, pSound);

	return S_OK;
}

HRESULT CSoundManager::Load_Sound(const char* _folderName)
{
	_finddata_t fd;
	char path[MAX_PATH] = "..\\Bin\\Resources\\Sound\\";
	strcat_s(path, MAX_PATH, _folderName);
	strcat_s(path, MAX_PATH, "\\*.ogg");

	intptr_t handle = _findfirst(path, &fd);
	if (handle == -1)
		return E_FAIL;


	int iResult = 0;
	char szCurPath[MAX_PATH] = "..\\Bin\\Resources\\Sound\\";
 	char szFullPath[MAX_PATH] = "";

	while (iResult != -1)
	{
		strcpy_s(szFullPath, szCurPath);

		// sound ������ �ִ� ī�װ� ��������
		strcat_s(szFullPath, _folderName);
		strcat_s(szFullPath, MAX_PATH, "\\");
		strcat_s(szFullPath, fd.name);
		// "../Sound/" + "judgingLoop.wav"
		// ��ȸ�� �ش��ϴ� ������ �̸�����

		FMOD::Sound* pSound = nullptr;

		FMOD_RESULT eRes = m_pSystem->createSound(szFullPath, FMOD_DEFAULT, 0, &pSound);

		if (eRes == FMOD_OK)
		{
			int iLength = (_int)strlen(fd.name) + 1;

			TCHAR* pSoundKey = new TCHAR[iLength];
			ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);
			// �ƽ�Ű �ڵ� ���ڿ��� �����ڵ� ���ڿ��� ��ȯ
			MultiByteToWideChar(CP_ACP, 0, fd.name, iLength, pSoundKey, iLength);

			m_mapSound.emplace(pSoundKey, pSound);
		}
		//���̻� ���ٸ� -1�� ����
		iResult = _findnext(handle, &fd);
	}
	m_pSystem->update();

	_findclose(handle);

	return S_OK;
}

FMOD::Sound* CSoundManager::Find_Sound(TCHAR* _TagSound)
{
	auto iter = m_mapSound.find(_TagSound);

	if (m_mapSound.end() == iter)
	{
		return nullptr;
	}

	return (*iter).second;
}
