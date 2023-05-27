// holds logic for the UI for the Main Menu of our Pong Game
/*
	We should have a title for the game, and 2 buttons representing the options
	1) single player
	2) multiplayer
	3) slider for ball speed (how fast the ball moves)
	4) slider for bar speed (how fast the pong paddles can move)
	5) slider for maximum score (when should the game end?)
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

		float ballSpeed = 1.0f;
		float barSpeed = 1.0f;
		int maxScore = 5;
		ImGui::SliderFloat("ball speed:", &ballSpeed, 0.0f, 10.0f, "%.2f");
		ImGui::SliderFloat("paddle speed:", &barSpeed, 0.0f, 10.0f, "%.2f");
		ImGui::SliderInt("maximum score:", &maxScore, 1, 20);
		
	}
	ImGui::End();
}
