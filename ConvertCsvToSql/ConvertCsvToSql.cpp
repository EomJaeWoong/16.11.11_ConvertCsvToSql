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

	//데이터 담을 배열 초기화
	for (int iCnt = 0; iCnt < ROWSIZE; iCnt++)
	{
		for (int iCnt2 = 0; iCnt2 < COLSIZE; iCnt2++)
			memset(stRow[iCnt].wColumn[iCnt2], 0, VALSIZE);
	}

	//sql 파일 오픈
	_wfopen_s(&SQLOut, L"data.sql", L"w,ccs=UTF-8");
	if (SQLOut == NULL)
	{
		wprintf(L"SQLOut Not Open\n");
		return;
	}

	//BOM 코드 삭제
	rewind(SQLOut);

	//트랜젝션 시작
	fputws(L"SET AUTOCOMMIT = 0;\n", SQLOut);
	fputws(L"START TRANSACTION;\n\n", SQLOut);
	fputws(L"DELETE FROM `csv_sql`.`data1`;\n\n", SQLOut);

	_wfopen_s(&SQLIn, L"DATA1.csv", L"r,ccs=UTF-8");

	//data1.csv 파일을 열어서 첫번째 줄을 헤더로 분석
	memset(wRow, 0, STRSIZE);
	fgetws(wRow, STRSIZE, SQLIn);
	wValue = wcstok_s(wRow, L",", &wContext);

	//헤더 데이터배열에 삽입
	for (int iCntCol = 0; wValue != NULL; iCntCol++)
	{
		wcscpy_s(stRow[0].wColumn[iCntCol], wcslen(wValue) * 2, wValue);
		wValue = wcstok_s(NULL, L",\n", &wContext);
	}

	//데이터 데이터배열에 삽입
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

	//데이터 배열문으로 INSERT문 만들기
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

	//완료되면 커밋 후 트랜젝션 끝
	fputws(L"\n", SQLOut);
	fputws(L"COMMIT;\n",SQLOut);
	fputws(L"SET AUTOCOMMIT = 1;", SQLOut);

	printf("convert to SQL Query.....\n");

	fclose(SQLIn);
	fclose(SQLOut);
}