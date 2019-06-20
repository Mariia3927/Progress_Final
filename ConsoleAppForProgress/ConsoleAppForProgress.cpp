// ConsoleAppForProgress.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>

int main()
{
	ProgressObserver observer;
	Progress mainProgress;
	mainProgress.AddObserver(&observer);

	Progress childProgress1(&mainProgress);
	mainProgress.AddChild(&childProgress1);
	Progress childProgress2(&mainProgress);
	mainProgress.AddChild(&childProgress2);
	Progress childProgress3(&mainProgress);
	mainProgress.AddChild(&childProgress3);

	//std::vector<std::string> fileNames = { "The_Hardkiss_Stones.mp3", "The_Hardkiss_Stones.mp3", "The_Hardkiss_Stones.mp3" };

	std::vector<std::string> fileNames = { "Odin_doma_3.avi", "Odin_doma_3.avi", "Odin_doma_3.avi" };

	CopyFiles(fileNames, &mainProgress);

	mainProgress.RemoveObserver(&observer);
	mainProgress.RemoveChild(&childProgress1);
	mainProgress.RemoveChild(&childProgress2);
	mainProgress.RemoveChild(&childProgress3);
}

