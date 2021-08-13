/*
 * Functions used for the AI to choose its targets. The noteworthy function is
 * enemyTurn(), which should be called when it is the computer's turn.
 * by Sean T. Hayes
 *
 * Version 1.11 (Included missing string header)
 * Version 1.10 (Removed stack and accounted for hits in the prob dist.)
 *
 * If SIMULATION is defined, hit and miss information is not displayed.
 *
 * Some of this AI was inspired from the DataGenetics post found here:
 * https://datagenetics.com/blog/december32011/
 *
 * Instead of using a stack, a probability bonus the areas near a known hit.
 * Concern: Can we know for sure if a previous hit was on a ship
 *          that is now sunk? Right now, that is assumed (slight cheat?).
 *
 */
#ifndef ___ENEMYAI___
#define ___ENEMYAI___

#include <iostream>
#include <iomanip>
#include <string>
#include <cstdlib>
#include <vector>
using namespace std;

#include "gameSpecs.h"

/**
 * Namespace to encapsulate all the game logic for the computer player.
 */
namespace EnemyAI
{
	/**
	 * This function semi-intelligently chooses the shot for the enemy
	 * (computer) to make. It displays the shot information to the user and
	 * updates the playersBoard and hitsRemainingPlayer accordingly.
	 * @param playersBoard        The 2D array containing the player's fleet.
	 * @param hitsRemainingPlayer An array containing the remaining hits before
	 *                            each ship is sunk.
	 * @return                    The text to display to the user containing the
	 *                            information about the hit/miss.
	 */
	string enemyTurn(Tile playersBoard[][BOARD_LENGTH],
		int hitsRemainingPlayer[]);

	/**
	 * Takes a blank 2D array to create a probably matrix of possible locations
	 * for a ship of all remaining ships.
	 * @param playersBoard       Used to check previous miss locations.
	 * @param shipLocProbability Assumed to already initialized to all zeros.
	 */
	void calculateProbabilityMatrix(const Tile playersBoard[][BOARD_LENGTH],
		int shipLocProbability[][BOARD_LENGTH], int hitsRemaining[]);

	/**
	 * Returns the location of the highest probability. If multiple locations
	 * are equal to the max, one location is returned at random.
	 * @param  matrix The probability matrix to be used.
	 * @return        The 1D position. Divide by 2 to get the row and the
	 *                remainder is the column.
	 */
	int randomFromMaxProbability(const int matrix[][BOARD_LENGTH]);

	/**
	 * Picks completely a position completely at random based on the probability
	 * matrix. This function could be used for an easier version of the game.
	 * @param  matrix The probability matrix to be used. Must be not all zero.
	 * @return        The 1D position. Divide by 2 to get the row and the
	 *                remainder is the column.
	 */
	int randomFromProbabilityMatrix(const int matrix[][BOARD_LENGTH]);
} // namespace EnemyAI

string EnemyAI::enemyTurn (Tile playersBoard [][BOARD_LENGTH],
	int hitsRemainingPlayer[])
{
	int shotRow, shotCol; // will hold the shot to make this turn.

	// A matrix containing the unnormalized probabilities of each tile
	// containing the biggest ship left on the board.
	int shipLocProbability[BOARD_LENGTH][BOARD_LENGTH] = {0};

	int enemyPick; // Location of pick in 1D (row * BOARD_LENGTH + col)
	Tile tilePicked; // The tile value for the chosen shot.
	string outputMessage; // the information about the shot to be returned.

	// Keep looking for a good pick until we find one that has not been
	// previously selected.
	do
	{
		// perform educated guess.
		// Calculated a the probability of any location containing the
		// biggest ship.
		EnemyAI::calculateProbabilityMatrix(playersBoard, shipLocProbability,
			hitsRemainingPlayer);

		// Pick the next shot randomly based on probable locations.
		enemyPick = EnemyAI::randomFromMaxProbability(shipLocProbability);
		shotRow = enemyPick / BOARD_LENGTH;
		shotCol = enemyPick % BOARD_LENGTH;

	} while(playersBoard[shotRow][shotCol] >= MISS); // Get a new shot.

	outputMessage = static_cast<char>(shotRow + 'A')
		+ std::to_string(shotCol + 1);

	// If we did not hit water, we hit a ship.
	if (playersBoard[shotRow][shotCol] != WATER)
	{
		// Hit!
		outputMessage += " Hit!\n";
		//displayAsIfTyped(" Hit!");

		// Update hits remaining
		tilePicked = playersBoard[shotRow][shotCol];
		--hitsRemainingPlayer[tilePicked];

		// Check if ship was sunk
		if (hitsRemainingPlayer[tilePicked] == 0)
		{
			outputMessage += "\tThe enemy has sunk your "
				+ shipToString(tilePicked) + "!\n";
		}
	} // End handling a hit.
	else
	{
		// Miss.
		outputMessage += " Miss.\n";
		//displayAsIfTyped(" Miss.\n");
	}

	// Update board to indicate the enemy's shot
	playersBoard[shotRow][shotCol] =
		static_cast<Tile>(playersBoard[shotRow][shotCol] + MISS);

	return outputMessage;
}

void EnemyAI::calculateProbabilityMatrix (
	const Tile playersBoard [][BOARD_LENGTH],
	int shipLocProbability[][BOARD_LENGTH], int hitsRemaining[])
{
	bool isPossible; // true if a ship of shipLength could fit at this location.
	int rowIndex;	// for iterating the board and prob. matrix
	int colIndex;	// for iterating the board and prob. matrix
	int posShipLoc;	// for iterating the positions in a possible ship location.
	Tile currentTile; // The position on the board we are considering.
	int shipLength; // used to track the ship length of the current ship.

	// set higher if we have already hit something in this potential location
	int bonusLikelyhood;

	// Consider every ship that has not been sunk
	for (Tile ship = static_cast<Tile>(1); ship < MISS;
		ship = static_cast<Tile>(ship + 1))
	{
		if (hitsRemaining[ship] == 0)
			continue; // ship sunk ships

		shipLength = SHIP_SIZE[ship];

		// Look at the possible horizontal positions for the ship.
		for (rowIndex = 0; rowIndex < BOARD_LENGTH; ++rowIndex)
		{
			for (colIndex = 0; colIndex <= BOARD_LENGTH - shipLength;
				++colIndex)
			{
				isPossible = true; // Assume possible until we find otherwise
				bonusLikelyhood = 1;

				// Check if the ship can fit in this space
				for (posShipLoc = 0; posShipLoc < shipLength; ++posShipLoc)
				{
					currentTile = playersBoard[rowIndex][colIndex + posShipLoc];

					// If we have fired a shot here and missed and shank the
					// ship, then the ship cannot fit.
					// There is a little cheat here. A human may not know
					// exactly, which ship a hit of a sunk ship is.
					// However, usually it is obvious.
					if (currentTile == MISS ||
						(currentTile > MISS &&
						hitsRemaining[currentTile - MISS] == 0))
					{
						// Flag this sequence as not possible
						isPossible = false;
						colIndex += posShipLoc;
						break;  // end the inner loop early
					}
					else if (currentTile > MISS) // hit on floating ship
					{
						// bonus for overlap with hit
						bonusLikelyhood += 6 - SHIP_SIZE[ship];
					}
				}

				if (isPossible)
				{
					// Add 1 to the probability of all the locations for this
					// ship position
					for (posShipLoc = colIndex;
						posShipLoc < colIndex + shipLength; ++posShipLoc)
					{
						currentTile = playersBoard[rowIndex][posShipLoc];
						if(currentTile < MISS)
						{
							shipLocProbability[rowIndex][posShipLoc]
								+= bonusLikelyhood;
						}
					}
				}
			}
		}

		// Look at the possible vertical positions for the ship.
		for (colIndex = 0; colIndex < BOARD_LENGTH; ++colIndex)
		{
			for (rowIndex = 0; rowIndex <= BOARD_LENGTH - shipLength;
				++rowIndex)
			{
				isPossible = true; // Assume possible until we find otherwise
				bonusLikelyhood = 1;

				// Check if the ship can fit in this space
				for (posShipLoc = 0; posShipLoc < shipLength; ++posShipLoc)
				{
					currentTile = playersBoard[rowIndex + posShipLoc][colIndex];

					// If we have fired a shot here and missed and shank the
					// ship, then the ship cannot fit.
					if (currentTile == MISS ||
						(currentTile > MISS &&
						hitsRemaining[currentTile - MISS] == 0))
					{
						// Flag this sequence as not possible
						isPossible = false;
						rowIndex += posShipLoc;
						break;  // end the inner loop early
					}
					else if (currentTile > MISS)  // hit on unsunk ship
					{
						// bonus for hit on ship
						bonusLikelyhood += 6 - SHIP_SIZE[ship];
					}
				}

				if (isPossible)
				{
					// Add 1 to the probability of all the locations for this
					// ship position
					for (posShipLoc = rowIndex;
						posShipLoc < rowIndex + shipLength; ++posShipLoc)
					{
						currentTile = playersBoard[posShipLoc][colIndex];
						if(currentTile < MISS)
						{
							shipLocProbability[posShipLoc][colIndex]
								+= bonusLikelyhood;
						}
					}
				}
			}
		}
	} // End ship loop

	// // Output the probability matrix for debugging
	// for (rowIndex=0; rowIndex < BOARD_LENGTH; ++rowIndex)
	// {
	// 	cout << setw(2) << static_cast<char>(rowIndex + 'A');
	// 	for (colIndex=0; colIndex < BOARD_LENGTH; ++colIndex)
	// 	{
	// 		cout << setw(3) << shipLocProbability[rowIndex][colIndex];
	// 	}
	// 	cout << endl;
	// }
}

int EnemyAI::randomFromMaxProbability (const int matrix[][BOARD_LENGTH])
{
	int max = 0;
	int rowIndex, colIndex;
	vector<int> highestProbLocations;

	// Find Max value and locations with the max value;
	for (rowIndex = BOARD_LENGTH - 1; rowIndex >= 0; --rowIndex)
	{
		for (colIndex = BOARD_LENGTH - 1; colIndex >= 0; --colIndex)
		{
			if (matrix[rowIndex][colIndex] >= max)
			{
				if (matrix[rowIndex][colIndex] > max)
				{
					highestProbLocations.clear();
					max = matrix[rowIndex][colIndex];
				}
				highestProbLocations.push_back(
					rowIndex * BOARD_LENGTH + colIndex);
			}
		}
	}

	// Select 1 location at random
	return highestProbLocations[rand() % highestProbLocations.size()];
}

int EnemyAI::randomFromProbabilityMatrix (const int matrix[][BOARD_LENGTH])
{
	int total = 0; // total of all the numbers in the matrix (array)
	int randProb; // a random number between 0 and the total
	int rowIndex; // used for iterating over the matrix
	int colIndex; // used for iterating over the matrix

	// Calculate total
	for (rowIndex = BOARD_LENGTH - 1; rowIndex >= 0; --rowIndex)
	{
		for (colIndex = BOARD_LENGTH - 1; colIndex >= 0; --colIndex)
		{
			total += matrix[rowIndex][colIndex];
		}
	}

	// Pick random number between all the possibilities
	randProb = rand() % total;

	// Calculate total up to random number
	total = 0; // recalculate total until random number is reached.
	for (rowIndex = BOARD_LENGTH - 1; rowIndex >= 0; --rowIndex)
	{
		for (colIndex = BOARD_LENGTH - 1; colIndex >= 0; --colIndex)
		{
			total += matrix[rowIndex][colIndex];
			if (total >= randProb)
			{
				// Return the 1D index at this point.
				return rowIndex * BOARD_LENGTH + colIndex;
			}
		}
	}

	// This should never happen, because our random number is always less
	// than the total.
	cerr << "ERROR: Invalid Probability" << endl;

	return 0;
}

#endif