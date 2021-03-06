////////////////////////////
// CLASS StreamBaseClient //
////////////////////////////
#include "StreamBaseClient.h"
#include <Windows.h>
#include <iostream>

StreamBaseClient::StreamBaseClient()
{
	// Set the initial data
	// ...
}

bool StreamBaseClient::ConnectToServer(LPCWSTR _pipeName)
{
	// Try to connect to the server
	m_ServerPipeHandle = CreateFile(_pipeName,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_OVERLAPPED,
		NULL);

	// Check if we are ok
	if (m_ServerPipeHandle == INVALID_HANDLE_VALUE)
	{
		// Informative text
		std::cout << "Failed to connect with server with pipe: " << ASYNCPIPENAME << ", are you sure the server is running?" << std::endl;
		
		return false;
	}

	// Informative text
	std::cout << "Successfully connected to the server on pipe: " << ASYNCPIPENAME << std::endl;

	// Create the asynchronous manager thread
	m_AsynchronousManagerThread = std::thread(&StreamBaseClient::AsynchronousAuxiliarMethod, this);

	return true;
}

void StreamBaseClient::SendServerString(std::string _string, bool _asynchronous)
{
	SendServerMessage(MessageTypeString, (unsigned char*)_string.c_str(), unsigned int(_string.length()) + 1, _asynchronous);
}

void StreamBaseClient::SendServerNumber(long long _number, bool _asynchronous)
{
	SendServerMessage(MessageTypeNumber, (unsigned char*)&_number, sizeof(long long), _asynchronous);
}

void StreamBaseClient::CreateObjectOnServer(bool _asynchronous)
{
	SendServerMessage(MessageTypeCreateObject, nullptr, 0, _asynchronous);
}

void StreamBaseClient::GetServerObjectList(bool _asynchronous)
{
	// Send the request
	SendServerMessage(MessageTypeListObjects, nullptr, 0, _asynchronous);

	// If this is not an asynchronous operation
	if (!_asynchronous)
	{
		// Receive the object list
		auto serverResult = GetServerMessage();

		// Call the callback
		m_ReceivedObjectListCallback(nlohmann::json::parse(std::string(serverResult.begin(), serverResult.end())));
	}
}

void StreamBaseClient::GetObjectFromServerAtIndex(int _index, bool _asynchronous)
{
	// Send the request
	SendServerMessage(MessageTypeRetrieveObject, (unsigned char*)&_index, sizeof(int), _asynchronous);

	// If this is not an asynchronous operation
	if (!_asynchronous)
	{
		// Receive the object
		auto serverResult = GetServerMessage();

		// Transform the server result into a json
		nlohmann::json j = nlohmann::json::parse(std::string(serverResult.begin(), serverResult.end()));

		// Call the callback
		m_ReceivedObjectCallback(j.get<StreamBaseCommonObject>());
	}
}

void StreamBaseClient::UpdateCurrentObjectOnServer(StreamBaseCommonObject _object, bool _asynchronous)
{
	// Transform the object into a json
	nlohmann::json j = _object;

	// Transform the json into a string
	std::string resultString = j.dump(4);

	// Send the object to be updated on the server side
	SendServerMessage(MessageTypeStoreObject, (unsigned char*)resultString.c_str(), unsigned int(resultString.length()) + 1, _asynchronous);
}

void StreamBaseClient::SendServerMessage(int _messageType, unsigned char* _messageData, unsigned int _dataSize, bool _asynchronous)
{
	DWORD dwWritten;
	unsigned char buffer[PIPE_BUFFER_SIZE];

	// Prepare the message
	MainMessageHeader messageHeader = {};
	messageHeader.messageType = _messageType;
	messageHeader.messageDataSize = _dataSize;
	memcpy(buffer, &messageHeader, sizeof(MainMessageHeader));
	if(_dataSize > 0 && _messageData != nullptr)
		memcpy(&buffer[sizeof(MainMessageHeader)], _messageData, _dataSize);

	// If this is an asynchronous write
	if (_asynchronous)
	{
		// Add a new event
		m_AsynchronousEvents.push_back({ _messageType ,  { 0,0,0 } });
		m_AsynchronousEventHandles.push_back(CreateEvent(NULL, TRUE, FALSE, NULL));

		AsynchronousEvent& eventObject = m_AsynchronousEvents.back();
		HANDLE& eventObjectHandle = m_AsynchronousEventHandles.back();
		eventObject.overlappedStructure.hEvent = eventObjectHandle;

		// Send to the server
		WriteFile(m_ServerPipeHandle,
			&buffer,
			sizeof(MainMessageHeader) + _dataSize,
			&dwWritten,
			&eventObject.overlappedStructure);
	}
	else
	{
		// Send to the server
		WriteFile(m_ServerPipeHandle,
			&buffer,
			sizeof(MainMessageHeader) + _dataSize,
			&dwWritten,
			NULL);
	}
}

std::vector<unsigned char> StreamBaseClient::GetServerMessage()
{
	unsigned char buffer[PIPE_BUFFER_SIZE];
	DWORD dwRead;

	// Read a client message
	while (ReadFile(m_ServerPipeHandle, buffer, sizeof(buffer) - 1, &dwRead, NULL) != FALSE)
	{
		return std::vector<unsigned char>(buffer, buffer + dwRead);
	}

	return std::vector<unsigned char>();
}

void StreamBaseClient::AsynchronousAuxiliarMethod()
{
	// Do forever
	while (true)
	{
		// If we don't have at least one event to wait for
		if (!m_AsynchronousEvents.size())
		{
			// Sleep a little and check again
			Sleep(10);
			continue;
		}

		// Wait for an event
		DWORD dwWait = WaitForMultipleObjects(unsigned int(m_AsynchronousEventHandles.size()), m_AsynchronousEventHandles.data(), FALSE, INFINITE);

		// Retrieve the index
		int i = dwWait - WAIT_OBJECT_0;
		if (i < 0 || i >(m_AsynchronousEventHandles.size() - 1))
		{
			std::cout << "Event received for an invalid index!" << std::endl;

			continue;
		}

		// Get the event object
		auto currentEvent = m_AsynchronousEvents[i];

		// Switch the event message
		switch (currentEvent.expectedMessage)
		{
			case MessageTypeListObjects:
			{
				// Receive the object list
				auto serverResult = GetServerMessage();

				// Call the callback
				m_ReceivedObjectListCallback(nlohmann::json::parse(std::string(serverResult.begin(), serverResult.end())));

				break;
			}
			case MessageTypeRetrieveObject:
			{
				// Receive the object
				auto serverResult = GetServerMessage();

				// Transform the server result into a json
				nlohmann::json j = nlohmann::json::parse(std::string(serverResult.begin(), serverResult.end()));

				// Call the callback
				m_ReceivedObjectCallback(j.get<StreamBaseCommonObject>());

				break;
			}
			default:
			{
				// Just a simple text to inform the asynchronous completion
				std::cout << "Asynchronous server call completed!" << std::endl;
			}
		}

		// Remove the current event and handle
		CloseHandle(m_AsynchronousEventHandles[i]);
		m_AsynchronousEventHandles.erase(m_AsynchronousEventHandles.begin() + i);
		m_AsynchronousEvents.erase(m_AsynchronousEvents.begin() + i);
	}
}

void StreamBaseClient::RegisterObjectListCallback(std::function<void(nlohmann::json)> _callback)
{
	m_ReceivedObjectListCallback = _callback;
}

void StreamBaseClient::RegisterObjectRetrievalCallback(std::function<void(StreamBaseCommonObject)> _callback)
{
	m_ReceivedObjectCallback = _callback;
}