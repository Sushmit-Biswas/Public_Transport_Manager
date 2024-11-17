#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include "auth.h"

#define ADMIN_USERNAME "Alpha"
#define ADMIN_PASSWORD "Noob@iiitb"
#define MAX_USERNAME 50
#define MAX_PASSWORD 50

// Helper function to get password without echoing
void get_password(char *password, int max_len) {
    struct termios old_term, new_term;
    int i = 0;
    char ch;

    // Get the terminal settings
    if (tcgetattr(STDIN_FILENO, &old_term) != 0) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }
    new_term = old_term;

    // Turn off echo
    new_term.c_lflag &= ~(ECHO | ICANON);
    if (tcsetattr(STDIN_FILENO, TCSANOW, &new_term) != 0) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }

    while ((ch = getchar()) != '\n' && i < max_len - 1) {
        if (ch == 127 || ch == 8) {  // Handle backspace
            if (i > 0) {
                i--;
                printf("\b \b");
            }
        } else {
            password[i++] = ch;
            printf("*");
        }
    }
    password[i] = '\0';
    printf("\n");

    // Restore terminal settings
    if (tcsetattr(STDIN_FILENO, TCSANOW, &old_term) != 0) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}

int authenticate() {
    int choice;
    printf("\nWelcome to Transport Management System\n");
    printf("1. Admin Login\n");
    printf("2. Client Portal\n");
    printf("Enter your choice (1-2): ");
    if (scanf("%d", &choice) != 1) {
        printf("Invalid input!\n");
        return 0;
    }
    getchar(); // Consume newline

    switch(choice) {
        case 1:
            return admin_login();
        case 2: {
            int client_choice;
            printf("\nClient Portal\n");
            printf("1. Login\n");
            printf("2. Register\n");
            printf("Enter your choice (1-2): ");
            if (scanf("%d", &client_choice) != 1) {
                printf("Invalid input!\n");
                return 0;
            }
            getchar(); // Consume newline
            
            if (client_choice == 1) {
                return client_login();
            } else if (client_choice == 2) {
                client_register();
                return client_login();
            }
            break;
        }
        default:
            printf("Invalid choice!\n");
            return 0;
    }
    return 0;
}

int admin_login() {
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];

    printf("\nAdmin Login\n");
    printf("Username: ");
    if (fgets(username, MAX_USERNAME, stdin) == NULL) {
        printf("Error reading username!\n");
        return 0;
    }
    username[strcspn(username, "\n")] = 0; // Remove newline
    
    printf("Password: ");
    get_password(password, MAX_PASSWORD);

    if (strcmp(username, ADMIN_USERNAME) == 0 && strcmp(password, ADMIN_PASSWORD) == 0) {
        printf("Admin login successful!\n");
        return 1;
    } else {
        printf("Invalid credentials!\n");
        return 0;
    }
}

int client_login() {
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];

    printf("\nClient Login\n");
    printf("Username: ");
    if (fgets(username, MAX_USERNAME, stdin) == NULL) {
        printf("Error reading username!\n");
        return 0;
    }
    username[strcspn(username, "\n")] = 0; // Remove newline
    
    printf("Password: ");
    get_password(password, MAX_PASSWORD);

    return validate_client(username, password);
}

void client_register() {
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];

    printf("\nClient Registration\n");
    printf("Enter username: ");
    if (fgets(username, MAX_USERNAME, stdin) == NULL) {
        printf("Error reading username!\n");
        return;
    }
    username[strcspn(username, "\n")] = 0; // Remove newline
    
    printf("Enter password: ");
    get_password(password, MAX_PASSWORD);

    save_client(username, password);
    printf("Registration successful!\n");
}

int validate_client(char *username, char *password) {
    FILE *fp = fopen("clients.txt", "r");
    if (fp == NULL) {
        perror("Error opening database");
        return 0;
    }

    char stored_username[MAX_USERNAME];
    char stored_password[MAX_PASSWORD];
    
    while (fscanf(fp, "%s %s", stored_username, stored_password) == 2) {
        if (strcmp(username, stored_username) == 0 && strcmp(password, stored_password) == 0) {
            fclose(fp);
            printf("Client login successful!\n");
            return 1;
        }
    }

    fclose(fp);
    printf("Invalid credentials!\n");
    return 0;
}

void save_client(char *username, char *password) {
    FILE *fp = fopen("clients.txt", "a");
    if (fp == NULL) {
        perror("Error opening database");
        return;
    }

    fprintf(fp, "%s %s\n", username, password);
    fclose(fp);
}
