#include <windows.h>

#define WINDOW_PADDING_PIXEL	(10)
#define BLOCK_PIXEL_SIZE		(15)
#define FIELD_HEIGHT_BLOCKS		(40)
#define FIELD_WIDTH_BLOCKS		(20)
#define SCORE_FIELD_WIDTH_PIXEL	(200)
#define TETRIMINO_KINDS			(7)
#define TETRIMINO_HEIGHT		(4)
#define TETRIMINO_WIDTH			(4)

// ブロックの種類
enum blockType {
	// ブロックがない
	FREE_BLOCK,
	// 固定ブロック
	FIXED_BLOCK,
	// 制御中のブロック
	CONTROL_BLOCK
};

// フィールド
int playField[FIELD_HEIGHT_BLOCKS][FIELD_WIDTH_BLOCKS];

// テトリミノ
int tetriminos[TETRIMINO_KINDS][TETRIMINO_HEIGHT][TETRIMINO_WIDTH] = {
	{
		{1,0,0,0},
		{1,0,0,0},
		{1,0,0,0},
		{1,0,0,0}
	},
	{
		{0,1,0,0},
		{1,1,1,0},
		{0,0,0,0},
		{0,0,0,0}
	},
	{
		{1,1,0,0},
		{0,1,1,0},
		{0,0,0,0},
		{0,0,0,0}
	},
	{
		{0,1,1,0},
		{1,1,0,0},
		{0,0,0,0},
		{0,0,0,0}
	},
	{
		{1,1,0,0},
		{1,1,0,0},
		{0,0,0,0},
		{0,0,0,0}
	},
	{
		{1,1,0,0},
		{1,0,0,0},
		{1,0,0,0},
		{1,0,0,0}
	}
};

// 指定位置にテトリミノを設置する
BOOL setTetrimino(int type, int px, int py) {
	int x, y;

	// [TODO]
	// 配置判定

	for (y = 0; y < TETRIMINO_HEIGHT; y++) {
		for (x = 0; x < TETRIMINO_HEIGHT; x++) {
			if (tetriminos[type][y][x]) {
				playField[py+y][px+x] = CONTROL_BLOCK;
			}
		}
	}
}

// ブロック1つを描画する
void drawBlock(HDC hdc, int x, int y) {
	int left, top, right, bottom;

	left = WINDOW_PADDING_PIXEL + (x * BLOCK_PIXEL_SIZE);
	top = WINDOW_PADDING_PIXEL + (y * BLOCK_PIXEL_SIZE);
	right = left + BLOCK_PIXEL_SIZE;
	bottom = top + BLOCK_PIXEL_SIZE;
	Rectangle(hdc, left, top, right, bottom);
}

// フィールドを描画する
void drawField(HDC hdc) {
	int x, y;

	SelectObject(hdc, GetStockObject(WHITE_PEN));
	for (y = 0; y < FIELD_HEIGHT_BLOCKS; y++) {
		for (x = 0; x < FIELD_WIDTH_BLOCKS; x++) {
			if (playField[y][x] != FREE_BLOCK) {
				drawBlock(hdc, x, y);
			}
		}
	}
}

// フィールドの枠線を描画する
void drawFieldBoundary(HDC hdc) {
	SelectObject(hdc, GetStockObject(WHITE_PEN));
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
			drawFieldBoundary(hdc);
			setTetrimino(0, 1, 0);
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
