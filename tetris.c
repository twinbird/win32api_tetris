#include <windows.h>

#define WINDOW_PADDING_PIXEL	(10)
#define BLOCK_PIXEL_SIZE		(15)
#define FIELD_HEIGHT_BLOCKS		(40)
#define FIELD_WIDTH_BLOCKS		(20)
#define SCORE_FIELD_WIDTH_PIXEL	(200)
#define TETRIMINO_KINDS			(7)
#define TETRIMINO_HEIGHT		(4)
#define TETRIMINO_WIDTH			(4)

// �u���b�N�̎��
enum blockType {
	// �u���b�N���Ȃ�
	FREE_BLOCK,
	// �Œ�u���b�N
	FIXED_BLOCK,
	// ���䒆�̃u���b�N
	CONTROL_BLOCK
};

// �t�B�[���h
int playField[FIELD_HEIGHT_BLOCKS][FIELD_WIDTH_BLOCKS];

// �e�g���~�m
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

// �e�g���~�m
typedef struct _Tetrimino {
	int x;
	int y;
	int type;
} Tetrimino;

// �e�g���~�m�̈ړ�����
typedef enum _MoveType {
	MOVE_TO_LEFT,
	MOVE_TO_RIGHT,
	MOVE_TO_DOWN,
	MOVE_TO_DOWN_FAST
} MoveType;

// ���쒆�̃e�g���~�m
Tetrimino currentTetrimino; 

// �e�g���~�m��ݒu����
BOOL setTetrimino(Tetrimino t) {
	int x, y;

	// [TODO]
	// �z�u����

	for (y = 0; y < TETRIMINO_HEIGHT; y++) {
		for (x = 0; x < TETRIMINO_HEIGHT; x++) {
			if (tetriminos[t.type][y][x]) {
				playField[t.y+y][t.x+x] = CONTROL_BLOCK;
			}
		}
	}
	return TRUE;
}

// �e�g���~�m����菜��
BOOL unsetTetrimino(Tetrimino t) {
	int x, y;

	// [TODO]
	// �z�u����

	for (y = 0; y < TETRIMINO_HEIGHT; y++) {
		for (x = 0; x < TETRIMINO_HEIGHT; x++) {
			if (playField[t.y+y][t.x+x] == CONTROL_BLOCK) {
				playField[t.y+y][t.x+x] = FREE_BLOCK;
			}
		}
	}
	return TRUE;
}

// �e�g���~�m���ړ�����
BOOL moveTetrimino(Tetrimino t, MoveType type) {
	// ���̏ꏊ�̃e�g���~�m
	Tetrimino next_t = t;
	switch (type) {
		case MOVE_TO_LEFT:
			next_t.x = next_t.x - 1;
			break;
		case MOVE_TO_RIGHT:
			next_t.x = next_t.x + 1;
			break;
		case MOVE_TO_DOWN:
			next_t.y = next_t.y + 1;
			break;
		case MOVE_TO_DOWN_FAST:
			break;
	}

	// ��x��菜��
	unsetTetrimino(t);
	// �����Ă݂�
	if (setTetrimino(next_t) == FALSE) {
		// ���߂Ȃ�߂�
		unsetTetrimino(next_t);
		setTetrimino(t);
		return FALSE;
	}
	// ���쒆�̃e�g���~�m�������ւ�
	currentTetrimino = next_t;
	return TRUE;
}

// �u���b�N1��`�悷��
void drawBlock(HDC hdc, int x, int y) {
	int left, top, right, bottom;

	left = WINDOW_PADDING_PIXEL + (x * BLOCK_PIXEL_SIZE);
	top = WINDOW_PADDING_PIXEL + (y * BLOCK_PIXEL_SIZE);
	right = left + BLOCK_PIXEL_SIZE;
	bottom = top + BLOCK_PIXEL_SIZE;
	Rectangle(hdc, left, top, right, bottom);
}

// �t�B�[���h��`�悷��
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

// �t�B�[���h�̘g����`�悷��
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

// �E�B���h�E�T�C�Y��ύX
BOOL setClientSize(HWND hwnd, int width, int height) {
	RECT rw, rc;
	GetWindowRect(hwnd, &rw);
	GetClientRect(hwnd, &rc);

	int new_width = (rw.right - rw.left) - (rc.right - rc.left) + width;
	int new_height = (rw.bottom - rw.top) - (rc.bottom - rc.top) + height;

	return SetWindowPos(hwnd, NULL, 0, 0, new_width, new_height, SWP_NOMOVE | SWP_NOZORDER);
}

// �L�[���쏈��
void keyProc(WPARAM wp) {
	switch (wp) {
		case VK_RIGHT:
			moveTetrimino(currentTetrimino, MOVE_TO_RIGHT);
			break;
		case VK_LEFT:
			moveTetrimino(currentTetrimino, MOVE_TO_LEFT);
			break;
		case VK_DOWN:
			moveTetrimino(currentTetrimino, MOVE_TO_DOWN);
			break;
		case VK_UP:
			moveTetrimino(currentTetrimino, MOVE_TO_DOWN_FAST);
			break;
		case VK_RETURN:
			MessageBox(NULL, TEXT("ENTER"), TEXT("caption"), MB_OK);
			break;
		case VK_SPACE:
			MessageBox(NULL, TEXT("SPACE"), TEXT("caption"), MB_OK);
			break;
	}
}

// �A�v���P�[�V�����Ƃ��Ă̏���������
void initializeApp() {
	currentTetrimino.x = 0;
	currentTetrimino.y = 0;
	currentTetrimino.type = 0;
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
			setTetrimino(currentTetrimino);
			drawField(hdc);
			EndPaint(hwnd, &ps);
			return 0;
		case WM_KEYDOWN:
			keyProc(wp);
			InvalidateRect(hwnd, NULL, TRUE);
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

	// �E�B���h�E�T�C�Y��ύX
	// �t�B�[���h�T�C�Y�ɍ��E���̃}�[�W���ƃX�R�A�\���������������T�C�Y�ɂ���
	setClientSize(hwnd,
			(BLOCK_PIXEL_SIZE * FIELD_WIDTH_BLOCKS) + (WINDOW_PADDING_PIXEL * 2) + SCORE_FIELD_WIDTH_PIXEL,
			(BLOCK_PIXEL_SIZE * FIELD_HEIGHT_BLOCKS) + (WINDOW_PADDING_PIXEL * 2));

	// �A�v���P�[�V�����̏�����
	initializeApp();

	while (GetMessage(&msg, NULL, 0, 0)) {
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
