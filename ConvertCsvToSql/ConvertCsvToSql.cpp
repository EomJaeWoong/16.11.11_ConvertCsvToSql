#include <stdio.h>
#include <windows.h>

#define STRSIZE 512
#define COLSIZE 8
#define VALSIZE 20
#define ROWSIZE 21

struct st_Row
{
	WCHAR wColumn[COLSIZE][VALSIZE];
};

void main()
{
	FILE *SQLOut, *SQLIn;
	WCHAR wRow[STRSIZE], *wValue, *wContext;
	st_Row stRow[ROWSIZE];

	//������ ���� �迭 �ʱ�ȭ
	for (int iCnt = 0; iCnt < ROWSIZE; iCnt++)
	{
		for (int iCnt2 = 0; iCnt2 < COLSIZE; iCnt2++)
			memset(stRow[iCnt].wColumn[iCnt2], 0, VALSIZE);
	}

	//sql ���� ����
	_wfopen_s(&SQLOut, L"data.sql", L"w,ccs=UTF-8");
	if (SQLOut == NULL)
	{
		wprintf(L"SQLOut Not Open\n");
		return;
	}

	//BOM �ڵ� ����
	rewind(SQLOut);

	//Ʈ������ ����
	fputws(L"SET AUTOCOMMIT = 0;\n", SQLOut);
	fputws(L"START TRANSACTION;\n\n", SQLOut);
	fputws(L"DELETE FROM `csv_sql`.`data1`;\n\n", SQLOut);

	_wfopen_s(&SQLIn, L"DATA1.csv", L"r,ccs=UTF-8");

	//data1.csv ������ ��� ù��° ���� ����� �м�
	memset(wRow, 0, STRSIZE);
	fgetws(wRow, STRSIZE, SQLIn);
	wValue = wcstok_s(wRow, L",", &wContext);

	//��� �����͹迭�� ����
	for (int iCntCol = 0; wValue != NULL; iCntCol++)
	{
		wcscpy_s(stRow[0].wColumn[iCntCol], wcslen(wValue) * 2, wValue);
		wValue = wcstok_s(NULL, L",\n", &wContext);
	}

	//������ �����͹迭�� ����
	for (int iCntRow = 1; iCntRow < ROWSIZE; iCntRow++)
	{
		memset(wRow, 0, STRSIZE);
		fgetws(wRow, STRSIZE, SQLIn);
		wValue = wcstok_s(wRow, L",", &wContext);

		for (int iCntCol = 0; wValue != NULL; iCntCol++)
		{
			wcscpy_s(stRow[iCntRow].wColumn[iCntCol], wcslen(wValue) * 2, wValue);
			wValue = wcstok_s(NULL, L",\n", &wContext);
		}
	}

	//������ �迭������ INSERT�� �����
	for (int iCntRow = 1; iCntRow < ROWSIZE; iCntRow++)
	{
		memset(wRow, 0, STRSIZE);
		wcscat_s(wRow, 32, L"INSERT INTO `csv_sql`.`data1` (");

		for (int iCntCol = 0; iCntCol < COLSIZE; iCntCol++)
		{
			wcscat_s(wRow, wcslen(wRow) + 2, L"`");
			wcscat_s(wRow, wcslen(wRow) + wcslen(stRow[0].wColumn[iCntCol]) + 1, stRow[0].wColumn[iCntCol]);
			wcscat_s(wRow, wcslen(wRow) + 2, L"`");
			if (iCntCol + 1 != COLSIZE)		wcscat_s(wRow, wcslen(wRow) + 2, L",");
		}

		wcscat_s(wRow, wcslen(wRow) + 11, L") VALUES (");

		for (int iCntCol = 0; iCntCol < COLSIZE; iCntCol++)
		{
			wcscat_s(wRow, wcslen(wRow) + 2, L"'");
			wcscat_s(wRow, wcslen(wRow) + wcslen(stRow[iCntRow].wColumn[iCntCol]) + 1, stRow[iCntRow].wColumn[iCntCol]);
			wcscat_s(wRow, wcslen(wRow) + 2, L"'");
			if (iCntCol + 1 != COLSIZE)		wcscat_s(wRow, wcslen(wRow) + 2, L",");
		}
		wcscat_s(wRow, wcslen(wRow) + 4, L");\n");

		fputws(wRow, SQLOut);
	}

	//�Ϸ�Ǹ� Ŀ�� �� Ʈ������ ��
	fputws(L"\n", SQLOut);
	fputws(L"COMMIT;\n",SQLOut);
	fputws(L"SET AUTOCOMMIT = 1;", SQLOut);

	printf("convert to SQL Query.....\n");

	fclose(SQLIn);
	fclose(SQLOut);
}