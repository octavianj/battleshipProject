/*
 * This file holds global constants, data types, and their associated helper
 * functions for the game of battleship.
 */
#ifndef ___GAME_SPECS___
#define ___GAME_SPECS___

#include <string>
using namespace std;

//declare tiles
enum Tile {WATER, AIRCRAFT_CARRIER, BATTLESHIP, SUBMARINE, DESTROYER, 
	PATROL_BOAT, MISS, AIRCRAFT_CARRIER_HIT, BATTLESHIP_HIT, SUBMARINE_HIT,
	DESTROYER_HIT, PATROL_BOAT_HIT};

//take ship and convert to corresponding symbol
char tileToSymbol(Tile ship);

//take ship and convert to corresponding string
string shipToString(Tile ship);

const int BOARD_LENGTH = 10; //set board length
const int SHIP_SIZE[] = { 0, 5, 4, 3, 3, 2 }; //init ship_size array

char tileToSymbol(Tile ship) 
{
	switch (ship) 
	{
	case WATER:
		return '.';
		break;
	case AIRCRAFT_CARRIER:
		return 'A';
		break;
	case AIRCRAFT_CARRIER_HIT:
		return 'a';
		break;
	case BATTLESHIP:
		return 'B';
		break;
	case BATTLESHIP_HIT:
		return 'b';
		break;
	case SUBMARINE:
		return 'S';
		break;
	case SUBMARINE_HIT:
		return 's';
		break;
	case DESTROYER:
		return 'D';
		break;
	case DESTROYER_HIT:
		return 'd';
		break;
	case PATROL_BOAT:
		return 'P';
		break;
	case PATROL_BOAT_HIT:
		return 'p';
		break;
	case MISS:
		return '~';
		break;
	default:
		cout << "Error: invalid tile.";
		return ' ';
		break;
	}
}

string shipToString(Tile ship) 
{
	switch (ship)
	{
	case WATER:
		return "Water";
		break;
	case MISS:
		return "Miss";
		break;
	case AIRCRAFT_CARRIER:
	case AIRCRAFT_CARRIER_HIT:
		return "Aircraft Carrier";
		break;
	case BATTLESHIP:
	case BATTLESHIP_HIT:
		return "Battleship";
		break;
	case SUBMARINE:
	case SUBMARINE_HIT:
		return "Submarine";
		break;
	case DESTROYER:
	case DESTROYER_HIT:
		return "Destroyer";
		break;
	case PATROL_BOAT:
	case PATROL_BOAT_HIT:
		return "Patrol Boat";
		break;
	default:
		return "Error";
		break;
	}
}

#endif