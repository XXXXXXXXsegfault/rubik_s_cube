#include "vector.c"
#include "rect.c"

struct r3d_camera
{
	unsigned int *fb;
	float *zbuf;
	int fbw;
	int fbh;
	int center_x;
	int center_y;
	struct vector pos;
	struct vector dirz;
	struct vector dirx;
	struct vector diry;
	float transform[12];
};
void r3d_camera_init(struct r3d_camera *camera)
{
	struct vector *vz,*vx,*vy;
	float vz_len;
	vz=&camera->dirz;
	vx=&camera->dirx;
	vy=&camera->diry;
	vz_len=vector_len(vz);
	vector_cprod(vx,vz,vy);
	vector_mul(vy,1.0/vz_len,vy);
	calculate_transform_matrix(&camera->pos,vx,vy,vz,camera->transform);
}
void r3d_zbuf_clean(struct r3d_camera *camera)
{
	memset(camera->zbuf,0x0,sizeof(float)*camera->fbw*camera->fbh);
}
#define IS_ZERO(a) ((a)<0.0000001&&(a)>-0.0000001)
void _r3d_render_line(float *zbuf,unsigned int *pbuf,unsigned int color,float cx,float a,float c,int len)
{
	while(len>0)
	{
		if(cx>*zbuf)
		{
			*zbuf=cx;
			*pbuf=color;
		}
		cx+=a;
		++zbuf;
		++pbuf;
		--len;
	}
}
void r3d_render_line(float *zbuf,unsigned int *pbuf,unsigned int color,float cx,float a,float c,int len)
{
	c=1.0/c;
	a*=c;
	cx*=c;
	_r3d_render_line(zbuf,pbuf,color,cx,a,c,len);
}

void _p_triangle(int n,int n1,struct r3d_camera *camera,struct vector *P1,struct vector *P2,struct vector *P3,unsigned int color,float a,float b,float c)
{
	int x,y,x4,x5,t,i,j;
	float z,z1,cx,cy,*zbuf;
	unsigned int *fb;
	int x1,y1,x2,y2,x3,y3;
	int Y1,Y2;
	Y1=camera->fbh*n1/n;
	Y2=camera->fbh*(n1+1)/n;
	fb=camera->fb;
	zbuf=camera->zbuf;
	x1=camera->center_x+(int)(P1->x/P1->z);
	y1=camera->center_y-(int)(P1->y/P1->z);
	x2=camera->center_x+(int)(P2->x/P2->z);
	y2=camera->center_y-(int)(P2->y/P2->z);
	x3=camera->center_x+(int)(P3->x/P3->z);
	y3=camera->center_y-(int)(P3->y/P3->z);
	if(y1>y3)
	{
		t=y1;
		y1=y3;
		y3=t;
		t=x1;
		x1=x3;
		x3=t;
	}
	if(y1>y2)
	{
		t=y1;
		y1=y2;
		y2=t;
		t=x1;
		x1=x2;
		x2=t;
	}
	if(y2>y3)
	{
		t=y2;
		y2=y3;
		y3=t;
		t=x2;
		x2=x3;
		x3=t;
	}
	y=y1;
	if(y<Y1)
	{
		y=Y1;
	}
	i=y*camera->fbw;
	cy=1.0-b*(float)(camera->center_y-y);
	while(y<y2&&y<Y2)
	{
		x4=x1+(y-y1)*(x2-x1)/(y2-y1);
		x5=x1+(y-y1)*(x3-x1)/(y3-y1);
		if(x4>x5)
		{
			t=x4;
			x4=x5;
			x5=t;
		}
		if(x4<0)
		{
			x4=0;
		}
		if(x5>camera->fbw)
		{
			x5=camera->fbw;
		}
		x=x4;
		cx=cy-a*(float)(x-camera->center_x);
		r3d_render_line(zbuf+i+x,fb+i+x,color,cx,-a,c,x5-x4);
		i+=camera->fbw;
		cy+=b;
		y+=1;
	}
	while(y<y3&&y<Y2)
	{
		x4=x2+(y-y2)*(x3-x2)/(y3-y2);
		x5=x1+(y-y1)*(x3-x1)/(y3-y1);
		if(x4>x5)
		{
			t=x4;
			x4=x5;
			x5=t;
		}
		if(x4<0)
		{
			x4=0;
		}
		if(x5>camera->fbw)
		{
			x5=camera->fbw;
		}
		x=x4;
		cx=cy-a*(float)(x-camera->center_x);
		r3d_render_line(zbuf+i+x,fb+i+x,color,cx,-a,c,x5-x4);
		i+=camera->fbw;
		cy+=b;
		y+=1;
	}
}
void __p_intersection(struct vector *P1,struct vector *P2,struct vector *result)
{
	struct vector v;
	float n;
	vector_sub(P1,P2,&v);
	if(IS_ZERO(v.z))
	{
		result->x=P2->x;
		result->y=P2->y;
		result->z=P2->z;
		return;
	}
	n=(1.0-P2->z)/v.z;
	vector_mul(&v,n,&v);
	vector_add(P2,&v,result);
}
void r3d_paint_triangle2(int n,int n1,struct r3d_camera *camera,struct vector *p1,struct vector *p2,struct vector *p3,unsigned int color)
{
	struct vector P1,P2,P3,P4,P5;
	float a,b,c;
	int X1,X2,X3,Y1,Y2,Y3,Y4,Y5;
	coord_transform(camera->transform,p1,&P1);
	coord_transform(camera->transform,p2,&P2);
	coord_transform(camera->transform,p3,&P3);
	if(P1.z<1.0&&P2.z<1.0&&P3.z<1.0)
	{
		return;
	}
	X1=camera->center_x+(int)(P1.x/P1.z);
	X2=camera->center_x+(int)(P2.x/P2.z);
	X3=camera->center_x+(int)(P3.x/P3.z);
	Y1=camera->center_y-(int)(P1.y/P1.z);
	Y2=camera->center_y-(int)(P2.y/P2.z);
	Y3=camera->center_y-(int)(P3.y/P3.z);
	Y4=camera->fbh*n1/n;
	Y5=camera->fbh*(n1+1)/n;
	if(X1<0&&X2<0&&X3<0)
	{
		return;
	}
	if(Y1<Y4&&Y2<Y4&&Y3<Y4)
	{
		return;
	}
	if(X1>=camera->fbw&&X2>=camera->fbw&&X3>=camera->fbw)
	{
		return;
	}
	if(Y1>=Y5&&Y2>=Y5&&Y3>=Y5)
	{
		return;
	}
	a=__solve3(
P1.x,P1.y,1.0,P1.z,
P2.x,P2.y,1.0,P2.z,
P3.x,P3.y,1.0,P3.z);
	b=__solve3(
P1.y,P1.x,1.0,P1.z,
P2.y,P2.x,1.0,P2.z,
P3.y,P3.x,1.0,P3.z);
	c=__solve3(
1.0,P1.x,P1.y,P1.z,
1.0,P2.x,P2.y,P2.z,
1.0,P3.x,P3.y,P3.z);
	if(P1.z<1.0)
	{
		if(P2.z<1.0)
		{
			__p_intersection(&P3,&P1,&P4);
			__p_intersection(&P3,&P2,&P5);
			_p_triangle(n,n1,camera,&P3,&P4,&P5,color,a,b,c);
		}
		else if(P3.z<1.0)
		{
			__p_intersection(&P2,&P1,&P4);
			__p_intersection(&P2,&P3,&P5);
			_p_triangle(n,n1,camera,&P2,&P4,&P5,color,a,b,c);
		}
		else
		{
			__p_intersection(&P2,&P1,&P4);
			__p_intersection(&P3,&P1,&P5);
			_p_triangle(n,n1,camera,&P2,&P3,&P5,color,a,b,c);
			_p_triangle(n,n1,camera,&P5,&P4,&P2,color,a,b,c);
		}
	}
	else if(P2.z<1.0)
	{
		if(P3.z<1.0)
		{
			__p_intersection(&P1,&P2,&P4);
			__p_intersection(&P1,&P3,&P5);
			_p_triangle(n,n1,camera,&P1,&P4,&P5,color,a,b,c);
		}
		else
		{
			__p_intersection(&P1,&P2,&P4);
			__p_intersection(&P3,&P2,&P5);
			_p_triangle(n,n1,camera,&P1,&P3,&P5,color,a,b,c);
			_p_triangle(n,n1,camera,&P5,&P4,&P1,color,a,b,c);
		}
	}
	else if(P3.z<1.0)
	{
		__p_intersection(&P1,&P3,&P4);
		__p_intersection(&P2,&P3,&P5);
		_p_triangle(n,n1,camera,&P1,&P2,&P5,color,a,b,c);
		_p_triangle(n,n1,camera,&P5,&P4,&P1,color,a,b,c);
	}
	else
	{
		_p_triangle(n,n1,camera,&P1,&P2,&P3,color,a,b,c);
	}
}
#define r3d_paint_triangle(camera,p1,p2,p3,color) r3d_paint_triangle2(1,0,camera,p1,p2,p3,color)
#undef IS_ZERO
