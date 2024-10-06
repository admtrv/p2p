/*
 *  cli.h
 */

#pragma once

#include <iostream>
#include <string>

#include "../entities/node.h"


class CLI {
public:
    CLI(Node* node);
    ~CLI();

    void run();

private:
    Node* node;
    void display_help();
};

