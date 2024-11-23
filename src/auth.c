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

    // Check if password length is less than minimum length
    if (len < MIN_PASSWORD_LENGTH) {
        return 0;
    }

    // ctype.h functions to check for uppercase, lowercase, digit, and special character
    // Check if password contains at least one uppercase letter, one lowercase letter, one digit, and one special character
    for (size_t i = 0; i < len; i++) {
        if (isupper(password[i])) has_upper = 1;
        else if (islower(password[i])) has_lower = 1;
        else if (isdigit(password[i])) has_digit = 1;
        else if (strchr("@#$%^&*!?", password[i])) has_special = 1;
    }

    // Return true if all conditions are met
    return has_upper && has_lower && has_digit && has_special;
}

// Helper function to get password without echoing
void get_password(char *password, int max_len) {
    struct termios old_term, new_term; // termios structure to store terminal attributes
    int i = 0; // index for password array
    char ch; // character read from stdin

    // Get terminal attributes
    if (tcgetattr(STDIN_FILENO, &old_term) != 0) { // get current terminal attributes
        perror("tcgetattr"); // print error message if tcgetattr fails
        exit(EXIT_FAILURE); // exit program with failure status
    }
    new_term = old_term; // copy current terminal attributes to new_term

    // Disable echo and canonical mode
    new_term.c_lflag &= ~(ECHO | ICANON);
    if (tcsetattr(STDIN_FILENO, TCSANOW, &new_term) != 0) { // set new terminal attributes
        perror("tcsetattr"); // print error message if tcsetattr fails
        exit(EXIT_FAILURE); // exit program with failure status
    }

    // Read characters until newline or max length is reached
    while ((ch = getchar()) != '\n' && i < max_len - 1) {
        if (ch == 127 || ch == 8) { // 127 is ASCII code for delete, 8 is ASCII code for backspace
            if (i > 0) {
                i--; // decrement index if backspace/delete is pressed
                printf("\b \b"); // move cursor back, print space, move cursor back again to overwrite the space with backspace character
            }
        } else {
            password[i++] = ch; // increment index and store character in password array
            printf("*"); // print asterisk to indicate character input
        }
    }
    password[i] = '\0'; // null-terminate the password string
    printf("\n"); // print newline after password input

    // Restore terminal attributes
    if (tcsetattr(STDIN_FILENO, TCSANOW, &old_term) != 0) {
        perror("tcsetattr"); // print error message if tcsetattr fails
        exit(EXIT_FAILURE); // exit program with failure status
    }
}

// Function to authenticate user
int authenticate() {
    int choice; // choice of user
    printf("\n%s%sWelcome to Bengaluru Public Transport Management System%s\n", BOLD, BLUE, RESET);
    printf("%s%s1.%s Admin Login\n", BOLD, GREEN, RESET);
    printf("%s%s2.%s Client Portal\n", BOLD, CYAN, RESET);
    printf("%s%sEnter your choice (1-2):%s ", BOLD, YELLOW, RESET);
    if (scanf("%d", &choice) != 1) { // check if input is an integer
        printf("%s%sInvalid input!%s\n", BOLD, RED, RESET);
        return 0;
    }
    getchar(); // consume newline character

    switch(choice) {
        case 1: {
            int attempts = 0; // number of attempts
            while (attempts < MAX_ATTEMPTS) {
                if (admin_login()) { // check if admin login is successful
                    return 1; // return 1 if admin login is successful
                }
                attempts++; // increment attempts
                if (attempts < MAX_ATTEMPTS) { // check if attempts are less than maximum attempts
                    printf("%s%sInvalid credentials! You have 1 more attempt.%s\n", BOLD, RED, RESET);
                }
            }
            printf("%s%sMaximum login attempts exceeded. Exiting...%s\n", BOLD, RED, RESET);
            exit(1); // exit program with failure status
        }
        case 2: {
            int client_choice; // choice of client
            printf("\n%s%sClient Portal%s\n", BOLD, BLUE, RESET);
            printf("%s%s1.%s Login\n", BOLD, GREEN, RESET);
            printf("%s%s2.%s Register\n", BOLD, CYAN, RESET);
            printf("%s%sEnter your choice (1-2):%s ", BOLD, YELLOW, RESET);
            if (scanf("%d", &client_choice) != 1) { // check if input is an integer
                printf("%s%sInvalid input!%s\n", BOLD, RED, RESET);
                return 0;
            }
            getchar(); // consume newline character
            
            if (client_choice == 1) { // check if client choice is 1
                int attempts = 0; // number of attempts
                while (attempts < MAX_ATTEMPTS) {
                    if (client_login()) { // check if client login is successful
                        return 2;
                    }
                    attempts++; // increment attempts
                    if (attempts < MAX_ATTEMPTS) { // check if attempts are less than maximum attempts
                        printf("%s%sInvalid credentials! You have 1 more attempt.%s\n", BOLD, RED, RESET);
                    }
                }
                printf("%s%sMaximum login attempts exceeded. Exiting...%s\n", BOLD, RED, RESET);
                exit(1); // exit program with failure status
            } else if (client_choice == 2) { // check if client choice is 2
                client_register(); // register client
                return client_login(); // login client  
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
    if (fgets(username, MAX_USERNAME, stdin) == NULL) { // check if fgets fails
        printf("%s%sError reading username!%s\n", BOLD, RED, RESET);
        return 0;
    }
    username[strcspn(username, "\n")] = 0;
    
    printf("%s%sPassword:%s ", BOLD, YELLOW, RESET);
    get_password(password, MAX_PASSWORD);

    if (validate_admin(username, password)) {
        // Store the username in a new variable
        current_user = strdup(username); // store the username in a new variable
        return 1; // return 1 if admin login is successful
    }
    return 0; // return 0 if admin login is unsuccessful
}

// Function to handle client login
int client_login() {
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];

    printf("\n%s%sClient Login%s\n", BOLD, BLUE, RESET);
    printf("%s%sUsername:%s ", BOLD, YELLOW, RESET);
    if (fgets(username, MAX_USERNAME, stdin) == NULL) { // check if fgets fails
        printf("%s%sError reading username!%s\n", BOLD, RED, RESET);
        return 0;
    }
    username[strcspn(username, "\n")] = 0;
    
    printf("%s%sPassword:%s ", BOLD, YELLOW, RESET);
    get_password(password, MAX_PASSWORD);

    return validate_client(username, password); // return 1 if client login is successful, 0 if unsuccessful
}

// Function to handle client registration
void client_register() {
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];

    printf("\n%s%sClient Registration%s\n", BOLD, BLUE, RESET);
    printf("%s%sEnter username:%s ", BOLD, YELLOW, RESET);
    if (fgets(username, MAX_USERNAME, stdin) == NULL) {
        printf("%s%sError reading username!%s\n", BOLD, RED, RESET);
        return; // return if fgets fails
    }
    username[strcspn(username, "\n")] = 0;
    
    do {
        printf("%s%sEnter password (min 8 chars, must include uppercase, lowercase, number, and special char):%s ", BOLD, YELLOW, RESET);
        get_password(password, MAX_PASSWORD); // get password without echoing

        if (!validate_password_strength(password)) { // check if password is strong
            printf("%s%sPassword is too weak! Please try again.%s\n", BOLD, RED, RESET);
        }
    } while (!validate_password_strength(password)); // check if password is strong

    save_client(username, password); // save client credentials
    printf("%s%sRegistration successful!%s\n", BOLD, GREEN, RESET);
}

// Function to validate admin credentials
int validate_admin(char *username, char *password) {
    FILE *fp = fopen("admin.txt", "r"); // open admin database
    if (fp == NULL) {
        perror("Error opening admin database"); // print error message if fopen fails
        return 0;
    }

    char stored_username[MAX_USERNAME];
    char stored_password[MAX_PASSWORD];
    
    while (fscanf(fp, "%s %s", stored_username, stored_password) == 2) { // read username and password from admin database
        if (strcmp(username, stored_username) == 0 && strcmp(password, stored_password) == 0) {
            fclose(fp);
            printf("%s%sAdmin login successful!%s\n", BOLD, GREEN, RESET);
            return 1;
        }
    }

    fclose(fp); // close admin database
    return 0; // return 0 if admin login is unsuccessful
}

// Function to validate client credentials
int validate_client(char *username, char *password) {
    FILE *fp = fopen("clients.txt", "r"); // open client database
    if (fp == NULL) {
        perror("Error opening client database"); // print error message if fopen fails
        return 0;
    }

    char stored_username[MAX_USERNAME];
    char stored_password[MAX_PASSWORD];
    
    while (fscanf(fp, "%s %s", stored_username, stored_password) == 2) { // read username and password from client database
        if (strcmp(username, stored_username) == 0 && strcmp(password, stored_password) == 0) { // check if username and password match
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
    FILE *fp = fopen("clients.txt", "a"); // open client database
    if (fp == NULL) {
        perror("Error opening client database"); // print error message if fopen fails
        return;
    }

    fprintf(fp, "%s %s\n", username, password);
    fclose(fp);
}

// Function to save admin credentials
void save_admin(char *username, char *password) {
    FILE *fp = fopen("admin.txt", "a"); // open admin database
    if (fp == NULL) {
        perror("Error opening admin database"); // print error message if fopen fails
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
        get_password(new_password, MAX_PASSWORD); // get password without echoing

        if (!validate_password_strength(new_password)) { // check if password is strong
            printf("%s%sPassword is too weak! Please try again.%s\n", BOLD, RED, RESET);
        }
    } while (!validate_password_strength(new_password)); // check if password is strong

    save_admin(new_username, new_password); // save admin credentials
    printf("%s%sNew admin account created successfully!%s\n", BOLD, GREEN, RESET);
}
