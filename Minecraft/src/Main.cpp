#include "pch.h"

#include "Application.h"

Application* g_Application;

int main(void)
{
	Application app;
	g_Application = &app;
	
	if (!app.CreateContext())
		return -1;
	
	app.Initialize();
	app.StartLoop();

	return 0;
}