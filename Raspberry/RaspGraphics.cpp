#include "ImGui-master/imgui.h"
#include "ImGui-master/backends/imgui_impl_x11.h"
#include "ImGui-master/backends/imgui_impl_opengl3.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "RaspGraphics.h"
#include <EGL/egl.h>
#include "EGLState.h"
#include "RaspMouse.h"
#include "XWindow.h"
#include "Game.h"

RaspGraphics::RaspGraphics() : window(new XWindow())
{
	window->CreateWindow();
}

void RaspGraphics::Quit()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplX11_Shutdown();
	ImGui::DestroyContext();
	XDestroyWindow(&window->GetDisplay(), window->GetWindow());
}

void RaspGraphics::SwapBuffer()
{
	EGLState state = window->GetState();
	eglSwapBuffers(state.display, state.surface);
}

void RaspGraphics::LockCursor()
{
	printf("Cursor locking not implemented on X11\n");
}

void RaspGraphics::UnlockCursor()
{
	printf("Cursor unlocking not implemented on X11\n");
}

void RaspGraphics::BeginFrame()
{
	// (Your code calls glfwPollEvents())
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplX11_NewFrame();
	ImGui::NewFrame();
}

void RaspGraphics::EndFrame()
{
    PollEvents();

    // (Your code clears your framebuffer, renders your other stuff etc.)
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	// (Your code calls glfwSwapBuffers() etc.)
}

void RaspGraphics::InitGUI()
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Platform/Renderer backends
	ImGui_ImplX11_Init(&window->GetDisplay(), window->GetWindow());          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
	ImGui_ImplOpenGL3_Init("#version 100");
}

void RaspGraphics::PollEvents()
{
    XEvent event;
    while (XPending(&window->GetDisplay()) > 0)
    {
        XNextEvent(&window->GetDisplay(), &event);
        ImGui_ImplX11_ProcessEvent(&event);
    }
}

XWindow& RaspGraphics::Window() const
{
	return *window;
}
