#include "..\Public\Input_Device.h"

Engine::CInput_Device::CInput_Device(void)
{

}

HRESULT Engine::CInput_Device::Initialize(HINSTANCE hInst, HWND hWnd)
{
	// DInput �İ�ü�� �����ϴ� �Լ�
	if (FAILED(DirectInput8Create(hInst,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&m_pInputSDK,
		nullptr)))
		return E_FAIL;

	// Ű���� ��ü ����
	if (FAILED((m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr))))
		return E_FAIL;

	// ������ Ű���� ��ü�� ���� ������ �� ��ü���� �����ϴ� �Լ�
	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);

	// ��ġ�� ���� �������� �������ִ� �Լ�, (Ŭ���̾�Ʈ�� ���ִ� ���¿��� Ű �Է��� ������ ������ �����ϴ� �Լ�)
	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// ��ġ�� ���� access ������ �޾ƿ��� �Լ�
	m_pKeyBoard->Acquire();


	// ���콺 ��ü ����
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;

	// ������ ���콺 ��ü�� ���� ������ �� ��ü���� �����ϴ� �Լ�
	m_pMouse->SetDataFormat(&c_dfDIMouse);

	// ��ġ�� ���� �������� �������ִ� �Լ�, Ŭ���̾�Ʈ�� ���ִ� ���¿��� Ű �Է��� ������ ������ �����ϴ� �Լ�
	m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// ��ġ�� ���� access ������ �޾ƿ��� �Լ�
	m_pMouse->Acquire();


	return S_OK;
}

void Engine::CInput_Device::Update(void)
{
	/* Ű����� ���콺�� ���¸� ���ͼ� �������ش�. */
	memcpy(m_byPrevKeyState, m_byKeyState, sizeof(m_byKeyState));
	m_pKeyBoard->GetDeviceState(256, m_byKeyState);

	// -Njh ������Ʈ ���� �������¿� ���� ���¸� ����ȭ �Ѵ�
	m_tPrevMouseState = m_tMouseState;
	m_pMouse->GetDeviceState(sizeof(m_tMouseState), &m_tMouseState);
}

_bool CInput_Device::Get_DIKeyState_Once(_ubyte byKeyID)
{
	return (m_byKeyState[byKeyID] & 0x80) && !(m_byPrevKeyState[byKeyID] & 0x80);
}

_bool CInput_Device::Get_DIKeyState_Once_Up(_ubyte byKeyID)
{
	return !(m_byKeyState[byKeyID] & 0x80) && (m_byPrevKeyState[byKeyID] & 0x80);
}

_bool CInput_Device::Get_DIMouseState_Once(MOUSEKEYSTATE eMouse)
{
	return (m_tMouseState.rgbButtons[eMouse] & 0x80) && !(m_tPrevMouseState.rgbButtons[eMouse] & 0x80);
}

_bool CInput_Device::Get_DIMouseState_Once_Up(MOUSEKEYSTATE eMouse)
{
	return !(m_tMouseState.rgbButtons[eMouse] & 0x80) && (m_tPrevMouseState.rgbButtons[eMouse] & 0x80);
}

CInput_Device * CInput_Device::Create(HINSTANCE hInst, HWND hWnd)
{
	CInput_Device*		pInstance = new CInput_Device();

	if (FAILED(pInstance->Initialize(hInst, hWnd)))
	{
		MSG_BOX(TEXT("Failed to Created : CInput_Device"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Engine::CInput_Device::Free(void)
{
	Safe_Release(m_pKeyBoard);
	Safe_Release(m_pMouse);
	Safe_Release(m_pInputSDK);
}

