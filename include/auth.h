#ifndef AUTH_H
#define AUTH_H

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_USERNAME 50
#define MAX_PASSWORD 50
#define MAX_ATTEMPTS 2

// Function declarations for authentication
int authenticate(void);
int admin_login(void);
int client_login(void);
void client_register(void);
int validate_client(char *username, char *password);
void save_client(char *username, char *password);
void create_admin(char *admin_username); // Only logged in admin can create new admin
int validate_admin(char *username, char *password);
void save_admin(char *username, char *password);
void load_admins_from_file(void);
void save_admins_to_file(void);
int get_login_attempts(void);
void reset_login_attempts(void);

// Global variables
extern char* current_user;
extern int is_admin;

#ifdef __cplusplus
}
#endif

#endif // AUTH_H
