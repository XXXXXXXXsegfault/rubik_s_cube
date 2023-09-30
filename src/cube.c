unsigned char colors[54];
int rotating;
double rotate_angle;
unsigned long rotate_time;
void cube_init(void)
{
	memset(colors+0,1,9);
	memset(colors+9,2,9);
	memset(colors+18,3,9);
	memset(colors+27,4,9);
	memset(colors+36,5,9);
	memset(colors+45,6,9);
}
void rotate_x(double sin_a,double cos_a,struct vector *v)
{
	double x,y;
	x=v->y*cos_a+v->z*sin_a;
	y=-v->y*sin_a+v->z*cos_a;
	v->y=x;
	v->z=y;
}
void rotate_y(double sin_a,double cos_a,struct vector *v)
{
	double x,y;
	x=v->z*cos_a+v->x*sin_a;
	y=-v->z*sin_a+v->x*cos_a;
	v->z=x;
	v->x=y;
}
void rotate_z(double sin_a,double cos_a,struct vector *v)
{
	double x,y;
	x=v->x*cos_a+v->y*sin_a;
	y=-v->x*sin_a+v->y*cos_a;
	v->x=x;
	v->y=y;
}
void axis_rotate(struct vector *vx,struct vector *vy,double angle)
{
	struct vector x,y,t[2];
	double s,c;
	x.x=vx->x;
	x.y=vx->y;
	x.z=vx->z;
	y.x=vy->x;
	y.y=vy->y;
	y.z=vy->z;
	s=sin(angle);
	c=cos(angle);
	vector_add(vector_mul(&x,c,t+0),vector_mul(&y,s,t+1),vx);
	vector_add(vector_mul(&x,-s,t+0),vector_mul(&y,c,t+1),vy);
}
int needs_rotate(int i,int x)
{
	if(rotating==0)
	{
		return 0;
	}
	if(i==rotating)
	{
		return 1;
	}
	if(i-1>>1==rotating-1>>1)
	{
		return 0;
	}
	if(x==4)
	{
		return 0;
	}
	if(rotating==1)
	{
		if(x==0||x==1||x==2)
		{
			return 1;
		}
	}
	else if(rotating==2)
	{
		if(x==6||x==7||x==8)
		{
			return 1;
		}
	}
	else if(rotating==3)
	{
		if(i==1&&(x==6||x==7||x==8))
		{
			return 1;
		}
		if(i==2&&(x==0||x==1||x==2))
		{
			return 1;
		}
		if(i==5&&(x==0||x==3||x==6))
		{
			return 1;
		}
		if(i==6&&(x==2||x==5||x==8))
		{
			return 1;
		}
	}
	else if(rotating==4)
	{
		if(i==1&&(x==0||x==1||x==2))
		{
			return 1;
		}
		if(i==2&&(x==6||x==7||x==8))
		{
			return 1;
		}
		if(i==5&&(x==2||x==5||x==8))
		{
			return 1;
		}
		if(i==6&&(x==0||x==3||x==6))
		{
			return 1;
		}
	}
	else if(rotating==5)
	{
		if(i==4)
		{
			if(x==0||x==3||x==6)
			{
				return 1;
			}
		}
		else
		{
			if(x==2||x==5||x==8)
			{
				return 1;
			}
		}
	}
	else if(rotating==6)
	{
		if(i==4)
		{
			if(x==2||x==5||x==8)
			{
				return 1;
			}
		}
		else
		{
			if(x==0||x==3||x==6)
			{
				return 1;
			}
		}
	}
	return 0;
}
void cube_index(int i,int x,int *ix,int *iy)
{
	int y;
	y=x/3;
	x%=3;
	if(i==1)
	{
		*ix=y-1;
		*iy=x-1;
	}
	else if(i==2)
	{
		*ix=1-y;
		*iy=1-x;
	}
	else if(i==3)
	{
		*ix=x-1;
		*iy=1-y;
	}
	else if(i==4)
	{
		*ix=1-x;
		*iy=y-1;
	}
	else if(i==5)
	{
		*ix=1-y;
		*iy=1-x;
	}
	else if(i==6)
	{
		*ix=y-1;
		*iy=x-1;
	}
}

void cube_rotate(int x,int y)
{
	double l,s,c,l1;
	if(x==0&&y==0)
	{
		return;
	}
	mutex_lock(&lock);
	l=sqrt((double)(x*x+y*y));
	s=(double)x/l;
	c=(double)-y/l;
	rotate_x(-c,s,&X);
	rotate_x(-c,s,&Y);
	rotate_x(-c,s,&Z);
	l=sin(l*PI/180.0);
	l1=sqrt(1.0-l*l);
	rotate_z(-l,l1,&X);
	rotate_z(-l,l1,&Y);
	rotate_z(-l,l1,&Z);
	rotate_x(c,s,&X);
	rotate_x(c,s,&Y);
	rotate_x(c,s,&Z);

	mutex_unlock(&lock);
}
void p_square(int side,double x,double y,double z,double len,double r,double g,double b,int rotate)
{
	struct vector vx,vy,vz,vnx,vny,va;
	struct vector P1,P2,P3,P4;
	struct vector x1,y1,z1;
	double angle;
	unsigned int color;
	x1.x=X.x;
	x1.y=X.y;
	x1.z=X.z;
	y1.x=Y.x;
	y1.y=Y.y;
	y1.z=Y.z;
	z1.x=Z.x;
	z1.y=Z.y;
	z1.z=Z.z;
	if(rotate)
	{
		if(rotating==1)
		{
			axis_rotate(&x1,&y1,rotate_angle);
		}
		else if(rotating==2)
		{
			axis_rotate(&x1,&y1,-rotate_angle);
		}
		else if(rotating==3)
		{
			axis_rotate(&y1,&z1,rotate_angle);
		}
		else if(rotating==4)
		{
			axis_rotate(&y1,&z1,-rotate_angle);
		}
		else if(rotating==5)
		{
			axis_rotate(&z1,&x1,rotate_angle);
		}
		else if(rotating==6)
		{
			axis_rotate(&z1,&x1,-rotate_angle);
		}
	}
	if(side==1)
	{
		vector_mul(&x1,1.0,&vx);
		vector_mul(&y1,1.0,&vy);
		vector_mul(&z1,1.0,&vz);
	}
	else if(side==2)
	{
		vector_mul(&x1,1.0,&vx);
		vector_mul(&y1,-1.0,&vy);
		vector_mul(&z1,-1.0,&vz);
	}
	else if(side==3)
	{
		vector_mul(&y1,1.0,&vx);
		vector_mul(&z1,1.0,&vy);
		vector_mul(&x1,1.0,&vz);
	}
	else if(side==4)
	{
		vector_mul(&y1,1.0,&vx);
		vector_mul(&z1,-1.0,&vy);
		vector_mul(&x1,-1.0,&vz);
	}
	else if(side==5)
	{
		vector_mul(&z1,1.0,&vx);
		vector_mul(&x1,1.0,&vy);
		vector_mul(&y1,1.0,&vz);
	}
	else if(side==6)
	{
		vector_mul(&z1,-1.0,&vx);
		vector_mul(&x1,1.0,&vy);
		vector_mul(&y1,-1.0,&vz);
	}
	vector_mul(&vz,z,&va);
	vector_sub(&va,&camera.pos,&va);
	angle=vector_iprod(&va,&vz)/(vector_len(&va)*vector_len(&vz));
	if(angle>-0.05)
	{
		return;
	}
	vector_mul(&vx,-1.0,&vnx);
	vector_mul(&vy,-1.0,&vny);
	vector_add(&vx,&vy,&P1);
	vector_add(&vx,&vny,&P2);
	vector_add(&vnx,&vy,&P3);
	vector_add(&vnx,&vny,&P4);
	vector_mul(&P1,len*0.5,&P1);
	vector_mul(&P2,len*0.5,&P2);
	vector_mul(&P3,len*0.5,&P3);
	vector_mul(&P4,len*0.5,&P4);
	vector_mul(&vx,x,&vx);
	vector_mul(&vy,y,&vy);
	vector_mul(&vz,z,&vz);
	vector_add(vector_add(vector_add(&P1,&vx,&P1),&vy,&P1),&vz,&P1);
	vector_add(vector_add(vector_add(&P2,&vx,&P2),&vy,&P2),&vz,&P2);
	vector_add(vector_add(vector_add(&P3,&vx,&P3),&vy,&P3),&vz,&P3);
	vector_add(vector_add(vector_add(&P4,&vx,&P4),&vy,&P4),&vz,&P4);
	r*=-angle;
	g*=-angle;
	b*=-angle;
	color=(int)(250.0*r);
	color=(color<<8)|(int)(250.0*g);
	color=(color<<8)|(int)(250.0*b);
	r3d_paint_triangle(&camera,&P1,&P2,&P4,color);
	r3d_paint_triangle(&camera,&P1,&P3,&P4,color);
}
void p_cube(void)
{
	int i,x,j;
	int ix,iy;
	double r,g,b;
	struct vector vx,vy,vz;
	r3d_zbuf_clean(&camera);
	i=1;
	j=0;
	while(i<=6)
	{
		x=0;
		while(x<9)
		{
			cube_index(i,x,&ix,&iy);
			if(colors[j]==1)
			{
				r=0.8;
				g=0.8;
				b=0.0;
			}
			else if(colors[j]==2)
			{
				r=0.8;
				g=0.8;
				b=0.8;
			}
			else if(colors[j]==3)
			{
				r=0.0;
				g=0.0;
				b=0.8;
			}
			else if(colors[j]==4)
			{
				r=0.0;
				g=0.8;
				b=0.0;
			}
			else if(colors[j]==5)
			{
				r=0.8;
				g=0.0;
				b=0.0;
			}
			else if(colors[j]==6)
			{
				r=0.8;
				g=0.4;
				b=0.0;
			}
			p_square(i,(double)ix,(double)iy,1.52,0.8,r,g,b,needs_rotate(i,x));
			p_square(i,(double)ix,(double)iy,1.5,1.0,0.5,0.5,0.5,needs_rotate(i,x));
			++j;
			++x;
		}
		++i;
	}
	if(rotating)
	{
		i=(rotating-1^1)+1;
		p_square(rotating,0.0,0.0,0.5,3.0,0.5,0.5,0.5,0);
		p_square(i,0.0,0.0,-0.5,3.0,0.5,0.5,0.5,1);
	}
}
int check_if_clicked(struct vector *vx,struct vector *vy,struct vector *vz,struct vector *vc)
{
	struct vector va,vc1;
	double transform_matrix[12];
	vector_sub(vz,&camera.pos,&va);
	if(vector_iprod(vc,vz)>0)
	{
		return 0;
	}
	calculate_transform_matrix(&camera.pos,vx,vy,&va,transform_matrix);
	vector_add(&camera.pos,vc,&vc1);
	coord_transform(transform_matrix,&vc1,&vc1);
	vector_mul(&vc1,1.0/vc1.z,&vc1);
	if(vc1.x<1.5&&vc1.x>-1.5&&vc1.y<1.5&&vc1.y>-1.5)
	{
		return 1;
	}
	return 0;
}
int clicked_side(struct vector *vc)
{
	struct vector vx,vy,vz;
	vector_mul(&X,1.0,&vx);
	vector_mul(&Y,1.0,&vy);
	vector_mul(&Z,1.5,&vz);
	if(check_if_clicked(&vx,&vy,&vz,vc))
	{
		return 1;
	}
	vector_mul(&X,-1.0,&vx);
	vector_mul(&Y,-1.0,&vy);
	vector_mul(&Z,-1.5,&vz);
	if(check_if_clicked(&vx,&vy,&vz,vc))
	{
		return 2;
	}
	vector_mul(&Y,1.0,&vx);
	vector_mul(&Z,1.0,&vy);
	vector_mul(&X,1.5,&vz);
	if(check_if_clicked(&vx,&vy,&vz,vc))
	{
		return 3;
	}
	vector_mul(&Y,-1.0,&vx);
	vector_mul(&Z,-1.0,&vy);
	vector_mul(&X,-1.5,&vz);
	if(check_if_clicked(&vx,&vy,&vz,vc))
	{
		return 4;
	}
	vector_mul(&Z,1.0,&vx);
	vector_mul(&X,1.0,&vy);
	vector_mul(&Y,1.5,&vz);
	if(check_if_clicked(&vx,&vy,&vz,vc))
	{
		return 5;
	}
	vector_mul(&Z,-1.0,&vx);
	vector_mul(&X,-1.0,&vy);
	vector_mul(&Y,-1.5,&vz);
	if(check_if_clicked(&vx,&vy,&vz,vc))
	{
		return 6;
	}
	return 0;
}
void side_shift(char *indexes)
{
	unsigned char tmp;
	int i,j1,j2;
	i=0;
	j1=indexes[0]-'0';
	j1=j1*9+indexes[1]-'0';
	tmp=colors[j1];
	while(i<9)
	{
		j1=indexes[i*2]-'0';
		j1=j1*9+indexes[i*2+1]-'0';
		j2=indexes[i*2+6]-'0';
		j2=j2*9+indexes[i*2+7]-'0';
		colors[j1]=colors[j2];
		i+=3;
	}
	colors[j2]=tmp;
	j1=indexes[2]-'0';
	j1=j1*9+indexes[3]-'0';
	tmp=colors[j1];
	i=1;
	while(i<10)
	{
		j1=indexes[i*2]-'0';
		j1=j1*9+indexes[i*2+1]-'0';
		j2=indexes[i*2+6]-'0';
		j2=j2*9+indexes[i*2+7]-'0';
		colors[j1]=colors[j2];
		i+=3;
	}
	colors[j2]=tmp;
	j1=indexes[4]-'0';
	j1=j1*9+indexes[5]-'0';
	tmp=colors[j1];
	i=2;
	while(i<11)
	{
		j1=indexes[i*2]-'0';
		j1=j1*9+indexes[i*2+1]-'0';
		j2=indexes[i*2+6]-'0';
		j2=j2*9+indexes[i*2+7]-'0';
		colors[j1]=colors[j2];
		i+=3;
	}
	colors[j2]=tmp;
}
void side_rotate(int side,int dir)
{
	unsigned char *p,tmp;
	p=colors+(side-1)*9;
	if(dir)
	{
		tmp=p[0];
		p[0]=p[6];
		p[6]=p[8];
		p[8]=p[2];
		p[2]=tmp;
		tmp=p[1];
		p[1]=p[3];
		p[3]=p[7];
		p[7]=p[5];
		p[5]=tmp;
	}
	else
	{
		tmp=p[0];
		p[0]=p[2];
		p[2]=p[8];
		p[8]=p[6];
		p[6]=tmp;
		tmp=p[1];
		p[1]=p[5];
		p[5]=p[7];
		p[7]=p[3];
		p[3]=tmp;
	}
	if(side==1)
	{
		if(dir)
		{
			side_shift("202122404142303132505152202122");
		}
		else
		{
			side_shift("202122505152303132404142202122");
		}
	}
	else if(side==2)
	{
		if(dir)
		{
			side_shift("262728565758363738464748262728");
		}
		else
		{
			side_shift("262728464748363738565758262728");
		}
	}
	else if(side==3)
	{
		if(dir)
		{
			side_shift("060708585552121110404346060708");
		}
		else
		{
			side_shift("060708404346121110585552060708");
		}
	}
	else if(side==4)
	{
		if(dir)
		{
			side_shift("020100484542161718505356020100");
		}
		else
		{
			side_shift("020100505356161718484542020100");
		}
	}
	else if(side==5)
	{
		if(dir)
		{
			side_shift("080502282522181512303336080502");
		}
		else
		{
			side_shift("080502303336181512282522080502");
		}
	}
	else if(side==6)
	{
		if(dir)
		{
			side_shift("000306383532101316202326000306");
		}
		else
		{
			side_shift("000306202326101316383532000306");
		}
	}
}
void click_cube(int x,int y,int rclick)
{
	struct vector vx,vy,vc;
	int side;
	mutex_lock(&lock);
	if(rotating)
	{
		mutex_unlock(&lock);
		return;
	}
	vector_mul(&camera.dirx,(double)x,&vx);
	vector_mul(&camera.diry,-(double)y,&vy);
	vector_add(&camera.dirz,&vx,&vc);
	vector_add(&vc,&vy,&vc);
	side=clicked_side(&vc);
	if(side==0)
	{
		mutex_unlock(&lock);
		return;
	}
	side_rotate(side,rclick);
	if(rclick)
	{
		rotate_angle=PI*0.5;
	}
	else
	{
		rotate_angle=-PI*0.5;
	}
	rotating=side;
	rotate_time=clock();
	paint=1;
	mutex_unlock(&lock);
}
