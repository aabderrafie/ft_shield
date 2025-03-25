#include "shield.hpp"
#include "server.hpp" // Include the Server class header

shield::shield() {
    // std::cout << "Shield constructor" << std::endl;
}

shield::~shield() {
    // std::cout << "Shield destructor" << std::endl;
}

void shield::check_root() {
    if (getuid() != 0)
        throw std::runtime_error("You must be root to run this program.");
}
void shield::copy_to_bin() {
    char exe_path[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    exe_path[len] = '\0';

    std::string copy_command = std::string("cp -f ") + exe_path + " /bin/shield > /dev/null 2>&1";
    system(copy_command.c_str());

    chmod("/bin/shield", 0755);

    // Create systemd service file
    std::ofstream serviceFile("/etc/systemd/system/shield.service");
    if (!serviceFile) {
        throw std::runtime_error("Failed to create service file");
    }

    serviceFile << R"(
[Unit]
Description=Shield Daemon
After=network.target

[Service]
Type=forking
ExecStart=/bin/shield
Restart=always
RestartSec=5s
User=root
PIDFile=/run/shield.pid

[Install]
WantedBy=multi-user.target
)";

    serviceFile.close();
    
    system("systemctl daemon-reload");
    system("systemctl enable  shield.service");
}


void shield::deamonize() {
    pid_t pid = fork();
    if (pid < 0) throw std::runtime_error("Fork failed");
    if (pid > 0) exit(0); // Exit the parent process

    if (setsid() < 0) throw std::runtime_error("Setsid failed");

    if (chdir("/") < 0) throw std::runtime_error("Failed to change working directory");

    // Open /dev/null for reading and writing
    int fd = open("/dev/null", O_RDWR);
    if (fd < 0) throw std::runtime_error("Failed to open /dev/null");

    // Redirect standard file descriptors to /dev/null
    dup2(fd, STDIN_FILENO);
    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);


    close(fd);
}

void shield::kill_oldes() {

    FILE* pipe = popen("pgrep -f shield", "r");
    char buffer[128];
    pid_t current_pid = getpid();

    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        pid_t pid = atoi(buffer);
        if (pid > 0 && pid != current_pid) 
            int result = kill(pid, SIGTERM);
    }
    
    pclose(pipe);
}
void shield::start() {
    check_root(); // Ensure the program is run as root
    std::cout << "Welcome to the shield!" << std::endl;
    std::cout << "Created by: Abderrafie Askal | @aaskal" << std::endl;
        
    kill_oldes(); // Kill any existing instances of the program
    copy_to_bin(); // Copy the program to /bin/shield
    deamonize(); // Daemonize the process

    Server server;
    server.start();
}