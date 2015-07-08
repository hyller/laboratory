/* ************************************
*����ͨWindows API��
* ʾ������
* CreateThread.c
* 6.3.1  �����̡߳��˳��̡߳���ȡ�߳���Ϣ
**************************************/
/* ͷ�ļ���*/
#include <windows.h>
#include <stdio.h>
/* �������塡*/
#define MAX_THREADS 5
/* �ṹ���͡�*/
typedef struct _THREAD_PARAM {
	DWORD i;
	DWORD dwRandom;
	DWORD dwData; 
} THREAD_PARAM, *LPTHREAD_PARAM;

/*************************************
* DWORD WINAPI ThreadProc( LPVOID lpParam )
* ����	�̺߳���
*		��������ӡ��
*
* ����	LPVOID lpParam	THREAD_PARAM �ṹ����
**************************************/
DWORD WINAPI ThreadProc( LPVOID lpParam ) 
{ 
	LPTHREAD_PARAM pData;
	// ������������
	pData = (LPTHREAD_PARAM)lpParam;
	// ��ʾ����
	printf("TID = %u,\t Parameters = %u, %u,  %u\n", 
		GetCurrentThreadId(), 
		pData->i, pData->dwRandom, pData->dwData); 
	// �ͷű���������ڴ棨�����߳��з���ģ�.
	HeapFree(GetProcessHeap(), 0, pData);
	return 0; 
} 
/*************************************
* void main()
* ����	���̺߳����������˶���߳�
**************************************/
void main()
{
	LPTHREAD_PARAM pData;
	DWORD dwThreadId[MAX_THREADS];
	HANDLE hThread[MAX_THREADS]; 
	int i;

	// ����MAX_THREADS ���߳�.
	for( i=0; i<MAX_THREADS; i++ )
	{
		// Ϊ�̺߳������������ڴ�
		pData = (LPTHREAD_PARAM)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
			sizeof(THREAD_PARAM));
		if( pData == NULL )
		{
			printf("HeapAlloc error;\n");
			ExitProcess(2);
		}
		// ���ò���
		pData->i = i;
		pData->dwRandom = rand();
		pData->dwData = 100;
		// �����߳�
		hThread[i] = CreateThread( 
			NULL,              // Ĭ�ϰ�ȫ����
			0,                 // Ĭ�϶�ջ��С 
			ThreadProc,        // �̺߳���
			pData,             // ����
			0,                 // Ĭ�ϴ�����־
			&dwThreadId[i]);   // ����TID
		// �ж��Ƿ񴴽��ɹ�
		if (hThread[i] == NULL) 
		{
			ExitProcess(i);
		}
	}
}