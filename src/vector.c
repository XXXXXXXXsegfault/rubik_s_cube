#include "float.c"
struct vector
{
	double x;
	double y;
	double z;
};
void __vector_add(struct vector *a,struct vector *b,struct vector *c);
asm "@__vector_add"
asm "mov 8(%rsp),%rax"
asm "mov 16(%rsp),%rcx"
asm "mov 24(%rsp),%rdx"
asm "movups (%rax),%xmm0"
asm "movups (%rcx),%xmm1"
asm "addpd %xmm1,%xmm0"
asm "movups %xmm0,(%rdx)"
asm "movsd 16(%rax),%xmm0"
asm "addsd 16(%rcx),%xmm0"
asm "movsd %xmm0,16(%rdx)"
asm "ret"
void __vector_sub(struct vector *a,struct vector *b,struct vector *c);
asm "@__vector_sub"
asm "mov 8(%rsp),%rax"
asm "mov 16(%rsp),%rcx"
asm "mov 24(%rsp),%rdx"
asm "movups (%rax),%xmm0"
asm "movups (%rcx),%xmm1"
asm "subpd %xmm1,%xmm0"
asm "movups %xmm0,(%rdx)"
asm "movsd 16(%rax),%xmm0"
asm "subsd 16(%rcx),%xmm0"
asm "movsd %xmm0,16(%rdx)"
asm "ret"
struct vector *vector_add(struct vector *a,struct vector *b,struct vector *c)
{
	__vector_add(a,b,c);
	return c;
}
struct vector *vector_sub(struct vector *a,struct vector *b,struct vector *c)
{
	__vector_sub(a,b,c);
	return c;
}
struct vector *vector_mul(struct vector *a,double b,struct vector *c)
{
	c->x=a->x*b;
	c->y=a->y*b;
	c->z=a->z*b;
	return c;
}
double vector_iprod(struct vector *a,struct vector *b)
{
	return a->x*b->x+a->y*b->y+a->z*b->z;
}
struct vector *vector_cprod(struct vector *a,struct vector *b,struct vector *c)
{
	struct vector d;
	d.x=a->y*b->z-a->z*b->y;
	d.y=a->z*b->x-a->x*b->z;
	d.z=a->x*b->y-a->y*b->x;
	c->x=d.x;
	c->y=d.y;
	c->z=d.z;
	return c;
}
double vector_len(struct vector *a)
{
	return sqrt(a->x*a->x+a->y*a->y+a->z*a->z);
}
struct vector *vinit(struct vector *v,double x,double y,double z)
{
	v->x=x;
	v->y=y;
	v->z=z;
	return v;
}
double __solve3(double a1,double a2,double a3,double a4,double a5,double a6,double a7,double a8,double a9,double a10,double a11,double a12)
{
	double c;
	c=a1*a6*a11+a2*a7*a9+a3*a5*a10-a3*a6*a9-a2*a5*a11-a1*a7*a10;
	if(c<0.0000000001&&c>-0.0000000001)
	{
		return 0.0;
	}
	return (a2*a7*a12+a3*a8*a10+a4*a6*a11-a4*a7*a10-a2*a8*a11-a3*a6*a12)/c;
}
void calculate_transform_matrix(struct vector *o,struct vector *x,struct vector *y,struct vector *z,double *transform_matrix)
{
	transform_matrix[0]=__solve3(
x->x,y->x,z->x,1.0,
x->y,y->y,z->y,0.0,
x->z,y->z,z->z,0.0);
	transform_matrix[1]=__solve3(
x->x,y->x,z->x,0.0,
x->y,y->y,z->y,1.0,
x->z,y->z,z->z,0.0);
	transform_matrix[2]=__solve3(
x->x,y->x,z->x,0.0,
x->y,y->y,z->y,0.0,
x->z,y->z,z->z,1.0);
	transform_matrix[3]=__solve3(
x->x,y->x,z->x,-o->x,
x->y,y->y,z->y,-o->y,
x->z,y->z,z->z,-o->z);
	transform_matrix[4]=__solve3(
y->x,x->x,z->x,1.0,
y->y,x->y,z->y,0.0,
y->z,x->z,z->z,0.0);
	transform_matrix[5]=__solve3(
y->x,x->x,z->x,0.0,
y->y,x->y,z->y,1.0,
y->z,x->z,z->z,0.0);
	transform_matrix[6]=__solve3(
y->x,x->x,z->x,0.0,
y->y,x->y,z->y,0.0,
y->z,x->z,z->z,1.0);
	transform_matrix[7]=__solve3(
y->x,x->x,z->x,-o->x,
y->y,x->y,z->y,-o->y,
y->z,x->z,z->z,-o->z);
	transform_matrix[8]=__solve3(
z->x,y->x,x->x,1.0,
z->y,y->y,x->y,0.0,
z->z,y->z,x->z,0.0);
	transform_matrix[9]=__solve3(
z->x,y->x,x->x,0.0,
z->y,y->y,x->y,1.0,
z->z,y->z,x->z,0.0);
	transform_matrix[10]=__solve3(
z->x,y->x,x->x,0.0,
z->y,y->y,x->y,0.0,
z->z,y->z,x->z,1.0);
	transform_matrix[11]=__solve3(
z->x,y->x,x->x,-o->x,
z->y,y->y,x->y,-o->y,
z->z,y->z,x->z,-o->z);
}
struct vector *coord_transform(double *matrix,struct vector *pin,struct vector *pout)
{
	struct vector p;
	p.x=pin->x*matrix[0]+pin->y*matrix[1]+pin->z*matrix[2]+matrix[3];
	p.y=pin->x*matrix[4]+pin->y*matrix[5]+pin->z*matrix[6]+matrix[7];
	p.z=pin->x*matrix[8]+pin->y*matrix[9]+pin->z*matrix[10]+matrix[11];
	pout->x=p.x;
	pout->y=p.y;
	pout->z=p.z;
	return pout;
}
void vector_intersection(struct vector *lp,struct vector *ld,struct vector *pp,struct vector *pd1,struct vector *pd2,double *l,double *p1,double *p2)
{
	*l=__solve3(
-ld->x,pd1->x,pd2->x,lp->x-pp->x,
-ld->y,pd1->y,pd2->y,lp->y-pp->y,
-ld->z,pd1->z,pd2->z,lp->z-pp->z);
	*p1=__solve3(
pd1->x,-ld->x,pd2->x,lp->x-pp->x,
pd1->y,-ld->y,pd2->y,lp->y-pp->y,
pd1->z,-ld->z,pd2->z,lp->z-pp->z);
	*p2=__solve3(
pd2->x,-ld->x,pd1->x,lp->x-pp->x,
pd2->y,-ld->y,pd1->y,lp->y-pp->y,
pd2->z,-ld->z,pd1->z,lp->z-pp->z);
}
