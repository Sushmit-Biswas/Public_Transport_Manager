#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include "auth.h"

// ANSI color codes
#define RESET   "\x1B[0m"
#define RED     "\x1B[31m"
#define GREEN   "\x1B[32m"
#define YELLOW  "\x1B[33m"
#define BLUE    "\x1B[34m"
#define MAGENTA "\x1B[35m"
#define CYAN    "\x1B[36m"
#define WHITE   "\x1B[37m"
#define BOLD    "\x1B[1m"

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
    printf("\n%s%sWelcome to Bengaluru Public Transport Management System%s\n", BOLD, BLUE, RESET);
    printf("%s%s1.%s Admin Login\n", BOLD, GREEN, RESET);
    printf("%s%s2.%s Client Portal\n", BOLD, CYAN, RESET);
    printf("%s%sEnter your choice (1-2):%s ", BOLD, YELLOW, RESET);
    if (scanf("%d", &choice) != 1) {
        printf("%s%sInvalid input!%s\n", BOLD, RED, RESET);
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
                    printf("%s%sInvalid credentials! You have 1 more attempt.%s\n", BOLD, RED, RESET);
                }
            }
            printf("%s%sMaximum login attempts exceeded. Exiting...%s\n", BOLD, RED, RESET);
            exit(1);
        }
        case 2: {
            int client_choice;
            printf("\n%s%sClient Portal%s\n", BOLD, BLUE, RESET);
            printf("%s%s1.%s Login\n", BOLD, GREEN, RESET);
            printf("%s%s2.%s Register\n", BOLD, CYAN, RESET);
            printf("%s%sEnter your choice (1-2):%s ", BOLD, YELLOW, RESET);
            if (scanf("%d", &client_choice) != 1) {
                printf("%s%sInvalid input!%s\n", BOLD, RED, RESET);
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
                        printf("%s%sInvalid credentials! You have 1 more attempt.%s\n", BOLD, RED, RESET);
                    }
                }
                printf("%s%sMaximum login attempts exceeded. Exiting...%s\n", BOLD, RED, RESET);
                exit(1);
            } else if (client_choice == 2) {
                client_register();
                return client_login();
            }
            break;
        }
        default:
            printf("%s%sInvalid choice!%s\n", BOLD, RED, RESET);
            return 0;
    }
    return 0;
}

int admin_login() {
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];

    printf("\n%s%sAdmin Login%s\n", BOLD, BLUE, RESET);
    printf("%s%sUsername:%s ", BOLD, YELLOW, RESET);
    if (fgets(username, MAX_USERNAME, stdin) == NULL) {
        printf("%s%sError reading username!%s\n", BOLD, RED, RESET);
        return 0;
    }
    username[strcspn(username, "\n")] = 0;
    
    printf("%s%sPassword:%s ", BOLD, YELLOW, RESET);
    get_password(password, MAX_PASSWORD);

    if (validate_admin(username, password)) {
        // Store the username in a new variable
        current_user = strdup(username);
        return 1;
    }
    return 0;
}

int client_login() {
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];

    printf("\n%s%sClient Login%s\n", BOLD, BLUE, RESET);
    printf("%s%sUsername:%s ", BOLD, YELLOW, RESET);
    if (fgets(username, MAX_USERNAME, stdin) == NULL) {
        printf("%s%sError reading username!%s\n", BOLD, RED, RESET);
        return 0;
    }
    username[strcspn(username, "\n")] = 0;
    
    printf("%s%sPassword:%s ", BOLD, YELLOW, RESET);
    get_password(password, MAX_PASSWORD);

    return validate_client(username, password);
}

void client_register() {
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];

    printf("\n%s%sClient Registration%s\n", BOLD, BLUE, RESET);
    printf("%s%sEnter username:%s ", BOLD, YELLOW, RESET);
    if (fgets(username, MAX_USERNAME, stdin) == NULL) {
        printf("%s%sError reading username!%s\n", BOLD, RED, RESET);
        return;
    }
    username[strcspn(username, "\n")] = 0;
    
    printf("%s%sEnter password:%s ", BOLD, YELLOW, RESET);
    get_password(password, MAX_PASSWORD);

    save_client(username, password);
    printf("%s%sRegistration successful!%s\n", BOLD, GREEN, RESET);
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
            printf("%s%sAdmin login successful!%s\n", BOLD, GREEN, RESET);
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
            printf("%s%sClient login successful!%s\n", BOLD, GREEN, RESET);
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

    printf("\n%s%sCreate New Admin Account%s\n", BOLD, BLUE, RESET);
    printf("%s%sEnter new admin username:%s ", BOLD, YELLOW, RESET);
    scanf("%s", new_username);
    getchar();

    printf("%s%sEnter password:%s ", BOLD, YELLOW, RESET);
    get_password(new_password, MAX_PASSWORD);

    save_admin(new_username, new_password);
    printf("%s%sNew admin account created successfully!%s\n", BOLD, GREEN, RESET);
}
