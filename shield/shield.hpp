#pragma once
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>

#define PATH_MAX 4096
class shield {
public:
    shield();
    ~shield();

    void start();

private:
    void check_root();
    void copy_to_bin();
    void deamonize();
    void kill_oldes();
};