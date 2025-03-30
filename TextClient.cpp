#include "NetworkCommon.h"
#include "TextProcessing.h"
#include <conio.h>
#include <fstream>
#include <windows.h>
#include <commdlg.h>

#pragma comment(lib, "comdlg32.lib")

// Function to display menu and get user choice
int displayMenu()
{
    system("cls");
    std::cout << "Text Processing Client" << std::endl;
    std::cout << "----------------------" << std::endl;
    std::cout << "Select a text processing function:" << std::endl;
    std::cout << "1. Count words" << std::endl;
    std::cout << "2. Count characters" << std::endl;
    std::cout << "3. Count lines" << std::endl;
    std::cout << "4. Convert to uppercase" << std::endl;
    std::cout << "5. Convert to lowercase" << std::endl;
    std::cout << "6. Replace words" << std::endl;
    std::cout << "7. Extract substring" << std::endl;
    std::cout << "8. Reverse text" << std::endl;
    std::cout << "9. Remove punctuation" << std::endl;
    std::cout << "10. Word frequency analysis" << std::endl;
    std::cout << "11. Remove extra spaces" << std::endl;
    std::cout << "12. Count sentences" << std::endl;
    std::cout << "0. Exit" << std::endl;
    std::cout << "Enter your choice: ";

    int choice;
    std::cin >> choice;
    std::cin.ignore(); // Clear the newline character
    return choice;
}

// Function to get file path using Windows file dialog
std::string getFilePathDialog()
{
    OPENFILENAMEA ofn;
    char szFile[260] = {0};

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "Text Files\0*.txt\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileNameA(&ofn) == TRUE)
    {
        return std::string(ofn.lpstrFile);
    }

    return "";
}

// Function to get user input for text
std::string getTextInput()
{
    std::cout << "Choose input method:" << std::endl;
    std::cout << "1. Manual input" << std::endl;
    std::cout << "2. From file" << std::endl;
    std::cout << "Enter your choice: ";

    int inputChoice;
    std::cin >> inputChoice;
    std::cin.ignore(); // Clear the newline character

    std::string text;

    if (inputChoice == 1)
    {
        // Manual input
        std::cout << "Enter text (press Ctrl+Z on a new line to finish):" << std::endl;
        std::string line;

        while (std::getline(std::cin, line) && !line.empty() && line != std::string(1, 26))
        { // 26 is Ctrl+Z
            text += line + "\n";
        }
    }
    else if (inputChoice == 2)
    {
        // File input using dialog
        std::string filePath = getFilePathDialog();

        if (filePath.empty())
        {
            std::cout << "No file selected. Falling back to manual input..." << std::endl;
            return getTextInput(); // Recursive call to try again
        }

        std::cout << "Selected file: " << filePath << std::endl;

        std::ifstream file(filePath);
        if (!file.is_open())
        {
            std::cerr << "Failed to open file: " << filePath << std::endl;
            std::cout << "Falling back to manual input..." << std::endl;
            return getTextInput(); // Recursive call to try again
        }

        std::string line;
        while (std::getline(file, line))
        {
            text += line + "\n";
        }
        file.close();
        std::cout << "File loaded successfully." << std::endl;
    }
    else
    {
        std::cout << "Invalid choice. Falling back to manual input..." << std::endl;
        return getTextInput(); // Recursive call to try again
    }

    return text;
}

// Create and send a request via TCP
bool sendTcpRequest(const NetworkCommon::TextRequest &request, NetworkCommon::TextResponse &response,
                    const std::string &serverIp, int port)
{
    // Create a TCP socket
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET)
    {
        std::cerr << "Error creating TCP socket: " << NetworkCommon::getLastErrorAsString() << std::endl;
        return false;
    }

    // Setup server address
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);

    // Convert IP address from string to binary form
    inet_pton(AF_INET, serverIp.c_str(), &(serverAddr.sin_addr));

    // Connect to the server
    if (connect(clientSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cerr << "Failed to connect to server: " << NetworkCommon::getLastErrorAsString() << std::endl;
        closesocket(clientSocket);
        return false;
    }

    // Send the request
    int bytesSent = send(clientSocket, reinterpret_cast<const char *>(&request), sizeof(request), 0);
    if (bytesSent == SOCKET_ERROR)
    {
        std::cerr << "send failed: " << NetworkCommon::getLastErrorAsString() << std::endl;
        closesocket(clientSocket);
        return false;
    }

    // Receive the response
    int bytesReceived = recv(clientSocket, reinterpret_cast<char *>(&response), sizeof(response), 0);
    if (bytesReceived <= 0)
    {
        std::cerr << "recv failed: " << NetworkCommon::getLastErrorAsString() << std::endl;
        closesocket(clientSocket);
        return false;
    }

    // Close the socket
    closesocket(clientSocket);
    return true;
}

// Create and send a request via UDP
bool sendUdpRequest(const NetworkCommon::TextRequest &request, NetworkCommon::TextResponse &response,
                    const std::string &serverIp, int port)
{
    // Create a UDP socket
    SOCKET clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (clientSocket == INVALID_SOCKET)
    {
        std::cerr << "Error creating UDP socket: " << NetworkCommon::getLastErrorAsString() << std::endl;
        return false;
    }

    // Setup server address
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);

    // Convert IP address from string to binary form
    inet_pton(AF_INET, serverIp.c_str(), &(serverAddr.sin_addr));

    // Set timeout for receiving
    DWORD timeout = 5000; // 5 seconds
    if (setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout)) == SOCKET_ERROR)
    {
        std::cerr << "setsockopt failed: " << NetworkCommon::getLastErrorAsString() << std::endl;
        closesocket(clientSocket);
        return false;
    }

    // Send the request
    int bytesSent = sendto(clientSocket, reinterpret_cast<const char *>(&request), sizeof(request), 0,
                           (sockaddr *)&serverAddr, sizeof(serverAddr));
    if (bytesSent == SOCKET_ERROR)
    {
        std::cerr << "sendto failed: " << NetworkCommon::getLastErrorAsString() << std::endl;
        closesocket(clientSocket);
        return false;
    }

    // Receive the response
    sockaddr_in fromAddr;
    int fromAddrSize = sizeof(fromAddr);

    int bytesReceived = recvfrom(clientSocket, reinterpret_cast<char *>(&response), sizeof(response), 0,
                                 (sockaddr *)&fromAddr, &fromAddrSize);
    if (bytesReceived <= 0)
    {
        std::cerr << "recvfrom failed: " << NetworkCommon::getLastErrorAsString() << std::endl;
        closesocket(clientSocket);
        return false;
    }

    // Close the socket
    closesocket(clientSocket);
    return true;
}

// Fill a request structure with user input based on function choice
void fillRequest(NetworkCommon::TextRequest &request, int functionChoice)
{
    std::string text;
    std::string target;
    std::string replacement;

    request.functionCode = static_cast<TextProcessing::FunctionCode>(functionChoice);

    // Clear previous data
    memset(request.text, 0, NetworkCommon::MAX_TEXT_SIZE);
    memset(request.param1, 0, 256);
    memset(request.param2, 0, 256);
    request.intParam1 = 0;
    request.intParam2 = 0;

    // Get text for processing
    text = getTextInput();
    strncpy_s(request.text, NetworkCommon::MAX_TEXT_SIZE, text.c_str(), text.length());

    // Get additional parameters for specific functions
    switch (functionChoice)
    {
    case TextProcessing::REPLACE_WORD:
        std::cout << "Enter word to replace: ";
        std::getline(std::cin, target);
        strncpy_s(request.param1, 256, target.c_str(), target.length());

        std::cout << "Enter replacement word: ";
        std::getline(std::cin, replacement);
        strncpy_s(request.param2, 256, replacement.c_str(), replacement.length());
        break;

    case TextProcessing::EXTRACT_SUBSTRING:
        std::cout << "Enter start position: ";
        std::cin >> request.intParam1;

        std::cout << "Enter length: ";
        std::cin >> request.intParam2;

        std::cin.ignore(); // Clear the newline character
        break;
    }
}

// Process text in chunks
bool processTextInChunks(const std::string &text, int functionChoice, const std::string &serverIp, int port, bool useTcp)
{
    // Prepare parameters for special functions
    std::string target;
    std::string replacement;
    int startPos = 0;
    int length = 0;

    // Get additional parameters for specific functions if needed
    if (functionChoice == TextProcessing::REPLACE_WORD)
    {
        std::cout << "Enter word to replace: ";
        std::getline(std::cin, target);

        std::cout << "Enter replacement word: ";
        std::getline(std::cin, replacement);
    }
    else if (functionChoice == TextProcessing::EXTRACT_SUBSTRING)
    {
        std::cout << "Enter start position: ";
        std::cin >> startPos;

        std::cout << "Enter length: ";
        std::cin >> length;

        std::cin.ignore(); // Clear the newline character
    }

    // If the text fits within a single chunk, process it normally
    if (text.length() < NetworkCommon::MAX_TEXT_SIZE - 1)
    {
        NetworkCommon::TextRequest request;
        NetworkCommon::TextResponse response;

        // Set function code
        request.functionCode = static_cast<TextProcessing::FunctionCode>(functionChoice);

        // Clear request data
        memset(request.text, 0, NetworkCommon::MAX_TEXT_SIZE);
        memset(request.param1, 0, 256);
        memset(request.param2, 0, 256);
        request.intParam1 = 0;
        request.intParam2 = 0;

        // Copy text and parameters
        strncpy_s(request.text, NetworkCommon::MAX_TEXT_SIZE, text.c_str(), text.length());

        if (functionChoice == TextProcessing::REPLACE_WORD)
        {
            strncpy_s(request.param1, 256, target.c_str(), target.length());
            strncpy_s(request.param2, 256, replacement.c_str(), replacement.length());
        }
        else if (functionChoice == TextProcessing::EXTRACT_SUBSTRING)
        {
            request.intParam1 = startPos;
            request.intParam2 = length;
        }

        // Send request
        bool success = false;
        if (useTcp)
        {
            success = sendTcpRequest(request, response, serverIp, port);
        }
        else
        {
            success = sendUdpRequest(request, response, serverIp, port);
        }

        // Display result
        if (success)
        {
            std::cout << "\nServer response:" << std::endl;
            if (response.success)
            {
                std::cout << response.result << std::endl;
                if (response.intResult != 0)
                {
                    std::cout << "Result value: " << response.intResult << std::endl;
                }
            }
            else
            {
                std::cout << "Error: " << response.result << std::endl;
            }
        }
        else
        {
            std::cout << "Failed to communicate with the server!" << std::endl;
            return false;
        }
    }
    else
    {
        // Process in chunks
        std::cout << "Text is too large. Processing in chunks..." << std::endl;

        // Initialize counters/results based on function
        int totalCount = 0;
        std::string fullResult;

        const size_t chunkSize = NetworkCommon::MAX_TEXT_SIZE - 100; // Leave some buffer space
        size_t position = 0;

        // For specific functions that can't be chunked properly
        if (functionChoice == TextProcessing::EXTRACT_SUBSTRING ||
            functionChoice == TextProcessing::REVERSE_TEXT)
        {
            std::cout << "Warning: This function can't be processed in chunks effectively.\n"
                      << "Only the first chunk will be processed." << std::endl;

            // Limit processing to first chunk
            std::string firstChunk = text.substr(0, chunkSize);

            NetworkCommon::TextRequest request;
            request.functionCode = static_cast<TextProcessing::FunctionCode>(functionChoice);

            memset(request.text, 0, NetworkCommon::MAX_TEXT_SIZE);
            memset(request.param1, 0, 256);
            memset(request.param2, 0, 256);

            strncpy_s(request.text, NetworkCommon::MAX_TEXT_SIZE, firstChunk.c_str(), firstChunk.length());

            if (functionChoice == TextProcessing::EXTRACT_SUBSTRING)
            {
                request.intParam1 = startPos;
                request.intParam2 = length;
            }

            NetworkCommon::TextResponse response;
            bool success = useTcp ? sendTcpRequest(request, response, serverIp, port) : sendUdpRequest(request, response, serverIp, port);

            if (success && response.success)
            {
                std::cout << "\nResult from first chunk:" << std::endl;
                std::cout << response.result << std::endl;
                if (response.intResult != 0)
                {
                    std::cout << "Result value: " << response.intResult << std::endl;
                }
            }
            else
            {
                std::cout << "Error processing first chunk!" << std::endl;
                return false;
            }

            return true;
        }

        // Process each chunk
        while (position < text.length())
        {
            // Extract chunk
            std::string chunk = text.substr(position, chunkSize);
            std::cout << "Processing chunk " << (position / chunkSize + 1) << "..." << std::endl;

            // Prepare request
            NetworkCommon::TextRequest request;
            request.functionCode = static_cast<TextProcessing::FunctionCode>(functionChoice);

            // Clear request data
            memset(request.text, 0, NetworkCommon::MAX_TEXT_SIZE);
            memset(request.param1, 0, 256);
            memset(request.param2, 0, 256);
            request.intParam1 = 0;
            request.intParam2 = 0;

            // Copy chunk and parameters
            strncpy_s(request.text, NetworkCommon::MAX_TEXT_SIZE, chunk.c_str(), chunk.length());

            if (functionChoice == TextProcessing::REPLACE_WORD)
            {
                strncpy_s(request.param1, 256, target.c_str(), target.length());
                strncpy_s(request.param2, 256, replacement.c_str(), replacement.length());
            }

            // Send request
            NetworkCommon::TextResponse response;
            bool success = false;

            if (useTcp)
            {
                success = sendTcpRequest(request, response, serverIp, port);
            }
            else
            {
                success = sendUdpRequest(request, response, serverIp, port);
            }

            // Process response
            if (success && response.success)
            {
                // Handle response based on function type
                switch (functionChoice)
                {
                case TextProcessing::COUNT_WORDS:
                case TextProcessing::COUNT_CHARS:
                case TextProcessing::COUNT_LINES:
                case TextProcessing::COUNT_SENTENCES:
                    totalCount += response.intResult;
                    break;

                case TextProcessing::TO_UPPERCASE:
                case TextProcessing::TO_LOWERCASE:
                case TextProcessing::REPLACE_WORD:
                case TextProcessing::REMOVE_PUNCTUATION:
                case TextProcessing::REMOVE_EXTRA_SPACES:
                    fullResult += response.result;
                    break;

                case TextProcessing::WORD_FREQUENCY:
                    // Just display result for each chunk
                    std::cout << "\nWord frequency for chunk " << (position / chunkSize + 1) << ":" << std::endl;
                    std::cout << response.result << std::endl;
                    break;
                }
            }
            else
            {
                std::cout << "Failed to process chunk at position " << position << std::endl;
                return false;
            }

            // Move to next chunk
            position += chunkSize;
        }

        // Display final result
        std::cout << "\nFinal result after processing all chunks:" << std::endl;
        if (functionChoice == TextProcessing::COUNT_WORDS ||
            functionChoice == TextProcessing::COUNT_CHARS ||
            functionChoice == TextProcessing::COUNT_LINES ||
            functionChoice == TextProcessing::COUNT_SENTENCES)
        {
            std::cout << "Total count: " << totalCount << std::endl;
        }
        else if (functionChoice != TextProcessing::WORD_FREQUENCY)
        {
            std::cout << fullResult << std::endl;
        }
    }

    return true;
}

int main()
{
    // Initialize Winsock
    if (!NetworkCommon::initializeWinsock())
    {
        return 1;
    }

    // Default server settings
    std::string serverIp = "127.0.0.1";
    int tcpPort = NetworkCommon::DEFAULT_TCP_PORT;
    int udpPort = NetworkCommon::DEFAULT_UDP_PORT;

    // Ask user for server IP
    std::cout << "Enter server IP address [default: " << serverIp << "]: ";
    std::string userInput;
    std::getline(std::cin, userInput);
    if (!userInput.empty())
    {
        serverIp = userInput;
    }

    // Main program loop
    while (true)
    {
        int choice = displayMenu();

        if (choice == 0)
        {
            break; // Exit
        }

        if (choice < 0 || choice > 12)
        {
            std::cout << "Invalid choice. Press any key to continue...";
            _getch();
            continue;
        }

        // Choose protocol
        std::cout << "Select protocol:" << std::endl;
        std::cout << "1. TCP" << std::endl;
        std::cout << "2. UDP" << std::endl;
        std::cout << "Enter your choice: ";

        int protocolChoice;
        std::cin >> protocolChoice;
        std::cin.ignore(); // Clear the newline character

        // Check for valid protocol choice
        if (protocolChoice != 1 && protocolChoice != 2)
        {
            std::cout << "Invalid protocol choice! Press any key to continue...";
            _getch();
            continue;
        }

        bool useTcp = (protocolChoice == 1);
        int port = useTcp ? tcpPort : udpPort;

        // Get text for processing
        std::string text = getTextInput();

        // Process text in chunks if necessary
        processTextInChunks(text, choice, serverIp, port, useTcp);

        std::cout << "\nPress any key to continue...";
        _getch();
    }

    // Cleanup Winsock
    NetworkCommon::cleanupWinsock();

    return 0;
}