#include "t_dah.h"

#define BGR_CHANS 3
#define GRAY_CHAN 1
#define PYR_LVL 2
#define PYR_OFFSET 5

bool TDahMe3Fc::open(char *conf_file)
{
	return false;
}

bool TDahMe3Fc::open(int tt, double e, double f, int b)
{
	int seq[] = {ROI_0};

	// initialize certain class members
	n_roi = sizeof(seq)/sizeof(int);
	img_w = FC_MAX_WIDTH;
	img_h = FC_MAX_HEIGHT;
	roi_w = img_w;
	roi_h = img_h;
	frame_time = f;

	// bgr_img is associated with the entire full resolution frame
	bgr_img = cvCreateImage(cvSize(img_w, img_h), IPL_DEPTH_8U, BGR_CHANS);
	if(!bgr_img) return false;

	// gr_img is associated with the ROI that is sent back
	gr_img = cvCreateImageHeader(cvSize(roi_w, roi_h), IPL_DEPTH_8U, GRAY_CHAN);
	if(!gr_img) return false;

	// initialize me3 and fastconfig
	if(me3_fc_init(&fg, tt, img_w*img_h*b, b) != FG_OK) {
		me3_err("open");
		return false;
	}

	for(int i = 0; i < MAX_ROI; i++) {
		// set all ROIs at full frame
		if(roi_window(i, 0, roi_w, 0, roi_h) != FG_OK) {
			me3_err("open");
			return false;
		}

		// set all ROIs at same exposure and frame time
		if(roi_exposure(i, e, f) != FG_OK) {
			me3_err("open");
			return false;
		}
	}

	// start grabbing images in order of ROIs specified in seq
	if(me3_fc_acquire(fg, seq, n_roi) != FG_OK) {
		me3_err("open");
		return false;
	}

	return true;
}

void TDahMe3Fc::close()
{
	cvReleaseImage(&bgr_img);
	cvReleaseImageHeader(&gr_img);

	if(me3_fc_deinit(fg) != FG_OK) {
		me3_err("close");
	}
}

bool TDahMe3Fc::grabFrame()
{
	int trigger;

	if(Fg_getParameter(fg, FG_TRIGGERMODE, &trigger, PORT_A) != FG_OK) {
		me3_err("grabFrame");
		return false;
	}

	if(trigger == ASYNC_SOFTWARE_TRIGGER 
		&& Fg_sendSoftwareTrigger(fg, PORT_A) != FG_OK) {
		me3_err("grabFrame");
		return false;
	}

	return true;
}

IplImage *TDahMe3Fc::retrieveFrame(int img_nr)
{
	int act_nr;
	CvMat imgData;

	if(img_nr <= 0) {
		// get newest image
		img_nr = Fg_getImage(fg, SEL_NEW_IMAGE, 0, PORT_A, TIMEOUT);
		if(img_nr < FG_OK) {
			me3_err("retrieveFrame");
			return NULL;
		}
	}

	act_nr = Fg_getLastPicNumberBlocking(fg, img_nr, PORT_A, TIMEOUT);
	if(act_nr < FG_OK) {
		me3_err("retrieveFrame");
		return NULL;
	}

	imgData = cvMat(img_h, img_w, CV_8UC1, 
		Fg_getImagePtr(fg, act_nr, PORT_A));

	cvResetImageROI(bgr_img);
	cvMerge(&imgData, &imgData, &imgData, NULL, bgr_img);

	return bgr_img;
}

int TDahMe3Fc::initROIs(int n, int rw, int rh, char *s, bool use_kal, bool use_tmplt)
{
	int rc;
	int seq[] = {ROI_0, ROI_1, ROI_2, ROI_3, ROI_4, ROI_5, ROI_6, ROI_7};

	if(Fg_stopAcquire(fg, PORT_A) != FG_OK) {
		me3_err("initROIs");
		return Fg_getLastErrorNumber(fg);
	}

	// start grabbing images in order of ROIs specified in seq
	if(me3_fc_acquire(fg, seq, n) != FG_OK) {
		me3_err("initROIs");
		return Fg_getLastErrorNumber(fg);
	}

	if(TDah::initROIs(n, rw, rh, s, use_kal, use_tmplt) == CV_OK) {

	}

	return !CV_OK;
}

void TDahMe3Fc::showROILoc(void)
{
	cvResetImageROI(gr_img);
	cvResetImageROI(bgr_img);
	cvCvtColor(gr_img, bgr_img, CV_GRAY2BGR);
	show_position(gr, n_roi, kal, wr, NULL, bgr_img);
}

int TDahMe3Fc::getROILoc(int img_nr, ROILoc *r)
{
	int act_nr, cur_roi;
	uint64 ts;

	act_nr = Fg_getLastPicNumberBlocking(fg, img_nr, PORT_A, TIMEOUT);
	if(act_nr < FG_OK) {
		me3_err("retrieveFrame");
		return Fg_getLastErrorNumber(fg);
	}

	// get image tag, tag == X => ROI_X
	cur_roi = act_nr;
	if(Fg_getParameter(fg, FG_IMAGE_TAG, &cur_roi, PORT_A) != FG_OK) {
		me3_err("grabFrame");
		return Fg_getLastErrorNumber(fg);
	}
	cur_roi = cur_roi >> 16;

	ts = act_nr;
	if(Fg_getParameter(fg, FG_TIMESTAMP_LONG, &ts, PORT_A)
		!= FG_OK) {
		me3_err("grabFrame");
		return Fg_getLastErrorNumber(fg);
	}

	cvSetData(gr_img, Fg_getImagePtr(fg, act_nr, PORT_A), CV_AUTOSTEP);
	cvSetImageROI(gr_img, cvGetImageROI(gr[cur_roi]));
	cvCopyImage(gr_img, gr[cur_roi]);

	r->ts = (double) ts;
	r->img_nr = act_nr;
	r->roi_nr = cur_roi;

	updateROILoc(cur_roi, gr_img, r, (float) frame_time);
	return act_nr;
}
