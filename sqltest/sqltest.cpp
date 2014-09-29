// sqltest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../Sqlite/sqlite3.h"

#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <sstream>
#include <Windows.h>
#include <fstream>

using namespace std;

void GetFormatFile(char* lpSqlPath, char* fFileName);

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		printf("how to use:\r\n"
			"\t\ta.exe taget\\path.sqlite\r\n");
	}
	else
	{
		char* lpSqlPath = argv[1];
		char fFileName[MAX_PATH] = {"default.csv"};
		char* mark1 = strrchr( argv[1], '\\' );
		char* mark2 = strrchr(argv[1], '.');
		if (mark1 && mark2)
		{
			mark1 += 1;
			int nlen = mark2 - mark1;
			memcpy(fFileName, mark1, nlen);
			fFileName[nlen] = '\0';
			strcat_s(fFileName, ".csv");
			GetFormatFile(lpSqlPath, fFileName);
		}
		else
		{
			printf("the target path is wrong !\r\n");
		}
	}
	system("pause");
	return 0;
}

void GetFormatFile(char* lpSqlPath, char* fFileName)
{
	sqlite3 *pDb = NULL;
	sqlite3_stmt *pStmtChampions = NULL;
	char* sqlPath = lpSqlPath;
	ofstream outFile(fFileName);
	char* selectChampions = "SELECT * FROM champions";
	stringstream outLine ;
	outLine << "name,displayName,ability,abilityQ,abilityW,abilityE,abilityR\r\n";
	outFile.write(outLine.str().c_str(), outLine.str().length());
	outFile.flush();
	int nRes = sqlite3_open(sqlPath, &pDb);
	if (SQLITE_OK == nRes)
	{
		if ( SQLITE_OK == sqlite3_prepare(pDb, selectChampions, strlen(selectChampions), &pStmtChampions, NULL) )
		{
			while ( SQLITE_ROW == sqlite3_step(pStmtChampions) )
			{
				outLine.str("");	// Çå¿Õ
				int	id = sqlite3_column_int(pStmtChampions, 0);
				char* name = (char*)sqlite3_column_text(pStmtChampions, 1);
				char* displayeName = (char*)sqlite3_column_text(pStmtChampions, 2);
				
				sqlite3_stmt *pStmtChampionsAblt = NULL;
				char selectChampionsAblt[MAX_PATH] = {NULL};
				sprintf_s(selectChampionsAblt, MAX_PATH, "SELECT rank,name FROM championAbilities WHERE championId=%d ORDER BY rank", id);

				outLine << name << "," << displayeName ;
				
				if ( SQLITE_OK == sqlite3_prepare(pDb, selectChampionsAblt, strlen(selectChampionsAblt), &pStmtChampionsAblt, NULL) )
				{
					for (int i = 0; i < 5 && SQLITE_ROW == sqlite3_step(pStmtChampionsAblt) ; ++i)
					{
						//int rank = sqlite3_column_int(pStmtChampionsAblt, 0);
						char* ablt = (char*)sqlite3_column_text(pStmtChampionsAblt, 1);	// ±»¶¯£¬Q W E R
						outLine << "," << ablt;
						__asm nop	
					}
				}
				outLine << "\r\n";
				outFile.write(outLine.str().c_str(), outLine.str().length());
				outFile.flush();
				printf("%s", outLine.str().c_str());
			}
		}
		sqlite3_close(pDb);
	}
	outFile.close();
}

