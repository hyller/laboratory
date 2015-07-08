/* ************************************
*����ͨWindows API��
* ʾ������
* schedule.c
* 6.3.2 ���𡢻ָ����л�����ֹ�߳�
**************************************/
/* ͷ�ļ�*/
#include <windows.h>
#include <stdio.h>

/*************************************
* DWORD WINAPI ThreadProc( LPVOID lpParam )
* ����	�̺߳���
*		���ѭ����ӡ���
*
* ����LPVOID lpParam ָ��DWORD
**************************************/
DWORD WINAPI ThreadProc( LPVOID lpParam )
{
	LPDWORD pData;
	DWORD i = 0;
	// ������������
	pData = (LPDWORD)lpParam;
	// ѭ����ӡ���
	for(i; i<10; i++)
	{
		Sleep(100);		// ÿ�����ӡһ��
		printf("TID = %u,\t Parameters = %u\t i = %u\n",
			GetCurrentThreadId(), *pData, i);
	}
	ExitThread(i);
	return 0;
}

/*************************************
* void main()
* �������̺߳�������ʾ�̵߳���
**************************************/
void main()
{
	DWORD dwData;
	DWORD dwThreadId[2];
	HANDLE hThread[2];
	// �����߳�
	dwData = 1;
	hThread[0] = CreateThread(
		NULL,0,
		ThreadProc,
		&dwData,
		CREATE_SUSPENDED,	// �����½��Ľ���
		&dwThreadId[0]);
	if (hThread[0] == NULL)
	{
		ExitProcess(0);
	}

	/// �����߳�
	dwData = 2;
	hThread[1] = CreateThread(
		NULL,0,
		ThreadProc,
		&dwData,
		0,					// Ĭ�ϱ�־
		&dwThreadId[1]);
	if (hThread[1] == NULL)
	{
		ExitProcess(1);
	}

	// �ȴ�200���룬�ָ��̵߳�ִ��
	Sleep(200);
	ResumeThread(hThread[0]);
	// �����̵߳�ִ��
	SuspendThread(hThread[1]);
	// �ȴ�300���룬��ֹ�̣߳��ָ��߳�
	Sleep(300);
	TerminateThread(hThread[0],0);
	ResumeThread(hThread[1]);

	//�ȴ������߳�ִ�н���
	WaitForMultipleObjects(2, hThread, TRUE, INFINITE);
	// �ر������̵߳ľ��
	CloseHandle(hThread[0]);
	CloseHandle(hThread[1]);
}