#include <iostream>
#include <Windows.h>
#include "StreamBaseClient.h"

/////////////
// GLOBALS //
/////////////

// Ugly but I think is better then creating a class and passing its reference between functions just to
// "simulate" a global variable, in a huge project global/static variable should be avoided
StreamBaseCommonObject	s_CurrentObject;
bool					s_HasValidObject = false;

/////////////
// METHODS //
/////////////

void OnClientConnected(StreamBaseClient& _client)
{
	while (true)
	{
		// Option text
		std::cout << "0 - Send string to the server" <<
			std::endl << "1 - Send number to the server" <<
			std::endl << "2 - Create new object on the server" <<
			std::endl << "3 - Get a list of all objects on the server" <<
			std::endl << "4 - Retrieve an object from the server" <<
			std::endl << "5 - Update the most recently retrieved object from the server" <<
			std::endl << "6 - Store the most recently retrieved object on the server" <<
			std::endl << "7 - Exit" << std::endl;

		// Get the option from the user
		int option;
		std::cin >> option;

		// Switch it
		switch (option)
		{
			// Send string
			case 0:
			{
				std::cout << "Please write the string that you want to send: ";
				std::string sendString;
				std::cin >> sendString;

				// Send the string to the server
				_client.SendServerString(sendString);

				break;
			}
			// Send number
			case 1:
			{
				std::cout << "Please write the number that you want to send: ";
				long long sendNumber;
				std::cin >> sendNumber;

				// Send the number to the server
				_client.SendServerNumber(sendNumber);

				break;
			}
			// Create object
			case 2:
			{
				// Request the server to create a new object
				_client.CreateObjectOnServer();

				break;
			}
			// Get list
			case 3:
			{
				// Request the object list from the server
				auto objectList = _client.GetServerObjectList();

				// Print the list
				std::cout << "Received object list from server:" << std::endl;
				std::cout << objectList.dump(4) << std::endl;

				break;
			}
			// Retrieve object
			case 4:
			{
				// Get from the user the object index
				std::cout << "Please write the object index you want to retrieve: ";
				int objectIndex;
				std::cin >> objectIndex;

				// Request the object from the server
				s_CurrentObject = _client.GetObjectFromServerAtIndex(objectIndex);

				// Set that we have a valid object
				s_HasValidObject = true;

				std::cout << "Object with name: " << s_CurrentObject.objectName 
					<< " sucessfully retrieved from the server!" << std::endl;

				break;
			}
			// Update object
			case 5:
			{
				break;
			}
			// Store object
			case 6:
			{
				break;
			}
			// Exit
			case 7:
			{
				return;
			}
		}

		std::cout << std::endl << std::endl;
	}

}

int main()
{
	StreamBaseClient client;

	while (true)
	{
		// Option text
		std::cout << "0 - Connect to server synchronously" <<
			std::endl << "1 - Connect to server assynchronously" <<
			std::endl << "2 - Exit" << std::endl;

		int option;
		std::cin >> option;

		// Check the selected option
		switch (option)
		{
			// Connect to server synchronously
			case 0:
			{
				// Try to connect with the server
				if (client.ConnectToServer(TEXT(SYNCPIPENAME)))
				{
					// Proceed to the client connected method
					OnClientConnected(client);

					// We finished, return
					return 0;
				}

				break;
			}
			// Connect to server asynchronously
			case 1:
			{
				// Try to connect with the server
				if (client.ConnectToServer(TEXT(ASYNCPIPENAME)))
				{
					// Proceed to the client connected method
					OnClientConnected(client);

					// We finished, return
					return 0;
				}

				break;
			}
			// Exit
			case 2:
			{
				// Exit
				return 0;
			}
		}

		std::cout << std::endl << std::endl;
	}
}