/*
	����������������
	���ߣ�������
	ʱ�䣺2019-09-11
*/
#include "pch.h"
#include "BugReport.h"
#include <tchar.h>
#include <mutex>

#pragma region ȫ���쳣����
//ȫ�ֻ�����
std::mutex mtx;
int _timeTick = 0;
//char* _productName;
TCHAR sproductName[MAX_PATH];
//Crash�쳣�ص�
long   __stdcall   callback(_EXCEPTION_POINTERS* excp)
{
	printf("���ص���crash!\n");
	//ͬ��������ֹ����߳�ͬʱ������ͬʱ�����쳣�ص�
	mtx.lock();
	MEMORY_BASIC_INFORMATION mbi = { 0 };
	//��ȡ�쳣ģ��·��
	WCHAR excInfo[MAX_PATH] = L"";
	TCHAR excInfoBuffer[MAX_PATH] = { 0 };
	memset(excInfoBuffer, 0, sizeof(excInfoBuffer));
	int rtQuery = VirtualQuery(excp->ExceptionRecord->ExceptionAddress, &mbi, sizeof(mbi));
	if (rtQuery) {
		UINT_PTR h_module = (UINT_PTR)mbi.AllocationBase;
		GetModuleFileNameW((HMODULE)h_module, excInfo, MAX_PATH);
		swprintf_s(excInfoBuffer, _T("\"%s\""), excInfo);
		//MessageBox(0, excInfoBuffer, L"����", MB_OK);
	}
	//��ȡ��ǰ����Ŀ¼�������ļ���
	LPTSTR cWinDir = new TCHAR[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, cWinDir);
	//��ȡ��ǰ����Ŀ¼�����ļ���
	TCHAR szPath[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, szPath, MAX_PATH);

	TCHAR path[MAX_PATH] = { 0 };
	memset(path, 0, sizeof(path));
	//printf("��ʼ��ȡ����ʱ\n");
	swprintf_s(path, _T("\"%s\""), szPath);
	//printf("��ȡ��ǰ����Ŀ¼��%s\n", szPath);
	//MessageBox(0, path, L"����", MB_OK);
	//if (!(excInfo && *excInfo != '\0')) {
	//	//��ȡ�쳣ģ��ʧ��
	//}

	//����ʱ
	TCHAR timeTick[MAX_PATH] = { 0 };
	memset(timeTick, 0, sizeof(timeTick));
	//printf("��ʼ��ȡ����ʱ\n");
	swprintf_s(timeTick, _T("%d"), _timeTick);
	//printf("����ʱ��%d\n", _timeTick);

	//��Ʒ����_stdcall
	//TCHAR productName[MAX_PATH] = { 0 };
	//memset(productName, 0, sizeof(productName));
	//swprintf_s(productName, _T("%s"), _productName);
	////printf("%s\n", productName);
	//MessageBox(0, productName, L"����", MB_OK);

	TCHAR excpAddress[MAX_PATH] = { 0 };
	memset(excpAddress, 0, sizeof(excpAddress));
	swprintf_s(excpAddress, _T("0x%04X"), excp->ExceptionRecord->ExceptionAddress);
	//printf("����ƫ������%s\n", excpAddress);
	//MessageBox(0, excpAddress, L"����", MB_OK);
//	TCHAR sproductName[MAX_PATH] = { 0 };
//#ifdef UNICODE
//	MultiByteToWideChar(CP_ACP, 0, _productName, -1, sproductName, MAX_PATH);
//#else
//	strcpy(sproductName, _productName);
//#endif
	//MessageBox(0, sproductName, L"����", MB_OK);

	TCHAR strCmdLine[MAX_PATH] = { 0 };
	memset(strCmdLine, 0, sizeof(strCmdLine));
	//���θ�BugReport ��ǰ��������Ŀ¼������ģ��·��������ƫ����
	swprintf_s(strCmdLine, _T("BugReport.exe %s %s %s %s %s"), path, excInfoBuffer, excpAddress, timeTick, sproductName);
	//MessageBox(0, strCmdLine, L"����", MB_OK);
	//printf("������%s\n", strCmdLine);

	//����BugReprot.exe
	printf("����BugReprot.exe");
	STARTUPINFO si = { 0 };
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi;
	BOOL bRet = CreateProcess(
		NULL,   //  ָ��һ��NULL��β�ġ�����ָ����ִ��ģ��Ŀ��ֽ��ַ���  
		strCmdLine, // �������ַ���  
		NULL, //    ָ��һ��SECURITY_ATTRIBUTES�ṹ�壬����ṹ������Ƿ񷵻صľ�����Ա��ӽ��̼̳С�  
		NULL, //    ���lpProcessAttributes����Ϊ�գ�NULL������ô������ܱ��̳С�<ͬ��>  
		false,//    ָʾ�½����Ƿ�ӵ��ý��̴��̳��˾����   
		0,  //  ָ�����ӵġ���������������ͽ��̵Ĵ����ı�  
			//  CREATE_NEW_CONSOLE  �¿���̨���ӽ���  
			//  CREATE_SUSPENDED    �ӽ��̴��������ֱ������ResumeThread����  
		NULL, //    ָ��һ���½��̵Ļ����顣����˲���Ϊ�գ��½���ʹ�õ��ý��̵Ļ���  
		NULL, //    ָ���ӽ��̵Ĺ���·��  
		&si, // �����½��̵������������ʾ��STARTUPINFO�ṹ��  
		&pi  // �����½��̵�ʶ����Ϣ��PROCESS_INFORMATION�ṹ��  
	);
	if (bRet) {
		WaitForSingleObject(pi.hProcess, INFINITE);//�ȴ��������̽���
		DWORD dwExitCode;
		GetExitCodeProcess(pi.hProcess, &dwExitCode);
		printf("BugReport�˳��룺%s\n", dwExitCode);
	}
	printf("�����ɹ�");
	mtx.unlock();
	return EXCEPTION_EXECUTE_HANDLER;
}

//SetUnhandledExceptionFilter�򲹶�������ڴ����ϻ����� ret 4�����ú������ע���Լ��Ĵ���ص�
void PatchSetUnhandledExceptionFilter()
{
	void* addr = (void*)GetProcAddress(LoadLibrary(L"kernel32.dll"),
		"SetUnhandledExceptionFilter");
	if (addr)
	{
		unsigned char code[16];
		int size = 0;

#ifdef _M_IX86
		code[size++] = 0x33;
		code[size++] = 0xC0;
		code[size++] = 0xC2;
		code[size++] = 0x04;
		code[size++] = 0x00;
#elif _M_X64
		code[size++] = 0x33;
		code[size++] = 0xC0;
		code[size++] = 0xC3;
#else
#error "The following code only works for x86 and x64!"
#endif
		DWORD dwOldFlag, dwTempFlag;
		VirtualProtect(addr, size, PAGE_READWRITE, &dwOldFlag);
		WriteProcessMemory(GetCurrentProcess(), addr, code, size, NULL);
		VirtualProtect(addr, size, dwOldFlag, &dwTempFlag);
	}
	//static BYTE RETURN_CODE[] = { 0xc2, 0x04, 0x00 }; //�����룺ret 4 ��ͬ C++�� return
	//MEMORY_BASIC_INFORMATION   mbi;
	//DWORD dwOldProtect = 0;
	//DWORD pfnSetFilter = (DWORD)GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "SetUnhandledExceptionFilter");
	//bool virtualQuery1 = VirtualQuery((void*)pfnSetFilter, &mbi, sizeof(mbi));
	//printf("virtualQuery1=%d\n", virtualQuery1);
	//bool virtualProtectEx1 = VirtualProtectEx(GetCurrentProcess(), (void*)pfnSetFilter, sizeof(RETURN_CODE), PAGE_READWRITE, &dwOldProtect);
	//printf("virtualProtectEx1=%d\n", virtualProtectEx1);
	////PAGE_EXECUTE_READWRITE    PAGE_READWRITE
	//bool writeProcessMemory = WriteProcessMemory(GetCurrentProcess(), (void*)pfnSetFilter, RETURN_CODE, sizeof(RETURN_CODE), NULL);
	//printf("writeProcessMemory=%d\n", writeProcessMemory);
	////bool virtualProtect2 = VirtualProtect((void*)pfnSetFilter, sizeof(RETURN_CODE), mbi.Protect, 0);
	////printf("virtualProtect2=%d\n", virtualProtect2);
	//bool flushInstructionCache = FlushInstructionCache(GetCurrentProcess(), (void*)pfnSetFilter, sizeof(RETURN_CODE));
	//printf("flushInstructionCache=%d\n", flushInstructionCache);
}

bool is_str_utf8(const char* str)
{
	unsigned int nBytes = 0;//UFT8����1-6���ֽڱ���,ASCII��һ���ֽ�  
	unsigned char chr = *str;
	bool bAllAscii = true;

	for (unsigned int i = 0; str[i] != '\0'; ++i) {
		chr = *(str + i);
		//�ж��Ƿ�ASCII����,�������,˵���п�����UTF8,ASCII��7λ����,���λ���Ϊ0,0xxxxxxx 
		if (nBytes == 0 && (chr & 0x80) != 0) {
			bAllAscii = false;
		}

		if (nBytes == 0) {
			//�������ASCII��,Ӧ���Ƕ��ֽڷ�,�����ֽ���  
			if (chr >= 0x80) {

				if (chr >= 0xFC && chr <= 0xFD) {
					nBytes = 6;
				}
				else if (chr >= 0xF8) {
					nBytes = 5;
				}
				else if (chr >= 0xF0) {
					nBytes = 4;
				}
				else if (chr >= 0xE0) {
					nBytes = 3;
				}
				else if (chr >= 0xC0) {
					nBytes = 2;
				}
				else {
					return false;
				}

				nBytes--;
			}
		}
		else {
			//���ֽڷ��ķ����ֽ�,ӦΪ 10xxxxxx 
			if ((chr & 0xC0) != 0x80) {
				return false;
			}
			//����Ϊ��Ϊֹ
			nBytes--;
		}
	}

	//Υ��UTF8������� 
	if (nBytes != 0) {
		return false;
	}

	if (bAllAscii) { //���ȫ������ASCII, Ҳ��UTF8
		return true;
	}

	return true;
}

bool is_str_gbk(const char* str)
{
	unsigned int nBytes = 0;//GBK����1-2���ֽڱ���,�������� ,Ӣ��һ�� 
	unsigned char chr = *str;
	bool bAllAscii = true; //���ȫ������ASCII,  

	for (unsigned int i = 0; str[i] != '\0'; ++i) {
		chr = *(str + i);
		if ((chr & 0x80) != 0 && nBytes == 0) {// �ж��Ƿ�ASCII����,�������,˵���п�����GBK
			bAllAscii = false;
		}

		if (nBytes == 0) {
			if (chr >= 0x80) {
				if (chr >= 0x81 && chr <= 0xFE) {
					nBytes = +2;
				}
				else {
					return false;
				}

				nBytes--;
			}
		}
		else {
			if (chr < 0x40 || chr>0xFE) {
				return false;
			}
			nBytes--;
		}//else end
	}

	if (nBytes != 0) {		//Υ������ 
		return false;
	}

	if (bAllAscii) { //���ȫ������ASCII, Ҳ��GBK
		return true;
	}

	return true;
}

//�ṩ���ⲿ������õĵ�������
void _stdcall SetGlobalExceptionFilter(int timeTick, char* productName) {
	//ע��ص�
	SetUnhandledExceptionFilter(callback);
	//�򲹶�
	PatchSetUnhandledExceptionFilter();
	//����ʱ
	//int timeTick = 10;
	_timeTick = timeTick;
	//_productName = productName;
#ifdef UNICODE
	MultiByteToWideChar(CP_ACP, 0, productName, -1, sproductName, MAX_PATH);
#else
	strcpy(sproductName, _productName);
#endif
	//puts(productName);
	//MessageBox(0, sproductName, L"��ʾ", MB_OK);
	/*bool isz= is_str_utf8(productName);
	if(isz)
		printf("is utf8\n");*/

		//printf("����ʱ��%d\n", _timeTick);

		//python�ַ����봫�������������ת��һ��
		/*isz = is_str_gbk(productName);
		if (isz)
			printf("is gbk\n");*/
			//char* szStr = productName;
			//WCHAR wszClassName[MAX_PATH];
			//memset(wszClassName, 0, sizeof(wszClassName));
			//MultiByteToWideChar(CP_ACP, 0, szStr, strlen(szStr) + 1, wszClassName,
			//	sizeof(wszClassName) / sizeof(wszClassName[0]));
			//
			////��Ʒ����
			//_productName = wszClassName;
			//printf("��Ʒ���ƣ�%s\n", _productName);
			//Sleep(200000);
			//MessageBox(0, L"hanjunjun", L"��ʾ", MB_OK);
			//int* ptr = NULL;
			//*ptr = 1;
			//_asm   int   3   //ֻ��Ϊ���ó������
			//MessageBox(0, L"�����쳣����ģ��ɹ���", L"��ʾ", MB_OK);
}

void _stdcall Test() {
	//MessageBox(0, L"my name is ������", L"��ʾ", MB_OK);
}
#pragma endregion