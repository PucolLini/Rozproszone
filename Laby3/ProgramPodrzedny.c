#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <tlhelp32.h>

#define LICZBA_W_PARAMETRZE 10

typedef struct {
    DWORD threadID;
    BOOL fromHere;
    int priority;
} Thread;

typedef struct {
    char name[50];
    int number;
} Param;

Thread *threads;
int threadCounter = 0;

int priorities[3] = {THREAD_PRIORITY_BELOW_NORMAL, THREAD_PRIORITY_NORMAL, THREAD_PRIORITY_ABOVE_NORMAL};

void menu() {
    printf("MENU\n");
    printf("a. Utworz nowy watek\n");
    printf("b. Usun wybrany watek\n");
    printf("c. Zmien priorytet wybranego watku\n");
    printf("d. Wyswietl liste watkow w tym procesie\n");
    printf("e. Zamknij dany proces\n");
    printf(">");
}

DWORD WINAPI ThreadFunction(void *arguments) {
    Param *param = (Param *)arguments;
    long long int result = 1;
    for (int i = 1; i <= param->number; i++) {
        result += i;
        Sleep(1000);
        //printf("Silnia z %d: %d\n", i, result);
    }
    printf("Rezultat: %d\n", result);
    return result;
}

void newThread(int priority) {
    Param *param = malloc(sizeof(Param));
    if (param == NULL) {
        printf("BLAD - Nie udalo sie zaalokowac pamieci.\n");
        return;
    }

    printf("Podaj parametr: ");
    scanf("%d", &(param->number));

    HANDLE hThread = CreateThread(
        NULL,
        0,
        ThreadFunction,
        param,
        0,
        NULL
    );

    if (hThread == NULL) {
        printf("BLAD - Watek nie zostal stworzony.\n");
        free(param);
    }
    else {
        printf("Watek zostal stworzony.\n");
        threadCounter++;
        threads = realloc(threads, threadCounter * sizeof(Thread));
        if (threads == NULL) {
            printf("Blad alokacji pamieci dla tablicy threads\n");
        }
        threads[threadCounter - 1].threadID = GetThreadId(hThread);
        threads[threadCounter - 1].fromHere = TRUE;

        if(!SetThreadPriority(hThread, priorities[priority])){
            printf("Blad w nadawaniu priorytetu watka.\n");
        }

        threads[threadCounter - 1].priority = priorities[priority];

        CloseHandle(hThread);
    }
}

void deleteThread(int threadNumber) {
    if (threadNumber < 0 || threadNumber >= threadCounter) {
        printf("BLAD - Nieprawidlowy numer watku.\n");
        return;
    }

    DWORD threadID = threads[threadNumber].threadID;
    HANDLE hThread = OpenThread(THREAD_TERMINATE, FALSE, threadID);

    if (hThread == NULL) {
        printf("BLAD - Watek nie zostal znaleziony.\n");
        return;
    }

    if (TerminateThread(hThread, 0)) {
        printf("Watek zostal usuniety.\n");
        CloseHandle(hThread);

        threads[threadNumber] = threads[threadCounter - 1];
        threadCounter--;
        threads = realloc(threads, threadCounter * sizeof(Thread));
    }
    else {
        printf("BLAD - Watek nie zostal usuniety.\n");
        CloseHandle(hThread);
    }
}

void changePriority(int threadNumber, int newPriority) {
    DWORD threadID = threads[threadNumber].threadID;
    printf("threadID: %d, index: %d\n", threadID, threadNumber);
    HANDLE hThread = OpenThread(THREAD_SET_INFORMATION, FALSE, threadID);

    if (hThread == NULL) {
        printf("BLAD - Watek nie zostal znaleziony.\n");
        return;
    }

    if (newPriority < THREAD_PRIORITY_IDLE || newPriority > THREAD_PRIORITY_TIME_CRITICAL) {
        printf("BLAD - Nieprawidlowy priorytet.\n");
        CloseHandle(hThread);
        return;
    }

    if (SetThreadPriority(hThread, newPriority)) {
        printf("Zmieniono priorytet watku %lu.\n", threadID);
        threads[threadNumber].priority = newPriority;
    }
    else {
        printf("Nie udalo sie zmienic priorytetu watku.\n");
    }

    CloseHandle(hThread);
}

void showThreadList() {
    HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hThreadSnap == INVALID_HANDLE_VALUE) {
        printf("Blad podczas snapshot watkow (%d)\n", GetLastError());
        return;
    }

    THREADENTRY32 te32;
    te32.dwSize = sizeof(THREADENTRY32);

    if (!Thread32First(hThreadSnap, &te32)) {
        printf("Blad podczas pobierania informacji o pierwszym watku (%d)\n", GetLastError());
        CloseHandle(hThreadSnap);
        return;
    }

    printf("WATKI UTWORZONE PRZEZ TEN PROCES:\n");
    int index = 0;
    printf("thCounter %d\n", threadCounter);
    for(int i=0; i<threadCounter; i++){
        printf("%d. ID: %d, priorytet: ", i, threads[i].threadID);

        if(threads[i].priority == 0){
            printf("THREAD_PRIORITY_NORMAL\n");
        }
        else if(threads[i].priority == 1){
            printf("THREAD_PRIORITY_ABOVE_NORMAL\n");
        }
        else if(threads[i].priority == -1){
            printf("THREAD_PRIORITY_BELOW_NORMAL\n");
        }
    }

    CloseHandle(hThreadSnap);
}

void endProcess() {
    for (int i = 0; i < threadCounter; i++) {
        deleteThread(i);
    }
    free(threads); // Zwolnienie pamięci po tablicy threads
    exit(0);
}

int priorityMenu(){
    int priority=0;
    printf("Wybierz priorytet:\n");
    printf("1. THREAD_PRIORITY_BELOW_NORMAL\n");
    printf("2. THREAD_PRIORITY_NORMAL\n");
    printf("3. THREAD_PRIORITY_ABOVE_NORMAL\n");
    scanf("%d", &priority);
    if(priority < 1 || priority > 3){
        priority = 2;
        printf("Ustawiono priorytet watka na normalny.\n");
    }
    return priority;
}

int main() {
    char option;
    threads = malloc(0); // Inicjalizacja tablicy threads
    printf("START\n");
    do {
        do {
            menu();
            scanf(" %c", &option);
            if (option < 'a' || option > 'e') {
                printf("Zly input. Sprobuj ponownie.\n");
            }
        } while (option < 'a' || option > 'e');

        int suboption = 0;
        switch (option) {
            case 'a':
                newThread(priorityMenu()-1);
                break;
            case 'b':
                printf("Ktory watek chcesz usunac?\n");
                scanf("%d", &suboption);
                deleteThread(suboption);
                break;
            case 'c':
                printf("U ktorego watka chcesz zmienic priorytet?\n");
                scanf("%d", &suboption);
                changePriority(suboption, priorities[priorityMenu() - 1]);
                break;
            case 'd':
                showThreadList();
                break;
            case 'e':
                endProcess();
                break;
            default:
                break;
        }
    } while (option != 'e');

    return 0;
}
