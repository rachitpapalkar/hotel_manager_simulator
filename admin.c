#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

typedef struct {
    int termination_flag;
    int busy_flag;
} TerminationData;

int main() {
    key_t key = ftok("admin.c", 'a');
    if (key == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    int termination_shmid = shmget(key, sizeof(TerminationData), IPC_CREAT | 0666);
    if (termination_shmid == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    TerminationData *termination_data = (TerminationData *)shmat(termination_shmid, NULL, 0);
    if (termination_data == (void *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    char choice;
    do {
        printf("\nDo you want to close the hotel? Enter Y for Yes and N for No: ");
        scanf(" %c", &choice);

        if (choice == 'Y' || choice == 'y') {
            termination_data->termination_flag = 1;
            printf("\nChecking if all tables are free... \n");
            sleep(3);
            if (termination_data->busy_flag == 1) {
                printf("Tables occupied, can't close.\n");
                termination_data->termination_flag = 0;
                continue;
            } else {
                printf("\n------------Hotel Closed------------\n\n");
                break;
            }
        } else if (choice == 'N' || choice == 'n') {
            continue;
        } else {
            printf("Invalid input. Please enter Y or N.\n");
        }
    } while (1);

    if (shmdt(termination_data) == -1) {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }

    return 0;
}

