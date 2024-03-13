//*****************************************************************************
//
// Main.cpp : Defines the entry point for the application.
// Creates a white RGB image with a black line and displays it.
// Two images are displayed on the screen.
// Left Pane: Input Image, Right Pane: Modified Image
//
// Author - 
// Code used by students as a starter code to display and modify images
//
//*****************************************************************************


// Include class files
#include "Image.h"
#include <iostream>
#include <cmath>
#include <vector>


#define MAX_LOADSTRING 100

// Global Variables:
MyImage			inImage;						// image objects
HINSTANCE		hInst;							// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// The title bar text

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);


// Main entry point for a windows application
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	MSG msg;
	HACCEL hAccelTable;

	// Create a separate console window to display output to stdout
	AllocConsole();
	freopen("CONOUT$", "w", stdout);

	// The rest of command line argument is truncated.
	// If you want to use it, please modify the code.
	if (lpCmdLine[0] == 0) {
		wprintf(L"No command line argument.");
		return -1;
	}

	int numArgs = 0;
	std::vector<char*> arguments; // Use a vector to store the object paths

	char* token = strtok(lpCmdLine, " ");
	while (token != NULL) {
		arguments.push_back(token);
		token = strtok(NULL, " ");
	}

	std::cout << "Number of arguments: " << arguments.size() << std::endl;
	std::cout << "Image Path: " << arguments[0] << std::endl;

	// Print object paths
	std::cout << "Object Paths:" << std::endl;
	for (size_t i = 1; i < arguments.size(); ++i) {
		std::cout << "  " << arguments[i] << std::endl;
	}

	// Set up the images
	int w = 640;
	int h = 480;
	inImage.setWidth(w);
	inImage.setHeight(h);
	inImage.setImagePath(arguments[0]);
	inImage.ReadImage();
	// Set up the images
	std::vector<std::unique_ptr<MyImage>> obImages; // Store pointers to MyImage objects


	for (size_t i = 1; i < arguments.size(); ++i) {
		auto obImage = std::make_unique<MyImage>(); // Using std::unique_ptr
		obImage->setWidth(w);
		obImage->setHeight(h);
		obImage->setImagePath(arguments[i]);
		obImage->ReadImage();
		char* obj_data = obImage->getImageData();

		obImages.push_back(std::move(obImage)); // Transfer ownership to the vector
	}
	inImage.Modify(obImages);

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_IMAGE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}
	
	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_IMAGE);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_IMAGE);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_IMAGE;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}


//
//   FUNCTION: InitInstance(HANDLE, int)
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

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }
   
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
// TO DO: part useful to render video frames, may place your own code here in this function
// You are free to change the following code in any way in order to display the video

	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);
	RECT rt;
	static RECT rect, storedRect;
	GetClientRect(hWnd, &rt);
	static bool ctrlKeyPressed = false;
	static int windowSize = 50; // Adjust the size of the square window as needed
	static POINT cursorPos;    // Store the cursor position when the control key is pressed


	switch (message) 
	{
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDM_ABOUT:
				   DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
				   break;
				case ID_MODIFY_IMAGE:
				   InvalidateRect(hWnd, &rt, false); 
				   break;
				case IDM_EXIT:
				   DestroyWindow(hWnd);
				   break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;

		case WM_KEYDOWN:
			if (wParam == VK_CONTROL)
			{
				ctrlKeyPressed = true;
				
				InvalidateRect(hWnd, NULL, FALSE);
			}
			break;

		case WM_KEYUP:
			if (wParam == VK_CONTROL)
			{
				ctrlKeyPressed = false;
				// Invalidate the window to trigger a redraw
				InvalidateRect(hWnd, NULL, FALSE);
			}
			break;

		case WM_MOUSEMOVE:
			if (ctrlKeyPressed) {
				int mouseX = LOWORD(lParam);
				int mouseY = HIWORD(lParam);

				//std::cout << disp << std::endl;


				

				// Store the current window position


				// Invalidate the window to trigger a repaint
				//InvalidateRect(hWnd, &storedRect, false);
				//InvalidateRect(hWnd, &rect, false);
			}




		case WM_PAINT:
			{
			hdc = BeginPaint(hWnd, &ps);
			// TO DO: Add any drawing code here...

			// Draw OpImage
			BITMAPINFO bmi;
			CBitmap bitmap;
			memset(&bmi, 0, sizeof(bmi));

			bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
			bmi.bmiHeader.biWidth = inImage.getWidth();
			bmi.bmiHeader.biHeight = -(inImage.getHeight());  // Use negative height. DIB is top-down.
			bmi.bmiHeader.biPlanes = 1;
			bmi.bmiHeader.biBitCount = 24;
			bmi.bmiHeader.biCompression = BI_RGB;
			bmi.bmiHeader.biSizeImage = inImage.getHeight() * inImage.getWidth();


			//SetDIBitsToDevice(hdc,
			//	0, 0, obImage.getWidth(), obImage.getHeight(),
			//	0, 0, 0, obImage.getHeight(),
			//	obImage.getImageData(), &bmi, DIB_RGB_COLORS);


			SetDIBitsToDevice(hdc,
				0, 0, inImage.getWidth(), inImage.getHeight(),
				0, 0, 0, inImage.getHeight(),
				inImage.getImageData(), &bmi, DIB_RGB_COLORS);

			// Draw the black box around the mouse cursor
			//HBRUSH hBlackBrush;  // Declare this in your global or class scope

			//// Inside your WM_CREATE handler or another appropriate place:
			//hBlackBrush = CreateSolidBrush(RGB(0, 0, 0));
			//FillRect(hdc, &rect, hBlackBrush);

			EndPaint(hWnd, &ps);


							   
				//EndPaint(hWnd, &ps);
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}




// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}


