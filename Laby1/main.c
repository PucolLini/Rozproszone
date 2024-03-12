#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LENGTH 100

struct element
{
    struct element* prev;
    char* string;
    int length;
    struct element* next;
}*begin;


void firstCreate() {
    begin = (struct element*)malloc(sizeof(struct element));
    begin->next = NULL;
    begin->prev = NULL;
}

struct element* insert(char* obj, struct element* ptr)
{
   
    struct element* p;
    p = ptr->next; /* wez adres analizowanego elementu */

    if (p != NULL) /* czy koniec listy */
    {
        if (strcmp(obj, p->string) <= 0) //(obj > (p->string))
        {
            p->next = insert(obj, p);
        }
        else
        {
            /* utworz nowy element */
            p->prev = (struct element*)malloc(sizeof(struct element));
            /* zapamietaj adres nowego elementu */
            p = p->prev;
            /* zainicjuj skladowe nowego elementu */
            p->string = (char*)malloc(strlen(obj) + 1); // Alokuje pamięć dla p->string, włączając null terminator
            if (p->string == NULL) {
                printf("Błąd alokacji pamięci!\n");
                free(p);
                exit(1);
            }
            strcpy_s(p->string, strlen(obj) + 1, obj);
            //p->string = (char*)malloc(sizeof(obj));
            //p->string = obj;
            p->length = strlen(obj) + 1;
            p->next = ptr->next;
            p->prev = ptr;
        }
    }
    else /* koniec listy */
    { /* utworz nowy element */
        p = (struct element*)malloc(sizeof(struct element));
        /* zainicjuj skladowe nowego elementu */
        p->string = (char*)malloc(strlen(obj) + 1); // Alokuje pamięć dla p->string, włączając null terminator
        if (p->string == NULL) {
            free(p);
            exit(1);
        }
        strcpy_s(p->string, strlen(obj) + 1, obj);
        //p->string = (char*)malloc(sizeof(obj));
        //p->string = obj;
        p->length = strlen(obj) + 1;
        p->next = NULL;
        p->prev = ptr;
    }
    return p; /* zwroc adres nowego elementu */
}

void display() {
    struct element* p;
    p = begin->next;
    while (p != NULL) {
        printf_s("%s\n", p->string);
        p = p->next;
    }
    free(p);
}

void deleteValue(int index) {
    if (begin != NULL) {
        struct element* p;
        p = begin->next;
        int i = 0;
        while (p != NULL) {
            if (index == i) {
                //usuwanie elementu
                if (p == begin) {
                    begin = p->next;
                }

                if (p->prev != NULL) {
                    p->prev->next = p->next;
                }

                if (p->next != NULL) {
                    p->next->prev = p->prev;
                }

                free(p->string);
                break;
            }
            p = p->next;
            i++;
        }
        free(p);
    }
}

int main() {
    int choice, first = 1;
    char input[MAX_LENGTH];

    while (1) {
        printf("\nMenu:\n");
        printf("1. Wstaw element do listy\n");
        printf("2. Wyswietl wszystkie elementy listy\n");
        printf("3. Usun element z listy\n");
        printf("4. Wyjdz\n");
        printf("Wybierz opcje: ");
        scanf_s("%d", &choice);

        switch (choice) {
        case 1:
            printf("Podaj tekst do wstawienia: ");
            scanf_s("%s", input, sizeof(input));
            int dot = sizeof(input);
            if (first == 1) {
                firstCreate();
                first++;
            }
            begin->next = insert(input, begin); // adres ptr

            break;
        case 2:
            display();
            break;
        case 3:
            printf("Podaj indeks elementu do usunięcia: ");
            int index;
            scanf_s("%d", &index);
            deleteValue(index);
            break;
        case 4:
            exit(0);
        default:
            printf("Niepoprawny wybór. Wybierz ponownie.\n");
        }
    }

    return 0;
}
