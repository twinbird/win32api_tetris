#include <windows.h>
#include <stdlib.h>
#include <time.h>

#define WINDOW_PADDING_PIXEL	(10)
#define BLOCK_PIXEL_SIZE		(30)
#define FIELD_HEIGHT_BLOCKS		(20)
#define FIELD_WIDTH_BLOCKS		(10)
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

// �Q�[���J�n����
DWORD play_start_time;

// �Q�[���̃X�R�A
int playing_score;

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
		{0,0,0,0}
	},
	{
		{1,1,0,0},
		{0,1,0,0},
		{0,1,0,0},
		{0,0,0,0}
	}
};

// �e�g���~�m
typedef struct _Tetrimino {
	int x;
	int y;
	int type;
	int tetrimino[TETRIMINO_HEIGHT][TETRIMINO_WIDTH];
} Tetrimino;

// �e�g���~�m�̈ړ�����
typedef enum _MoveType {
	MOVE_TO_LEFT,
	MOVE_TO_RIGHT,
	MOVE_TO_DOWN
} MoveType;

// �e�g���~�m�̉�]����
typedef enum _RotateType {
	CLOCKWISE,
	COUNTER_CLOCKWISE
} RotateType;

// ���쒆�̃e�g���~�m
Tetrimino currentTetrimino; 

// �e�g���~�m���Œ肷��
void fixTetrimino(Tetrimino t) {
	int x, y;
	for (y = 0; y < TETRIMINO_HEIGHT; y++) {
		for (x = 0; x < TETRIMINO_WIDTH; x++) {
			if (playField[t.y + y][t.x + x] == CONTROL_BLOCK) {
				playField[t.y + y][t.x + x] = FIXED_BLOCK;
			}
		}
	}
}

// �w��ʒu�Ƀe�g���~�m���z�u�ł��邩�m�F
BOOL collisionTetrimino(Tetrimino t, int px, int py) {
	int x, y;

	// �N�_���t�B�[���h�̈�𒴂��Ă���
	if (px < 0 || FIELD_WIDTH_BLOCKS <= px) {
		return FALSE;
	}
	if (py < 0 || FIELD_HEIGHT_BLOCKS <= py) {
		return FALSE;
	}

	// �e�g���~�m�̕����̃`�F�b�N
	for (y = 0; y < TETRIMINO_HEIGHT; y++) {
		for (x = 0; x < TETRIMINO_WIDTH; x++) {
			// �e�g���~�m�̃u���b�N���Ȃ���Δ���O
			if (t.tetrimino[y][x] == 0) {
				continue;
			}
			// �e�g���~�m���t�B�[���h�Ƀ}�b�s���O�������W
			int block_x = px + x;
			int block_y = py + y;

			// �t�B�[���h�̈�𒴂���
			if (block_x < 0 || FIELD_WIDTH_BLOCKS <= block_x) {
				return FALSE;
			}
			if (block_y < 0 || FIELD_HEIGHT_BLOCKS <= block_y) {
				return FALSE;
			}
			// ���łɃt�B�[���h�Ƀu���b�N��������
			if (playField[block_y][block_x] != FREE_BLOCK) {
				return FALSE;
			}
		}
	}
	return TRUE;
}

// �e�g���~�m��ݒu����
BOOL setTetrimino(Tetrimino t) {
	int x, y;

	// �z�u����
	if (collisionTetrimino(t, t.x, t.y) == FALSE) {
		return FALSE;
	}

	for (y = 0; y < TETRIMINO_HEIGHT; y++) {
		for (x = 0; x < TETRIMINO_WIDTH; x++) {
			if (t.tetrimino[y][x]) {
				playField[t.y+y][t.x+x] = CONTROL_BLOCK;
			}
		}
	}
	return TRUE;
}

// �e�g���~�m����菜��
void unsetTetrimino(Tetrimino t) {
	int x, y;

	for (y = 0; y < TETRIMINO_HEIGHT; y++) {
		for (x = 0; x < TETRIMINO_HEIGHT; x++) {
			if (playField[t.y+y][t.x+x] == CONTROL_BLOCK) {
				playField[t.y+y][t.x+x] = FREE_BLOCK;
			}
		}
	}
}

// �V�����e�g���~�m���쐬���đ��쒆�ɐݒ�
void createTetrimino(int x, int y, int type) {
	int i, k;
	Tetrimino t;

	// ���삷��e�g���~�m��p�ӂ��Ĕz�u
	t.x = x;
	t.y = y;
	t.type = type;
	for (i = 0; i < TETRIMINO_HEIGHT; i++) {
		for (k = 0; k < TETRIMINO_WIDTH; k++) {
			t.tetrimino[i][k] = tetriminos[type][i][k];
		}
	}
	currentTetrimino = t;

	// �z�u
	setTetrimino(currentTetrimino);
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
	}

	// ��x��菜��
	unsetTetrimino(t);
	// �����Ă݂�
	if (setTetrimino(next_t) == FALSE) {
		// ���߂Ȃ�߂�
		setTetrimino(t);
		return FALSE;
	}
	// ���쒆�̃e�g���~�m�������ւ�
	currentTetrimino = next_t;
	return TRUE;
}

// �t�B�[���h��n�s�ڈȑO���ȍ~��1�s���l�߂�
void compactionLines(int n) {
	int i, k;

	for (i = n; 0 <= i; i--) {
		for (k = 0; k < FIELD_WIDTH_BLOCKS; k++) {
			playField[i][k] = playField[i-1][k];
		}
	}
	// ��ԏ�̍s��0���߂��Ă���
	for (k = 0; k < FIELD_WIDTH_BLOCKS; k++) {
		playField[0][k] = 0;
	}
}

// n�s�ڂ�������Ă����TRUE
BOOL isCompleteLine(int n) {
	int i;

	for (i = 0; i < FIELD_WIDTH_BLOCKS; i++) {
		if (playField[n][i] == FREE_BLOCK) {
			return FALSE;
		}
	}
	return TRUE;
}

// �t�B�[���h�𒲂ׂď����郉�C��������Ώ���
// �������s����Ԃ�
int eraseLines() {
	int ret = 0;
	int i;

	for (i = FIELD_HEIGHT_BLOCKS-1; 0 <= i; i--) {
		// ������Ă����1�s�l�߂�
		if (isCompleteLine(i) == TRUE) {
			compactionLines(i);
			ret++;
		}
	}
	return ret;
}

// �e�g���~�m��1���Ƃ�
BOOL downTetrimino() {
	if (moveTetrimino(currentTetrimino, MOVE_TO_DOWN) == FALSE) {
		fixTetrimino(currentTetrimino);
		playing_score += eraseLines();
		createTetrimino(0, 0, rand() % TETRIMINO_KINDS);
		return FALSE;
	}
	return TRUE;
}

// �w��s��0�݂̂Ȃ�TRUE
BOOL isEmptyRow(Tetrimino *t, int n) {
	int i;

	if (n < 0 || TETRIMINO_HEIGHT <= n) {
		return FALSE;
	}
	
	for (i = 0; i < TETRIMINO_WIDTH; i++) {
		if (t->tetrimino[n][i] != 0) {
			return FALSE;
		}
	}
	return TRUE;
}

// �w��s�ȍ~���l�߂�
void compactionRowToUp(Tetrimino *t, int n) {
	int i, k;

	for (i = n; i < TETRIMINO_HEIGHT-1; i++) {
		for (k = 0; k < TETRIMINO_WIDTH; k++) {
			t->tetrimino[i][k] = t->tetrimino[i+1][k];
			// �󂢂��Ƃ���ɂ�0�𖄂߂�
			t->tetrimino[i+1][k] = 0;
		}
	}
}

// �w���0�݂̂Ȃ�TRUE
BOOL isEmptyColumn(Tetrimino *t, int n) {
	int i;

	if (n < 0 || TETRIMINO_WIDTH <= n) {
		return FALSE;
	}
	
	for (i = 0; i < TETRIMINO_HEIGHT; i++) {
		if (t->tetrimino[i][n] != 0) {
			return FALSE;
		}
	}
	return TRUE;
}

// �w�����l�߂�
void compactionColumn(Tetrimino *t, int n) {
	int i, k;

	for (i = 0; i < TETRIMINO_HEIGHT; i++) {
		for (k = n; k < TETRIMINO_WIDTH-1; k++) {
			t->tetrimino[i][k] = t->tetrimino[i][k+1];
			// �󂢂��Ƃ���ɂ�0�𖄂߂�
			t->tetrimino[i][k+1] = 0;
		}
	}
}

// �e�g���~�m�̃o�b�t�@������ɋl�߂�悤�ɂ���
void compactionTetriminoBuffer(Tetrimino *t) {
	int i;
	// 0�����Ȃ��s���l�߂�
	for (i = 0; i < TETRIMINO_HEIGHT; i++) {
		if (isEmptyRow(t, 0) == TRUE) {
			compactionRowToUp(t, 0);
		}
	}
	// 0�����Ȃ�����l�߂�
	for (i = 0; i < TETRIMINO_WIDTH; i++) {
		if (isEmptyColumn(t, 0) == TRUE) {
			compactionColumn(t, 0);
		}
	}
}

// �e�g���~�m����]����
BOOL rotateTetrimino(Tetrimino t, RotateType type) {
	int i, k;
	Tetrimino next_t = t;
	// ��]�����e�g���~�m��p��
	if (type == CLOCKWISE) {
		for (i = 0; i < TETRIMINO_HEIGHT; i++) {
			for (k = 0; k < TETRIMINO_WIDTH; k++) {
				next_t.tetrimino[i][k] = t.tetrimino[TETRIMINO_HEIGHT-1-k][i];
			}
		}
	} else {
		for (i = 0; i < TETRIMINO_HEIGHT; i++) {
			for (k = 0; k < TETRIMINO_WIDTH; k++) {
				next_t.tetrimino[i][k] = t.tetrimino[k][TETRIMINO_WIDTH-1-i];
			}
		}
	}
	// ����ɋl�߂�
	compactionTetriminoBuffer(&next_t);
	
	// ��x��菜��
	unsetTetrimino(t);
	// �����Ă݂�
	if (setTetrimino(next_t) == FALSE) {
		// ���߂Ȃ�߂�
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

	for (y = 0; y < FIELD_HEIGHT_BLOCKS; y++) {
		for (x = 0; x < FIELD_WIDTH_BLOCKS; x++) {
			if (playField[y][x] == CONTROL_BLOCK) {
				SelectObject(hdc, CreateSolidBrush(RGB(0, 0, 255)));
				drawBlock(hdc, x, y);
			} else if (playField[y][x] == FIXED_BLOCK) {
				SelectObject(hdc, CreateSolidBrush(RGB(0, 255, 0)));
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
			downTetrimino(currentTetrimino);
			break;
		case VK_UP:
			while (downTetrimino() == TRUE)
				;
			break;
		case VK_RETURN:
			rotateTetrimino(currentTetrimino, CLOCKWISE);
			break;
		case VK_SPACE:
			rotateTetrimino(currentTetrimino, COUNTER_CLOCKWISE);
			break;
	}
}

// �A�v���P�[�V�����Ƃ��Ă̏���������
void initializeApp() {
	// �X�R�A�����Z�b�g
	playing_score = 0;
	// �e�g���~�m�I��p�ɂ��������̎�
	srand((unsigned)time(NULL));
	// ���삷��e�g���~�m��p�ӂ��Ĕz�u
	createTetrimino(0, 0, rand() % TETRIMINO_KINDS);
	// �Q�[���J�n�������L�^
	play_start_time = timeGetTime();
}

// �^�C�}�[�ŌĂяo����郁�C�����[�v
void mainLoop(HWND hwnd) {
	// �e�g���~�m�𗎂Ƃ�
	downTetrimino();

	// �ĕ`��
	InvalidateRect(hwnd, NULL, TRUE);
}

// �Q�[���̃X�R�A�t�B�[���h����\��
void drawScoreField(HDC hdc) {
	DWORD now = timeGetTime();
	static TCHAR play_time_buf[128];
	static TCHAR current_tetrimino_buf[128];
	static TCHAR score_buf[128];

	// �v���C����
	DWORD during = (now - play_start_time) / 1000;
	wsprintf(play_time_buf, "�v���C����: %d", during);

	SetTextColor(hdc , RGB(255 , 255, 255));
	SetBkColor(hdc, RGB(0 , 0, 0));
	TextOut(hdc , 350 , 10 , play_time_buf , lstrlen(play_time_buf));

	// ���݂̃e�g���~�m���
	wsprintf(current_tetrimino_buf, "type: %d, x: %d, y: %d", currentTetrimino.type, currentTetrimino.x, currentTetrimino.y);
	SetTextColor(hdc , RGB(255 , 255, 255));
	SetBkColor(hdc, RGB(0 , 0, 0));
	TextOut(hdc , 350 , 30 , current_tetrimino_buf , lstrlen(current_tetrimino_buf));
	
	// ���݂̃X�R�A
	wsprintf(score_buf, "Score: %d", playing_score * 10);
	SetTextColor(hdc , RGB(255 , 255, 255));
	SetBkColor(hdc, RGB(0 , 0, 0));
	TextOut(hdc , 350 , 60 , score_buf , lstrlen(score_buf));
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
	HDC hdc;
	PAINTSTRUCT ps;
	
	switch (msg) {
		case WM_CREATE:
			SetTimer(hwnd, 1, 1000, NULL);
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		case WM_PAINT:
			hdc = BeginPaint(hwnd, &ps);
			drawFieldBoundary(hdc);
			drawField(hdc);
			drawScoreField(hdc);
			EndPaint(hwnd, &ps);
			return 0;
		case WM_TIMER:
			mainLoop(hwnd);
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
