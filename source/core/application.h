#pragma once

struct ApplicationSettings
{
	bool vsync = false;
	bool fullscreen = false;
	int windowWidth = 0;
	int windowHeight = 0;
};

void InitializeApplication(ApplicationSettings newInfo);
ApplicationSettings GetApplicationSettings();