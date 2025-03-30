# Text Processing Network Application

This application demonstrates text processing over a network using Winsock. It enables client-server text processing with both TCP and UDP protocols, implementing various text manipulation functions.

## Features

- Client-server architecture with Winsock
- Support for both TCP and UDP protocols
- Concurrent server handling with multithreading
- 12 different text processing functions:
  1. Count words
  2. Count characters
  3. Count lines
  4. Convert to uppercase
  5. Convert to lowercase
  6. Replace words
  7. Extract substring
  8. Reverse text
  9. Remove punctuation
  10. Word frequency analysis
  11. Remove extra spaces
  12. Count sentences

## Components

- `TextProcessing.h/cpp`: Text processing function declarations and implementations
- `NetworkCommon.h/cpp`: Common networking utilities, structures, and constants
- `TextServer.cpp`: Server application that handles both TCP and UDP connections
- `TextClient.cpp`: Client application that can use either TCP or UDP to communicate

## Building the Application

1. Open the solution in Visual Studio
2. Add all .cpp and .h files to the project
3. Build the solution (Build > Build Solution)

You'll need to create two separate projects:

- One for the server (`TextServer.cpp`)
- One for the client (`TextClient.cpp`)

Both projects should include the common files (`TextProcessing.cpp`, `NetworkCommon.cpp`).

## Running the Application

1. Start the server application

   - The server will listen on the default ports (TCP: 8888, UDP: 8889)
   - You'll see messages indicating that the server is running

2. Start the client application

   - Enter the server IP address (default: 127.0.0.1 for localhost)
   - Select a text processing function from the menu
   - Choose the protocol (TCP or UDP)
   - Enter the text to be processed
   - For functions requiring additional parameters (like Replace Word), follow the prompts

3. View the results received from the server

## How It Works

1. The client sends a `TextRequest` structure to the server containing:

   - The function code indicating which operation to perform
   - The text to be processed
   - Any additional parameters required for the operation

2. The server processes the request:

   - For TCP: It handles each client in a separate thread
   - For UDP: It processes datagrams as they arrive

3. The server sends back a `TextResponse` structure containing:

   - A success flag
   - The result (text or numeric)

4. The client displays the result to the user

## Requirements

- Windows operating system
- Visual Studio or any C++ compiler with Winsock support

## Implementation Details

- Concurrency is implemented using threads for handling multiple TCP clients
- Error handling includes Winsock error reporting
- Text processing functions use iterative algorithms
- Communication is structured using predefined message formats
