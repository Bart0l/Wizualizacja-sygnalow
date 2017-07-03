// draw.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "draw2.h"
#include <vector>
#include <cstdio>
#include <string>
#include <iostream>
#include <fstream>
#include <math.h>

#define M_PI 3.14159265358979323846

#define MAX_LOADSTRING 100
#define TMR_1 1

#define DATA_AMOUNT				1141			// amount of samples

#define GRAPH_POS_X				0
#define GRAPH_POS_Y				10
#define GRAPH_WIDTH				DATA_AMOUNT		//nice init value (one pixel, one sample)
#define GRAPH_HEIGHT			200
#define	GRAPH_ZERO_Y			GRAPH_POS_Y + (GRAPH_HEIGHT / 2)

#define COMPASS_POS_X			1200
#define COMPASS_POS_Y			10
#define COMPASS_SIZE			200
#define COMPASS_POINTER_LENGHT	90

RECT compass_area = { COMPASS_POS_X, COMPASS_POS_Y, COMPASS_POS_X + COMPASS_SIZE + 1, COMPASS_POS_Y + COMPASS_SIZE + 1};
RECT graph_area = { GRAPH_POS_X , GRAPH_POS_Y, GRAPH_POS_X + GRAPH_WIDTH + 1, GRAPH_POS_Y + GRAPH_HEIGHT + 1};


// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

int frequency = 1;
unsigned int lastTime = 0;							// tells how much data do we have already draw
unsigned int maxTime = 0;							// tells how much data do we have collected
unsigned int maxAmptitude = 0;						// to auto scale graph
unsigned int maxAmplitudeMultipler;					// to multiply data from input to fit in frame
unsigned int dataAmount = DATA_AMOUNT;				// how many samples do we have to fit in frame
float scaller = 1;									// to change time scale

bool draw_x = false;
bool draw_y = false;
bool draw_z = false;

// buttons
HWND hwndButton;
HWND hwndTextBox1;

// sent data - to draw
std::vector<float> data_roll;
std::vector<float> data_pitch;
std::vector<float> data_yaw;

// data_backup								// needs to ignore first few samples
std::vector<float> backup_data_roll;
std::vector<float> backup_data_pitch;
std::vector<float> backup_data_yaw;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Buttons(HWND, UINT, WPARAM, LPARAM);


//
//	Graph drawing
//

void DrawGraphFrame(HDC hdc) {
	Graphics graphics(hdc);
	Pen pen(Color(255, 0, 0, 0));
	graphics.DrawRectangle(&pen, Rect(GRAPH_POS_X, GRAPH_POS_Y, GRAPH_WIDTH, GRAPH_HEIGHT));
	graphics.DrawLine(&pen, GRAPH_POS_X, GRAPH_POS_Y + (GRAPH_HEIGHT / 2), GRAPH_POS_X + GRAPH_WIDTH, GRAPH_POS_Y + (GRAPH_HEIGHT / 2));
}


void DrawX(HDC hdc){
	Graphics graphics(hdc);
	Pen pen_roll(Color(255, 0, 0, 255));	// x
	for (int i = 1; i < lastTime; i++)
		graphics.DrawLine(&pen_roll, (int)((i-1)*scaller), (int) (((data_roll[i - 1] * GRAPH_HEIGHT / 2) / maxAmptitude) + GRAPH_ZERO_Y), (int)(i*scaller), (int)(((data_roll[i] * GRAPH_HEIGHT / 2) / maxAmptitude) + GRAPH_ZERO_Y));

}


void DrawY(HDC hdc){
	Graphics graphics(hdc);
	Pen pen_pitch(Color(255, 0, 255, 0));	// y
	for (int i = 1; i < lastTime; i++)
		graphics.DrawLine(&pen_pitch, (int)((i - 1)*scaller), (int)(((data_pitch[i - 1] * GRAPH_HEIGHT / 2) / maxAmptitude) + GRAPH_ZERO_Y), (int)(i*scaller), (int)(((data_pitch[i - 1] * GRAPH_HEIGHT / 2) / maxAmptitude) + GRAPH_ZERO_Y));

}


void DrawZ(HDC hdc){
	Graphics graphics(hdc);
	Pen pen_yaw(Color(255, 255, 0, 0));		// z
	for (int i = 1; i < lastTime; i++)
		graphics.DrawLine(&pen_yaw, (int)((i - 1)*scaller), (int)(((data_yaw[i - 1] * GRAPH_HEIGHT / 2) / maxAmptitude) + GRAPH_ZERO_Y), (int)(i*scaller), (int)(((data_yaw[i - 1] * GRAPH_HEIGHT / 2) / maxAmptitude) + GRAPH_ZERO_Y));

}


void DrawStepX(HDC hdc, unsigned int step) {
	Graphics graphics(hdc);
	Pen pen_roll(Color(255, 0, 0, 255));	// x
	graphics.DrawLine(&pen_roll, (int)((step - 1)*scaller), (int)(((data_roll[step - 1] * GRAPH_HEIGHT / 2) / maxAmptitude) + GRAPH_ZERO_Y), (int)(step*scaller), (int)(((data_roll[step] * GRAPH_HEIGHT / 2) / maxAmptitude) + GRAPH_ZERO_Y));
}


void DrawStepY(HDC hdc, unsigned int step) {
	Graphics graphics(hdc);
	Pen pen_pitch(Color(255, 0, 255, 0));	// y
	graphics.DrawLine(&pen_pitch, (int)((step - 1)*scaller), (int)(((data_pitch[step - 1] * GRAPH_HEIGHT / 2) / maxAmptitude) + GRAPH_ZERO_Y), (int)(step*scaller), (int)(((data_pitch[step] * GRAPH_HEIGHT / 2) / maxAmptitude) + GRAPH_ZERO_Y));
}


void DrawStepZ(HDC hdc, unsigned int step) {
	Graphics graphics(hdc);
	Pen pen_yaw(Color(255, 255, 0, 0));		// z
	graphics.DrawLine(&pen_yaw, (int)((step - 1)*scaller), (int)(((data_yaw[step - 1] * GRAPH_HEIGHT / 2) / maxAmptitude) + GRAPH_ZERO_Y), (int)(step*scaller), (int)(((data_yaw[step] * GRAPH_HEIGHT / 2) / maxAmptitude) + GRAPH_ZERO_Y));
}


void RepaintGraph(HWND hWnd, HDC &hdc, PAINTSTRUCT &ps){
	InvalidateRect(hWnd, &graph_area, TRUE); // repaint all
	hdc = BeginPaint(hWnd, &ps);
	DrawGraphFrame(hdc);
	if (draw_x)
		DrawX(hdc);
	if (draw_y)
		DrawY(hdc);
	if (draw_z)
		DrawZ(hdc);
	EndPaint(hWnd, &ps);
}


void RepaintGraphFrame(HWND hWnd, HDC &hdc, PAINTSTRUCT &ps) {
	InvalidateRect(hWnd, &graph_area, TRUE);
	hdc = BeginPaint(hWnd, &ps);
	DrawGraphFrame(hdc);
	EndPaint(hWnd, &ps);
}


void DrawOneStep(HWND hWnd, HDC &hdc, PAINTSTRUCT &ps) {
	std::cout << lastTime*scaller << "\n";
	std::cout << lastTime << " / " << dataAmount << "\n";
		RECT drawArea = { (GRAPH_POS_X + lastTime)*scaller, GRAPH_POS_Y, (GRAPH_POS_X + lastTime + 2)*scaller, GRAPH_POS_Y + GRAPH_HEIGHT };

		InvalidateRect(hWnd, &drawArea, TRUE); // repaint
		hdc = BeginPaint(hWnd, &ps);

		Graphics graphics(hdc);
		Pen pen(Color(255, 255, 0, 0));

		lastTime++;
		DrawGraphFrame(hdc);
		if (draw_x)
			DrawStepX(hdc, lastTime);
		if (draw_y)
			DrawStepY(hdc, lastTime);
		if (draw_z)
			DrawStepZ(hdc, lastTime);

		EndPaint(hWnd, &ps);
}

//
//	Compass drawing
//

void DrawCompass(HDC hdc, int angle) {
	double X = COMPASS_POS_X + (COMPASS_SIZE / 2) + (COMPASS_POINTER_LENGHT * sin((double)angle * M_PI / 180));
	double Y = COMPASS_POS_Y + (COMPASS_SIZE / 2) - (COMPASS_POINTER_LENGHT * cos((double)angle * M_PI / 180));
	Graphics graphics(hdc);
	Pen pen(Color(255, 0, 0, 0));
	graphics.DrawEllipse(&pen, COMPASS_POS_X, COMPASS_POS_Y, COMPASS_SIZE, COMPASS_SIZE);
	graphics.DrawLine(&pen, COMPASS_POS_X + COMPASS_SIZE / 2, COMPASS_POS_Y + COMPASS_SIZE / 2, X, Y);
}


void RepaintCompass(HWND hWnd, HDC &hdc, PAINTSTRUCT &ps, int angle) {
	InvalidateRect(hWnd, &compass_area, TRUE);
	hdc = BeginPaint(hWnd, &ps);
	DrawCompass(hdc, angle);
	EndPaint(hWnd, &ps);
}

//
//	Data input and calculating
//

void DeleteSamples(unsigned int samples) {
	dataAmount = DATA_AMOUNT - samples;
	std::vector<float> temp_data_roll;
	std::vector<float> temp_data_pitch;
	std::vector<float> temp_data_yaw;
	for (size_t i = 0; i < DATA_AMOUNT - samples; i++) {
		temp_data_roll.push_back(backup_data_roll[i + samples]);
		temp_data_pitch.push_back(backup_data_pitch[i + samples]);
		temp_data_yaw.push_back(backup_data_yaw[i + samples]);
	}
	data_roll = temp_data_roll;
	data_pitch = temp_data_pitch;
	data_yaw = temp_data_yaw;
}


void SetSamplesToDeleeteAndDelete() {
	// getting value from textbox
	int len = GetWindowTextLengthW(hwndTextBox1) + 1;
	std::vector<wchar_t> str(len);
	GetWindowTextW(hwndTextBox1, &str[0], str.size());
	int value = 0;

	// convert string to int
	for (size_t i = 0; i < len - 1; i++) {
		value += (str[i] - 48) * pow(10, len - i - 2);
	}

	DeleteSamples(value);
}


void CheckMaxAmplitude(int amplitude) {
	if (amplitude > (int)maxAmptitude) {
		maxAmptitude = amplitude;
	}
	if (-amplitude > (int)maxAmptitude) {
		maxAmptitude = -amplitude;
	}
}


unsigned int GetMaxAmplitudeMultipler() { return ((GRAPH_HEIGHT / 2) / maxAmptitude); }


void inputData(){	
	std::fstream file("datainput.log");
	if (!file) {
		// error
	}
	float number;
	for (size_t i = 0; i < 2000; i++)
	{
		file >> number;
		data_roll.push_back(number);
		CheckMaxAmplitude(number);

		file >> number;
		data_pitch.push_back(number);
		CheckMaxAmplitude(number);

		file >> number;
		data_yaw.push_back(number);
		CheckMaxAmplitude(number);

		for (size_t j = 0; j < 9; j++)
		{
			file >> number;
		}

		maxTime++;
	}
	maxAmplitudeMultipler = GetMaxAmplitudeMultipler();

	backup_data_roll = data_roll;
	backup_data_pitch = data_pitch;
	backup_data_yaw = data_yaw;
}


int OnCreate(HWND window){
	inputData();
	return 0;
}


// main function (exe hInstance)
int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{

	//// *****show console***** //
	//AllocConsole();
	//AttachConsole(GetCurrentProcessId());
	//FILE *stream;
	//freopen_s(&stream, "CON", "w", stdout);
	//// ********************** //

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_DRAW, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);



	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DRAW));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	GdiplusShutdown(gdiplusToken);

	return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DRAW));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_DRAW);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;


	hInst = hInstance; // Store instance handle (of exe) in our global variable

	// main window
	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	// create button and store the handle                                                       
	
	hwndButton = CreateWindow(TEXT("button"),                      // The class name required is button
		TEXT("<---"),                  // the caption of the button
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,  // the styles
		600, 220,                                  // the left and top co-ordinates
		80, 50,                              // width and height
		hWnd,                                 // parent window handle
		(HMENU)ID_BUTTON1,                   // the ID of your button
		hInstance,                            // the instance of your application
		NULL);                               // extra bits you dont really need

	hwndButton = CreateWindow(TEXT("button"),                      // The class name required is button
		TEXT("--->"),                  // the caption of the button
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,  // the styles
		700, 220,                                  // the left and top co-ordinates
		80, 50,                              // width and height
		hWnd,                                 // parent window handle
		(HMENU)ID_BUTTON2,                   // the ID of your button
		hInstance,                            // the instance of your application
		NULL);                               // extra bits you dont really need

	hwndButton = CreateWindow(TEXT("static"),                      // The class name required is button
		TEXT("samples to ignore"),                  // the caption of the button
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,  // the styles
		500, 300,                                  // the left and top co-ordinates
		150, 20,                              // width and height
		hWnd,                                 // parent window handle
		(HMENU)ID_TEXTBOX1,                   // the ID of your button
		hInstance,                            // the instance of your application
		NULL);                               // extra bits you dont really need

	hwndTextBox1 = CreateWindow(TEXT("edit"),                      // The class name required is button
		TEXT("25"),                  // the caption of the button
		WS_CHILD | WS_VISIBLE | WS_BORDER,  // the styles
		650, 300,                                  // the left and top co-ordinates
		50, 20,                              // width and height
		hWnd,                                 // parent window handle
		(HMENU)ID_TEXTBOXLABEL1,                   // the ID of your button
		hInstance,                            // the instance of your application
		NULL);                               // extra bits you dont really need

	hwndButton = CreateWindow(TEXT("button"),                      // The class name required is button
		TEXT("Set"),                  // the caption of the button
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,  // the styles
		700, 300,                                  // the left and top co-ordinates
		50, 20,                              // width and height
		hWnd,                                 // parent window handle
		(HMENU)ID_BUTTON3,                   // the ID of your button
		hInstance,                            // the instance of your application
		NULL);                               // extra bits you dont really need

	

	// create button and store the handle                                                       

	hwndButton = CreateWindow(TEXT("button"), TEXT("Timer ON"),
		WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		150, 250, 100, 30, hWnd, (HMENU)ID_RBUTTON1, GetModuleHandle(NULL), NULL);

	hwndButton = CreateWindow(TEXT("button"), TEXT("Timer OFF"),
		WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		150, 280, 100, 30, hWnd, (HMENU)ID_RBUTTON2, GetModuleHandle(NULL), NULL);

	// graph checkbuttons

	hwndButton = CreateWindow(TEXT("button"),                      // The class name required is button
		TEXT("Roll [x]"),                  // the caption of the button
		WS_CHILD | WS_VISIBLE | BS_CHECKBOX ,  // the styles
		20, 250,                                  // the left and top co-ordinates
		100, 20,                              // width and height
		hWnd,                                 // parent window handle
		(HMENU)ID_CBUTTON1,                   // the ID of your button
		hInstance,                            // the instance of your application
		NULL);                               // extra bits you dont really need

	hwndButton = CreateWindow(TEXT("button"),                      // The class name required is button
		TEXT("Pich [y]"),                  // the caption of the button
		WS_CHILD | WS_VISIBLE | BS_CHECKBOX,  // the styles
		20, 270,                                  // the left and top co-ordinates
		100, 20,                              // width and height
		hWnd,                                 // parent window handle
		(HMENU)ID_CBUTTON2,                   // the ID of your button
		hInstance,                            // the instance of your application
		NULL);                               // extra bits you dont really need

	hwndButton = CreateWindow(TEXT("button"),                      // The class name required is button
		TEXT("Yaw [z]"),                  // the caption of the button
		WS_CHILD | WS_VISIBLE | BS_CHECKBOX,  // the styles
		20, 290,                                  // the left and top co-ordinates
		100, 20,                              // width and height
		hWnd,                                 // parent window handle
		(HMENU)ID_CBUTTON3,                   // the ID of your button
		hInstance,                            // the instance of your application
		NULL);                               // extra bits you dont really need



	OnCreate(hWnd);

	if (!hWnd)
	{
		return FALSE;
	}



	ShowWindow(hWnd, nCmdShow);

	// draw at start
	PAINTSTRUCT ps;
	HDC hdc;
	RepaintGraphFrame(hWnd, hdc, ps);
	RepaintCompass(hWnd, hdc, ps, 0);

	UpdateWindow(hWnd);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window (low priority)
//  WM_DESTROY	- post a quit message and return
//
//


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		// MENU & BUTTON messages
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case ID_BUTTON1 :
			scaller /= 1.2;
			RepaintGraph(hWnd, hdc, ps);
			break;
		case ID_BUTTON2 :
			scaller *= 1.2;
			RepaintGraph(hWnd, hdc, ps);
			break;
		case ID_BUTTON3:
			SetSamplesToDeleeteAndDelete();
			RepaintGraph(hWnd, hdc, ps);
			break;
		case ID_RBUTTON1:
			RepaintGraph(hWnd, hdc, ps);
			SetTimer(hWnd, TMR_1, frequency, 0);
			break;
		case ID_RBUTTON2:
			RepaintGraph(hWnd, hdc, ps);
			KillTimer(hWnd, TMR_1);
			break;
		case ID_CBUTTON1:
			if (IsDlgButtonChecked(hWnd, ID_CBUTTON1)) {
				CheckDlgButton(hWnd, ID_CBUTTON1, BST_UNCHECKED);
				draw_x = false;
			}
			else {
				CheckDlgButton(hWnd, ID_CBUTTON1, BST_CHECKED);
				draw_x = true;
			}
			RepaintGraph(hWnd, hdc, ps);
			break;
		case ID_CBUTTON2:
			if (IsDlgButtonChecked(hWnd, ID_CBUTTON2)) {
				CheckDlgButton(hWnd, ID_CBUTTON2, BST_UNCHECKED);
				draw_y = false;
			}
			else {
				CheckDlgButton(hWnd, ID_CBUTTON2, BST_CHECKED);
				draw_y = true;
			}
			RepaintGraph(hWnd, hdc, ps);
			break;
		case ID_CBUTTON3:
			if (IsDlgButtonChecked(hWnd, ID_CBUTTON3)) {
				CheckDlgButton(hWnd, ID_CBUTTON3, BST_UNCHECKED);
				draw_z = false;
			}
			else {
				CheckDlgButton(hWnd, ID_CBUTTON3, BST_CHECKED);
				draw_z = true;
			}
			RepaintGraph(hWnd, hdc, ps);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here (not depend on timer, buttons)
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_TIMER:
		switch (wParam)
		{
		case TMR_1:
			if (lastTime*scaller < GRAPH_WIDTH - 1 && lastTime < dataAmount-1) {
				// draw point on graph
				DrawOneStep(hWnd, hdc, ps);
				// set compass
				RepaintCompass(hWnd, hdc, ps, data_yaw[lastTime]);
			}
			break;
		}

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
