#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <iostream>
#include <thread>
#include <vector>
#include "TextProcessing.h"

#pragma comment(lib, "ws2_32.lib")

namespace NetworkCommon
{
    // Default port numbers
    constexpr int DEFAULT_TCP_PORT = 8888;
    constexpr int DEFAULT_UDP_PORT = 8889;

    // Maximum buffer sizes
    constexpr int MAX_BUFFER_SIZE = 8192;
    constexpr int MAX_TEXT_SIZE = 4096;

    // Request structure for text processing
    struct TextRequest
    {
        TextProcessing::FunctionCode functionCode;
        char text[MAX_TEXT_SIZE];

        // Additional parameters for specific functions
        char param1[256]; // For functions requiring additional parameters like replaceWord
        char param2[256]; // For functions requiring additional parameters like replaceWord
        int intParam1;    // For functions like extractSubstring (start position)
        int intParam2;    // For functions like extractSubstring (length)
    };

    // Response structure for text processing results
    struct TextResponse
    {
        bool success;
        char result[MAX_TEXT_SIZE];
        int intResult; // For functions that return integers
    };

    // Utility functions
    bool initializeWinsock();
    void cleanupWinsock();
    std::string getLastErrorAsString();
}