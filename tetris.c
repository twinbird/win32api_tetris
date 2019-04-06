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

// ブロックの種類
enum blockType {
	// ブロックがない
	FREE_BLOCK,
	// 固定ブロック
	FIXED_BLOCK,
	// 制御中のブロック
	CONTROL_BLOCK
};

// ゲーム開始時刻
DWORD play_start_time;

// ゲームのスコア
int playing_score;

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
		{0,0,0,0}
	},
	{
		{1,1,0,0},
		{0,1,0,0},
		{0,1,0,0},
		{0,0,0,0}
	}
};

// テトリミノ
typedef struct _Tetrimino {
	int x;
	int y;
	int type;
	int tetrimino[TETRIMINO_HEIGHT][TETRIMINO_WIDTH];
} Tetrimino;

// テトリミノの移動方式
typedef enum _MoveType {
	MOVE_TO_LEFT,
	MOVE_TO_RIGHT,
	MOVE_TO_DOWN
} MoveType;

// テトリミノの回転方式
typedef enum _RotateType {
	CLOCKWISE,
	COUNTER_CLOCKWISE
} RotateType;

// 操作中のテトリミノ
Tetrimino currentTetrimino; 

// テトリミノを固定する
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

// 指定位置にテトリミノが配置できるか確認
BOOL collisionTetrimino(Tetrimino t, int px, int py) {
	int x, y;

	// 起点がフィールド領域を超えている
	if (px < 0 || FIELD_WIDTH_BLOCKS <= px) {
		return FALSE;
	}
	if (py < 0 || FIELD_HEIGHT_BLOCKS <= py) {
		return FALSE;
	}

	// テトリミノの部分のチェック
	for (y = 0; y < TETRIMINO_HEIGHT; y++) {
		for (x = 0; x < TETRIMINO_WIDTH; x++) {
			// テトリミノのブロックがなければ判定外
			if (t.tetrimino[y][x] == 0) {
				continue;
			}
			// テトリミノをフィールドにマッピングした座標
			int block_x = px + x;
			int block_y = py + y;

			// フィールド領域を超えた
			if (block_x < 0 || FIELD_WIDTH_BLOCKS <= block_x) {
				return FALSE;
			}
			if (block_y < 0 || FIELD_HEIGHT_BLOCKS <= block_y) {
				return FALSE;
			}
			// すでにフィールドにブロックがあった
			if (playField[block_y][block_x] != FREE_BLOCK) {
				return FALSE;
			}
		}
	}
	return TRUE;
}

// テトリミノを設置する
BOOL setTetrimino(Tetrimino t) {
	int x, y;

	// 配置判定
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

// テトリミノを取り除く
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

// 新しいテトリミノを作成して操作中に設定
void createTetrimino(int x, int y, int type) {
	int i, k;
	Tetrimino t;

	// 操作するテトリミノを用意して配置
	t.x = x;
	t.y = y;
	t.type = type;
	for (i = 0; i < TETRIMINO_HEIGHT; i++) {
		for (k = 0; k < TETRIMINO_WIDTH; k++) {
			t.tetrimino[i][k] = tetriminos[type][i][k];
		}
	}
	currentTetrimino = t;

	// 配置
	setTetrimino(currentTetrimino);
}

// テトリミノを移動する
BOOL moveTetrimino(Tetrimino t, MoveType type) {
	// 次の場所のテトリミノ
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

	// 一度取り除く
	unsetTetrimino(t);
	// おいてみる
	if (setTetrimino(next_t) == FALSE) {
		// だめなら戻す
		setTetrimino(t);
		return FALSE;
	}
	// 操作中のテトリミノを差し替え
	currentTetrimino = next_t;
	return TRUE;
}

// フィールドのn行目以前を以降に1行分詰める
void compactionLines(int n) {
	int i, k;

	for (i = n; 0 <= i; i--) {
		for (k = 0; k < FIELD_WIDTH_BLOCKS; k++) {
			playField[i][k] = playField[i-1][k];
		}
	}
	// 一番上の行は0埋めしておく
	for (k = 0; k < FIELD_WIDTH_BLOCKS; k++) {
		playField[0][k] = 0;
	}
}

// n行目がそろっていればTRUE
BOOL isCompleteLine(int n) {
	int i;

	for (i = 0; i < FIELD_WIDTH_BLOCKS; i++) {
		if (playField[n][i] == FREE_BLOCK) {
			return FALSE;
		}
	}
	return TRUE;
}

// フィールドを調べて消せるラインがあれば消す
// 消した行数を返す
int eraseLines() {
	int ret = 0;
	int i;

	for (i = FIELD_HEIGHT_BLOCKS-1; 0 <= i; i--) {
		// そろっていれば1行詰める
		if (isCompleteLine(i) == TRUE) {
			compactionLines(i);
			ret++;
		}
	}
	return ret;
}

// テトリミノを1つ落とす
BOOL downTetrimino() {
	if (moveTetrimino(currentTetrimino, MOVE_TO_DOWN) == FALSE) {
		fixTetrimino(currentTetrimino);
		playing_score += eraseLines();
		createTetrimino(0, 0, rand() % TETRIMINO_KINDS);
		return FALSE;
	}
	return TRUE;
}

// 指定行が0のみならTRUE
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

// 指定行以降を詰める
void compactionRowToUp(Tetrimino *t, int n) {
	int i, k;

	for (i = n; i < TETRIMINO_HEIGHT-1; i++) {
		for (k = 0; k < TETRIMINO_WIDTH; k++) {
			t->tetrimino[i][k] = t->tetrimino[i+1][k];
			// 空いたところには0を埋める
			t->tetrimino[i+1][k] = 0;
		}
	}
}

// 指定列が0のみならTRUE
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

// 指定列を詰める
void compactionColumn(Tetrimino *t, int n) {
	int i, k;

	for (i = 0; i < TETRIMINO_HEIGHT; i++) {
		for (k = n; k < TETRIMINO_WIDTH-1; k++) {
			t->tetrimino[i][k] = t->tetrimino[i][k+1];
			// 空いたところには0を埋める
			t->tetrimino[i][k+1] = 0;
		}
	}
}

// テトリミノのバッファを左上に詰めるようにする
void compactionTetriminoBuffer(Tetrimino *t) {
	int i;
	// 0しかない行を詰める
	for (i = 0; i < TETRIMINO_HEIGHT; i++) {
		if (isEmptyRow(t, 0) == TRUE) {
			compactionRowToUp(t, 0);
		}
	}
	// 0しかない列を詰める
	for (i = 0; i < TETRIMINO_WIDTH; i++) {
		if (isEmptyColumn(t, 0) == TRUE) {
			compactionColumn(t, 0);
		}
	}
}

// テトリミノを回転する
BOOL rotateTetrimino(Tetrimino t, RotateType type) {
	int i, k;
	Tetrimino next_t = t;
	// 回転したテトリミノを用意
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
	// 左上に詰める
	compactionTetriminoBuffer(&next_t);
	
	// 一度取り除く
	unsetTetrimino(t);
	// おいてみる
	if (setTetrimino(next_t) == FALSE) {
		// だめなら戻す
		setTetrimino(t);
		return FALSE;
	}
	// 操作中のテトリミノを差し替え
	currentTetrimino = next_t;
	return TRUE;
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

// キー操作処理
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

// アプリケーションとしての初期化処理
void initializeApp() {
	// スコアをリセット
	playing_score = 0;
	// テトリミノ選択用につかう乱数の種
	srand((unsigned)time(NULL));
	// 操作するテトリミノを用意して配置
	createTetrimino(0, 0, rand() % TETRIMINO_KINDS);
	// ゲーム開始時刻を記録
	play_start_time = timeGetTime();
}

// タイマーで呼び出されるメインループ
void mainLoop(HWND hwnd) {
	// テトリミノを落とす
	downTetrimino();

	// 再描画
	InvalidateRect(hwnd, NULL, TRUE);
}

// ゲームのスコアフィールド欄を表示
void drawScoreField(HDC hdc) {
	DWORD now = timeGetTime();
	static TCHAR play_time_buf[128];
	static TCHAR current_tetrimino_buf[128];
	static TCHAR score_buf[128];

	// プレイ時間
	DWORD during = (now - play_start_time) / 1000;
	wsprintf(play_time_buf, "プレイ時間: %d", during);

	SetTextColor(hdc , RGB(255 , 255, 255));
	SetBkColor(hdc, RGB(0 , 0, 0));
	TextOut(hdc , 350 , 10 , play_time_buf , lstrlen(play_time_buf));

	// 現在のテトリミノ情報
	wsprintf(current_tetrimino_buf, "type: %d, x: %d, y: %d", currentTetrimino.type, currentTetrimino.x, currentTetrimino.y);
	SetTextColor(hdc , RGB(255 , 255, 255));
	SetBkColor(hdc, RGB(0 , 0, 0));
	TextOut(hdc , 350 , 30 , current_tetrimino_buf , lstrlen(current_tetrimino_buf));
	
	// 現在のスコア
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

	// ウィンドウサイズを変更
	// フィールドサイズに左右分のマージンとスコア表示幅分を加えたサイズにする
	setClientSize(hwnd,
			(BLOCK_PIXEL_SIZE * FIELD_WIDTH_BLOCKS) + (WINDOW_PADDING_PIXEL * 2) + SCORE_FIELD_WIDTH_PIXEL,
			(BLOCK_PIXEL_SIZE * FIELD_HEIGHT_BLOCKS) + (WINDOW_PADDING_PIXEL * 2));

	// アプリケーションの初期化
	initializeApp();

	while (GetMessage(&msg, NULL, 0, 0)) {
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
