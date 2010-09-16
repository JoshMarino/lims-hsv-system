#include "Calibration.h"

using std::vector;
using std::string;
using cv::Mat;
using cv::Point2f;
using cv::Point3f;
using cv::Size;
using cv::TermCriteria;
using cv::VideoCapture;
using cv::Range;
using cv::Mat_;


/**
* This function collects world coordinates and corresponding pixel coordinates 
* using a checkerboard calibration pattern.  No actual calibration is done.
* A default world coordinate system is used where the distance between two
* neighboring corners defines a unit of change in the x and y directions.
* Note that if the grid pattern is not square, then there will be two different
* units, one in the x and another in the y.  As further illustrated in the
* ascii-art below, the x-axis runs positive from left-to-right across columns 
* of an image and the y-axis is positive from bottom-to-top along the rows 
* of an image.  This is commonly the xy-frame that is traditionally used in 
* the math and sciences.  The origin of this frame is always located at 
* the first pixel location found by the OpenCV 
* <code> findChessboardCorners </code> function.
*
*		  (0,0) o-------o (1, 0)
*				|		|
*				|		|
*				|		|
*		 (0,-1) o-------o (1,-1)
*
*	^
*	| y
*	o---> x
*
* @param[in] cam the camera to take the pictures from
* @param[inout] calib the calibration parameters
* @param[in] n the number of checkerboards to find
* @param[in] prompt if true, then ask an image should be used or ignored
*
* @note to ignore an image that has a checkerboard in it press the 'i' key
* @note to quit the grabbing process at anytime press the 'q' key.
*/

int Calibration::getChessboardViews(VideoCapture& cam, int n, bool prompt)
{
	int kb_input, good_imgs;
	int num_points, cols;
	bool chssbd_found;
	Mat img, draw_corners;
	vector<Point2f> corners;
	vector<Point3f> world_loc;
	Size win, zz, grid;
	TermCriteria crit;

	// initialize parameters and create window
	good_imgs = 0;
	grid = find_chessboard.grid;
	num_points = grid.area();
	cols = grid.width;
	win = sub_pixel.win;
	zz = sub_pixel.zz;
	crit = sub_pixel.crit;
	cv::namedWindow("calibration", CV_WINDOW_AUTOSIZE);

	// findChessboardCorners(...) places the grid points in row major order
	// so setup the world frame beforehand using this property of the function
	for(int i = 0; i < num_points; ++i) {
		float x = (float) (i % cols);
		float y = (float) (-i / cols);
		world_loc.push_back(Point3f(x, y, 0));
	}

	// look for the checkerboard pattern
	while(good_imgs < n) {
		cam >> img;
		if(img.empty()) break;

		chssbd_found = cv::findChessboardCorners(img, grid, corners);
		if(chssbd_found) {
			// get subpixel accuracy on locations
			cv::cornerSubPix(img, corners, win, zz, crit);

			// store pixel and world locations
			views.pixel.push_back(corners);
			views.world.push_back(world_loc);

			// save the checkerboard
			if(views.save_views) views.chessboards.push_back(img);
			good_imgs++;
		}

		// show the resulting image and, if found, checkerboard on screen
		draw_corners = Mat(corners);
		cv::drawChessboardCorners(img, grid, draw_corners, chssbd_found);
		cv::imshow("calibration", img);

		// prompt user for next step
		if(chssbd_found && prompt) {
			// prompt user to keep results or overwrite it
			kb_input = cv::waitKey(0);
			if(kb_input == 'i') good_imgs--; // ignore image
			else if(kb_input == 'q') break; // quit
		}
		else if(cv::waitKey(5) == 'q') break; // quit
	}

	// can't find cv:: equivalent, so using C version
	cvDestroyWindow("calibration");
	return good_imgs;
}