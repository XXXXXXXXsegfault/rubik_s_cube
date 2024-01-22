

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
	double transform[12];
};
void r3d_camera_init(struct r3d_camera *camera)
{
	struct vector *vz,*vx,*vy;
	double vz_len;
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
#define IS_ZERO(a) ((a)<0.0001&&(a)>-0.0001)
void _r3d_render_line(float *zbuf,unsigned int *pbuf,unsigned int color,float a1,float *a2,float *b,int len);
asm "@_r3d_render_line"
asm "push %rbx"
asm "mov 48(%rsp),%rax"
asm "movups (%rax),%xmm5"
asm "mov 56(%rsp),%rax"
asm "movups (%rax),%xmm6"
asm "mov 16(%rsp),%rax"
asm "mov 24(%rsp),%rdx"
asm "movss 32(%rsp),%xmm7"
asm "shufps $0,%xmm7,%xmm7"
asm "mov 64(%rsp),%ecx"
asm "sub $4,%ecx"
asm "jb @_r3d_render_line_X1"
asm "@_r3d_render_line_X2"
asm "movups (%rax),%xmm1"
asm "movups (%rdx),%xmm2"
asm "movups %xmm1,%xmm3"
asm "movups %xmm1,%xmm8"
asm "cmpps $1,%xmm5,%xmm1"
asm "cmpps $5,%xmm5,%xmm3"
asm "movups %xmm5,%xmm4"
asm "andps %xmm3,%xmm8"
asm "andps %xmm1,%xmm4"
asm "andps %xmm2,%xmm3"
asm "andps %xmm7,%xmm1"
asm "orps %xmm3,%xmm1"
asm "orps %xmm4,%xmm8"
asm "movups %xmm1,(%rdx)"
asm "movups %xmm8,(%rax)"
asm "add $16,%rax"
asm "add $16,%rdx"
asm "addps %xmm6,%xmm5"
asm "sub $4,%ecx"
asm "jae @_r3d_render_line_X2"
asm "@_r3d_render_line_X1"
asm "test $2,%cl"
asm "je @_r3d_render_line_X3"
asm "movsd (%rax),%xmm1"
asm "movsd (%rdx),%xmm2"
asm "movups %xmm1,%xmm3"
asm "movups %xmm1,%xmm8"
asm "cmpps $1,%xmm5,%xmm1"
asm "cmpps $5,%xmm5,%xmm3"
asm "movups %xmm5,%xmm4"
asm "andps %xmm3,%xmm8"
asm "andps %xmm1,%xmm4"
asm "andps %xmm2,%xmm3"
asm "andps %xmm7,%xmm1"
asm "orps %xmm3,%xmm1"
asm "orps %xmm4,%xmm8"
asm "movsd %xmm1,(%rdx)"
asm "movsd %xmm8,(%rax)"
asm "add $8,%rax"
asm "add $8,%rdx"
asm "addss 40(%rsp),%xmm5"
asm "addss 40(%rsp),%xmm5"
asm "@_r3d_render_line_X3"
asm "test $1,%cl"
asm "je @_r3d_render_line_X4"
asm "movss (%rax),%xmm1"
asm "movss (%rdx),%xmm2"
asm "movups %xmm1,%xmm3"
asm "movups %xmm1,%xmm8"
asm "cmpps $1,%xmm5,%xmm1"
asm "cmpps $5,%xmm5,%xmm3"
asm "movups %xmm5,%xmm4"
asm "andps %xmm3,%xmm8"
asm "andps %xmm1,%xmm4"
asm "andps %xmm2,%xmm3"
asm "andps %xmm7,%xmm1"
asm "orps %xmm3,%xmm1"
asm "orps %xmm4,%xmm8"
asm "movss %xmm1,(%rdx)"
asm "movss %xmm8,(%rax)"
asm "@_r3d_render_line_X4"
asm "pop %rbx"
asm "ret"

void r3d_render_line(int *zbuf,unsigned int *pbuf,unsigned int color,float cx,float a,int len)
{
	float a2[4],b[4];
	a2[0]=cx;
	a2[1]=cx+a;
	a2[2]=cx+a*2.0;
	a2[3]=cx+a*3.0;
	b[0]=a*4.0;
	b[1]=a*4.0;
	b[2]=a*4.0;
	b[3]=a*4.0;
	_r3d_render_line(zbuf,pbuf,color,a,a2,b,len);
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
void _p_triangle(struct r3d_camera *camera,struct vector *P1,struct vector *P2,struct vector *P3,unsigned int color,int thread,int num_threads)
{
	double a,b,c,d1,d2,depth[2],d,t[2];
	double x[3];
	double y[3];
	double z[3];
	struct vector *p;
	int y1,y2,y3,x_[5],y_[3];
	int offset;
	double x1,x2,Y,X;
	int X1,X2;
	int ry1,ry2;
	ry1=camera->fbh*thread/num_threads;
	ry2=camera->fbh*(thread+1)/num_threads;
	if(P1->y*P2->z<P2->y*P1->z)
	{
		p=P1;
		P1=P2;
		P2=p;
	}
	if(P1->y*P3->z<P3->y*P1->z)
	{
		p=P1;
		P1=P3;
		P3=p;
	}
	if(P2->y*P3->z<P3->y*P2->z)
	{
		p=P2;
		P2=P3;
		P3=p;
	}
	x[0]=P1->x/P1->z;
	x[1]=P2->x/P2->z;
	x[2]=P3->x/P3->z;
	y[0]=P1->y/P1->z;
	y[1]=P2->y/P2->z;
	y[2]=P3->y/P3->z;
	z[0]=1.0/P1->z;
	z[1]=1.0/P2->z;
	z[2]=1.0/P3->z;
	x_[0]=(int)x[0];
	x_[1]=(int)x[1];
	x_[2]=(int)x[2];
	y_[0]=(int)y[0];
	y_[1]=(int)y[1];
	y_[2]=(int)y[2];
	depth[0]=z[0];
	depth[1]=z[0];
	t[0]=z[0]+(z[2]-z[0])*(y[1]-y[0])/(y[2]-y[0]);
	t[1]=x[0]+(x[2]-x[0])*(y[1]-y[0])/(y[2]-y[0]);
	if(IS_ZERO(t[1]-x[1]))
	{
		return;
	}
	a=(t[0]-z[1])/(t[1]-x[1]);
	b=(z[2]-z[0])/(y[2]-y[0]);
	y1=camera->center_y-y_[0];
	y2=camera->center_y-y_[1];
	y3=y_[0];
	Y=(double)y_[0]-y[0];
	x1=x[0];
	x2=x1;
	if(y2>ry2)
	{
		y2=ry2;
	}
	if(y_[0]<=y_[2])
	{
		x_[3]=x_[0];
		x_[4]=x_[0];
		if(x_[1]<x_[3])
		{
			x_[3]=x_[1];
		}
		if(x_[2]<x_[3])
		{
			x_[3]=x_[2];
		}
		if(x_[1]>x_[4])
		{
			x_[4]=x_[1];
		}
		if(x_[2]>x_[4])
		{
			x_[4]=x_[2];
		}
		X1=x_[3]+camera->center_x;
		X2=x_[4]+camera->center_x;
		d=z[0];
		if(X1<0)
		{
			d-=a*(double)X1;
			X1=0;
		}
		X=(double)x_[3]-x[0];
		d+=a*X;
		if(X2>camera->fbw)
		{
			X2=camera->fbw;
		}
		if(X1<X2&&y1>=ry1&&y1<ry2)
		{
			offset=y1*camera->fbw+X1;
			r3d_render_line(camera->zbuf+offset,camera->fb+offset,color,d,a,X2-X1);
		}
		return;
	}
	if(y1<y2)
	{
		c=(z[1]-z[0])/(y[1]-y[0]);
		d1=(x[1]-x[0])/(y[1]-y[0]);
		d2=(x[2]-x[0])/(y[2]-y[0]);
		x1+=d1*Y;
		x2+=d2*Y;
		depth[0]+=c*Y;
		depth[1]+=b*Y;
		if(y1<ry1)
		{
			x1+=d1*(double)(y1-ry1);
			x2+=d2*(double)(y1-ry1);
			depth[0]+=c*(double)(y1-ry1);
			depth[1]+=b*(double)(y1-ry1);
			y3+=y1-ry1;
			y1=ry1;
		}
		while(y1<y2)
		{
			x_[3]=x_[0]+(x_[1]-x_[0])*(y3-y_[0])/(y_[1]-y_[0]);
			x_[4]=x_[0]+(x_[2]-x_[0])*(y3-y_[0])/(y_[2]-y_[0]);
			if(x1<x2)
			{
				X1=x_[3]+camera->center_x;
				X2=x_[4]+camera->center_x;
				d=depth[0];
				X=(double)x_[3]-x1;
				d+=a*X;
				if(X1<0)
				{
					d-=a*(double)X1;
					X1=0;
				}
				if(X2>camera->fbw)
				{
					X2=camera->fbw;
				}
				if(X1<X2)
				{
					offset=y1*camera->fbw+X1;
					r3d_render_line(camera->zbuf+offset,camera->fb+offset,color,d,a,X2-X1);
				}
			}
			else
			{
				X2=x_[3]+camera->center_x;
				X1=x_[4]+camera->center_x;
				d=depth[0];
				X=(double)x_[4]-x1;
				d+=a*X;
				if(X1<0)
				{
					d-=a*(double)X1;
					X1=0;
				}
				if(X2>camera->fbw)
				{
					X2=camera->fbw;
				}
				if(X1<X2)
				{
					offset=y1*camera->fbw+X1;
					r3d_render_line(camera->zbuf+offset,camera->fb+offset,color,d,a,X2-X1);
				}
			}
			x1-=d1;
			x2-=d2;
			depth[0]-=c;
			depth[1]-=b;
			++y1;
			--y3;
		}
	}
	y1=camera->center_y-y_[1];
	y2=camera->center_y-y_[2];
	y3=y_[1];
	Y=(double)y_[1]-y[1];
	x1=x[1];
	x2=t[1];
	depth[0]=z[1];
	depth[1]=t[0];
	if(y2>ry2)
	{
		y2=ry2;
	}
	if(y1<y2)
	{
		c=(z[2]-z[1])/(y[2]-y[1]);
		d1=(x[2]-x[1])/(y[2]-y[1]);
		d2=(x[2]-x[0])/(y[2]-y[0]);
		x1+=d1*Y;
		x2+=d2*Y;
		depth[0]+=c*Y;
		depth[1]+=b*Y;
		if(y1<ry1)
		{
			x1+=d1*(double)(y1-ry1);
			x2+=d2*(double)(y1-ry1);
			depth[0]+=c*(double)(y1-ry1);
			depth[1]+=b*(double)(y1-ry1);
			y3+=y1-ry1;
			y1=ry1;
		}
		while(y1<y2)
		{
			x_[3]=x_[1]+(x_[2]-x_[1])*(y3-y_[1])/(y_[2]-y_[1]);
			x_[4]=x_[0]+(x_[2]-x_[0])*(y3-y_[0])/(y_[2]-y_[0]);
			if(x1<x2)
			{
				X1=x_[3]+camera->center_x;
				X2=x_[4]+camera->center_x;
				d=depth[0];
				X=(double)x_[3]-x1;
				d+=a*X;
				if(X1<0)
				{
					d-=a*(double)X1;
					X1=0;
				}
				if(X2>camera->fbw)
				{
					X2=camera->fbw;
				}
				if(X1<X2)
				{
					offset=y1*camera->fbw+X1;
					r3d_render_line(camera->zbuf+offset,camera->fb+offset,color,d,a,X2-X1);
				}
			}
			else
			{
				X2=x_[3]+camera->center_x;
				X1=x_[4]+camera->center_x;
				d=depth[0];
				X=(double)x_[4]-x1;
				d+=a*X;
				if(X1<0)
				{
					d-=a*(double)X1;
					X1=0;
				}
				if(X2>camera->fbw)
				{
					X2=camera->fbw;
				}
				if(X1<X2)
				{
					offset=y1*camera->fbw+X1;
					r3d_render_line(camera->zbuf+offset,camera->fb+offset,color,d,a,X2-X1);
				}
			}
			x1-=d1;
			x2-=d2;
			depth[0]-=c;
			depth[1]-=b;
			++y1;
			--y3;
		}
	}
}
void r3d_paint_triangle2(struct r3d_camera *camera,struct vector *p1,struct vector *p2,struct vector *p3,unsigned int color,int thread,int num_threads)
{
	struct vector P1,P2,P3,P4,P5;
	coord_transform(camera->transform,p1,&P1);
	coord_transform(camera->transform,p2,&P2);
	coord_transform(camera->transform,p3,&P3);
	if(P1.z<1.0&&P2.z<1.0&&P3.z<1.0)
	{
		return;
	}
	if(P1.z<1.0)
	{
		if(P2.z<1.0)
		{
			__p_intersection(&P3,&P1,&P4);
			__p_intersection(&P3,&P2,&P5);
			_p_triangle(camera,&P3,&P4,&P5,color,thread,num_threads);
		}
		else if(P3.z<1.0)
		{
			__p_intersection(&P2,&P1,&P4);
			__p_intersection(&P2,&P3,&P5);
			_p_triangle(camera,&P2,&P4,&P5,color,thread,num_threads);
		}
		else
		{
			__p_intersection(&P2,&P1,&P4);
			__p_intersection(&P3,&P1,&P5);
			_p_triangle(camera,&P2,&P3,&P5,color,thread,num_threads);
			_p_triangle(camera,&P5,&P4,&P2,color,thread,num_threads);
		}
	}
	else if(P2.z<1.0)
	{
		if(P3.z<1.0)
		{
			__p_intersection(&P1,&P2,&P4);
			__p_intersection(&P1,&P3,&P5);
			_p_triangle(camera,&P1,&P4,&P5,color,thread,num_threads);
		}
		else
		{
			__p_intersection(&P1,&P2,&P4);
			__p_intersection(&P3,&P2,&P5);
			_p_triangle(camera,&P1,&P3,&P5,color,thread,num_threads);
			_p_triangle(camera,&P5,&P4,&P1,color,thread,num_threads);
		}
	}
	else if(P3.z<1.0)
	{
		__p_intersection(&P1,&P3,&P4);
		__p_intersection(&P2,&P3,&P5);
		_p_triangle(camera,&P1,&P2,&P5,color,thread,num_threads);
		_p_triangle(camera,&P5,&P4,&P1,color,thread,num_threads);
	}
	else
	{
		_p_triangle(camera,&P1,&P2,&P3,color,thread,num_threads);
	}
}
#define r3d_paint_triangle(camera,p1,p2,p3,color) r3d_paint_triangle2(camera,p1,p2,p3,color,0,1)

#undef IS_ZERO