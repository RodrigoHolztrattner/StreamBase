////////////////////////////
// CLASS StreamBaseServer //
////////////////////////////

#include "StreamBaseServer.h"
#include <Windows.h>
#include <iostream>

#include "StreamBaseObjectHolder.h"

StreamBaseServer::StreamBaseServer(StreamBaseObjectHolder& _objectHolder) : m_ObjectHolder(_objectHolder)
{
	// Set the initial data
	// ...
}

void StreamBaseServer::BeginListen()
{
	// Create a thread to listen incomming client connections synchronously
	m_ClientConnectionListenerThread = std::thread(&StreamBaseServer::SyncListenAux, this);

	// Go to the method that will handle asynchronous connections
	AsyncListenAux();
}

void StreamBaseServer::AsyncListenAux()
{
	// Setup the pipe for each possible asynchronous client that we can handle
	for (int i = 0; i < m_AsyncConnectedClients.size(); i++)
	{
		// Get the current client and event handle
		auto& client = m_AsyncConnectedClients[i];
		auto& eventHandle = m_AsyncEventHandles[i];

		// Create the event for this client
		eventHandle = CreateEvent(NULL, FALSE, TRUE, NULL); 
		if (eventHandle == NULL)
		{
			printf("CreateEvent failed with %d.\n", GetLastError());
			return;
		}

		// Setup the overlapped event and create our named pipe
		client.clientOverlappedHandle.hEvent = eventHandle;
		client.clientPipeHandle = CreateNamedPipe(TEXT(SYNCPIPENAME),
			PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
			PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
			m_AsyncConnectedClients.size(),
			PIPE_BUFFER_SIZE * sizeof(TCHAR),
			PIPE_BUFFER_SIZE * sizeof(TCHAR),
			NMPWAIT_USE_DEFAULT_WAIT, // 5000
			NULL);
		if (client.clientPipeHandle == INVALID_HANDLE_VALUE)
		{
			printf("CreateNamedPipe failed with %d.\n", GetLastError());
			return;
		}

		// Call the subroutine to connect to the new client
		ConnectNamedPipe(client.clientPipeHandle, &client.clientOverlappedHandle);

		// Set pending connection to true
		client.isPendingConnection = true;
	}

	// Try to accept incomming connection requests
	while (true)
	{
		// Wait for an event (this will block the current (main) thread)
		DWORD dwWait = WaitForMultipleObjects(m_AsyncEventHandles.size(),  m_AsyncEventHandles.data(), FALSE, INFINITE);

		// Retrieve the pipe index
		int i = dwWait - WAIT_OBJECT_0;
		if (i < 0 || i >(m_AsyncConnectedClients.size() - 1))
		{
			std::cout << "Event received for an invalid pipe index!" << std::endl;

			continue;
		}

		// Get the pipe client
		auto& client = m_AsyncConnectedClients[i];

		// Check if this client is pending connection
		if (client.isPendingConnection)
		{
			std::cout << "Client connected on async pipe!" << std::endl;

			// Create a new thread to handle this client messages
			client.clientThread = std::thread(&StreamBaseServer::HandleClient, this, &m_AsyncConnectedClients[i]);
		}
		else
		{
			// TODO: Handle error
		}
	}
}

void StreamBaseServer::SyncListenAux()
{
	// Infinite loop to handle multiple clients
	while (true)
	{
		// Create a named pipe
		HANDLE hPipe = CreateNamedPipe(TEXT(ASYNCPIPENAME),
			PIPE_ACCESS_DUPLEX,
			PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
			PIPE_UNLIMITED_INSTANCES,
			PIPE_BUFFER_SIZE * sizeof(TCHAR),
			PIPE_BUFFER_SIZE * sizeof(TCHAR),
			NMPWAIT_USE_DEFAULT_WAIT,
			NULL);

		// Check if the created pipe is valid
		if (hPipe == INVALID_HANDLE_VALUE)
		{
			// TODO Handle error
			continue;
		}
		
		// Wait for a client
		if (ConnectNamedPipe(hPipe, NULL) != FALSE)
		{
			// Create a new client structure
			ConnectedClientSync* newClient = new ConnectedClientSync;
			newClient->clientPipeHandle = hPipe;

			// Add this new client to our vector
			m_SyncConnectedClients.push_back(newClient);

			std::cout << "Client connected on sync pipe!" << std::endl;

			// Create a new thread to handle this client
			m_SyncConnectedClients.back()->clientThread = std::thread(&StreamBaseServer::HandleClient, this, m_SyncConnectedClients.back());
		}
	}
}

void StreamBaseServer::HandleClient(ConnectedClientBase* _clientInfo)
{
	char buffer[PIPE_BUFFER_SIZE];
	DWORD dwRead;

	// Read a client message
	while (ReadFile(_clientInfo->clientPipeHandle, buffer, sizeof(buffer) - 1, &dwRead, NULL) != FALSE)
	{
		// Cast the buffer to our main message header
		MainMessageHeader messageHeader = {};
		memcpy(&messageHeader, buffer, sizeof(MainMessageHeader));
		char* messageData = &buffer[sizeof(MainMessageHeader)];
		int clientId = (int)_clientInfo->clientPipeHandle;

		// Check the message type
		switch (messageHeader.messageType)
		{
			case MessageTypeString:
			{
				// Cast to a string
				std::string receivedString = std::string((char*)messageData);
				m_TrivialStringData.push_back({ clientId, receivedString });
				
				// Print it
				std::cout << "Received string '" << receivedString << "' from client with id: " << clientId << std::endl;

				break;
			}
			case MessageTypeNumber:
			{
				// Cast to a long long
				long long receivedNumber = *(long long*)messageData;
				m_TrivialNumberData.push_back({ clientId, receivedNumber });

				// Print it
				std::cout << "Received number '" << receivedNumber << "' from client with id: " << clientId << std::endl;
					
				break;
			}
			case MessageTypeCreateObject:
			{
				// Tell our object holder to create a new internal object
				m_ObjectHolder.CreateNewObject();

				// Print an info text
				std::cout << "Created an object for client with id: " << clientId << std::endl;

				break;
			}
			case MessageTypeListObjects:
			{
				// Get a list with all object names
				auto objectList = m_ObjectHolder.GetObjectNameList();
				std::string resultString = objectList.dump(4);

				// Send to the server
				DWORD dwWritten;
				WriteFile(_clientInfo->clientPipeHandle,
					resultString.data(),
					resultString.size(),
					&dwWritten,
					NULL);

				// Print an info text
				std::cout << "Object name list successfully sent for client with id: " << clientId << std::endl;

				break;
			}
			case MessageTypeRetrieveObject:
			{
				// Cast to a int to get the index
				int objectIndex = *(int*)messageData;

				// Get the object for the given index
				auto object = m_ObjectHolder.GetObjectOnIndex(objectIndex);
				std::string resultString = object.dump(4);

				// Send to the server
				DWORD dwWritten;
				WriteFile(_clientInfo->clientPipeHandle,
					resultString.data(),
					resultString.size(),
					&dwWritten,
					NULL);

				// Print an info text
				std::cout << "Object on index: " << std::to_string(objectIndex) 
					<< " was sent to client client with id: " << clientId
					<< " successfully!" 
					<< std::endl;

				break;
			}
			case MessageTypeStoreObject:
			{
				// Cast to a string
				std::string receivedString = std::string((char*)messageData);

				// Transform it into a json
				nlohmann::json j = nlohmann::json::parse(receivedString);

				// Try to update the object that is inside the given json
				StreamBaseCommonObject updatedObject;
				bool result = m_ObjectHolder.UpdateInternalObject(j, updatedObject);
				if (result)
				{
					// Informative text
					std::cout << "Successfully updated an object for client with id: " << clientId << std::endl;
					std::cout << " - Object name: " << updatedObject.GetName() << std::endl;
					std::cout << " - Object counter: " << updatedObject.GetCounter() << std::endl << std::endl;
				}
				else
				{
					// Informative text
					std::cout << "Attempted to update an object but it isn't located on our database!" << std::endl;
				}

				break;
			}
			case MessageTypeUnknow:
			default:
			{
				// TODO Handle error
				// ...
			}
		}
	}
}