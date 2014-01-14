
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#pragma comment(lib, "cv210.lib")
#pragma comment(lib, "highgui210.lib")
#pragma comment(lib, "cxcore210.lib")


IplImage *img = cvCreateImage(cvSize(N, N), IPL_DEPTH_8U, 4);

void img_init() {
	
	cvNamedWindow("A");
	cvNamedWindow("B");
}

int neighbor_num(node *G, int k) {
	int n = 0;
	for (int i = 0; i < LATTICE; i++) {
		if (G[G[k].base[i].x * N + G[k].base[i].y].alive) {
			n++;
		}
	}
	return n;
}

void img_print(node *G, bool isA) {
	for (int i = 0; i < N; i++)	  {
		for (int j = 0; j < N; j++) {
			if (G[i*N + j].alive) {
				if (G[i*N + j].type == MONOMER)	{
					img->imageData[i * img->widthStep + img->nChannels*j] = 0;
					img->imageData[i * img->widthStep + img->nChannels*j + 1] = 0;
					img->imageData[i * img->widthStep + img->nChannels*j + 2] = 255;
				} else {
					img->imageData[i * img->widthStep + img->nChannels*j] = 255;
					img->imageData[i * img->widthStep + img->nChannels*j + 1] = 0;
					img->imageData[i * img->widthStep + img->nChannels*j + 2] = 0;
				} 			
				//img->imageData[i * img->widthStep + img->nChannels*j] = 0 + (int)(255 / 8 * neighbor_num(G, i*N + j));
				//img->imageData[i * img->widthStep + img->nChannels*j + 1] = 255+(int)(255 / 8 * neighbor_num(G, i*N + j));
				//img->imageData[i * img->widthStep + img->nChannels*j + 2] = 0 + (int)(255 / 8 * neighbor_num(G, i*N + j));
							   
			} else {
				img->imageData[i * img->widthStep + img->nChannels*j] = 0;
				img->imageData[i * img->widthStep + img->nChannels*j + 1] = 0;
				img->imageData[i * img->widthStep + img->nChannels*j + 2] = 0;
			}
		}
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