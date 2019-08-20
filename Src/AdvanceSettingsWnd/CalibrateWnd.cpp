#include "CalibrateWnd.h"
#include "BaseApp.h"

#include <unistd.h>
#include <stdio.h>

static char PromptMsg[64] = "按任意键退出";

static int sort_by_x(const void* a, const void *b)
{
	return (((struct ts_sample *)a)->x - ((struct ts_sample *)b)->x);
}

static int sort_by_y(const void* a, const void *b)
{
	return (((struct ts_sample *)a)->y - ((struct ts_sample *)b)->y);
}

CCalibrateWnd::CCalibrateWnd()
{
	ts = NULL;
	memset((char *)(&cal),0,sizeof(cal));
}

CCalibrateWnd::~CCalibrateWnd()
{
	DbgOutput(DBG_VERB, "function %s quit\n", __FUNCTION__);
//	DeInitCalibration();
}

void CCalibrateWnd::Create(
		CBaseWnd *pParent,
		RECT rc)
{
	CParentClass::Create("CalibrateWnd", pParent, rc);
	SetZorderNumber(WINDOWS_ZORDER_WAITINGWNDPOPUP);

	RECT rcControl;
	SetRectXY(&rcControl, 40, 40, 35, 35);
	mDotWnd.CreateStatic(this, rcControl);

	SetRectXY(&rcControl, 500, 360, 280, 40);
	mPromptWnd.CreateStatic(this, rcControl);
	mPromptWnd.SetWindowVisible(FALSE);
	mPromptWnd.SetFontSize(40);

	LoadResource();

	DbgOutput(DBG_INFO, "CCalibrateWnd Create.\n");
}

void CCalibrateWnd::LoadResource()
{
	/*init position by xml*/
	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "CalibrateWnd.xml");
	if(false == parser.Load(cFileName))
	{
		printf("failed to load xml file!!\n");
	}

	// read from xml
	CTexture bkTexture;
	char imgPath[64] = {0};

	strcpy(imgPath, parser.GetStrValue("CalibrateBg", "path", "AdvanceSettings/TouchScreenCal/CalibrateBG"));
	//strcpy(imgPath,"AdvanceSettings/TouchScreenCal/CalibrateBG");
	CreateImgTexture(imgPath, &bkTexture);
	SetBackgroundTexture(&bkTexture);

	strcpy(imgPath, parser.GetStrValue("CalibrateDotBg", "path", "AdvanceSettings/TouchScreenCal/CalibrateDot"));
	//strcpy(imgPath,"AdvanceSettings/TouchScreenCal/CalibrateDot");
	CreateImgTexture(imgPath, &bkTexture);
	mDotWnd.SetBackgroundTexture(&bkTexture);

	strcpy(PromptMsg, parser.GetStrValue("Prompt", "msg", "按任意键退出"));
	mPromptWnd.SetWindowTextA(PromptMsg);
}

void CCalibrateWnd::InitCalibration()
{
	char *tsdevice = NULL;

	DbgOutput(DBG_ERR, "InitCalibration\n");
	if( (tsdevice = getenv("TSLIB_TSDEVICE")) != NULL )
	{
		ts = ts_open(tsdevice,0);
	}
	else
	{
		if (!(ts = ts_open("/dev/input/event0", 0)))
			ts = ts_open("/dev/touchscreen/ucb1x00", 0);
	}

	DbgOutput(DBG_ERR, "ts=%x!\n", ts);
	if (!ts)
	{
		perror("ts_open");
		return ;
	}

	DbgOutput(DBG_ERR, "ts_config!\n");
	if (ts_config(ts))
	{
		perror("ts_config");
		return ;
	}
}

void CCalibrateWnd::DeInitCalibration()
{
	if(ts != NULL)
	{
		ts_close(ts);
		ts = NULL;
	}
}

void CCalibrateWnd::PromptVisible(BOOL visible)
{
	mPromptWnd.SetWindowVisible(visible);
}

void CCalibrateWnd::Getxy(struct tsdev *ts,
		int *x,int *y)
{
#define MAX_SAMPLES 128
	struct ts_sample samp[MAX_SAMPLES];
	int index, middle;

	do {
		if (ts_read_raw(ts, &samp[0], 1) < 0) {
			perror("ts_read");
			return ;
		}

	} while (samp[0].pressure == 0);

	/* Now collect up to MAX_SAMPLES touches into the samp array. */
	index = 0;
	do {
		if (index < MAX_SAMPLES-1)
			index++;
		if (ts_read_raw(ts, &samp[index], 1) < 0) {
			perror("ts_read");
			return ;
		}
	} while (samp[index].pressure > 0);
	printf("Took %d samples...\n",index);

	/*
	 * At this point, we have samples in indices zero to (index-1)
	 * which means that we have (index) number of samples.  We want
	 * to calculate the median of the samples so that wild outliers
	 * don't skew the result.  First off, let's assume that arrays
	 * are one-based instead of zero-based.  If this were the case
	 * and index was odd, we would need sample number ((index+1)/2)
	 * of a sorted array; if index was even, we would need the
	 * average of sample number (index/2) and sample number
	 * ((index/2)+1).  To turn this into something useful for the
	 * real world, we just need to subtract one off of the sample
	 * numbers.  So for when index is odd, we need sample number
	 * (((index+1)/2)-1).  Due to integer division truncation, we
	 * can simplify this to just (index/2).  When index is even, we
	 * need the average of sample number ((index/2)-1) and sample
	 * number (index/2).  Calculate (index/2) now and we'll handle
	 * the even odd stuff after we sort.
	 */
	middle = index/2;
	if (x) {
		qsort(samp, index, sizeof(struct ts_sample), sort_by_x);
		if (index & 1)
			*x = samp[middle].x;
		else
			*x = (samp[middle-1].x + samp[middle].x) / 2;
	}	mPromptWnd.SetWindowVisible(FALSE);
	if (y) {
		qsort(samp, index, sizeof(struct ts_sample), sort_by_y);
		if (index & 1)
			*y = samp[middle].y;
		else
			*y = (samp[middle-1].y + samp[middle].y) / 2;
	}
}

void CCalibrateWnd::GetSample(int index,
		int x,int y,const char *name)
{
	static int last_x = -1, last_y;

	if (last_x != -1) {
#define NR_STEPS 10
		int dx = ((x - last_x) << 16) / NR_STEPS;
		int dy = ((y - last_y) << 16) / NR_STEPS;
		int i;
		last_x <<= 16;
		last_y <<= 16;
		for (i = 0; i < NR_STEPS; i++) {
		//	put_cross (last_x >> 16, last_y >> 16, 2 | XORMODE);
			usleep (1000);
		//	put_cross (last_x >> 16, last_y >> 16, 2 | XORMODE);
			last_x += dx;
			last_y += dy;
		}
	}

	RECT rcControl;
	rcControl.left = x -20;
	rcControl.top = y - 20;
	rcControl.right = x + 20;
	rcControl.bottom = y + 20;

	mDotWnd.MoveWindow(&rcControl);
	Getxy (ts, &cal.x [index], &cal.y [index]);
	mDotWnd.MoveWindow(&rcControl);

	cal.xfb [index] = x;
	cal.yfb [index] = y;

	printf("%s : X = %4d Y = %4d Xfb=%4d Yfd=%4d\n", name, cal.x [index], cal.y [index],cal.xfb[index],cal.yfb[index]);
}

int CCalibrateWnd::PerformCalibration()
{
	int j;
	float n, x, y, x2, y2, xy, z, zx, zy;
	float det, a, b, c, e, f, i;
	float scaling = 65536.0;

// Get sums for matrix
	n = x = y = x2 = y2 = xy = 0;
	for(j=0;j<5;j++) {
		n += 1.0;
		x += (float)cal.x[j];
		y += (float)cal.y[j];
		x2 += (float)(cal.x[j]*cal.x[j]);
		y2 += (float)(cal.y[j]*cal.y[j]);
		xy += (float)(cal.x[j]*cal.y[j]);
	}

// Get determinant of matrix -- check if determinant is too small
	det = n*(x2*y2 - xy*xy) + x*(xy*y - x*y2) + y*(x*xy - y*x2);
	if(det < 0.1 && det > -0.1) {
		printf("ts_calibrate: determinant is too small -- %f\n",det);
		return 0;
	}

// Get elements of inverse matrix
	a = (x2*y2 - xy*xy)/det;
	b = (xy*y - x*y2)/det;
	c = (x*xy - y*x2)/det;
	e = (n*y2 - y*y)/det;
	f = (x*y - n*xy)/det;
	i = (n*x2 - x*x)/det;

// Get sums for x calibration
	z = zx = zy = 0;
	for(j=0;j<5;j++) {
		z += (float)cal.xfb[j];
		zx += (float)(cal.xfb[j]*cal.x[j]);
		zy += (float)(cal.xfb[j]*cal.y[j]);
	}

// Now multiply out to get the calibration for framebuffer x coord
	cal.a[0] = (int)((a*z + b*zx + c*zy)*(scaling));
	cal.a[1] = (int)((b*z + e*zx + f*zy)*(scaling));
	cal.a[2] = (int)((c*z + f*zx + i*zy)*(scaling));

	printf("%f %f %f\n",(a*z + b*zx + c*zy),
				(b*z + e*zx + f*zy),
				(c*z + f*zx + i*zy));

// Get sums for y calibration
	z = zx = zy = 0;
	for(j=0;j<5;j++) {
		z += (float)cal.yfb[j];
		zx += (float)(cal.yfb[j]*cal.x[j]);
		zy += (float)(cal.yfb[j]*cal.y[j]);
	}

// Now multiply out to get the calibration for framebuffer y coord
	cal.a[3] = (int)((a*z + b*zx + c*zy)*(scaling));
	cal.a[4] = (int)((b*z + e*zx + f*zy)*(scaling));
	cal.a[5] = (int)((c*z + f*zx + i*zy)*(scaling));

	printf("%f %f %f\n",(a*z + b*zx + c*zy),
				(b*z + e*zx + f*zy),
				(c*z + f*zx + i*zy));

// If we got here, we're OK, so assign scaling to a[6] and return
	cal.a[6] = (int)scaling;
	PerformCalibration1();
	return 1;
/*
// This code was here originally to just insert default values
	for(j=0;j<7;j++) {
		c->a[j]=0;
	}
	c->a[1] = c->a[5] = c->a[6] = 1;
	return 1;
*/
}

int CCalibrateWnd::PerformCalibration1()
{
	int j;
	float n, x, y, x2, y2, xy, z, zx, zy;
	float det, det1, det2, det3;
	float scaling = 65536.0;
	n = x = y = x2 = y2 = xy = 0;
	for (j = 0; j < 5; j++)
	{
		n += 1.0;
		x += (float)cal.x[j];
		y += (float)cal.y[j];
		x2 += (float)(cal.x[j] * cal.x[j]);
		y2 += (float)(cal.y[j] * cal.y[j]);
		xy += (float)(cal.x[j] * cal.y[j]);
	}
	det = n * (x2*y2 - xy*xy) + x * (xy*y - x*y2) + y * (x*xy - y*x2);
	if (det < 0.1 && det > -0.1)
	{
		printf("Determinant is too small!\n");
		return 1;
	}
	z = zx = zy = 0;
	for (j = 0; j < 5; j++)
	{
		z += (float)cal.xfb[j];
		zx += (float)(cal.xfb[j] * cal.x[j]);
		zy += (float)(cal.xfb[j] * cal.y[j]);
	}
	det1 = n * (zx*y2 - xy*zy) + z * (xy*y - x*y2) + y * (x*zy - y*zx);
	det2 = n * (x2*zy - zx*xy) + x * (zx*y - x*zy) + z * (x*xy - y*x2);
	det3 = z * (x2*y2 - xy*xy) + x * (xy*zy - zx*y2) + y * (zx*xy - zy*x2);
	cal.a[0] = (int)((det1 / det) * scaling);
	cal.a[1] = (int)((det2 / det) * scaling);
	cal.a[2] = (int)((det3 / det) * scaling);
	printf("%10d %10d %10d\n", cal.a[0], cal.a[1], cal.a[2]);
	z = zx = zy = 0;
	for (j = 0; j < 5; j++)
	{
		z += (float)cal.yfb[j];
		zx += (float)(cal.yfb[j] * cal.x[j]);
		zy += (float)(cal.yfb[j] * cal.y[j]);
	}
	det1 = n * (zx*y2 - xy*zy) + z * (xy*y - x*y2) + y * (x*zy - y*zx);
	det2 = n * (x2*zy - zx*xy) + x * (zx*y - x*zy) + z * (x*xy - y*x2);
	det3 = z * (x2*y2 - xy*xy) + x * (xy*zy - zx*y2) + y * (zx*xy - zy*x2);
	cal.a[3] = (int)((det1 / det) * scaling);
	cal.a[4] = (int)((det2 / det) * scaling);
	cal.a[5] = (int)((det3 / det) * scaling);
	printf("%10d %10d %10d\n", cal.a[3], cal.a[4], cal.a[5]);
	cal.a[6] = (int)scaling;
	return 0;
}

void CCalibrateWnd::ClearBuf()
{
	int fd = ts_fd(ts);
	fd_set fdset;
	struct timeval tv;
	int nfds;
	struct ts_sample sample;

	while (1) {
		FD_ZERO(&fdset);
		FD_SET(fd, &fdset);

		tv.tv_sec = 0;
		tv.tv_usec = 0;

		nfds = select(fd + 1, &fdset, NULL, NULL, &tv);
		if (nfds == 0) break;

		if (ts_read_raw(ts, &sample, 1) < 0) {
			perror("ts_read");
			return ;
		}
	}
}

void CCalibrateWnd::SaveToConfig()
{
	char cal_buffer[256];
	char *calfile = NULL;
	unsigned int i, len;
	FILE *cal_fd;

	printf ("Calibration constants: ");
	for (i = 0; i < 7; i++) printf("%d ", cal.a [i]);
	printf("\n");
	if ((calfile = getenv("TSLIB_CALIBFILE")) != NULL)
	{
		cal_fd = fopen(calfile, "wb");
	}
	else
	{
		cal_fd = fopen ("test", "wb");
	}
	len = sprintf(cal_buffer,"%d %d %d %d %d %d %d",
			              cal.a[0], cal.a[1], cal.a[2],
			              cal.a[3], cal.a[4], cal.a[5], cal.a[6]);
	fwrite (cal_buffer, len,1,cal_fd);
	fclose (cal_fd);
	cal_fd = NULL;
	system("sync");
}


