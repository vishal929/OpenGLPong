// functions for our main menu view 
// mainMenu.hpp
#ifndef MAINMENU_H
#define MAINMENU_H

/*
	Function that builds the main menu UI for our pong game
	gameState is a pointer to our game state setting: 0 for menu, 1 for single player, and 2 for multiplayer
	setBallSpeed is a pointer to our ball speed to use (modified by slider in this menu)
	setBarSpeed is a pointer to our bar speed setting (modified by slider in this menu)
	setMaxScore is a pointer to our max score setting (modified by slider in this menu)
*/
void buildMenu(int* gameState, float* setBallSpeed, float* setBarSpeed, int* setMaxScore);

#endif

