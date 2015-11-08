#pragma once

#include <windows.h> 

const HANDLE gOutputFileSemaphone = CreateSemaphore(NULL, 1, 1, NULL);
const HANDLE gWriteFileSemaphone = CreateSemaphore(NULL, 1, 1, NULL);

static int fileCount = 0;

void DisplayError(LPTSTR lpszFunction);
char* getNewOutputFile();
