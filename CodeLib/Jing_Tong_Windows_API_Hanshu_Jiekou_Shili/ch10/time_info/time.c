/* ************************************
*����ͨWindows API�� 
* ʾ������
* time.c
* 10.2  ϵͳʱ��
**************************************/
#include <Windows.h>
#include <stdio.h>
/* ************************************
* int main()
* ����	��ȡ����ʾϵͳ��ǰʱ�䣬Ȼ��ʱ����ǰһ��Сʱ
**************************************/
int main()
{
	SYSTEMTIME st;
	// ��ȡ��ǰʱ�䣬�Ա�ʱ��ʱ���ʽ
	GetLocalTime( &st );
	printf("Now: %d-%d-%d, %d:%d:%d",
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	// ��ǰһСʱ
	st.wHour --;
	// ���õ�ǰϵͳʱ��
	SetLocalTime( &st );
}