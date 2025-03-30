#include "NetworkCommon.h"
#include "TextProcessing.h"

// Function to process a text request
NetworkCommon::TextResponse processRequest(const NetworkCommon::TextRequest &request)
{
    NetworkCommon::TextResponse response;
    response.success = true;
    response.intResult = 0;
    memset(response.result, 0, NetworkCommon::MAX_TEXT_SIZE);

    std::string text(request.text);
    std::string result;

    try
    {
        switch (request.functionCode)
        {
        case TextProcessing::COUNT_WORDS:
            response.intResult = TextProcessing::countWords(text);
            sprintf_s(response.result, NetworkCommon::MAX_TEXT_SIZE, "Word count: %d", response.intResult);
            break;

        case TextProcessing::COUNT_CHARS:
            response.intResult = TextProcessing::countChars(text);
            sprintf_s(response.result, NetworkCommon::MAX_TEXT_SIZE, "Character count: %d", response.intResult);
            break;

        case TextProcessing::COUNT_LINES:
            response.intResult = TextProcessing::countLines(text);
            sprintf_s(response.result, NetworkCommon::MAX_TEXT_SIZE, "Line count: %d", response.intResult);
            break;

        case TextProcessing::TO_UPPERCASE:
            result = TextProcessing::toUppercase(text);
            strncpy_s(response.result, NetworkCommon::MAX_TEXT_SIZE, result.c_str(), result.length());
            break;

        case TextProcessing::TO_LOWERCASE:
            result = TextProcessing::toLowercase(text);
            strncpy_s(response.result, NetworkCommon::MAX_TEXT_SIZE, result.c_str(), result.length());
            break;

        case TextProcessing::REPLACE_WORD:
            result = TextProcessing::replaceWord(text, request.param1, request.param2);
            strncpy_s(response.result, NetworkCommon::MAX_TEXT_SIZE, result.c_str(), result.length());
            break;

        case TextProcessing::EXTRACT_SUBSTRING:
            result = TextProcessing::extractSubstring(text, request.intParam1, request.intParam2);
            strncpy_s(response.result, NetworkCommon::MAX_TEXT_SIZE, result.c_str(), result.length());
            break;

        case TextProcessing::REVERSE_TEXT:
            result = TextProcessing::reverseText(text);
            strncpy_s(response.result, NetworkCommon::MAX_TEXT_SIZE, result.c_str(), result.length());
            break;

        case TextProcessing::REMOVE_PUNCTUATION:
            result = TextProcessing::removePunctuation(text);
            strncpy_s(response.result, NetworkCommon::MAX_TEXT_SIZE, result.c_str(), result.length());
            break;

        case TextProcessing::WORD_FREQUENCY:
            result = TextProcessing::wordFrequency(text);
            strncpy_s(response.result, NetworkCommon::MAX_TEXT_SIZE, result.c_str(), result.length());
            break;

        case TextProcessing::REMOVE_EXTRA_SPACES:
            result = TextProcessing::removeExtraSpaces(text);
            strncpy_s(response.result, NetworkCommon::MAX_TEXT_SIZE, result.c_str(), result.length());
            break;

        case TextProcessing::COUNT_SENTENCES:
            response.intResult = TextProcessing::countSentences(text);
            sprintf_s(response.result, NetworkCommon::MAX_TEXT_SIZE, "Sentence count: %d", response.intResult);
            break;

        default:
            strncpy_s(response.result, NetworkCommon::MAX_TEXT_SIZE, "Unknown function code", 20);
            response.success = false;
            break;
        }
    }
    catch (const std::exception &e)
    {
        strncpy_s(response.result, NetworkCommon::MAX_TEXT_SIZE, e.what(), strlen(e.what()));
        response.success = false;
    }

    return response;
}

// TCP client handler thread function
void handleTcpClient(SOCKET clientSocket)
{
    char recvBuffer[NetworkCommon::MAX_BUFFER_SIZE];
    int bytesReceived;

    // Client handling loop
    while (true)
    {
        // Receive data from the client
        bytesReceived = recv(clientSocket, recvBuffer, sizeof(NetworkCommon::TextRequest), 0);
        if (bytesReceived <= 0)
        {
            if (bytesReceived == 0)
            {
                std::cout << "Client disconnected" << std::endl;
            }
            else
            {
                std::cerr << "recv failed: " << NetworkCommon::getLastErrorAsString() << std::endl;
            }
            break;
        }

        // Process the request
        NetworkCommon::TextRequest *request = reinterpret_cast<NetworkCommon::TextRequest *>(recvBuffer);
        NetworkCommon::TextResponse response = processRequest(*request);

        // Send the response back to the client
        int bytesSent = send(clientSocket, reinterpret_cast<char *>(&response), sizeof(response), 0);
        if (bytesSent == SOCKET_ERROR)
        {
            std::cerr << "send failed: " << NetworkCommon::getLastErrorAsString() << std::endl;
            break;
        }
    }

    // Close the client socket
    closesocket(clientSocket);
}

// TCP server function
void runTcpServer(int port)
{
    // Create a TCP socket
    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET)
    {
        std::cerr << "Error creating TCP socket: " << NetworkCommon::getLastErrorAsString() << std::endl;
        return;
    }

    // Setup server address
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    // Bind the socket
    if (bind(listenSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cerr << "TCP bind failed: " << NetworkCommon::getLastErrorAsString() << std::endl;
        closesocket(listenSocket);
        return;
    }

    // Listen for connections
    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cerr << "TCP listen failed: " << NetworkCommon::getLastErrorAsString() << std::endl;
        closesocket(listenSocket);
        return;
    }

    std::cout << "TCP server started on port " << port << std::endl;

    // Client acceptance loop
    std::vector<std::thread> clientThreads;

    while (true)
    {
        // Accept a client connection
        SOCKET clientSocket = accept(listenSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET)
        {
            std::cerr << "accept failed: " << NetworkCommon::getLastErrorAsString() << std::endl;
            continue;
        }

        std::cout << "New TCP client connected" << std::endl;

        // Create a thread to handle the client
        clientThreads.push_back(std::thread(handleTcpClient, clientSocket));

        // Detach the thread to allow it to run independently
        clientThreads.back().detach();
    }

    // Close the listening socket
    closesocket(listenSocket);
}

// UDP server function
void runUdpServer(int port)
{
    // Create a UDP socket
    SOCKET udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (udpSocket == INVALID_SOCKET)
    {
        std::cerr << "Error creating UDP socket: " << NetworkCommon::getLastErrorAsString() << std::endl;
        return;
    }

    // Setup server address
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    // Bind the socket
    if (bind(udpSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cerr << "UDP bind failed: " << NetworkCommon::getLastErrorAsString() << std::endl;
        closesocket(udpSocket);
        return;
    }

    std::cout << "UDP server started on port " << port << std::endl;

    // UDP message handling loop
    while (true)
    {
        char recvBuffer[NetworkCommon::MAX_BUFFER_SIZE];
        sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr);

        // Receive a datagram from a client
        int bytesReceived = recvfrom(udpSocket, recvBuffer, NetworkCommon::MAX_BUFFER_SIZE, 0,
                                     (sockaddr *)&clientAddr, &clientAddrSize);

        if (bytesReceived == SOCKET_ERROR)
        {
            std::cerr << "recvfrom failed: " << NetworkCommon::getLastErrorAsString() << std::endl;
            continue;
        }

        // Process the request if it's the right size
        if (bytesReceived == sizeof(NetworkCommon::TextRequest))
        {
            NetworkCommon::TextRequest *request = reinterpret_cast<NetworkCommon::TextRequest *>(recvBuffer);
            NetworkCommon::TextResponse response = processRequest(*request);

            // Send the response back to the client
            int bytesSent = sendto(udpSocket, reinterpret_cast<char *>(&response), sizeof(response), 0,
                                   (sockaddr *)&clientAddr, clientAddrSize);

            if (bytesSent == SOCKET_ERROR)
            {
                std::cerr << "sendto failed: " << NetworkCommon::getLastErrorAsString() << std::endl;
            }
        }
    }

    // Close the socket
    closesocket(udpSocket);
}

int main()
{
    // Initialize Winsock
    if (!NetworkCommon::initializeWinsock())
    {
        return 1;
    }

    std::cout << "Text Processing Server" << std::endl;
    std::cout << "----------------------" << std::endl;

    // Start TCP and UDP servers in separate threads
    std::thread tcpThread(runTcpServer, NetworkCommon::DEFAULT_TCP_PORT);
    std::thread udpThread(runUdpServer, NetworkCommon::DEFAULT_UDP_PORT);

    // Wait for the threads to finish (they won't in this case since they run infinite loops)
    tcpThread.join();
    udpThread.join();

    // Cleanup Winsock
    NetworkCommon::cleanupWinsock();

    return 0;
}