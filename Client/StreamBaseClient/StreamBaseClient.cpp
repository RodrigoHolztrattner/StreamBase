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

	return true;
}

void StreamBaseClient::SendServerString(std::string _string)
{
	SendServerMessage(MessageTypeString, (unsigned char*)_string.c_str(), _string.length() + 1);
}

void StreamBaseClient::SendServerNumber(long long _number)
{
	SendServerMessage(MessageTypeNumber, (unsigned char*)&_number, sizeof(long long));
}

void StreamBaseClient::SendServerMessage(int _messageType, unsigned char* _messageData, unsigned int _dataSize)
{
	DWORD dwWritten;
	unsigned char buffer[PIPE_BUFFER_SIZE];

	// Prepare the message
	MainMessageHeader messageHeader = {};
	messageHeader.messageType = _messageType;
	messageHeader.messageDataSize = _dataSize;
	memcpy(buffer, &messageHeader, sizeof(MainMessageHeader));
	memcpy(&buffer[sizeof(MainMessageHeader)], _messageData, _dataSize);

	// Send to the server
	WriteFile(m_ServerPipeHandle,
		&buffer,
		sizeof(MainMessageHeader) + _dataSize,
		&dwWritten,
		NULL);
}