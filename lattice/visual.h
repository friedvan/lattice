
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#pragma comment(lib, "cv210.lib")
#pragma comment(lib, "highgui210.lib")
#pragma comment(lib, "cxcore210.lib")


IplImage *img = cvCreateImage(cvSize(N, N), IPL_DEPTH_8U, 1);

void img_init() {
	
	cvNamedWindow("A");
	cvNamedWindow("B");
}

void img_print(node *G, bool isA) {
	//char c = (rand()%255+150) % 255;
	for(int i=0; i<N; i++)
		for (int j=0; j<N; j++) {
			if (G[i*N+j].alive) 
				img->imageData[i * img->widthStep + j] = 255;
			else
				img->imageData[i * img->widthStep + j] = 0;
		}


	if (isA)
		cvShowImage("A", img);
	else
		cvShowImage("B", img);
	cvWaitKey(0);
	
}

void img_destroy() {
	cvDestroyAllWindows();
	cvReleaseImage(&img);
}