#include<Windows.h>
#include<string>
#include<sstream>
#include<vector>
#include<algorithm>
#include"resource.h"

CONST CHAR g_sz_CLASS_NAME[] = "MyCalc";

CONST CHAR* g_sz_OPERATIONS[] = { "+", "-", "*", "/" };
CONST CHAR* g_sz_EDIT[] = { "<-", "C", "=" };

//g_i_ - Global Integer
CONST INT g_i_BUTTON_SIZE = 50;
CONST INT g_i_INTERVAL = 1;
CONST INT g_i_BUTTON_SPACE = g_i_BUTTON_SIZE + g_i_INTERVAL;
//CONST INT g_i_BUTTON_SPACE_DOUBLE = (g_i_BUTTON_SIZE + g_i_INTERVAL) * 2;

CONST INT g_i_BUTTON_SIZE_DOUBLE = g_i_BUTTON_SIZE * 2 + g_i_INTERVAL;
CONST INT g_i_START_X = 10;
CONST INT g_i_START_Y = 10;
CONST INT g_i_DISPLAY_HEIGHT = 22;
CONST INT g_i_DISPLAY_WIDTH = g_i_BUTTON_SIZE * 5 + g_i_INTERVAL * 4;
CONST INT g_i_BUTTON_START_X = g_i_START_X;
CONST INT g_i_BUTTON_START_Y = g_i_START_Y + g_i_DISPLAY_HEIGHT + g_i_INTERVAL;

CONST INT g_i_WINDOW_WIDTH = g_i_DISPLAY_WIDTH + 2 * g_i_START_X + 16;
CONST INT g_i_WINDOW_HEIGHT = (g_i_DISPLAY_HEIGHT + g_i_INTERVAL) + 
g_i_BUTTON_SPACE * 4 + 2 * g_i_START_Y + 24 + 16;

CONST INT g_SIZE = 256;

INT WINAPI WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

double EvaluateExpression(const std::string& expression) 
{
	std::stringstream ss(expression);
	std::vector<double> nums;
	std::vector<char> ops;
	double num;
	char op;

	if (!(ss >> num))
		return 0.0; 

	nums.push_back(num);

	while (ss >> op) 
	{
		if (!(ss >> num))
			return 0.0;
		ops.push_back(op);
		nums.push_back(num);
	}

	for (size_t i = 0; i < ops.size(); ) 
	{
		if (ops[i] == '*' || ops[i] == '/') 
		{
			if (ops[i] == '*') 
			{
				nums[i] = nums[i] * nums[i + 1];
			}
			else 
			{
				if (nums[i + 1] != 0)
					nums[i] = nums[i] / nums[i + 1];
				else
					return 0.0;
			}

			nums.erase(nums.begin() + i + 1);
			ops.erase(ops.begin() + i);
		}
		else 
		{
			++i;
		}
	}

	double result = nums[0];
	for (size_t i = 0; i < ops.size(); ++i) 
	{
		if (ops[i] == '+') 
		{
			result += nums[i + 1];
		}
		else if (ops[i] == '-') 
		{
			result -= nums[i + 1];
		}
	}

	return result;
}

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow)
{
	//1) Ðåãèñòðàöèÿ êëàññà îêíà:
	WNDCLASSEX wClass;
	ZeroMemory(&wClass, sizeof(wClass));

	wClass.style = 0;
	wClass.cbSize = sizeof(wClass);
	wClass.cbWndExtra = 0;
	wClass.cbClsExtra = 0;

	wClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

	wClass.hInstance = hInstance;
	wClass.lpszMenuName = NULL;
	wClass.lpszClassName = g_sz_CLASS_NAME;
	wClass.lpfnWndProc = (WNDPROC)WndProc;

	if (!RegisterClassEx(&wClass))
	{
		MessageBox(NULL, "Class registration failed", "", MB_OK | MB_ICONERROR);
		return 0;
	}

	//2) Ñîçäàíèå îêíà:
	HWND hwnd = CreateWindowEx
	(
		NULL,
		g_sz_CLASS_NAME,
		g_sz_CLASS_NAME,
		//WS_OVERLAPPED | WS_SYSMENU /*| WS_THICKFRAME*/ | WS_MINIMIZEBOX /*| WS_MAXIMIZEBOX*/,
		WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT,
		g_i_WINDOW_WIDTH, g_i_WINDOW_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL
	);
	ShowWindow(hwnd, nCmdShow);	//https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-showwindow
	UpdateWindow(hwnd);			//			taskkill /f /im  calc.exe

	//3) Çàïóñê öèêëà ñîîáùåíèé:
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

INT WINAPI WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
	{
		HWND hEditDisplay = CreateWindowEx
		(
			NULL, "Edit", "0",
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT,
			g_i_BUTTON_START_X, g_i_START_Y,
			g_i_DISPLAY_WIDTH, g_i_DISPLAY_HEIGHT,
			hwnd,
			(HMENU)IDC_EDIT_DISPLAY,
			GetModuleHandle(NULL),
			NULL
		);
		INT iDigit = IDC_BUTTON_1;
		CHAR szDigit[2] = {};
		for (int i = 6; i >= 0; i -= 3)
		{
			for (int j = 0; j < 3; j++)
			{
				szDigit[0] = iDigit - IDC_BUTTON_0 + '0';
				CreateWindowEx
				(
					NULL, "Button", szDigit,
					WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
					g_i_BUTTON_START_X + (g_i_BUTTON_SIZE + g_i_INTERVAL) * j,
					g_i_BUTTON_START_Y + (g_i_BUTTON_SIZE + g_i_INTERVAL) * i / 3,
					g_i_BUTTON_SIZE, g_i_BUTTON_SIZE,
					hwnd,
					(HMENU)iDigit,
					GetModuleHandle(NULL),
					NULL
				);
				iDigit++;
			}
		}
		CreateWindowEx
		(
			NULL, "Button", "0",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			g_i_BUTTON_START_X, g_i_BUTTON_START_Y + (g_i_BUTTON_SPACE) * 3,
			g_i_BUTTON_SIZE_DOUBLE, g_i_BUTTON_SIZE,
			hwnd,
			(HMENU)IDC_BUTTON_0,
			GetModuleHandle(NULL),
			NULL
		);
		CreateWindowEx
		(
			NULL, "Button", ".",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			g_i_BUTTON_START_X + g_i_BUTTON_SPACE * 2,
			g_i_BUTTON_START_Y + g_i_BUTTON_SPACE * 3,
			g_i_BUTTON_SIZE, g_i_BUTTON_SIZE,
			hwnd,
			(HMENU)IDC_BUTTON_POINT,
			GetModuleHandle(NULL),
			NULL
		);
		//////////////////////////////////////////////////////
		for (int i = 0; i < 4; i++)
		{
			CreateWindowEx
			(
				NULL, "Button", g_sz_OPERATIONS[i],
				WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
				g_i_BUTTON_START_X + g_i_BUTTON_SPACE * 3, g_i_BUTTON_START_Y + g_i_BUTTON_SPACE * (3 - i),
				g_i_BUTTON_SIZE, g_i_BUTTON_SIZE,
				hwnd,
				(HMENU)(IDC_BUTTON_PLUS + i),
				GetModuleHandle(NULL),
				NULL
			);
		}
		for (int i = 0; i < 3; i++)
		{
			CreateWindowEx
			(
				NULL, "Button", g_sz_EDIT[i],
				WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
				g_i_BUTTON_START_X + g_i_BUTTON_SPACE * 4,
				g_i_BUTTON_START_Y + g_i_BUTTON_SPACE * i,
				g_i_BUTTON_SIZE, i < 2 ? g_i_BUTTON_SIZE : g_i_BUTTON_SIZE_DOUBLE,
				hwnd,
				(HMENU)(IDC_BUTTON_BSP + i),
				GetModuleHandle(NULL),
				NULL
			);
		}
	}
	break;
	case WM_COMMAND:
	{
		CHAR szDisplay[g_SIZE] = {};
		CHAR szDigit[2] = {};
		HWND hEditDisplay = GetDlgItem(hwnd, IDC_EDIT_DISPLAY);
		if (LOWORD(wParam) >= IDC_BUTTON_0 && LOWORD(wParam) <= IDC_BUTTON_POINT)
		{
			if (LOWORD(wParam) == IDC_BUTTON_POINT)
				szDigit[0] = '.';
			else
				szDigit[0] = LOWORD(wParam) - IDC_BUTTON_0 + '0';
			SendMessage(hEditDisplay, WM_GETTEXT, g_SIZE, (LPARAM)szDisplay);
			if (szDisplay[0] == '0' && szDisplay[1] != '.')szDisplay[0] = 0;
			if (szDigit[0] == '.' && strchr(szDisplay, '.'))break;
			//https://legacy.cplusplus.com/reference/cstring/strchr/
			//Ôóíêöèÿ strchr() âûïîëíÿåò ïîèñê ñèìâîëà â ñòðîêå.
			//Åñëè ñèìâîë íàéäåí, òî strchr() âîçâðàùàåò óêàçàòåëü íà íàéäåííûé ñèìâîë,
			//â ïðîòèâíîì ñëó÷àå âîçâðàùàåò 'nullptr' (ôèçè÷åñêèé 0);
			//0 - ýòî ''false;
			//true - ýòî âñå ÷òî ÍÅ 0;
			strcat_s(szDisplay, szDigit);
			SendMessage(hEditDisplay, WM_SETTEXT, 0, (LPARAM)szDisplay);
		}

		else if (LOWORD(wParam) >= IDC_BUTTON_PLUS && LOWORD(wParam) <= IDC_BUTTON_SLASH)
		{
			CHAR szOp[2] = {};
			szOp[0] = g_sz_OPERATIONS[LOWORD(wParam) - IDC_BUTTON_PLUS][0];
			szOp[1] = 0;
			SendMessage(hEditDisplay, WM_GETTEXT, g_SIZE, (LPARAM)szDisplay);


			if (strlen(szDisplay) > 0) 
			{
				strcat_s(szDisplay, g_SIZE, szOp);
				SendMessage(hEditDisplay, WM_SETTEXT, 0, (LPARAM)szDisplay);
			}
		}

		else if (LOWORD(wParam) >= IDC_BUTTON_BSP && LOWORD(wParam) <= IDC_BUTTON_EQUAL)
		{
			switch (LOWORD(wParam))
			{
				case IDC_BUTTON_BSP:
				{
				SendMessage(hEditDisplay, WM_GETTEXT, g_SIZE, (LPARAM)szDisplay);
				size_t len = strlen(szDisplay);

				if (len > 0)
				{
					szDisplay[len - 1] = 0;
				}
				SendMessage(hEditDisplay, WM_SETTEXT, 0, (LPARAM)szDisplay);

				break;
				}

				case IDC_BUTTON_CLR:
				{
				SendMessage(hEditDisplay, WM_SETTEXT, 0, (LPARAM)"0");

				break;
				}

				case IDC_BUTTON_EQUAL:
				{
				SendMessage(hEditDisplay, WM_GETTEXT, g_SIZE, (LPARAM)szDisplay);
				
					double result = EvaluateExpression(szDisplay);
					sprintf_s(szDisplay, g_SIZE, "%lf", result);

				SendMessage(hEditDisplay, WM_SETTEXT, 0, (LPARAM)szDisplay);
				break;
				}
				default:
				break;
			}
		}
		break;
	}
	break;

	case WM_KEYDOWN:
	{
		if (GetKeyState(VK_LSHIFT))
		{
			// Shift is pressed
			if (wParam == VK_LSHIFT)
			{
				SendMessage(hwnd, WM_COMMAND, IDC_BUTTON_ASTER, 0);
			}
		}
		else
		{
			// Shift is not pressed
			if (wParam >= '0' && wParam <= '9')
			{
				SendMessage(hwnd, WM_COMMAND, LOWORD(wParam - '0' + IDC_BUTTON_0), 0);
			}
			switch (wParam)
			{
				// Decimal point
			case VK_OEM_PERIOD:
			case VK_DECIMAL:
				SendMessage(hwnd, WM_COMMAND, IDC_BUTTON_POINT, 0);
				break;

				// Plus
			case VK_ADD:
			case VK_OEM_PLUS:
				SendMessage(hwnd, WM_COMMAND, IDC_BUTTON_PLUS, 0);
				break;

				// Minus
			case VK_OEM_MINUS:
				SendMessage(hwnd, WM_COMMAND, IDC_BUTTON_MINUS, 0);
				break;

				// Multiply
			case VK_MULTIPLY: // VK_MULTIPLY is the numpad *
				SendMessage(hwnd, WM_COMMAND, IDC_BUTTON_ASTER, 0);
				break;

				// Divide
			case VK_DIVIDE:
			case VK_OEM_2: // Check for forward slash (/)
				SendMessage(hwnd, WM_COMMAND, IDC_BUTTON_SLASH, 0);
				break;

				// Backspace
			case VK_BACK:
				SendMessage(hwnd, WM_COMMAND, IDC_BUTTON_BSP, 0);
				break;

				// Escape (Clear)
			case VK_ESCAPE:
				SendMessage(hwnd, WM_COMMAND, IDC_BUTTON_CLR, 0);
				break;

				// Enter (Equals)
			case VK_RETURN:
				SendMessage(hwnd, WM_COMMAND, IDC_BUTTON_EQUAL, 0);
				break;
			}
		}
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	default:return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return FALSE;
}