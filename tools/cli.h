#pragma once

#include <iostream>
#include <string>

#include "../entities/node.h"


class CLI {
public:
    CLI();
    void run();

private:
    Node* node;
    void display_help();
};

