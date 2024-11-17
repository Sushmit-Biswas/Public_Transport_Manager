#include <stdio.h>
#include "routes.h"
#include "schedule.h"
#include "auth.h"

void display_menu() {
    printf("Public Transport Route and Schedule Management\n");
    printf("1. Add Route\n");
    printf("2. Delete Route\n");
    printf("3. Display Routes\n");
    printf("4. Add Schedule\n");
    printf("5. Delete Schedule\n");
    printf("6. Display Schedules\n");
    printf("7. Exit\n");
}

int main() {
    if (!authenticate()) {
        printf("Authentication failed. Exiting...\n");
        return 1;
    }

    int choice;
    while (1) {
        display_menu();
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input! Please enter a number.\n");
            while (getchar() != '\n'); // Clear input buffer
            continue;
        }
        switch (choice) {
            case 1:
                add_route();
                break;
            case 2:
                delete_route();
                break;
            case 3:
                display_routes();
                break;
            case 4:
                add_schedule();
                break;
            case 5:
                delete_schedule();
                break;
            case 6:
                display_schedules();
                break;
            case 7:
                return 0;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
    return 0;
}