#include <stdio.h>      // For input and output
#include <string.h>     // For string manipulation
#include <stdlib.h>     // For exit function
#include <termios.h>    // For password input (works on Linux)
#include <unistd.h>     // For getchar function     
#include "auth.h"       // For authentication functions
#include <ctype.h>      // For character checking

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
#define MIN_PASSWORD_LENGTH 8

// Helper function to validate password strength
int validate_password_strength(const char *password) {
    int has_upper = 0, has_lower = 0, has_digit = 0, has_special = 0;
    size_t len = strlen(password);

    if (len < MIN_PASSWORD_LENGTH) {
        return 0;
    }

    for (size_t i = 0; i < len; i++) {
        if (isupper(password[i])) has_upper = 1;
        else if (islower(password[i])) has_lower = 1;
        else if (isdigit(password[i])) has_digit = 1;
        else if (strchr("@#$%^&*!?", password[i])) has_special = 1;
    }

    return has_upper && has_lower && has_digit && has_special;
}

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

// Function to authenticate user
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

// Function to handle admin login
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

// Function to handle client login
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

// Function to handle client registration
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
    
    do {
        printf("%s%sEnter password (min 8 chars, must include uppercase, lowercase, number, and special char):%s ", BOLD, YELLOW, RESET);
        get_password(password, MAX_PASSWORD);
        
        if (!validate_password_strength(password)) {
            printf("%s%sPassword is too weak! Please try again.%s\n", BOLD, RED, RESET);
        }
    } while (!validate_password_strength(password));

    save_client(username, password);
    printf("%s%sRegistration successful!%s\n", BOLD, GREEN, RESET);
}

// Function to validate admin credentials
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

// Function to validate client credentials
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

// Function to save client credentials
void save_client(char *username, char *password) {
    FILE *fp = fopen("clients.txt", "a");
    if (fp == NULL) {
        perror("Error opening client database");
        return;
    }

    fprintf(fp, "%s %s\n", username, password);
    fclose(fp);
}

// Function to save admin credentials
void save_admin(char *username, char *password) {
    FILE *fp = fopen("admin.txt", "a");
    if (fp == NULL) {
        perror("Error opening admin database");
        return;
    }

    fprintf(fp, "%s %s\n", username, password);
    fclose(fp);
}

// Function to create a new admin account
void create_admin(char* admin_username) {
    char new_username[MAX_USERNAME];
    char new_password[MAX_PASSWORD];

    printf("\n%s%sCreate New Admin Account%s\n", BOLD, BLUE, RESET);
    printf("%s%sEnter new admin username:%s ", BOLD, YELLOW, RESET);
    scanf("%s", new_username);
    getchar();

    do {
        printf("%s%sEnter password (min 8 chars, must include uppercase, lowercase, number, and special char):%s ", BOLD, YELLOW, RESET);
        get_password(new_password, MAX_PASSWORD);
        
        if (!validate_password_strength(new_password)) {
            printf("%s%sPassword is too weak! Please try again.%s\n", BOLD, RED, RESET);
        }
    } while (!validate_password_strength(new_password));

    save_admin(new_username, new_password);
    printf("%s%sNew admin account created successfully!%s\n", BOLD, GREEN, RESET);
}
