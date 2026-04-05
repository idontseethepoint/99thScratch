#include "Application.h"

Application* Application::_app = new Application();

Application* Application::App()
{
	return _app;
}

Application::Application() {}
