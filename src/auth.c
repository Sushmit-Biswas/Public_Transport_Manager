#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include "auth.h"

#define MAX_USERNAME 50
#define MAX_PASSWORD 50
#define MAX_ATTEMPTS 2

// Helper function to get password without echoing
void get_password(char *password, int max_len) {
    struct termios old_term, new_term;
    int i = 0;
    char ch;

    if (tcgetattr(STDIN_FILENO, &old_term) != 0) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }
    new_term = old_term;

    new_term.c_lflag &= ~(ECHO | ICANON);
    if (tcsetattr(STDIN_FILENO, TCSANOW, &new_term) != 0) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }

    while ((ch = getchar()) != '\n' && i < max_len - 1) {
        if (ch == 127 || ch == 8) {
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

    if (tcsetattr(STDIN_FILENO, TCSANOW, &old_term) != 0) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}

int authenticate() {
    int choice;
    printf("\nWelcome to Bengaluru Public Transport Management System\n");
    printf("1. Admin Login\n");
    printf("2. Client Portal\n");
    printf("Enter your choice (1-2): ");
    if (scanf("%d", &choice) != 1) {
        printf("Invalid input!\n");
        return 0;
    }
    getchar();

    switch(choice) {
        case 1: {
            int attempts = 0;
            while (attempts < MAX_ATTEMPTS) {
                if (admin_login()) {
                    return 1;
                }
                attempts++;
                if (attempts < MAX_ATTEMPTS) {
                    printf("Invalid credentials! You have 1 more attempt.\n");
                }
            }
            printf("Maximum login attempts exceeded. Exiting...\n");
            exit(1);
        }
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
            getchar();
            
            if (client_choice == 1) {
                int attempts = 0;
                while (attempts < MAX_ATTEMPTS) {
                    if (client_login()) {
                        return 2;
                    }
                    attempts++;
                    if (attempts < MAX_ATTEMPTS) {
                        printf("Invalid credentials! You have 1 more attempt.\n");
                    }
                }
                printf("Maximum login attempts exceeded. Exiting...\n");
                exit(1);
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
    username[strcspn(username, "\n")] = 0;
    
    printf("Password: ");
    get_password(password, MAX_PASSWORD);

    return validate_admin(username, password);
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
    username[strcspn(username, "\n")] = 0;
    
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
    username[strcspn(username, "\n")] = 0;
    
    printf("Enter password: ");
    get_password(password, MAX_PASSWORD);

    save_client(username, password);
    printf("Registration successful!\n");
}

int validate_admin(char *username, char *password) {
    FILE *fp = fopen("admin.txt", "r");
    if (fp == NULL) {
        perror("Error opening admin database");
        return 0;
    }

    char stored_username[MAX_USERNAME];
    char stored_password[MAX_PASSWORD];
    
    while (fscanf(fp, "%s %s", stored_username, stored_password) == 2) {
        if (strcmp(username, stored_username) == 0 && strcmp(password, stored_password) == 0) {
            fclose(fp);
            printf("Admin login successful!\n");
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

int validate_client(char *username, char *password) {
    FILE *fp = fopen("clients.txt", "r");
    if (fp == NULL) {
        perror("Error opening client database");
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
    return 0;
}

void save_client(char *username, char *password) {
    FILE *fp = fopen("clients.txt", "a");
    if (fp == NULL) {
        perror("Error opening client database");
        return;
    }

    fprintf(fp, "%s %s\n", username, password);
    fclose(fp);
}

void save_admin(char *username, char *password) {
    FILE *fp = fopen("admin.txt", "a");
    if (fp == NULL) {
        perror("Error opening admin database");
        return;
    }

    fprintf(fp, "%s %s\n", username, password);
    fclose(fp);
}

void create_admin(char* admin_username) {
    char new_username[MAX_USERNAME];
    char new_password[MAX_PASSWORD];

    printf("\nCreate New Admin Account\n");
    printf("Enter new admin username: ");
    scanf("%s", new_username);
    getchar();

    printf("Enter password: ");
    get_password(new_password, MAX_PASSWORD);

    save_admin(new_username, new_password);
    printf("New admin account created successfully!\n");
}
