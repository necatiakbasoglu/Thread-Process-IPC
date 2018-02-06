#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <string.h>



typedef struct
{
	int day;
	int threadNo;
	int* sum;
	char product[25];
}THREAD_PARAMETERS;

//in my structure hold day , sum is the item count, product is the item name like MILK

//global variables
HANDLE* handles;
THREAD_PARAMETERS* lpParameter;
int* threadID;

DWORD WINAPI threadWork(LPVOID parameters);

createThreadHelper(int number, int day, char product[]);

int main()
{
	HANDLE readPipe, writePipe;
	char message[10];

	int i = 0;
	DWORD read;
	//set pipe handles
	readPipe = GetStdHandle(STD_INPUT_HANDLE);
	writePipe = GetStdHandle(STD_OUTPUT_HANDLE);

	//read pipe the data which is written by parent process
	if (!ReadFile(readPipe, message, 10, &read, NULL))
	{
		printf("Unable to read");
	}

	//memory allocation of parameters
	handles = malloc(sizeof(HANDLE) * 4);
	lpParameter = malloc(sizeof(THREAD_PARAMETERS) * 4);
	threadID = malloc(sizeof(int) * 4);

	//create thread by using user define function
	createThreadHelper(0, atoi(message), "MILK");
	createThreadHelper(1, atoi(message), "BISCUIT");
	createThreadHelper(2, atoi(message), "CHIPS");
	createThreadHelper(3, atoi(message), "COKE");


	WaitForMultipleObjects(4, handles, TRUE, INFINITE);
	//wait for all threads to finish their jobs


	int bytesToWritten;
	int messageFromChild[4];

	messageFromChild[0] = lpParameter[0].sum;
	messageFromChild[1] = lpParameter[1].sum;
	messageFromChild[2] = lpParameter[2].sum;
	messageFromChild[3] = lpParameter[3].sum;

	//write data on pipe to read parent process
	if (!WriteFile(writePipe, messageFromChild, sizeof(messageFromChild), &bytesToWritten, NULL)) {
		printf("unable to write pipe child\n");
		system("pause");
		exit(0);
	}


	//deallocate variables
	free(handles);
	free(lpParameter);
	free(threadID);
	//close handess
	CloseHandle(writePipe);
	CloseHandle(readPipe);
	system("pause");
	return 1;
}


createThreadHelper(int number, int day, char product[]) {
	lpParameter[number].day = day;
	lpParameter[number].threadNo = (number + 1);//number +1 because parent send number from 0 to 6 via pipe
	strcpy_s(lpParameter[number].product, 15, product);

	handles[number] = CreateThread(NULL, 0, threadWork, &lpParameter[number], 0, &threadID[number]);
	if (handles[number] == INVALID_HANDLE_VALUE)
	{
		printf("error when creating thread\n");
		system("pause");
		exit(0);
	}
}

DWORD WINAPI threadWork(LPVOID parameters)
{
	//string array about start day and end day these are necesarry for reading file
	char *startDays[15] = { "#START DAY 1#","#START DAY 2#","#START DAY 3#","#START DAY 4#","#START DAY 5#","#START DAY 6#","#START DAY 7#" };
	char *endDays[15] = { "#END DAY 1#","#END DAY 2#","#END DAY 3#","#END DAY 4#","#END DAY 5#","#END DAY 6#","#END DAY 7#" };
	THREAD_PARAMETERS* param = (THREAD_PARAMETERS*)parameters;

	FILE *fp;
	errno_t error;
	char line[500];
	BOOL flag = FALSE;
	int sum = 0;
	error = fopen_s(&fp, "../market.txt", "r");
	if (error == 0) {

		char *token;
		while (fgets(line, sizeof(line), fp) != NULL)//get line until is null
		{
			line[strlen(line) - 1] = '\0';
			if (strstr(line, startDays[param->day - 1]) != NULL) //if line include corresponding start days then flag is true
				flag = TRUE;
			if (strstr(line, endDays[param->day - 1]) != NULL) // if line include corresponding end days then flag is false
				flag = FALSE;
			if (flag == TRUE) {                            //while flag is true then I can take tokens of line
				token = strtok(line, ",()");

				while (token != NULL) {

					if (strstr(token, param->product) != NULL) {  //if token includes corresponding product then increase its sum
						sum++;
					}
					token = strtok(NULL, ",()");
				}
				param->sum = sum;
			}
		}
		fclose(fp);
	}
	else
		printf("Doesn't open\n");
}
