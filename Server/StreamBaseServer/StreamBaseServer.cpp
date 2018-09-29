////////////////////////////
// CLASS StreamBaseServer //
////////////////////////////

#include "StreamBaseServer.h"
#include <Windows.h>
#include <iostream>

StreamBaseServer::StreamBaseServer()
{
	// Set the initial data
	m_AcceptingAsyncClientConnections = true;
}

void StreamBaseServer::BeginListen()
{
	// Create a thread to listen incomming client connections
	m_ClientConnectionListenerThread = std::thread(&StreamBaseServer::ListenAux, StreamBaseServer());
}

void StreamBaseServer::ServerShouldAcceptAsyncClientConnections(bool _status)
{
	if (_status == false && m_AcceptingAsyncClientConnections != _status)
	{
		// Print an informative text
		std::cout << "After the next client request, each new request will be synchronous!" << std::endl;

		// Use mutex here?
	}

	// TODO: ...
}

void StreamBaseServer::ListenAux()
{
	// Infinite loop to handle multiple clients
	while (true)
	{
		// Create a named pipe
		HANDLE hPipe = CreateNamedPipe(TEXT(PIPENAME),
			PIPE_ACCESS_DUPLEX,
			PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,   // FILE_FLAG_FIRST_PIPE_INSTANCE is not needed but forces CreateNamedPipe(..) to fail if the pipe already exists...
			PIPE_UNLIMITED_INSTANCES,
			1024 * 16, // TODO
			1024 * 16, // TODO
			NMPWAIT_USE_DEFAULT_WAIT,
			NULL);

		// Check if the created pipe is valid
		if (hPipe == INVALID_HANDLE_VALUE)
		{
			// TODO Handle error
			continue;
		}

		// Wait for a client
		if (ConnectNamedPipe(hPipe, NULL) != FALSE)   // wait for someone to connect to the pipe
		{
			// Create a new client structure
			ConnectedClient* newClient = new ConnectedClient;
			newClient->clientPipeHandle = hPipe;

			// Add this new client to our vector
			m_ConnectedClients.push_back(newClient);

			std::cout << "Client Connected!" << std::endl;

			// Create a new thread to handle this client
			m_ConnectedClients.back()->clientThread = std::thread(&StreamBaseServer::HandleClient, StreamBaseServer(), m_ConnectedClients.back());;
		}
	}
}

void StreamBaseServer::HandleClient(ConnectedClient* _clientInfo)
{
	char buffer[1024];
	DWORD dwRead;

	// Read a client message
	while (ReadFile(_clientInfo->clientPipeHandle, buffer, sizeof(buffer) - 1, &dwRead, NULL) != FALSE)
	{
		// Cast the buffer to our main message header
		MainMessageHeader messageHeader = {};
		memcpy(&messageHeader, buffer, sizeof(MainMessageHeader));
		char* messageData = &buffer[sizeof(MainMessageHeader)];

		// Check the message type
		switch (messageHeader.messageType)
		{
			case MessageTypeString:
			{
				// Cast to a string
				std::string receivedString = std::string((char*)messageData);

				// Print it
				std::cout << "Received string '" << receivedString << "' from client!" << std::endl;

				break;
			}
			case MessageTypeNumber:
			{
				// Cast to a long long
				long long receivedNumber = *(long long*)messageData;

				// Print it
				std::cout << "Received number '" << receivedNumber << "' from client!" << std::endl;
					
				break;
			}
			case MessageTypeCreateObject:
			{
				break;
			}
			case MessageTypeListObjects:
			{
				break;
			}
			case MessageTypeRetrieveObject:
			{
				break;
			}
			case MessageTypeStoreObject:
			{
				break;
			}
			case MessageTypeUnknow:
			default:
			{
				// TODO Handle error
				// ...
			}
		}

		/* add terminating zero */
		// buffer[dwRead] = '\0';

		/* do something with data in buffer */
		// printf("%s", buffer);
	}
}