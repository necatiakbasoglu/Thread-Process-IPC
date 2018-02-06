#include <stdio.h>
#include <windows.h>
#include <stdlib.h>


#define NO_OF_PROCESS 7

fillForTotal_Number_Of_Sold_Items_In_Seven_Days(int total_number_of_sold_items_in_seven_days[4],
	int total_number_of_sold_items_in_each_days[7][4]);

theTotalNumberOfEachItemSoldinEachDays(int *product[], int total_number_of_sold_items_in_each_days[7][4]);

theMostSoldItemInSevenDays(int *product[], int total_number_of_sold_items_in_seven_days[4]);

theMostSoldItemInEachDay(int *product[], int total_number_of_sold_items_in_each_days[7][4]);

theTotalNumberOfEachItemSoldInSevenDays(int *product[], int total_number_of_sold_items_in_seven_days[4]);

int main(int argc, char* argv[])
{
	STARTUPINFO si[NO_OF_PROCESS];
	PROCESS_INFORMATION pi[NO_OF_PROCESS];
	SECURITY_ATTRIBUTES sa[NO_OF_PROCESS];
	HANDLE processHandles[NO_OF_PROCESS];
	HANDLE writePipeFromParent[NO_OF_PROCESS], readPipeFromParent[NO_OF_PROCESS];
	HANDLE writePipeFromChild[NO_OF_PROCESS], readPipeFromChild[NO_OF_PROCESS];



	int j = 0;

	for (j = 0; j < NO_OF_PROCESS; j++) {
		//creating security attributes
		SecureZeroMemory(&sa[j], sizeof(SECURITY_ATTRIBUTES));
		sa[j].bInheritHandle = TRUE;
		sa[j].lpSecurityDescriptor = NULL;
		sa[j].nLength = sizeof(SECURITY_ATTRIBUTES);


		//creating the pipe for parent
		if (!CreatePipe(&readPipeFromParent[j], &writePipeFromParent[j], &sa[j], 0)) //use the default buffer size.
		{
			printf("unable to create pipe\n");
			system("pause");
			exit(0);
		}


		//create the pipe for child
		if (!CreatePipe(&readPipeFromChild[j], &writePipeFromChild[j], &sa[j], 0)) {
			printf("unable to create pipe\n");
			system("pause");
			exit(0);
		}

		//creating variables for child process
		SecureZeroMemory(&si[j], sizeof(STARTUPINFO));
		SecureZeroMemory(&pi[j], sizeof(PROCESS_INFORMATION));

		//assign necessary handles to necessary in out part of pipes
		si[j].cb = sizeof(STARTUPINFO);
		si[j].hStdInput = readPipeFromParent[j];
		si[j].hStdOutput = writePipeFromChild[j];
		si[j].hStdError = GetStdHandle(STD_ERROR_HANDLE);
		si[j].dwFlags = STARTF_USESTDHANDLES;



		//create child process
		if (!CreateProcess(NULL, "child.exe", NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si[j], &pi[j]))
		{
			printf("cannot create child process\n");
			system("pause");
			exit(0);
		}
		else {
			processHandles[j] = pi[j].hProcess;
		}

	}



	char message[10];
	int messageFromChild[4];
	int bytesToWrite = 0;
	int bytesWritten = 0;
	//send messages through pipe

	for (j = 0; j<NO_OF_PROCESS; j++)
	{

		sprintf(message, "%d", j + 1);
		bytesToWrite = strlen(message);
		
		// WriteFile function writes data to the specified file or input / output(I / O) device.
		if (!WriteFile(writePipeFromParent[j], message, bytesToWrite, &bytesWritten, NULL))
		{
			printf("unable to write to pipe parent\n");
			system("pause");
			exit(0);
		}



	}


	WaitForMultipleObjects(NO_OF_PROCESS, processHandles, TRUE, INFINITE);
	/*
	in total_number_of_sold_items_in_seven_days and other array
	index 0 = total Milk count in 7 days
	index 1 = total BISCUIT count in 7 days
	index 2 = total Chips count in 7 days
	index 3 = total Coke count in 7 days

	int total_number_of_sold_items_in_each_days is two dimensional array and it has 7 rows 4 columns basically
	[1.day milk count,1. day BISCUIT count, 1.day Chips count, 1.day Coke count]
	*
	*
	*
	[7.day milk count,7. day BISCUIT count, 7.day Chips count, 7.day Coke count]
	*/

	int total_number_of_sold_items_in_seven_days[4];
	int total_number_of_sold_items_in_each_days[7][4];
	char *product[25] = { "MILK","BISCUITE","CHIPS","COKE" };// this is basically string array in C for products
	
	int i;
	for (j = 0; j < NO_OF_PROCESS; j++) {
		//read message from child process for every pipe from child to parent
		if (!ReadFile(readPipeFromChild[j], messageFromChild, sizeof(messageFromChild), &bytesWritten, NULL)) {
			printf("unable to read to pipe\n");
			system("pause");
			exit(0);
		}
		for (i = 0; i < 4; i++)
			//fill the necessary array according to information which is came from child process
			total_number_of_sold_items_in_each_days[j][i] = messageFromChild[i];
	}

	//add all parameter in each days and fill the array total_number_of_sold_items_in_seven_days
	fillForTotal_Number_Of_Sold_Items_In_Seven_Days(total_number_of_sold_items_in_seven_days,
		total_number_of_sold_items_in_each_days);

	//print the most sold item in seven days
	theMostSoldItemInSevenDays(product, total_number_of_sold_items_in_seven_days);

	//print the most sold item in each days
	theMostSoldItemInEachDay(product, total_number_of_sold_items_in_each_days);

	//print the total number of each item sold in seven days
	theTotalNumberOfEachItemSoldInSevenDays(product, total_number_of_sold_items_in_seven_days);

	//print the total number of sold item sold in each days
	theTotalNumberOfEachItemSoldinEachDays(product, total_number_of_sold_items_in_each_days);


	for (j = 0; j < NO_OF_PROCESS; j++) {

		CloseHandle(readPipeFromParent[j]);
		CloseHandle(writePipeFromParent[j]);
		CloseHandle(readPipeFromChild[j]);
		CloseHandle(writePipeFromChild[j]);
		CloseHandle(pi[j].hThread);
		CloseHandle(pi[j].hProcess);
	}

	system("pause");
	return 1;
}



fillForTotal_Number_Of_Sold_Items_In_Seven_Days(int total_number_of_sold_items_in_seven_days[4],
	int total_number_of_sold_items_in_each_days[7][4]) {
	int i, j, sum;
	for (i = 0; i < 4; i++) {
		sum = 0; // read product by product and add them by count then assign the relating product in total_number_of_sold_items_in_seven_days
		for (j = 0; j < 7; j++) {
			sum += total_number_of_sold_items_in_each_days[j][i];
		}
		total_number_of_sold_items_in_seven_days[i] = sum;
	}
}

//calculate the total number of each item in each days and print it out
theTotalNumberOfEachItemSoldinEachDays(int *product[], int total_number_of_sold_items_in_each_days[7][4]) {
	printf("\n-----------The Total Number Of Each Item Sold for Each Day-----------\n");
	int j, i;
	for (j = 0; j < 7; j++) {
		printf("The Items for day %d ", j + 1);
		for (i = 0; i < 4; i++)
			printf("%s: %d ", product[i], total_number_of_sold_items_in_each_days[j][i]);
		printf("\n");
	}
}

//find and print out the most sold item in seven days
theMostSoldItemInSevenDays(int *product[], int total_number_of_sold_items_in_seven_days[4]) {
	printf("-----------The The Most Sold Item in Seven Days-----------\n");
	int i, holder;
	int most = 0;
	for (i = 0; i < 4; i++) {
		if (total_number_of_sold_items_in_seven_days[i] > most) {
			most = total_number_of_sold_items_in_seven_days[i];
			holder = i;
		}
	}
	printf("The most sold item in seven days is %s and it sold %d times \n", product[holder], most);
}

theMostSoldItemInEachDay(int *product[], int total_number_of_sold_items_in_each_days[7][4]) {
	printf("\n-----------The Most Sold Item for Each Day-----------\n");
	int i, holder;
	int most, j;
	for (i = 0; i < 7; i++) {
		most = 0;
		for (j = 0; j<4; j++) {
			if (total_number_of_sold_items_in_each_days[i][j] > most) {
				most = total_number_of_sold_items_in_each_days[i][j];
				holder = j;
			}
		}
		printf("The most sold item for day %d ->", i + 1);
		//The most sold items can be more than one
		for (j = 0; j < 4; j++)
			if (total_number_of_sold_items_in_each_days[i][j] == most)
				printf("%s: is sold %d times ", product[j], most);
		printf("\n");
	}
}


theTotalNumberOfEachItemSoldInSevenDays(int *product[], int total_number_of_sold_items_in_seven_days[4]) {
	//calculate and print total number of each item sold in seven days
	printf("\n-----------The Total Number Of Each Item Sold in Seven Days-----------\n");
	int j;
	for (j = 0; j < 3; j++) {
		printf("%s: %d - ", product[j], total_number_of_sold_items_in_seven_days[j]);
	}
	printf("%s: %d \n", product[3], total_number_of_sold_items_in_seven_days[3]);
}