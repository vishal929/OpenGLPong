// holds logic for the UI for the Main Menu of our Pong Game
/*
	We should have a title for the game, and 3 buttons representing the options
	1) single player
	2) multiplayer
	3) settings
*/

#include "imgui.h"

// builds the UI view for our main menu
void buildMenu() {
	if (ImGui::Begin("Menu",false,ImGuiWindowFlags_AlwaysAutoResize)) {

		if (ImGui::Button("Single Player")) {
			// transition state to the single player pong game
		}

		if (ImGui::Button("Multi-Player")) {
			// transition state to the multiplayer connection window
		}

		if (ImGui::Button("Settings")) {
			// transition state to the settings window
		}
	}
	ImGui::End();
}
