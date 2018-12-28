#include "application.h"

ApplicationSettings info;

void InitializeApplication(ApplicationSettings newInfo)
{
	info = newInfo;
}

ApplicationSettings GetApplicationSettings()
{
	return info;
}