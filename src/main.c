#include "windows.c"
#include "windef.c"
#include "mem.c"
#include "lock.c"
#include "3drenderer.c"
#define WINW 400
#define WINH 400
#define CX 200
#define CY 200
#define PI 3.141592653589793
void *lock;
int paint;
unsigned int pbuf[WINW*WINH];
double zbuf[WINW*WINH];
struct r3d_camera camera;
struct vector X,Y,Z;
struct wndclassex wc;
void *dc,*memdc;
void *bitmap;
#include "cube.c"
void paint_all(void)
{
	long t,t1;
	p_cube();
	if(rotating)
	{
		t=clock();
		t1=t-rotate_time;
		if(rotate_angle<0.0)
		{
			rotate_angle+=PI*(double)t1/500.0;
			if(rotate_angle>0.0)
			{
				rotating=0;
			}
		}
		else
		{
			rotate_angle-=PI*(double)t1/500.0;
			if(rotate_angle<0.0)
			{
				rotating=0;
			}
		}
		rotate_time=t;
		paint=1;
	}
}
void clear(void)
{
	memset(pbuf,0,sizeof(pbuf));
}
void display(void *dc)
{
	SetBitmapBits(bitmap,sizeof(pbuf),pbuf);
	BitBlt(dc,0,0,WINW,WINH,memdc,0,0,SRCCOPY);
}

int _T_paint(void *param); // NOTE: SCC uses a different calling convention
asm "@_T_paint"
asm "push %rcx"
asm "call @T_paint"
asm "add $8,%rsp"
asm "ret"
int T_paint(void *param)
{
	while(1)
	{
		if(paint)
		{
			paint=0;
			mutex_lock(&lock);
			clear();
			paint_all();
			display(dc);
			mutex_unlock(&lock);
		}
		Sleep(5);
	}
}
int _WndProc(void *hwnd,unsigned int Message,unsigned int wParam,unsigned int lParam); // NOTE: SCC uses a different calling convention
asm "@_WndProc"
asm "push %r9"
asm "push %r8"
asm "push %rdx"
asm "push %rcx"
asm "call @WndProc"
asm "add $32,%rsp"
asm "ret"
int WndProc(void *hwnd,unsigned int Message,unsigned int wParam,unsigned int lParam)
{
	static int left_click_valid,right_click_valid;
	static int left_click_x,left_click_y;
	static int right_click_x,right_click_y;
	static int left_total,right_total;
	int x,y,ax,ay;
	if(Message==WM_DESTROY)
	{
		exit(0);
	}
	else if(Message==WM_PAINT)
	{
		struct paintstruct ps;
		mutex_lock(&lock);
		BeginPaint(hwnd,&ps);
		clear();
		paint_all();
		display(ps.hdc);
		EndPaint(hwnd,&ps);
		mutex_unlock(&lock);
	}
	else if(Message==WM_LBUTTONDOWN)
	{
		left_click_valid=1;
		left_click_x=LOWORD(lParam);
		left_click_y=HIWORD(lParam);
		left_total=0;
	}
	else if(Message==WM_RBUTTONDOWN)
	{
		right_click_valid=1;
		right_click_x=LOWORD(lParam);
		right_click_y=HIWORD(lParam);
		right_total=0;
	}
	else if(Message==WM_LBUTTONUP)
	{
		if(left_total<15)
		{
			x=LOWORD(lParam);
			y=HIWORD(lParam);
			click_cube(x-CX,y-CY,0);
		}
		left_click_valid=0;
	}
	else if(Message==WM_RBUTTONUP)
	{
		if(right_total<15)
		{
			x=LOWORD(lParam);
			y=HIWORD(lParam);
			click_cube(x-CX,y-CY,1);
		}
		right_click_valid=0;
	}
	else if(Message==WM_MOUSEMOVE)
	{
		x=LOWORD(lParam);
		y=HIWORD(lParam);
		
		if(wParam&1&&left_click_valid)
		{
			ax=x-left_click_x;
			ay=y-left_click_y;
			if(ax<0)
			{
				ax=-ax;
			}
			if(ay<0)
			{
				ay=-ay;
			}
			cube_rotate(x-left_click_x,y-left_click_y);
			left_total+=ax+ay;
			left_click_x=x;
			left_click_y=y;
			paint=1;
		}
		if(wParam&2&&right_click_valid)
		{
			ax=x-right_click_x;
			ay=y-right_click_y;
			if(ax<0)
			{
				ax=-ax;
			}
			if(ay<0)
			{
				ay=-ay;
			}
			right_total+=ax+ay;
			right_click_x=x;
			right_click_y=y;
		}
	}
	else if(Message==WM_ACTIVATE)
	{
		if(wParam==0)
		{
			left_click_valid=0;
			right_click_valid=0;
		}
	}
	return DefWindowProcA(hwnd,Message,wParam,lParam);
}
int main(int argc,char **argv,void *hInstance)
{
	void *hwnd;
	struct msg msg;
	vinit(&camera.pos,-20.0,0.0,0.0);
	vinit(&camera.dirz,5.0,0.0,0.0);
	vinit(&camera.dirx,0.0,-0.004,0.0);
	camera.fb=pbuf;
	camera.zbuf=zbuf;
	camera.fbw=WINW;
	camera.fbh=WINH;
	camera.center_x=CX;
	camera.center_y=CY;
	r3d_camera_init(&camera);
	vinit(&X,1.0,0.0,0.0);
	vinit(&Y,0.0,1.0,0.0);
	vinit(&Z,0.0,0.0,1.0);
	cube_init();

	wc.cbSize=sizeof(wc);
	wc.lpfnWndProc=_WndProc;
	wc.lpszClassName="rubik_s_cube";
	wc.hInstance=hInstance;
	wc.hbrBackground=(void *)8;
	wc.hCursor=LoadCursorA(NULL,IDC_ARROW);
	wc.hIcon=LoadIconA(NULL,IDI_APPLICATION);
	if(!RegisterClassExA(&wc))
	{
		MessageBoxA(NULL,"Cannot register window class","Error",0);
		return 0;
	}
	hwnd=CreateWindowExA(WS_EX_WINDOWEDGE,"rubik_s_cube","Rubik\'s Cube",WS_VISIBLE|WS_SYSMENU|WS_CAPTION,0,0,WINW+6,WINH+29,NULL,NULL,hInstance,NULL);
	if(hwnd==NULL)
	{
		MessageBoxA(NULL,"Cannot create window","Error",0);
		return 0;
	}
	dc=GetDC(hwnd);
	memdc=CreateCompatibleDC(dc);
	bitmap=CreateCompatibleBitmap(dc,WINW,WINH);
	SelectObject(memdc,bitmap);
	
	lock=CreateMutexA(NULL,0,NULL);
	CreateThread(NULL,0,_T_paint,NULL,0,NULL);
	paint=1;
	while(GetMessageA(&msg,NULL,0,0)>0)
	{
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}
	return msg.wparam;
}