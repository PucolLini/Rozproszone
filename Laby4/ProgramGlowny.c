#include <stdio.h>
#include "Windows.h"
#include <tchar.h>
#include <psapi.h>
#include <tlhelp32.h>

typedef struct {
    DWORD processID;
    BOOL fromHere;
} Subprocess;

Subprocess *subprocesses;
int subprocessesCounter = 0;

int priorities[4] = {REALTIME_PRIORITY_CLASS, HIGH_PRIORITY_CLASS, NORMAL_PRIORITY_CLASS, IDLE_PRIORITY_CLASS};

void menu(){
    printf("MENU\n");
    printf("1. Stworz nowy proces\n");
    printf("2. Zatrzymaj wybrany proces\n");
    printf("3. Wyswietl liste procesow uruchomionych z aplikacji\n");
    printf("4. Zamknij program\n");
    printf(">");
}

int newProcess(){
    char exe_path[] = "C:\\Users\\Karina\\Desktop\\STUDIA\\SEM4\\Rozproszone\\Laby\\Lab4\\ProgramPodrzedny.exe";

    STARTUPINFO structStartupInfo; 
    PROCESS_INFORMATION structProcInfo; 
    structStartupInfo.cb = sizeof(structStartupInfo);

    GetStartupInfo(&structStartupInfo);

    printf("Wybierz priorytet:\n");
    printf("0. REALTIME_PRIORITY_CLASS\n");
    printf("1. HIGH_PRIORITY_CLASS\n");
    printf("2. NORMAL_PRIORITY_CLASS\n");
    printf("3. IDLE_PRIORITY_CLASS\n");

    int inputPriority;
    scanf("%d", &inputPriority);
    
    if(inputPriority < 0 || inputPriority > 3){
        inputPriority = NORMAL_PRIORITY_CLASS;
        printf("Ustawiono normalny priorytet procesu.\n");
    }

    inputPriority = priorities[inputPriority];

    if (!CreateProcess
        (NULL,              /* lpApplicationName - nazwa modułu, zostawiamy puste */
        exe_path,           /* lpCommandLine - lista parametrów, jeśli lpApplicationName == NULL to pierwszy parametr tu jest ścieżką do pliku EXE który będzie procesem podrzędnym */
        NULL,               /* lpProcessAttributes - wskaźnik do struktury SECURITY_ATTRIBUTES, która określa, czy zwrócone dojście do nowego obiektu procesu może być dziedziczone przez procesy potomne */
        NULL,               /* lpThreadAttributes */
        FALSE,              /* bInheritHandles */
        CREATE_NEW_CONSOLE, /* dwCreationFlags - ustawiając flagę CREATE_NEW_CONSOLE umożliwiamy otwarcie nowego okna konsoli dla procesu podrzędnego */
        0,                  /* lpEnvironment */
        0,                  /* lpCurrentDirectory */
        &structStartupInfo, /* lpStartupInfo - wskaźnik na strukturę startową */
        &structProcInfo     /* lpProcessInformation - wskaźnik na strukturę z danymi procesu podrzędnego */
    ))
    {
        printf("CreateProcess failed (%d).\n", GetLastError());
        return 1;
    }

    if(!SetPriorityClass(structProcInfo.hProcess, inputPriority)){
        printf("Set priority failed\n");
    }

    CloseHandle(structProcInfo.hProcess);
    CloseHandle(structProcInfo.hThread);

    subprocessesCounter++;
    subprocesses = realloc(subprocesses, subprocessesCounter * sizeof(Subprocess));
    if (subprocesses == NULL) {
        printf("Blad alokacji pamieci dla tablicy subprocesses\n");
        return 1;
    }
    subprocesses[subprocessesCounter - 1].processID = structProcInfo.dwProcessId;
    subprocesses[subprocessesCounter - 1].fromHere = TRUE;

    printf("Nowy proces zostal stworzony.\n");
    return 0;
} 

int stopProcess(int processNumber){
    DWORD processID = subprocesses[processNumber].processID;

    HANDLE hProcess = OpenProcess(
        PROCESS_TERMINATE, /*flaga dostepu - co chcemy zrobic z procesem*/
        FALSE, /*flaga dziedziczenia - czy watki tez maja sie zamknac????*/
        processID);

    DWORD exitCode;

    if(hProcess != NULL){
        if (TerminateProcess(hProcess, 0)) { //czy tu powinna byc negacja? 
            if(GetExitCodeProcess(hProcess, &exitCode)){
                printf("GetExitCodeProcess failed (%d).\n", GetLastError());
                CloseHandle(hProcess);
                return 0;
            }
            if(exitCode != STILL_ACTIVE) {
                printf("Proces o id: %d zostal zatrzymany.\n", processID);
                for (int i = processNumber; i < subprocessesCounter-1; i++) {
                    subprocesses[i] = subprocesses[i + 1];
                }
                subprocessesCounter--;
            }
        }
        else {
            printf("Nie udalo sie zamknac procesu o pid: %d\n", processID);
        }
        
        CloseHandle(hProcess);
        return 1;
    }
    else{
        return 0;
    }
}

void showProcessList(){
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        printf("Błąd podczas tworzenia migawki procesów (%d)\n", GetLastError());
        return;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hProcessSnap, &pe32)) {
        printf("Błąd podczas pobierania informacji o pierwszym procesie (%d)\n", GetLastError());
        CloseHandle(hProcessSnap);
        return;
    }

    printf("PROCESY UTWORZONE PRZEZ TEN PROGRAM:\n");
    do {
        for (int i = 0; i < subprocessesCounter; i++) {
            if (pe32.th32ProcessID == subprocesses[i].processID && subprocesses[i].fromHere) {
                printf("%d. PID: %d, Nazwa: %s\n", i, pe32.th32ProcessID, pe32.szExeFile);
                break;
            }
        }
    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);
}

void endProgram(){
    while(subprocessesCounter){
        if(!stopProcess(0)){
            subprocessesCounter--;
        }
    }
    printf("Wszystkie procesy zostaly usuniete");
    free(subprocesses);
    exit(0);
}

int main(){

    int option = 0;
    int suboption = 0;
    int res;
    printf("START\n");

    while(1){
        do {
            menu();
            scanf("%d", &option);
            if (option < 1 || option > 4) {
                printf("Zly input. Sprobuj ponownie.\n");
            }
        } while (option < 1 || option > 4);
    
        switch (option) {
        case 1:
            res = newProcess();
            break;
        case 2:
            printf("Ktory proces chcesz zatrzymac?\n");
            scanf("%d", &suboption);
            stopProcess(suboption);
            break;
        case 3:
            showProcessList();
            break;
        case 4:
            endProgram();
            return 0;
            break;
        default:
            break;
        }
    }
    
    return 0;
}