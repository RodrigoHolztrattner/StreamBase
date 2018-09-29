#include <iostream>
#include <Windows.h>

void Client()
{
	HANDLE hPipe;
	DWORD dwWritten;


	hPipe = CreateFile(TEXT("\\\\.\\pipe\\Pipe"),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);
	if (hPipe != INVALID_HANDLE_VALUE)
	{
		while (true)
		{
			WriteFile(hPipe,
				"Hello Pipe\n",
				12,   // = length of string + terminating '\0' !!!
				&dwWritten,
				NULL);

			Sleep(1000);
		}


		CloseHandle(hPipe);
	}
}


void Server()
{
	HANDLE hPipe;
	char buffer[1024];
	DWORD dwRead;


	hPipe = CreateNamedPipe(TEXT("\\\\.\\pipe\\Pipe"),
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,   // FILE_FLAG_FIRST_PIPE_INSTANCE is not needed but forces CreateNamedPipe(..) to fail if the pipe already exists...
		1,
		1024 * 16,
		1024 * 16,
		NMPWAIT_USE_DEFAULT_WAIT,
		NULL);
	while (hPipe != INVALID_HANDLE_VALUE)
	{
		if (ConnectNamedPipe(hPipe, NULL) != FALSE)   // wait for someone to connect to the pipe
		{
			while (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &dwRead, NULL) != FALSE)
			{
				/* add terminating zero */
				buffer[dwRead] = '\0';

				/* do something with data in buffer */
				printf("%s", buffer);
			}
		}

		DisconnectNamedPipe(hPipe);
	}
}

int main()
{
    std::cout << "0 for client, 1 for server" << std::endl; 

	// Check if this is a client or server
	int option;
	std::cin >> option;
	if (option == 0)
	{
		// Proceed to client mode
		Client();
	}
	else if (option == 1)
	{
		// Proceed to server mode
		Server();
	}
}
