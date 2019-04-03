#include <windows.h>

#define WINDOW_PADDING_PIXEL	(10)
#define BLOCK_PIXEL_SIZE		(15)
#define FIELD_HEIGHT_BLOCKS		(40)
#define FIELD_WIDTH_BLOCKS		(20)
#define SCORE_FIELD_WIDTH_PIXEL	(200)

// フィールドの枠線を描画する
void drawField(HDC hdc) {
	MoveToEx(hdc, WINDOW_PADDING_PIXEL, WINDOW_PADDING_PIXEL, NULL);
	LineTo(hdc,
			WINDOW_PADDING_PIXEL,
			(BLOCK_PIXEL_SIZE * FIELD_HEIGHT_BLOCKS) + WINDOW_PADDING_PIXEL);
	LineTo(hdc,
			(BLOCK_PIXEL_SIZE * FIELD_WIDTH_BLOCKS) + WINDOW_PADDING_PIXEL,
			(BLOCK_PIXEL_SIZE * FIELD_HEIGHT_BLOCKS) + WINDOW_PADDING_PIXEL);
	LineTo(hdc,
			(BLOCK_PIXEL_SIZE * FIELD_WIDTH_BLOCKS) + WINDOW_PADDING_PIXEL,
			WINDOW_PADDING_PIXEL);
}

// ウィンドウサイズを変更
BOOL setClientSize(HWND hwnd, int width, int height) {
	RECT rw, rc;
	GetWindowRect(hwnd, &rw);
	GetClientRect(hwnd, &rc);

	int new_width = (rw.right - rw.left) - (rc.right - rc.left) + width;
	int new_height = (rw.bottom - rw.top) - (rc.bottom - rc.top) + height;

	return SetWindowPos(hwnd, NULL, 0, 0, new_width, new_height, SWP_NOMOVE | SWP_NOZORDER);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
	HDC hdc;
	PAINTSTRUCT ps;
	
	switch (msg) {
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		case WM_PAINT:
			hdc = BeginPaint(hwnd, &ps);
			SelectObject(hdc, GetStockObject(WHITE_PEN));
			drawField(hdc);
			EndPaint(hwnd, &ps);
			return 0;
	}
	return DefWindowProc(hwnd, msg, wp, lp);
}

int WINAPI WinMain(
		HINSTANCE hInstance,
		HINSTANCE hPrevInstance,
		PSTR lpCmdLine,
		int nCmdShow) {
	HWND hwnd;
	MSG msg;
	WNDCLASS winc;

	winc.style = CS_HREDRAW | CS_VREDRAW;
	winc.lpfnWndProc = WndProc;
	winc.cbClsExtra = 0;
	winc.cbWndExtra = 0;
	winc.hInstance = hInstance;
	winc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	winc.hCursor = LoadCursor(NULL, IDC_ARROW);
	winc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	winc.lpszMenuName = NULL;
	winc.lpszClassName = TEXT("TETRIS_WINDOW_CLASS");

	if (!RegisterClass(&winc)) {
		return -1;
	}

	hwnd = CreateWindow(
			TEXT("TETRIS_WINDOW_CLASS"),
			TEXT("Tetris"),
			WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CAPTION,
			CW_USEDEFAULT, CW_USEDEFAULT,
			CW_USEDEFAULT, CW_USEDEFAULT,
			NULL, NULL, hInstance, NULL);

	if (hwnd == NULL) {
		return -1;
	}

	// ウィンドウサイズを変更
	// フィールドサイズに左右分のマージンとスコア表示幅分を加えたサイズにする
	setClientSize(hwnd,
			(BLOCK_PIXEL_SIZE * FIELD_WIDTH_BLOCKS) + (WINDOW_PADDING_PIXEL * 2) + SCORE_FIELD_WIDTH_PIXEL,
			(BLOCK_PIXEL_SIZE * FIELD_HEIGHT_BLOCKS) + (WINDOW_PADDING_PIXEL * 2));

	while (GetMessage(&msg, NULL, 0, 0)) {
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
