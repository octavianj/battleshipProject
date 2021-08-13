// Battleship Main File
// By Octavian T. Johnson


// Include standard headers
#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <cmath>
#include <ctime>
using namespace std;

// Include custom headers
#include "gameSpecs.h"
#include "enemyAI.h"

/**
 * Displays random battleship coordinates in quick succession to make the
 * illusion that the computer/enemy is thinking about which coordinate to shoot
 * at.
 *
 * After the function returns the last random coordinate is still on the screen,
 * but the cursor is back up so that it can be overwritten with something new.
 */
void randomCoordinatesAnimation();

//takes in the enemy board and the enemy hit array
//returns a string saying if the shot was a hit or a miss
string playersTurn(Tile eBoard[][BOARD_LENGTH], int eHits[]);

//clears board and changes all tiles to water
void wipeBoard(Tile board[][BOARD_LENGTH], Tile type = WATER );

//displays the player and enemy's fleet along with hits and misses
void displayBoards(const Tile player[][BOARD_LENGTH], 
	const Tile computer[][BOARD_LENGTH], bool showAll = false);

//places ships on the board horizontally
bool placeShipHorizontally(Tile ship, int shipSize, int row, int col, 
	Tile board[][BOARD_LENGTH]);

//places ships on the board vertically
bool placeShipVertically(Tile ship, int shipSize, int row, int col, 
	Tile board[][BOARD_LENGTH]);

//randomly places all ships on the board
void placeShipsRandomly(Tile board[][BOARD_LENGTH]);

//places player's ships and asks user if they want to use the 
//board displayed or to refresh and try a new one
void placePlayersShips(Tile board[][BOARD_LENGTH]);

//validates the user's coordinates and let's them know if 
//they sunk the enemy's ship or not
string playersTurn(Tile eBoard[][BOARD_LENGTH], int eHits[]);

//takes in an array and checks if all the values are zeroes
//returns true if they are
bool isAllZeroes(int shotsRemaining[], int shotListSize);

//displays message as if it has been typed by someone
void displayAsIfTyped(const string message, int time = 1000,
	int pause = 500);

int main ()
{
	//initate seed for rng later
	unsigned int seed = static_cast<unsigned int>(time(NULL));
	srand(seed);

	Tile playerBoard[BOARD_LENGTH][BOARD_LENGTH]; //player board
	Tile compBoard[BOARD_LENGTH][BOARD_LENGTH]; //enemy board
	int playerHits[] = { 0, 5, 4, 3, 3, 2 }; //player hits array
	int compHits[] = { 0, 5, 4, 3, 3, 2 }; //enemy hits array
	const int SIZE = 6; //size of the hits array

	string message; //var used for output strings

	//clear boards before starting
	wipeBoard(playerBoard);
	wipeBoard(compBoard);

	//places ships and player's ships (hiding the enemy's ships0 
	placeShipsRandomly(compBoard);
	placePlayersShips(playerBoard);
	
	do { //the game loop

		displayBoards(playerBoard, compBoard); //display boards
		cout << "Your shot: "; 

		//dispay info with animation
		displayAsIfTyped(playersTurn(compBoard, compHits), 200); 

		if (isAllZeroes(compHits, SIZE)) //if all ships have been sunk...
		{
			break;//...break loop
		}

		displayBoards(playerBoard, compBoard); //display board after player hit
		
		cout << "Enemy's shot: ";

		//animation that cycles through before enemy hit
		randomCoordinatesAnimation();
		
		//animate enemy's shot
		displayAsIfTyped(EnemyAI::enemyTurn(playerBoard, playerHits), 200); 
		
	} while (!isAllZeroes(compHits, SIZE)); //end game loop
	
	//if all enemy ships have been destroyed..
	if (!isAllZeroes(compHits, SIZE)) 
	{
		//tell user that they won
		cout << "You sunk the enemey's fleet!" << endl;
		cout << "You win!" << endl;
	}

	//if player ships have been destroyed
	else if (!isAllZeroes(playerHits, SIZE))
	{
		//tell user that they lost
		cout << "The enemy sunk your fleet!" << endl;
		cout << "You lose!" << endl;
	}
	//display boards with enemy ships shown
	displayBoards(playerBoard, compBoard, true);

	return 0; //end program
} 

void randomCoordinatesAnimation()
{
	// Variables to hold random coordinates to print
	int colNumber;
	char rowLetter;

	cout << left; // left align columns

	// Repeatedly output a random coordinate, then wait, then backup to 
	// overwrite the pauses increase in time exponentially until it is 
	// >= 160 msecs
	for (int msecs = 10; msecs < 160; msecs = static_cast<int>(msecs * 1.1))
	{
		// Get random numbers
		rowLetter = rand() % BOARD_LENGTH + 'A';
		colNumber = rand() % (BOARD_LENGTH) + 1;

		// Output numbers
		cout << rowLetter << setw(2) << colNumber << flush;

		// Wait the alloted time
		this_thread::sleep_for(chrono::milliseconds(msecs));

		// Backup for next output.
		cout << "\b\b\b";
	}

	cout << right; // reset alignment back to the default
}//end randomCoordinatesAnimation

void wipeBoard(Tile board[BOARD_LENGTH][BOARD_LENGTH], Tile type)
{
	//row and col variables
	int colChange;
	int rowChange;

	//loop through array rows
	for (rowChange = 0; rowChange < BOARD_LENGTH; rowChange++)
	{
		//loop through array columns
		for (colChange = 0; colChange < BOARD_LENGTH; colChange++)
		{
			//change board tiles to value (default water)
			board[rowChange][colChange] = type; 
		}
	}
} //end wipeBoard

void displayBoards(const Tile player[BOARD_LENGTH][BOARD_LENGTH],
	const Tile computer[BOARD_LENGTH][BOARD_LENGTH], bool showAll)
{
	
	int num; //col numbers
	int col; //board columns
	int row; //board rows
	char let = 'A'; //starting row letters

	//display titles
	cout << setw(15) << right << "Enemy's Fleet";
	cout << setw(21) << right << "Your Fleet";
	cout << endl << "  "; //space between titles

	//enemy fleet col numbers
	for (num = 1; num <= 10; num++)
	{
		cout << setw(2) << num; //set nums with space btw them
	}

	//space between numbers
	cout << setw(4) << " ";

	//your fleet col numbers
	for (num = 1; num <= 10; num++)
	{
		cout << setw(2) << num; //set nums with space btw them
	}

	//next line
	cout << endl;

	//begin loop through arrays
	for (row = 0; row < BOARD_LENGTH; row++)
	{
		cout << " " << let; //enemy row letter
		
		//loop enemy grid
		for (col = 0; col < BOARD_LENGTH; col++)
		{
			if (!showAll) //if showAll is false..
			{
				if ((tileToSymbol(computer[row][col]) >= 'a') &&
					(tileToSymbol(computer[row][col]) <= 'z'))
				{
					cout << setw(2) << "X"; //show X if a hit
				}
				else if (tileToSymbol(computer[row][col]) == '~')
				{
					//show ~ if a miss
					cout << setw(2) << tileToSymbol(computer[row][col]);

				}
				else
				{
					cout << setw(2) << "."; //show . if unknown
				}
			}
			else
			{
				//if showAll is true, show symbols as normal
				cout << setw(2) << tileToSymbol(computer[row][col]);
			}
		}
		
		cout << setw(3) << " "; //space between rows
		cout << let; //player grid row letter

		for (col = 0; col < BOARD_LENGTH; col++)
		{
			//output player grid
			cout << setw(2) << tileToSymbol(player[row][col]);
		}
		cout << endl;
		let++;
	}
} //end displayBoards

bool placeShipHorizontally(Tile ship, int shipSize, int row, int col, 
	Tile board[][BOARD_LENGTH])
{
	int check = 0; //var for checking space to see if ship is there

	//if ship is outside of board
	if ((row < 0 || col < 0 || col + shipSize > BOARD_LENGTH ||
		row > BOARD_LENGTH ))
	{
		return false;
	}
	else
	{
		//loop through space to check for water
		for (int place = col; place < col + shipSize; place++)
		{

			//if there's no water, has to be a boat 
			if (board[row][place] != WATER)  
			{
				check++; //add to check var
			}
		}

		if (check > 0) //if var is anything but 0
		{
			return false; //exit function
		}
		
		else if (check == 0) //if var is 0
		
		{
			//we've got room!
			//cycle through space for ship and place it
			for (int place2 = col; place2 < col + shipSize; place2++)
			{
				board[row][place2] = ship;
			}

			return true;
		}

		return false; //if all else failed then return false
	}
		
} //end placeShipHorizontally

bool placeShipVertically(Tile ship, int shipSize, int row, int col, 
	Tile board[][BOARD_LENGTH])
{
	int check = 0;

	//if ship is outside of board
	if ((row < 0 || col < 0 || row + shipSize > BOARD_LENGTH ||
		col > BOARD_LENGTH - 1 ))
	{
		return false;
	}
	else
	{
		for (int place = row; place < row + shipSize; place++)
		{
			
			//if there's no water, has to be a boat 
			if (board[place][col] != WATER) 
			{
				check++;

			}
		}

		if (check > 0)
		{
			return false;
		} 
		
		else if (check == 0)
		{
			//we've got room!
			//cycle through space for ship and place it
			for (int place2 = row; place2 < row + shipSize; place2++)
			{
				board[place2][col] = ship;
			}
			return true;
		}
		return false;  //if all else failed then return false
	}
} //end placeShipVertically

void placeShipsRandomly(Tile board[][BOARD_LENGTH])
{
	int row, col; //vars for random numbers for placement
	bool isValid; //returns valid if ship was placed sucessfully

	wipeBoard(board, WATER); //wipe board first

	//cycle through ships
	for (int iter = AIRCRAFT_CARRIER; iter != MISS; static_cast<Tile>(iter++))
	{
		isValid = false; //set isValid to false to do loop
		do {
			//set rand nums
			row = rand() % 10;
			col = rand() % 10;

			if (rand() % 2) //if num is even...
			{
				//...place ship horizontally
				isValid = placeShipHorizontally(static_cast<Tile>(iter), 
					SHIP_SIZE[iter], row, col, board);
			}
			else //odd num...
			{
				//...place ship vertically
				isValid = placeShipVertically(static_cast<Tile>(iter), 
					SHIP_SIZE[iter], row, col, board);
			}
		
			// loop until isvalid is true to ensure all ships are placed
		} while (!isValid);  
	}	
} //end placeShipsRandomly

void placePlayersShips(Tile board[][BOARD_LENGTH])
{
	bool show = false; //showAll var
	bool loop = false; //var for while loop purposes
	char ans; //user input

	placeShipsRandomly(board); //place player ships on board

	displayBoards(board, board, show); //show boards

	//ask user if they want to use generated board
	//or use a new one
	cout << "Do you want to play with this board? (y/n): ";
	cin >> ans;

	while (!loop)
	{
		//if user hasn't entered y, Y, n, or N...
		if (ans != 'y' && ans != 'Y' && ans != 'n' && ans != 'N')
		{
			//...ignore input and ask again
			cin.ignore(100, '\n');
			cout << "Please enter 'y' or 'n': ";
			cin >> ans;
		}
		else if (ans == 'n' || ans == 'N') //if they did but said no
		{
			placeShipsRandomly(board); //place ships again
			displayBoards(board, board, show); //display again
			cout << "Do you want to play with this board? (y/n): ";//ask again
			cin >> ans; //take input again
		}
		else if (ans == 'y' || ans == 'Y') //if they said yes
		{
			loop = true; //end loop and continue
		}
	}
}

string playersTurn(Tile eBoard[][BOARD_LENGTH], int eHits[])
{
	char let; //var for row letter
	int row = 0, col = 0; //var for coordinates
	string message; //output message
	Tile tilePicked; //tile that user picked

	do {
		cout << "Enter the coordinates for your shot (e.g. B2): ";
		cin >> let >> col; //take in user coordinates
		toupper(col); //change to uppercase
		col--; //subtract one to account for array index

		//if coordinate input is valid (uppercase or lowercase A-J)
		//column greater than 0 and less than the length
		if (((let >= 'A' && let <= 'J') || (let >= 'a' && let <= 'j')) &&
			((col >= 0) && (col <= (BOARD_LENGTH - 1))))
		{
			switch (let) //set the row to corresponding letter
			{
			case 'A':
			case 'a':
				row = 0;
				break;
			case 'B':
			case 'b':
				row = 1;
				break;
			case 'C':
			case 'c':
				row = 2;
				break;
			case 'D':
			case 'd':
				row = 3;
				break;
			case 'E':
			case 'e':
				row = 4;
				break;
			case 'F':
			case 'f':
				row = 5;
				break;
			case 'G':
			case 'g':
				row = 6;
				break;
			case 'H':
			case 'h':
				row = 7;
				break;
			case 'I':
			case 'i':
				row = 8;
				break;
			case 'J':
			case 'j':
				row = 9;
				break;
			}
			
			//if the tile is a hit already
			if (eBoard[row][col] > MISS || eBoard[row][col] != WATER)
			{

				//let the user know
				cout << "You have already shot at " << let << toupper(col + 1) 
					<< endl;
				cout << "  Pick a new coordinate." << endl; //restart loop and ask again
			}
		} else {
		
			//if input doesn't match requirements let user know and ask again
			cout << "Invalid coordinates." << endl;
			cout << "  Pick a row value between A and J and a column value" 
				<< " between 1 and 10" << endl;
		}
	
	} while (eBoard[row][col] >= MISS);
	
	message = static_cast<char>(row + 'A') //concat coordinate into message var
		+ to_string(col + 1);

	// If we did not hit water, we hit a ship.
	if (eBoard[row][col] != WATER)
	{
		// Hit!
		message += " Hit!\n";

		// Update hits remaining
		tilePicked = eBoard[row][col];
		--eHits[tilePicked];

		// Check if ship was sunk
		if (eHits[tilePicked] == 0)
		{
			message += "  You sunk the enemy's "
				+ shipToString(tilePicked) + "!\n";
		}
	} // End handling a hit.
	else
	{
		// Miss.
		message += " Miss.\n";
	}

	// Update board to indicate the enemy's shot
	eBoard[row][col] =
		static_cast<Tile>(eBoard[row][col] + MISS);
	return message;

} //end playersTurn

bool isAllZeroes(int shotsRemaining[], int shotListSize)
{
	int zeroCheck = 0; //init var to check for 0's
    
	//loop through variable
	for (int count = 0; count < shotListSize; count++)
	{
		if (shotsRemaining[count] != 0) //if the num isn't 0
		{
			zeroCheck++; //add one to checker
		}
	}
    
	if (zeroCheck == 0) //if no 0's
	{
		return true; 
	} else {
		return false;
	}
}// end isAllZeroes

void displayAsIfTyped(const string message, int time, int pause)
{
	//loop through message
	for (size_t count = 0; count != message.size(); count++)
	{
		cout << message[count]; //output letter

		//pause each output for the amount of time var
		this_thread::sleep_for(chrono::milliseconds(time));
	}

	//pause entire line for the amount of pause var
	this_thread::sleep_for(chrono::milliseconds(pause));
} //end displayAsIfTyped
