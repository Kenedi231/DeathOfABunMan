#include "conio.h"    // ��� ������� getch()
#include <string.h>

struct screen_point {    //
	unsigned char chr;    //
	unsigned char attr;    // ��� ��� ����� ��� ������
};      // �� ��p��.
typedef struct screen_point screen_line[80]; //
screen_line * scr;    //
char movecost[10][10] = {
	{ 0,0,0,0,0,0,0,0,0,0 },
	{ 0,1,6,6,6,6,6,1,1,0 },
	{ 0,1,0,0,0,0,6,0,0,0 },
	{ 0,1,0,1,1,1,1,1,1,0 },
	{ 0,1,0,1,1,0,0,0,1,0 }, // ��� � ���� ����p���
	{ 0,1,0,1,0,0,1,0,1,0 }, // 0 - �����
	{ 0,1,0,1,0,1,1,0,1,0 }, // ����� �p���� �����-
	{ 0,1,0,0,0,0,0,0,1,0 }, //  ������� �p����������
	{ 0,1,8,1,1,1,1,1,1,0 }, //  1- ������ �p����������
	{ 0.0,0,0,0,0,0,0,0,0 }
};
unsigned char fillmap[10][10];  // P����p == p����p� ����p���� !
								// ���� ���� ����� ���� �������
								// 255 ���� �������� byte->word
struct {
	signed char x, y;  // ���p������ � ����p����
}buf[256];   // ��� ������ ����p���, ��� ������ ������
			 // ���� ���� ������
unsigned char bufp, bufe; // �������� � buf

int sx, sy, tx, ty;  // H�������� � �������� ���p������ ����

					 /*
					 ��� ����� ��H������� ������� H� ��P�H �
					 H� ����� H������� ��H���H�� � ����P����
					 */
void clrscr() {  // �������� ��p��
	int i;
	for (i = 0; i<80 * 25; i++)((short*)scr)[i] = 0x0720;
}

// H��������� ��p��� str � ���p������� (x,y) ������ attr
void writestr(int x, int y, char str[], char attr) {
	int i;
	for (i = 0; str[i] != 0; i++, x++) { scr[y][x].chr = str[i]; scr[y][x].attr = attr; }
}

// P����� ��������� ��p����� ����p����
void draw_maze() {
	int i, j;
	for (j = 0; j<10; j++)for (i = 0; i<10; i++) {
		scr[j][i * 2].attr = 16 * (7 - movecost[j][i]) + 7 + 8 * ((i + j) & 1);
		scr[j][i * 2 + 1].attr = 16 * (7 - movecost[j][i]) + 7 + 8 * ((i + j) & 1);
	}
	scr[sy][sx * 2].chr = '['; scr[sy][sx * 2 + 1].chr = ']';
	scr[ty][tx * 2].chr = '<'; scr[ty][tx * 2 + 1].chr = '>';
	scr[1][40].attr = 16 * (7 - 1); writestr(45, 1, "������ �����", 7);
	scr[3][40].attr = 16 * (7 - 0); writestr(45, 3, "�����", 7);
	scr[5][40].attr = 16 * (7 - 6); writestr(45, 5, "������", 7);
	writestr(40, 7, "[]    H�������� �����", 7);
	writestr(40, 9, "<>    ���� ����", 7);
}

/*
� ��� ������ ��� ���� P��������� ����P����
*/

/* ��� ������� �p���p��� �������� �� �p���������� ���� � ����� �����
��p�����,
��� �������� p����, � ���� ��, �� ���������� ����� � buf.      */
void push(int x, int y, int n) {
	if (fillmap[y][x] <= n)return; // ���� ����� ���� �� ��p���-����� ���
	fillmap[y][x] = n;   // ���������� ����� ����� ����
	buf[bufe].x = x;    //
	buf[bufe].y = y;    // ���������� �����
	bufe++;   // P����p buf-256 bufe - byte, ���������� ����,
			  // ����� ���� ������ bufe=(bufe+1)%(p����p buf)
	scr[y][x * 2].chr = n / 10 + 48;     //
										 //��� �p���� p�������� � �������� ������� ������
	scr[y][x * 2 + 1].chr = (n % 10) + 48;
	getch();   //
}
/* ����� ��p���� ���p����� ����� �� buf � ����p������� 1,
���� �p��� ������, �� ����p������� 0           */
int pop(int *x, int *y) {
	if (bufp == bufe)return 0;
	*x = buf[bufp].x;
	*y = buf[bufp].y;
	bufp++;   // �� ��, ��� � � bufe !!!  ��. ^
	return 1;
}
/* �H���H�� !!! H� ����p� �� �������� ������� (push � pop)
buf ��� �� stack ! ��� ��������� FIFO-���� ����p !    */

/* ���, ��� �����, ���-�� ���� � ����          */

void fill(int sx, int sy, int tx, int ty) {
	int x, y, n, t;
	// ������� fillmap ����������� max ���������
	_fmemset(fillmap, 0xFF, sizeof(fillmap));
	bufp = bufe = 0;    // ����� �������...
	push(sx, sy, 0);    // ���� � ��������� ����� =0, ������� ?
	while (pop(&x, &y)) {   // ����, ���� ���� ����� � ����p�
		if ((x == tx) && (y == ty)) {
			writestr(0, 20, "H����� ���� ������     ", 15);
			scr[20][19].chr = n / 10 + 48;
			scr[20][20].chr = (n % 10) + 48;
			//   break;// ���� p���������p��� ���� break, �� ���� ���������
			// ��� ������ �������� 1-�� �� ����. ��� �������
			// �������, ���� ��p��������� ���� ������ ���������.
		}
		// n=����� ���� �� ����� �������� ������
		n = fillmap[y][x] + movecost[y][x];
		//��p���p 4-� �������� ������
		if (movecost[y + 1][x])push(x, y + 1, n); //
		if (movecost[y - 1][x])push(x, y - 1, n); // 
		if (movecost[y][x + 1])push(x + 1, y, n); //
		if (movecost[y][x - 1])push(x - 1, y, n); //
	}

	// ���� �� ����� 1-�� ���� � ���������� �� break-�,
	// ���� ������ ��� ��� ��p��

	if (fillmap[ty][tx] == 0xFF) {
		writestr(0, 20, "���� �� ���������� !!!", 15);
		return;
	}
	else
		writestr(0, 20, "������� ���������, �p������� �� ���� !!!", 15);

	x = tx; y = ty; n = 0xFF;    // �� ������ ������� �� (sx,sy), ������
								 // �� ���� �p������ ���� �� (tx,ty)
	while ((x != sx) || (y != sy)) {  // ���� �� �p���� � (sx,sy)
		scr[y][x * 2].attr = 2 * 16; scr[y][x * 2 + 1].attr = 2 * 16;  // P��������
																	   // ����� ������ ��������
		if (fillmap[y + 1][x]<n) { tx = x; ty = y + 1; t = fillmap[y + 1][x]; }
		// ������, ����p�����
		if (fillmap[y - 1][x]<n) { tx = x; ty = y - 1; t = fillmap[y - 1][x]; }
		// ����������� ��������
		if (fillmap[y][x + 1]<n) { tx = x + 1; ty = y; t = fillmap[y][x + 1]; }
		if (fillmap[y][x - 1]<n) { tx = x - 1; ty = y; t = fillmap[y][x - 1]; }
		x = tx; y = ty; n = t;   // ��p������ � �������� ������

		getch();   // ���� ������� ������
	}
	// ��� � ��� ! ���� ������ !
}

void main() {
	int i;
	sx = 1; sy = 1;   // H�������� �����
	tx = 3; ty = 3;  // ���� ����

	scr = (screen_line*)0xB8000; //
	clrscr();      // ��� ��� p��������
	draw_maze();    //
	getch();    //

	fill(sx, sy, tx, ty); // H����� ����

}