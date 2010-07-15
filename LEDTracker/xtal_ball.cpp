#include "t_dah.h"

#define GRAVITY 9.8f

void prediction(CvKalman *kal, float dt_k, float *z_k)
{
	CvMat *z = cvCreateMat(kal->MP, 1, CV_32FC1);
	CvMat *u = cvCreateMat(kal->CP, 1, CV_32FC1);

	float A_k[] = { 1, 0, dt_k, 0, 
					0, 1, 0, dt_k, 
					0, 0, 1, 0,
					0, 0, 0, 1};
	
	float B_k[] = { 0, 
					0.5f*GRAVITY*dt_k*dt_k, 
					0, 
					GRAVITY*dt_k};

	// u = u(x_prev, dt_k) = 0
	cvZero(u);

	memcpy(z->data.fl, z_k, sizeof(float)*Z_DIM);
	memcpy(kal->transition_matrix->data.fl, A_k, sizeof(A_k));
	memcpy(kal->control_matrix->data.fl, B_k, sizeof(B_k));

	cvKalmanPredict(kal, u);
	cvKalmanCorrect(kal, z);

	cvReleaseMat(&z);
	cvReleaseMat(&u);
}

void setup_kalman(CvKalman **kal, int n, float **x0, float **P0)
{
	int i;
	CvKalman *k;

	for(i = 0; i < n; i++) {
		k = kal[i];
		if(k == NULL) {
			continue;
		}

		// H matrix (mapping from state to camera measurements)
		// assumes first k->MP columns of state vector are 
		// configuration variables
		cvSetIdentity(k->measurement_matrix, cvRealScalar(1));

		// might be able to tune based on off-line analysis
		cvSetIdentity(k->process_noise_cov, cvRealScalar(1e-1));

		// should know after experiments
		cvSetIdentity(k->measurement_noise_cov, cvRealScalar(1e-5));

		// initial state
		if(x0 && x0[i]) {
			memcpy(k->state_post->data.fl, x0[i], k->DP*sizeof(float));
		}
		else {
			cvZero(k->state_post);
		}

		// initial covariance matrix
		if(P0 && P0[i]) {
			memcpy(k->error_cov_post->data.fl, P0, k->DP*k->DP*sizeof(float));
		}
		else {
			// don't trust initial guess
			cvSetIdentity(k->error_cov_post, cvRealScalar(100));
		}
	}
}