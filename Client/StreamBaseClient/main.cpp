#include <iostream>
#include <Windows.h>
#include "StreamBaseClient.h"

/////////////
// GLOBALS //
/////////////

/////////////
// METHODS //
/////////////

void OnUpdateObject(StreamBaseCommonObject& _currentObject)
{
	// Until we finish updating the current object
	while (true)
	{
		// Option text
		std::cout << "0 - Increment the current object counter" <<
			std::endl << "1 - Decrement the current object counter" <<
			std::endl << "2 - Zero the current object counter" <<
			std::endl << "3 - Back" << std::endl;

		// Get the option from the user
		int option;
		std::cin >> option;

		// Switch it
		switch (option)
		{
			// Increment counter
			case 0:
			{
				// Increment the object counter
				_currentObject.IncrementCounter();

				// Informative text
				std::cout << "The counter is: " << _currentObject.GetCounter() << std::endl;

				break;
			}
			// Decrement counter
			case 1:
			{
				// Decrement the object counter
				_currentObject.DecrementCounter();

				// Informative text
				std::cout << "The counter is: " << _currentObject.GetCounter() << std::endl;

				break;
			}
			// Reset counter
			case 2:
			{
				// Reset counter
				_currentObject.ResetCounter();

				// Informative text
				std::cout << "The counter was reseted!" << std::endl;

				break;
			}
			// Back
			case 3:
			{
				return;
			}
		}

		// Jump a line
		std::cout << std::endl;
	}
}

void OnClientConnected(StreamBaseClient& _client)
{
	StreamBaseCommonObject currentObject;
	bool hasValidObject = false;
	bool asynchronousSend = false;

	// Register the callbacks for the client object
	_client.RegisterObjectListCallback([](nlohmann::json j)
	{
		// Print the list
		std::cout << "Received object list from server:" << std::endl;
		std::cout << j.dump(4) << std::endl;
	});
	_client.RegisterObjectRetrievalCallback([&](StreamBaseCommonObject _receivedObject)
	{
		// Set the object
		currentObject = _receivedObject;

		// Set that we have a valid object
		hasValidObject = true;
		
		// Informative text
		std::cout << "Object with name: " << currentObject.GetName()
			<< " sucessfully retrieved from the server!" << std::endl;
	});

	// Until we decide to disconnect
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
			std::endl << "7 - Toggle synchronous/asynchronous mode, current is: " <<
			(asynchronousSend ? "asynchronous" : "synchronous") <<
			std::endl << "8 - Exit" << std::endl;

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
				_client.SendServerString(sendString, asynchronousSend);

				break;
			}
			// Send number
			case 1:
			{
				std::cout << "Please write the number that you want to send: ";
				long long sendNumber;
				std::cin >> sendNumber;

				// Send the number to the server
				_client.SendServerNumber(sendNumber, asynchronousSend);

				break;
			}
			// Create object
			case 2:
			{
				// Request the server to create a new object
				_client.CreateObjectOnServer(asynchronousSend);

				break;
			}
			// Get list
			case 3:
			{
				// Request the object list from the server
				_client.GetServerObjectList(asynchronousSend);

				break;
			}
			// Retrieve object
			case 4:
			{
				// Get, from the user, the object index
				std::cout << "Please write the object index you want to retrieve: ";
				int objectIndex;
				std::cin >> objectIndex;

				// Request the object from the server
				_client.GetObjectFromServerAtIndex(objectIndex, asynchronousSend);

				break;
			}
			// Update object
			case 5:
			{
				// If we have a valid object
				if (hasValidObject)
				{
					// Proceed to update the object
					OnUpdateObject(currentObject);
				}
				else
				{
					// Informative text
					std::cout << "There isn't a valid object, please retrieve one fist!" << std::endl;
				}

				break;
			}
			// Store object
			case 6:
			{
				// Check if the current object is valid
				if (hasValidObject)
				{
					// Request the server to update the given object with its attributes
					_client.UpdateCurrentObjectOnServer(currentObject, asynchronousSend);
				}
				else
				{
					// Informative text
					std::cout << "There isn't a valid object, please retrieve one fist!" << std::endl;
				}

				break;
			}
			// Toggle synchronous/asynchronous
			case 7:
			{
				asynchronousSend = !asynchronousSend;

				std::cout << "Send mode changed to: " << (asynchronousSend ? "asynchronous" : "synchronous") << std::endl;
				
				break;
			}			
			// Exit
			case 8:
			{
				return;
			}
		}

		// Jump a line
		std::cout << std::endl;
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
					OnClientConnected(client);

					// We finished, safelly return here
					exit(0);
				}
				else
				{
					// Informative text
					std::cout << "Error connecting to the server, are you sure it is listenning?" << std::endl;
				}

				break;
			}
			// Connect to server asynchronously
			case 1:
			{
				// Try to connect with the server
				if (client.ConnectToServer(TEXT(ASYNCPIPENAME)))
				{
					OnClientConnected(client);

					// We finished, safelly return here
					exit(0);
				}
				else
				{
					// Informative text
					std::cout << "Error connecting to the server, are you sure it is listenning and it has at least one valid asynchronous slot remaining?" << std::endl;
				}

				break;
			}
			// Exit
			case 2:
			{
				// Exit
				exit(0);
			}
		}

		// Jump one line to better formatting
		std::cout << std::endl;
	}
}