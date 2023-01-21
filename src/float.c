#define _ERROR 0.00000000000001
float sqrt(float a)
{
	float ret,b;
	ret=1.0;
	do
	{
		b=ret;
		ret=b*0.5+a/(2.0*b);
	}
	while(b/ret>1.0+_ERROR||b/ret<1.0-_ERROR||b-ret<-_ERROR||b-ret>_ERROR);
	return ret;
}
float _sin(float a)
{
	float n,ret,n1,b;
	ret=0.0;
	n=1.0;
	n1=-3.0;
	b=a;
	a=a*a;
	while(1.0/n>_ERROR||1.0/n<-_ERROR)
	{
		ret+=b/n;
		b*=a;
		n*=-n1*(n1+1.0);
		n1-=2.0;
	}
	return ret;
}
float cos(float a)
{
	if(a<1.0&&a>-1.0)
	{
		a=_sin(a);
		return sqrt(1.0-a*a);
	}
	a=cos(a*0.5);
	a=2.0*a*a-1.0;
	return a;
}
float sin(float a)
{
	if(a<1.0&&a>-1.0)
	{
		return _sin(a);
	}
	return cos(a-3.1415926535897932*0.5);
}
#undef _ERROR
