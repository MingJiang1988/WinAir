// WinAir.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <iostream>
#include "WinAir.h"
#include "opencv2\opencv.hpp"
#include "opencv2\core.hpp"
#include "opencv2\highgui.hpp"
#include "opencv2\imgcodecs.hpp"
#include "yolo_v2_class.h"
#include "opencv2/core/cuda.hpp"

#define MAX_LOADSTRING 100
using namespace cv;
using namespace std;


// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINAIR, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINAIR));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINAIR));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINAIR);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
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
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   //ShowWindow(hWnd, nCmdShow);
   //UpdateWindow(hWnd);
   fullScreen();

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
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




void fullScreen()
{
	string cfg_file = "air.cfg";				  //yolo model cfg file 
	string weight_file = "air.weights";	      //yolo model 	weights file 
	VideoCapture capture;
	string address = "2.mp4";
	capture.open(0);
	
	if (!capture.isOpened())
	{
		MessageBoxA(NULL, "Not found WebCam device. Need WebCam.", "Device Error", MB_OK);
		exit(0);
	}
	/*try
	{
		cuda::DeviceInfo gpuinfo = cuda::DeviceInfo::DeviceInfo();
	}
	catch (Exception e)
	{
		MessageBoxA(NULL, "NVIDIA Device or Driver is not installed.", "GPU Error", MB_OK);
		TerminateProcess(GetCurrentProcess(), 1);
	}*/

	Detector *pDetector = new Detector(cfg_file, weight_file);
	
	cvNamedWindow("Name", CV_WINDOW_NORMAL);
	cvSetWindowProperty("Name", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
	float zscale = 1;
	Mat frame;
	Mat iyframe;
	Mat show_frame;
	int rs_width = 0;
	int rs_height = 0;
	int rotate_flag = 0;
	int n_r_key = 0;
	int f_count = 1;
	while (capture.isOpened())
	{
		if (f_count == 1000)
			f_count = 1;
		if (f_count % 4 == 1)
		{
			rotate_flag = n_r_key % 3;
			try {
				capture >> frame;
				if (rotate_flag == 1)
				{
					cv::rotate(frame, frame, ROTATE_90_CLOCKWISE);
				}
				if (rotate_flag == 2)
				{
					cv::rotate(frame, frame, ROTATE_90_COUNTERCLOCKWISE);
				}
				int f_width = frame.size().width;
				int f_height = frame.size().height;
				if (f_width > f_height)
				{
					rs_width = f_width;
					rs_height = f_height;
				}
				if (f_height > f_width)
				{
					rs_width = f_height * (4.0 / 3);
					rs_height = f_height;
				}
				
				Rect ROI = Rect(0, 0, f_width, f_height);
				//Mat image;
				//cv::rotate(frame, frame, ROTATE_90_COUNTERCLOCKWISE);
				Mat image(rs_height, rs_width, CV_8UC3, Scalar(0, 0, 0));
				//cv::resize(frame, image, Size(rs_width, rs_height));
				frame.copyTo(image(ROI));
				//cv::rotate(image, image, ROTATE_90_CLOCKWISE);
				
				//cv::imwrite("rotate.jpg", image);
				cv::resize(image, iyframe, Size(int(image.size().width*zscale), int(image.size().height*zscale)));
				try
				{
					vector<bbox_t> result;
					result = pDetector->detect(iyframe);
					for (int j = 0; j < result.size(); j++)
					{
						if (result[j].obj_id == 0)
						{
							int x = int(result[j].x / zscale);
							int y = int(result[j].y / zscale);
							int w = int(result[j].w / zscale);
							int h = int(result[j].h / zscale);
							Rect boundrect = Rect(x, y, w, h);
							Rect bgrect = Rect(x, y - 20, 100, 20);
							rectangle(image, boundrect, CV_RGB(255, 255, 0), 1, CV_AA, 0);
							rectangle(image, bgrect, CV_RGB(255, 255, 0), -1);
							putText(image, "aeroplane", Point(x + 5, y - 8), CV_FONT_NORMAL, 0.5, CV_RGB(0, 0, 0), 1);
						}

					}
				}
				catch (Exception e)
				{
				}
				show_frame = image(ROI);
				cv::imshow("Name", show_frame);
				char k;
				k = cvWaitKey(40);
				if (k == 27)
				{
					//free(pDetector);
					//ExitProcess(0);
					TerminateProcess(GetCurrentProcess(), 1);
				}
				if (k == 'r')
				{
					n_r_key++;

				}

			}
			catch (Exception e)
			{
				capture.open(0);
				continue;
			}
		}
		f_count++;
		
	}
		

	
}
