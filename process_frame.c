/* Copying and distribution of this file, with or without modification,
 * are permitted in any medium without royalty. This file is offered as-is,
 * without any warranty.
 */

/*! @file process_frame.c
 * @brief Contains the actual algorithm and calculations.
 */

/* Definitions specific to this application. Also includes the Oscar main header file. */
#include "template.h"
#include <string.h>
#include <stdlib.h>

#define IMG_SIZE NUM_COLORS*OSC_CAM_MAX_IMAGE_WIDTH*OSC_CAM_MAX_IMAGE_HEIGHT

const int nc = OSC_CAM_MAX_IMAGE_WIDTH;
const int nr = OSC_CAM_MAX_IMAGE_HEIGHT;

int TextColor;
const int Border = 1;





void ResetProcess()
{
	//called when "reset" button is pressed
	if(TextColor == CYAN)
		TextColor = MAGENTA;
	else
		TextColor = CYAN;
}



void Binarize() {
	int r, c;
	//set result buffer to zero
	memset(data.u8TempImage[THRESHOLD], 0, IMG_SIZE);
	//loop over the rows
	for(r = Border*nc; r < (nr-Border)*nc; r += nc) {
		//loop over the columns
		for(c = Border; c < (nc-Border); c++) {
			unsigned char p = data.u8TempImage[SENSORIMG][r+c];
			//if the value is smaller than threshold value
			if(p < data.ipc.state.nThreshold) {
				//set pixel value to 255 in THRESHOLD
				data.u8TempImage[THRESHOLD][r+c] = 255;
			}
		}
	}
}

/*unsigned char OtsuThreshold(intInIndex){
	float Hist[256];
	unsigned char* p = data.u8TempImage[InIndex];
	float best;
	unsigned char w0,w1,mu0,mu1;
	memset(Hist, 0, sizeof(Hist));
	for(i1 = 0; i1 < nr*nc; i1++) {
		Hist[p[i1]] += 1;
	}
	//class 0 and 1 probabilities and averages
	for(int i1 = 0; i1 <= K; i1++) {
		w0 += Hist[i1];
		mu0 += (Hist[i1]*i1);
	}
	for(; i1 <= 255; i1++) {
		w1 += Hist[i1];
		mu1 += (Hist[i1]*i1);
	}
	//do normalization of average values
	mu0 /= w0;
	mu1 /= w1;


	return(sqrt(w0*w1*(mu0 -mu1)*(mu0 -mu1)));

}
*/

void Erode_3x3(int InIndex, int OutIndex) {
	int c, r;
	for(r = Border*nc; r < (nr-Border)*nc; r += nc) {
		for(c = Border; c < (nc-Border); c++) {
			unsigned char* p = &data.u8TempImage[InIndex][r+c];
			data.u8TempImage[OutIndex][r+c] =
					*(p-nc-1) & *(p-nc) & *(p-nc+1) &
					*(p-1)
					& *p
					& *(p+1)
					&
					*(p+nc-1) & *(p+nc) & *(p+nc+1);
		}
	}
}
void Dilate_3x3(int InIndex, int OutIndex){
	int c, r;
	for(r = Border*nc; r < (nr-Border)*nc; r += nc) {
		for(c = Border; c < (nc-Border); c++) {
			unsigned char* p = &data.u8TempImage[InIndex][r+c];
			data.u8TempImage[OutIndex][r+c] =
					*(p-nc-1) | *(p-nc) | *(p-nc+1) |
					*(p-1)
					| *p
					| *(p+1)
					|
					*(p+nc-1) & *(p+nc) | *(p+nc+1);
		}
	}
}



void ProcessFrame()
{
	char Text[] = "hallo world";
	//initialize counters
	if(data.ipc.state.nStepCounter == 1) {
		//use for initialization; only done in first step
		memset(data.u8TempImage[THRESHOLD], 0, IMG_SIZE);
		TextColor = CYAN;
	} else {
		//example for copying sensor image to background image
		memcpy(data.u8TempImage[BACKGROUND], data.u8TempImage[SENSORIMG], IMG_SIZE);

		//example for drawing output
		//draw line
		DrawLine(10, 100, 200, 20, RED);
		DrawLine(10, 130, 200, 20, BLUE);
		DrawLine(10, 150, 200, 20, GREEN);

		//draw open rectangle
		DrawBoundingBox(20, 10, 50, 40, false, GREEN);
		//draw filled rectangle
		DrawBoundingBox(80, 100, 110, 120, true, BLUE);
		DrawBoundingBox(320, 350, 370, 370, true, RED);
		DrawString(200, 200, strlen(Text), TINY, TextColor, Text);
		//THRESHOLD = OtsuThreshold(data.ipc.state.nStepCounter);
		Binarize();
		Erode_3x3(THRESHOLD, INDEX0);
		Dilate_3x3(INDEX0, THRESHOLD);

	}
}




