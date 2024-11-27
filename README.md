# 🚍🚖 Public Transport Route and Schedule Management System

Welcome to the Public Transport Route and Schedule Management System! This project helps manage public transport routes and schedules efficiently.

## 📋 Table of Contents

- [Getting Started](#getting-started)
- [Compiling and Running](#compiling-and-running)
- [Features](#features)
- [Team Members](#team-members)
- [Contributing](#contributing)
- [License](#license)

## 🏁 Getting Started

To get started with the project, you need to clone the repository or download it to your local machine. Ensure you have 'MAKE' installed to use the Makefile. If you're using Ubuntu and don't have MAKE installed, follow these steps to install it:

### 1. Install MAKE in Ubuntu

```bash
sudo apt update && sudo apt install build-essential
```

Once MAKE is installed, follow these steps to clone the repository and navigate to the project directory:

### 2. Clone the Repository

```bash
git clone https://github.com/Sushmit-Biswas/Public_Transport_Manager.git
```

### 3. Navigate to the Project Directory

```bash
cd Public_Transport_Manager
```

## 🚀 Compiling and Running

Follow these steps to compile and run the project in Ubuntu:

### 1. **Compile the Project**
```bash
make
```

### 2. **Run the Project**
```bash
bin/transport
```

### 3. **Clean Compiled Files**
```bash
make clean
```

## 🛠️ Features

### Admin Features
- **Route Management**: Add and delete routes
- **Schedule Management**: Add and delete schedules for routes
- **Display Routes**: View all available routes
- **Display Schedules**: View all schedules
- **Update Trip Fares**: Update fares for trips
- **Create Admin Accounts**: Create new admin accounts
- **View Bookings**: View all client bookings
- **View Admin Logs**: View logs of admin activities
- **Manage Vehicle Capacity**: Update vehicle capacity for routes
- **Manage Vehicle Availability**: Update vehicle availability for routes

### Client Features
- **Route Display**: View all available routes
- **Route Filtering**: Filter routes by location and vehicle type (AC/Non-AC Bus/Taxi)
- **Schedule Display**: View schedules for routes
- **Trip Management**: Book and cancel trips
- **Booking History**: View personal booking history

### General Features
- **Authentication**: Login system for admins and clients
- **Activity Logging**: Logging of admin actions for auditing
- **User Interface**: User-friendly interface with ANSI color formatting
- **Error Handling**: Robust error handling for user input and system operations

## 👨🏻👩🏻 Team Members
| Member | Contributions |
| --- | --- |
| Sushmit Biswas | auth.h, auth.c |
| Parth Sutaria | routes.h, routes.c |
| Yashwant Kadire | schedule.h, schedule.c |
| Shive S Bhat | README.md, Makefile |
| Hasini Samudrala | bookings.h, bookings.c |
| Perla Brinda | main.c, CONTRIBUTING.md |


## 🤝 Contributing

We welcome contributions to improve the project. Please see our [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines on how to submit improvements and bug fixes.

## 📝 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

_**Thank you**_ 🤍 _**for being so interested in the Public Transport Route and Schedule Management System! We hope it proves to be a valuable tool for your public transport needs.**_
