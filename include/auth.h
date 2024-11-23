#ifndef AUTH_H              // If AUTH_H is not defined (not used in this project)
// Preprocessor directive to prevent multiple inclusions of header file
// AUTH_H is defined in auth.h file

#define AUTH_H             // Define AUTH_H macro

#ifdef __cplusplus          // If C++ is being used (not used in this project)
extern "C" {                // Extern "C" block (not used in this project)  
#endif                    // End of extern "C" block


#define MAX_USERNAME 50     // Define maximum username length
#define MAX_PASSWORD 50     // Define maximum password length
#define MAX_ATTEMPTS 2      // Define maximum login attempts

// Function declarations for authentication
int authenticate(void);                              // Function to authenticate user
int admin_login(void);                               // Function to handle admin login
int client_login(void);                              // Function to handle client login
void client_register(void);                          // Function to handle client registration
int validate_client(char *username, char *password); // Function to validate client credentials
void save_client(char *username, char *password);    // Function to save client credentials
void create_admin(char *admin_username);             // Only logged in admin can create new admin
int validate_admin(char *username, char *password);  // Function to validate admin credentials
void save_admin(char *username, char *password);     // Function to save admin credentials
void load_admins_from_file(void);                    // Function to load admins from file
void save_admins_to_file(void);                      // Function to save admins to file
int get_login_attempts(void);                        // Function to get login attempts
void reset_login_attempts(void);                     // Function to reset login attempts

// Global variables
extern char* current_user;  // Global variable for current user
extern int is_admin;        // Global variable for admin status

#ifdef __cplusplus          // If C++ is being used (not used in this project)
}                           // End of extern "C" block
#endif                      // End of ifdef __cplusplus

#endif // AUTH_H // End of AUTH_H macro
