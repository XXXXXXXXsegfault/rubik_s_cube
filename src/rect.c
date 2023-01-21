
void w4(unsigned int *buf,unsigned int val,unsigned long n)
{
	unsigned long xval,n1;
	xval=val;
	xval=xval<<32|val;
	while(n>=8)
	{
		((unsigned long *)buf)[0]=xval;
		((unsigned long *)buf)[1]=xval;
		((unsigned long *)buf)[2]=xval;
		((unsigned long *)buf)[3]=xval;
		buf+=8;
		n-=8;
	}
	if(n&4)
	{
		buf[0]=val;
		buf[1]=val;
		buf[2]=val;
		buf[3]=val;
		buf+=4;
	}
	if(n&2)
	{
		buf[0]=val;
		buf[1]=val;
		buf+=2;
	}
	if(n&1)
	{
		buf[0]=val;
	}
}
void rect(unsigned int *dst,int dstw,int dsth,int rect_x,int rect_y,int rect_w,int rect_h,unsigned int color)
{
	int off;
	if(rect_x<0)
	{
		rect_w+=rect_x;
		rect_x=0;
	}
	if(rect_y<0)
	{
		rect_h+=rect_y;
		rect_y=0;
	}
	if(rect_x+rect_w>dstw)
	{
		rect_w=dstw-rect_x;
	}
	if(rect_y+rect_h>dsth)
	{
		rect_h=dsth-rect_y;
	}
	if(rect_w<=0||rect_h<=0)
	{
		return;
	}
	off=rect_y*dstw+rect_x;
	do
	{
		w4(dst+off,color,rect_w);
		off+=dstw;
		--rect_h;
	}
	while(rect_h);
}
