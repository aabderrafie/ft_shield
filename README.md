ft_shield
Overview
ft_shield is a project that involves creating a simple Trojan Horse. This binary, running with root privileges, will create a stealthy background process that acts as a daemon, allowing remote shell access while remaining undetectable.

Features
Mandatory Part
Root Execution: The program only runs with root privileges.

Stealth Installation: ft_shield creates a duplicate binary in a system-critical directory (/bin/ft_shield).

Daemon Process: The program runs in the background as a persistent system service.

Auto-start on Boot: The program ensures execution upon system restart.

Network Backdoor:

Listens on port 4242.

Accepts up to three simultaneous client connections.

Requires secure authentication (no hardcoded plaintext passwords).

Remote Root Shell Access:

Once authenticated, grants root shell access.

Shell access is hidden (cannot be accessed via direct commands).

Silent Execution: No error messages should appear during installation or usage.

Bonus Features
Activity Logging: Track I/O data from connected clients.

Enhanced Stealth: Methods to disguise the Trojan (e.g., process name manipulation, packing techniques).

Optimized Evasion: Make detection more challenging for security tools.

Usage Example
sh
Copy
Edit
# Run the binary
sudo ./shield
After execution:

A copy of ft_shield appears in /bin/.

A hidden daemon starts, listening on port 4242.

To connect:

sh
Copy
Edit
nc localhost 4242
Authenticate using a secure password, then access the hidden shell.

To verify its presence:

sh
Copy
Edit
netstat -tulnp | grep 4242
Installation & Setup
Clone the Repository

sh
Copy
Edit
git clone https://github.com/aabderrafie/ft_shield.git
cd ft_shield
cd shield
Compile the Program

sh
Copy
Edit
make
Run ft_shield

sh
Copy
Edit
./_shield
Verify Daemon is Running

sh
Copy
Edit
sudo systemctl status shield
Connect via Netcat

sh
Copy
Edit
nc localhost 4242
Notes
The project must be developed in C or Assembly.

The grading will be conducted in a Debian 7.0 (64-bit) virtual machine.

No external libraries can be used for core functionality.

Disclaimer
This project is for educational purposes only. Unauthorized use or deployment on systems without permission is illegal.

License
This project is licensed under the MIT License. See the LICENSE file for details.

