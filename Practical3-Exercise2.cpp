#include <Windows.h>
#include <gl/GL.h>
#include <math.h>
#pragma comment(lib, "OpenGL32.lib")

#define WINDOW_TITLE "OpenGL Window"

void drawRectangle();
void interactiveRectangle();
void windmill();
void drawFan(int noOfLeave, float speed);
void drawBranch();
void drawLeave();
void drawCloud(float size);
void drawCirle(float x1, float y1, float radius);

int page = 1, noOfLeave = 4;
float x = 0, y = 0, speed = 0, rotateDegree = 0;

// use dedicated GPU to run
extern "C"
{
	__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}
extern "C"
{
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

LRESULT WINAPI WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			PostQuitMessage(0);
		else if (wParam == VK_F1)
		{
			page = 1;
		}
		else if (wParam == VK_F2)
		{
			page = 2;
		}
		else if (wParam == VK_LEFT || wParam == VK_RIGHT)
		{
			x += 0.1;
		}
		else if (wParam == VK_UP || wParam == VK_DOWN)
		{
			y += 0.1;
		}
		else if (wParam == VK_SPACE)
		{
			x = 0;
			y = 0;
			speed = 0;
		}
		else if (wParam == VK_NUMPAD8)
		{
			if (speed < 50)
			{
				speed += 0.5;
			}
		}
		else if (wParam == VK_NUMPAD2)
		{
			if (speed > -50)
			{
				speed -= 0.5;
			}
		}
		else if (wParam == VK_ADD)
		{
			if (noOfLeave < 6)
			{
				noOfLeave += 1;
			}
		}
		else if (wParam == VK_SUBTRACT)
		{
			if (noOfLeave > 1)
			{
				noOfLeave -= 1;
			}
		}
		break;
	default:
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
//--------------------------------------------------------------------

bool initPixelFormat(HDC hdc)
{
	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.cAlphaBits = 8;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 0;

	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;

	pfd.iLayerType = PFD_MAIN_PLANE;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;

	// choose pixel format returns the number most similar pixel format available
	int n = ChoosePixelFormat(hdc, &pfd);

	// set pixel format returns whether it sucessfully set the pixel format
	if (SetPixelFormat(hdc, n, &pfd))
	{
		return true;
	}
	else
	{
		return false;
	}
}
//--------------------------------------------------------------------

void display()
{
	// handle resolution problem
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	double w = 1920;
	double h = 1080;
	double ar = w / h;
	glOrtho(-2 * ar, 2 * ar, -2, 2, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	switch (page)
	{
	case 1:
		interactiveRectangle();
		break;
	case 2:
		windmill();
		break;
	default:
		break;
	}
}
//--------------------------------------------------------------------

void interactiveRectangle()
{
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	glPushMatrix();
	glTranslatef(x, y, 0.0);
	drawRectangle();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-x, -y, 0.0);
	drawRectangle();
	glPopMatrix();
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmdShow)
{
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpfnWndProc = WindowProcedure;
	wc.lpszClassName = WINDOW_TITLE;
	wc.style = CS_HREDRAW | CS_VREDRAW;

	if (!RegisterClassEx(&wc))
		return false;

	HWND hWnd = CreateWindow(WINDOW_TITLE, WINDOW_TITLE, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 1920, 1080,
		NULL, NULL, wc.hInstance, NULL);

	//--------------------------------
	//	Initialize window for OpenGL
	//--------------------------------

	HDC hdc = GetDC(hWnd);

	//	initialize pixel format for the window
	initPixelFormat(hdc);

	//	get an openGL context
	HGLRC hglrc = wglCreateContext(hdc);

	//	make context current
	if (!wglMakeCurrent(hdc, hglrc))
		return false;

	//--------------------------------
	//	End initialization
	//--------------------------------

	ShowWindow(hWnd, nCmdShow);

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	int changeStarColor = 0;
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		display();

		// // reset rotateDegree,
		// if ((int)rotateDegree % 360 == 0)
		// {
		//     rotateDegree = 0;
		// }
		// adding rotation degree based on speed
		rotateDegree += speed;

		SwapBuffers(hdc);
	}
	UnregisterClass(WINDOW_TITLE, wc.hInstance);
	return true;
}

void drawRectangle()
{
	glLoadIdentity();
	glColor3f(1, 0, 0);

	glPushMatrix();
	glTranslatef(-x, -y, 0);

	glBegin(GL_QUADS);
	glVertex2f(-0.2, 0.1);
	glVertex2f(0.2, 0.1);
	glVertex2f(0.2, 0.0);
	glVertex2f(-0.2, 0.0);
	glEnd();

	glPopMatrix();
	glTranslatef(x, y, 0);

	glBegin(GL_QUADS);
	glVertex2f(-0.2, 0.1);
	glVertex2f(0.2, 0.1);
	glVertex2f(0.2, 0.0);
	glVertex2f(-0.2, 0.0);
	glEnd();
}

void windmill()
{
	// 135, 206, 235
	glClearColor(0, 0.5, 1, 0);
	// glClearColor(0.5294, 0.8078, 0.9216, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	// cloud
	glPushMatrix();
	glTranslatef(0.3, 1.2, 0.0);
	drawCloud(0.15);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-1.8, 1.2, 0.0);
	drawCloud(0.15);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(1.8, 1.2, 0.0);
	drawCloud(0.15);
	glPopMatrix();

	// ground
	glColor3ub(129, 199, 132);
	glBegin(GL_QUADS);
	glVertex2f(5, -0.8);
	glVertex2f(-5, -0.8);
	glVertex2f(-5, -5);
	glVertex2f(5, -5);
	glEnd();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	// mountain
	glBegin(GL_TRIANGLES);
	glColor3ub(121, 85, 72);
	glVertex2f(-3.5, -0.8);
	glVertex2f(-1.75, 0.5);
	glColor3ub(251, 192, 45);
	glVertex2f(0, -0.8);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3ub(121, 85, 72);
	glVertex2f(0, -0.8);
	glVertex2f(1.75, 0.5);
	glColor3ub(251, 192, 45);
	glVertex2f(3.5, -0.8);
	glEnd();

	// windmill base
	glBegin(GL_QUADS);
	glColor3ub(144, 164, 174);
	glVertex2f(0.25, -0.3);
	glVertex2f(-0.25, -0.3);
	glColor3ub(96, 125, 139);
	glVertex2f(-0.45, -0.95);
	glVertex2f(0.45, -0.95);
	glEnd();

	//body
	glBegin(GL_QUADS);
	glColor3ub(158, 158, 158);
	glVertex2f(-0.1, -0.3);
	glVertex2f(0.1, -0.3);
	glColor3ub(224, 224, 224);
	glVertex2f(0.1, 0.65);
	glVertex2f(-0.1, 0.65);
	glEnd();

	//// body
	//glColor3ub(171, 171, 171);
	//glBegin(GL_QUADS);
	//glVertex2f(-0.45, -0.65);
	//glVertex2f(0.45, -0.65);
	//glVertex2f(0.3, 0.5);
	//glVertex2f(-0.3, 0.5);
	//glEnd();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);


	drawFan(noOfLeave, speed);

	// fan center
	glColor3ub(0,0,0);
	glEnable(GL_POINT_SMOOTH);
	glPointSize(15);
	glBegin(GL_POINTS);
	glVertex2f(0, 0.6);
	glEnd();
}

void drawFan(int noOfLeave, float speed)
{
	float degree = 360 / noOfLeave;

	for (int i = 0; i < noOfLeave; i++)
	{
		glPushMatrix();
		glTranslatef(0, 0.6, 0.0);
		glRotatef(rotateDegree, 0, 0, 1);
		{
			drawBranch();
		}
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0, 0.6, 0.0);
		glColor3ub(174, 93, 33);
		glRotatef(rotateDegree, 0, 0, 1);
		{
			drawLeave();
		}
		glPopMatrix();

		rotateDegree = rotateDegree - degree - speed;
	}
}

void drawBranch()
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glBegin(GL_QUADS);
	{
		glColor3ub(94, 53, 177);
		glVertex2f(0, 0);
		glVertex2f(-0.04, 0);
		glVertex2f(-0.04, 0.1);
		glVertex2f(0, 0.1);
	}
	glEnd();
}

void drawLeave()
{
	glBegin(GL_QUADS);
	glColor3f(213, 0, 0);
	glVertex2f(0.16, 0.10);
	glVertex2f(-0.04, 0.10);
	glVertex2f(-0.04, 0.85);
	glVertex2f(0.16, 0.85);
	glEnd();
}

void drawCloud(float size)
{
	drawCirle(0, 0, size);
	drawCirle(0.2, 0, size);

	drawCirle(0.35, 0.1, size);

	drawCirle(0.2, 0.2, size);
	drawCirle(0, 0.2, size);

	drawCirle(-0.15, 0.1, size);
}

void drawCirle(float x1, float y1, float radius)
{
	float x2 = x1, y2 = y1;

	glColor3ub(246, 246, 246);
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(x1, y1);
	for (float angle = 0; angle <= 360; angle += 0.1)
	{
		x2 = x1 + cos(angle) * radius;
		y2 = y1 + sin(angle) * radius;
		glVertex2f(x2, y2);
	}
	glEnd();
}