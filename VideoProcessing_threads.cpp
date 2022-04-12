// G.Çetin, project started: 26 April 2020
// Let's do some threads!
// https://github.com/gurkanctn/Cpp-Projects
// See bottom of file for Licence
// Proudly uses the olcPixelGameEngine : https://community.onelonecoder.com/
// Uses ESCAPI for webcam capturing functions. 

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include "escapi.h"
#include <iostream>
#include <thread>
#include <chrono>

int nFrameWidth = 640;  //320
int nFrameHeight = 480; //240

bool bVideoBufferExists = false;
bool bContinueCapture = true;
float dTimeBetweenFrames = 0.0f;

unsigned int nSelectedCam = 0;

struct frame
{
	float *pixels = nullptr;

	frame()
	{
		pixels = new float[nFrameWidth * nFrameHeight];
	}

	~frame()
	{
		delete[] pixels;
	}


	float get(int x, int y)
	{
		if (x >= 0 && x < nFrameWidth && y >= 0 && y < nFrameHeight)
		{
			return pixels[y*nFrameWidth + x];
		}
		else
			return 0.0f;
	}

	void set(int x, int y, float p)
	{
		if (x >= 0 && x < nFrameWidth && y >= 0 && y < nFrameHeight)
		{
			pixels[y*nFrameWidth + x] = p;
		}
	}


	void operator=(const frame &f)
	{
		memcpy(this->pixels, f.pixels, nFrameWidth * nFrameHeight * sizeof(float));
	}
};

frame input, output, prev_input, activity;

union RGBint
	{
		int rgb;
		unsigned char c[4];
	};

int nCameras = 0;
SimpleCapParams capture;

class cCapture
{
public:
	SimpleCapParams SCP_capture;
	frame inputFrame;
};

//up to 4 cameras can be captured (cycled through)
cCapture mCapture[4]; //TODO: make it dynamic at Initialization

enum ALGORITHM
{
	THRESHOLD, MOTION, LOWPASS, CONVOLUTION,
	SOBEL, MORPHO, MEDIAN, ADAPTIVE, GURKAN,
};

enum MORPHOP
{
	DILATION,
	EROSION,
	EDGE
};

// Algorithm Currently Running
ALGORITHM algo = THRESHOLD;
MORPHOP morph = DILATION;
int nMorphCount = 1;

void initCameraForCapture() {
	// Initialise webcam to screen dimensions
	nCameras = setupESCAPI();
	if (nCameras == 0) {
		printf("\n \n ERROR: No cameras available! Or ESCAPI.DLL not found. Check your setup.\n");
		return;
	}

	printf("# of Cameras: %d\n\n", nCameras);
	nSelectedCam = 0;

	for (int i = 0; i < nCameras; i++) {
		mCapture[i].SCP_capture.mWidth = nFrameWidth;
		mCapture[i].SCP_capture.mHeight = nFrameHeight;
		mCapture[i].SCP_capture.mTargetBuf = new int[nFrameWidth * nFrameHeight];
		//capture = mCapture[i].SCP_capture;
		
		printf("H = %d, W= %d \n", nFrameHeight, nFrameWidth);//debug
		printf("%d SCP_mTargetBuf = %d\n", i, &mCapture[i].SCP_capture.mTargetBuf); //debug
		//printf("%d capture_mTargetBuf = %d\n", i, capture.mTargetBuf); //debug

		if (initCapture(i, &mCapture[i].SCP_capture) == 0) { //(mCapture[i].SCP_capture)
			printf("Capture Failed - %d device may already be in use.\n", i);
			return;
		} else printf("Camera #%d Init Complete. \n\n", i);
		Sleep(500);
	}
}

void CaptureVideo()
{
	while (bContinueCapture) {

	// printf("%d. ", nSelectedCam); //for debug

	// CAPTURING WEBCAM IMAGE

	//auto t1 = std::chrono::high_resolution_clock::now();
	//auto t2 = std::chrono::high_resolution_clock::now();
	//auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
	//std::cout << duration << std::endl;
	
	for (int tempSelectedCam = 0; tempSelectedCam < nCameras; tempSelectedCam++) {
		doCapture(tempSelectedCam);

		while (isCaptureDone(tempSelectedCam) == 0) {}
		if (tempSelectedCam == nSelectedCam) {
			for (int y = 0; y < mCapture[tempSelectedCam].SCP_capture.mHeight; y++)
				for (int x = 0; x < mCapture[tempSelectedCam].SCP_capture.mWidth; x++)
				{
					RGBint col;
					int id = y * nFrameWidth + x;
					//col.rgb = capture.mTargetBuf[id];
					col.rgb = mCapture[tempSelectedCam].SCP_capture.mTargetBuf[id];
					mCapture[tempSelectedCam].inputFrame.pixels[y*nFrameWidth + x] = (float)col.c[1] / 255.0f;
				}
			bVideoBufferExists = true;
		}
		else {
			for (int y = 0; y < mCapture[tempSelectedCam].SCP_capture.mHeight/2; y++)
				for (int x = 0; x < mCapture[tempSelectedCam].SCP_capture.mWidth/2; x++)
				{
					RGBint col;
					int id = y*4 * nFrameWidth + x*4;
					//col.rgb = capture.mTargetBuf[id];
					col.rgb = mCapture[tempSelectedCam].SCP_capture.mTargetBuf[id];
					mCapture[tempSelectedCam].inputFrame.pixels[y*nFrameWidth + x] = (float)col.c[1] / 255.0f;
				}
			bVideoBufferExists = true;
		}
		}
		
	input = mCapture[nSelectedCam].inputFrame;
	}
}

class WIP_ImageProcessing : public olc::PixelGameEngine
{
public:
	WIP_ImageProcessing()
	{
		sAppName = "WIP_ImageProcessing_multiCamera";
	}

public:

	std::wstring GCTEXT[6];

	bool OnUserCreate() override
	{
		GCTEXT[0].append(L"...XXXX..");
		GCTEXT[0].append(L"..X.....");
		GCTEXT[0].append(L"..X.....");
		GCTEXT[0].append(L"..XXXXX.");
		GCTEXT[0].append(L"..X...X.");
		GCTEXT[0].append(L"..X...X.");
		GCTEXT[0].append(L"..X...X.");
		GCTEXT[0].append(L"..XXXXX.");
		
		GCTEXT[1].append(L"...X.X..");
		GCTEXT[1].append(L"..X...X.");
		GCTEXT[1].append(L"..X...X.");
		GCTEXT[1].append(L"..X...X.");
		GCTEXT[1].append(L"..X...X.");
		GCTEXT[1].append(L"..X...X.");
		GCTEXT[1].append(L"..X...X.");
		GCTEXT[1].append(L"..XXXXX.");
		
		GCTEXT[2].append(L"..XXXX..");
		GCTEXT[2].append(L"..X...X.");
		GCTEXT[2].append(L"..X...X.");
		GCTEXT[2].append(L"..X.XXX.");
		GCTEXT[2].append(L"..X...X.");
		GCTEXT[2].append(L"..X...X.");
		GCTEXT[2].append(L"..X...X.");
		GCTEXT[2].append(L"..X...X.");
		
		GCTEXT[3].append(L"..X...X.");
		GCTEXT[3].append(L"..X..XX.");
		GCTEXT[3].append(L"..XXXX..");
		GCTEXT[3].append(L"..X.XX..");
		GCTEXT[3].append(L"..X..XX.");
		GCTEXT[3].append(L"..X...X.");
		GCTEXT[3].append(L"..X...X.");
		GCTEXT[3].append(L"..X...X.");
		
		GCTEXT[4].append(L"...XXX..");
		GCTEXT[4].append(L"..X...x.");
		GCTEXT[4].append(L"..X...x.");
		GCTEXT[4].append(L"..XXXXX.");
		GCTEXT[4].append(L"..X...X.");
		GCTEXT[4].append(L"..X...X.");
		GCTEXT[4].append(L"..X...X.");
		GCTEXT[4].append(L"..X...X.");
		
		GCTEXT[5].append(L"..X...X.");
		GCTEXT[5].append(L"..X...X.");
		GCTEXT[5].append(L"..XX..X.");
		GCTEXT[5].append(L"..XXX.X.");
		GCTEXT[5].append(L"..X.XXX.");
		GCTEXT[5].append(L"..X.XXX.");
		GCTEXT[5].append(L"..X..XX.");
		GCTEXT[5].append(L"..X..XX.");
		
		printf("\n ************************************* \n");
		printf("Image processing trials. G.Çetin.\n");
		return true;
	}

	int nMainCounter = 0;

	void DrawFrame(frame &f, int x, int y, int scale)
	{
		
		for (int i = 0; i < nFrameWidth/scale; i++)
			for (int j = 0; j < nFrameHeight/scale; j++)
			{
				int c = (int)std::min(std::max(0.0f, f.pixels[(j*scale)*nFrameWidth + (i*scale)] * 255.0f), 255.0f);
				Draw(x + i, y + j, olc::Pixel(c, c, c));
			}
	}

	float fThresholdValue = 0.5f;
	float fLowPassRC = 0.1f;
	float fAdaptiveBias = 1.1f;
	float fGCThreshold = 12.5f;

	float *pConvoKernel = kernel_blur;

	float kernel_blur[9] =
	{
		0.0f,   0.125,  0.0f,
		0.125f, 0.5f,   0.125f,
		0.0f,   0.125f, 0.0f,
	};

	float kernel_sharpen[9] =
	{
		0.0f,  -1.0f,  0.0f,
		-1.0f,  5.0f, -1.0f,
		0.0f,  -1.0f,  0.0f,
	};

	float kernel_sobel_v[9] =
	{
		-1.0f, 0.0f, +1.0f,
		-2.0f, 0.0f, +2.0f,
		-1.0f, 0.0f, +1.0f,
	};

	float kernel_sobel_h[9] =
	{
		-1.0f, -2.0f, -1.0f,
		 0.0f, 0.0f, 0.0f,
		+1.0f, +2.0f, +1.0f,
	};
	
	bool OnUserUpdate(float fElapsedTime) override
	{
		//using namespace std::literals::chrono_literals;
		//std::chrono::milliseconds timespan(50); // or whatever
		
		//Run this section Every Game Cycle (perhaps read user inputs? communicate with someone)
		{

			if (GetKey(olc::Key::K1).bReleased) algo = THRESHOLD; //bReleased or bPressed?
			if (GetKey(olc::Key::K2).bReleased) algo = MOTION;
			if (GetKey(olc::Key::K3).bReleased) algo = LOWPASS;
			if (GetKey(olc::Key::K4).bReleased) algo = CONVOLUTION;
			if (GetKey(olc::Key::K5).bReleased) algo = SOBEL;
			if (GetKey(olc::Key::K6).bReleased) algo = MORPHO;
			if (GetKey(olc::Key::K7).bReleased) algo = MEDIAN;
			if (GetKey(olc::Key::K8).bReleased) algo = ADAPTIVE;
			if (GetKey(olc::Key::K9).bReleased) algo = GURKAN;

			if (GetKey(olc::Key::TAB).bReleased) {
				nSelectedCam = (nSelectedCam + 1) % nCameras; //bReleased or bPressed?
				//printf(">Cam # %d\n", nSelectedCam);
			}
			if (GetKey(olc::Key::ESCAPE).bReleased) {
				bContinueCapture = false;  //do not continue capturing
				return false;
			}
		}
		dTimeBetweenFrames += fElapsedTime;

		// run the rest of the function code only if the camera frame is fresh
		if (!bVideoBufferExists) {
			//std::this_thread::sleep_for(timespan); //milliseconds
			dTimeBetweenFrames = 0.0f;
			return true;
		}
		else {
			//dTimeBetweenFrames = 0.0f;
		}

		switch (algo)
		{
		case THRESHOLD:
			// Respond to user input
			if (GetKey(olc::Key::Z).bHeld) fThresholdValue -= 2.5f * dTimeBetweenFrames;
			if (GetKey(olc::Key::X).bHeld) fThresholdValue += 2.5f * dTimeBetweenFrames;
			if (fThresholdValue > 1.0f) fThresholdValue = 1.0f;
			if (fThresholdValue < 0.0f) fThresholdValue = 0.0f;

			// Perform threshold per pixel
			for (int i = 0; i < nFrameWidth; i++)
				for (int j = 0; j < nFrameHeight; j++)
					output.set(i, j, input.get(i, j) >= fThresholdValue ? 1.0f : 0.0f);
			break;

		case MOTION:
			// Returns the absolute difference between successive frames per pixel
			for (int i = 0; i < nFrameWidth; i++)
				for (int j = 0; j < nFrameHeight; j++)
					output.set(i, j, 10.0f*fabs(input.get(i, j) - prev_input.get(i, j)));
			break;

		case LOWPASS:
			// Respond to user input
			if (GetKey(olc::Key::Z).bHeld) fLowPassRC -= 2.5f * dTimeBetweenFrames;
			if (GetKey(olc::Key::X).bHeld) fLowPassRC += 2.5f * dTimeBetweenFrames;
			if (fLowPassRC > 1.0f) fLowPassRC = 1.0f;
			if (fLowPassRC < 0.0f) fLowPassRC = 0.0f;

			// Pass each pixel through a temporal RC filter
			for (int i = 0; i < nFrameWidth; i++)
				for (int j = 0; j < nFrameHeight; j++)
				{
					float dPixel = input.get(i, j) - output.get(i, j);
					dPixel *= fLowPassRC;
					output.set(i, j, dPixel + output.get(i, j));
				}
			break;

		case CONVOLUTION:
			// Respond to user input
			if (GetKey(olc::Key::Z).bHeld) pConvoKernel = kernel_blur;
			if (GetKey(olc::Key::X).bHeld) pConvoKernel = kernel_sharpen;

			for (int i = 0; i < nFrameWidth; i++)
				for (int j = 0; j < nFrameHeight; j++)
				{
					float fSum = 0.0f;
					for (int n = -1; n < +2; n++)
						for (int m = -1; m < +2; m++)
							fSum += input.get(i + n, j + m) * pConvoKernel[(m + 1) * 3 + (n + 1)];

					output.set(i, j, fSum);
				}
			break;

		case SOBEL:
			for (int i = 0; i < nFrameWidth; i++)
				for (int j = 0; j < nFrameHeight; j++)
				{
					float fKernelSumH = 0.0f;
					float fKernelSumV = 0.0f;

					for (int n = -1; n < +2; n++)
						for (int m = -1; m < +2; m++)
						{
							fKernelSumH += input.get(i + n, j + m) * kernel_sobel_h[(m + 1) * 3 + (n + 1)];
							fKernelSumV += input.get(i + n, j + m) * kernel_sobel_v[(m + 1) * 3 + (n + 1)];
						}

					output.set(i, j, fabs((fKernelSumH + fKernelSumV) / 2.0f));
				}
			break;

		case MORPHO:

			// Respond to user input
			if (GetKey(olc::Key::Z).bHeld) morph = DILATION;
			if (GetKey(olc::Key::X).bHeld) morph = EROSION;
			if (GetKey(olc::Key::C).bHeld) morph = EDGE;

			if (GetKey(olc::Key::A).bReleased) nMorphCount--;
			if (GetKey(olc::Key::S).bReleased) nMorphCount++;
			if (nMorphCount > 10.0f) nMorphCount = 10.0f;
			if (nMorphCount < 1.0f) nMorphCount = 1.0f;

			// Threshold First to binarise image
			for (int i = 0; i < nFrameWidth; i++)
				for (int j = 0; j < nFrameHeight; j++)
				{
					activity.set(i, j, input.get(i, j) > fThresholdValue ? 1.0f : 0.0f);
				}

			//threshold = activity;

			switch (morph)
			{
			case DILATION:
				for (int n = 0; n < nMorphCount; n++)
				{
					output = activity;

					for (int i = 0; i < nFrameWidth; i++)
						for (int j = 0; j < nFrameHeight; j++)
						{
							if (activity.get(i, j) == 1.0f)
							{
								output.set(i, j, 1.0f);
								output.set(i - 1, j, 1.0f);
								output.set(i + 1, j, 1.0f);
								output.set(i, j - 1, 1.0f);
								output.set(i, j + 1, 1.0f);
								output.set(i - 1, j - 1, 1.0f);
								output.set(i + 1, j + 1, 1.0f);
								output.set(i + 1, j - 1, 1.0f);
								output.set(i - 1, j + 1, 1.0f);
							}
						}

					activity = output;
				}
				break;

			case EROSION:
				for (int n = 0; n < nMorphCount; n++)
				{
					output = activity;
					for (int i = 0; i < nFrameWidth; i++)
						for (int j = 0; j < nFrameHeight; j++)
						{

							float sum = activity.get(i - 1, j) + activity.get(i + 1, j) + activity.get(i, j - 1) + activity.get(i, j + 1) +
								activity.get(i - 1, j - 1) + activity.get(i + 1, j + 1) + activity.get(i + 1, j - 1) + activity.get(i - 1, j + 1);

							if (activity.get(i, j) == 1.0f && sum < 8.0f)
							{
								output.set(i, j, 0.0f);
							}
						}
					activity = output;
				}
				break;

			case EDGE:
				output = activity;
				for (int i = 0; i < nFrameWidth; i++)
					for (int j = 0; j < nFrameHeight; j++)
					{

						float sum = activity.get(i - 1, j) + activity.get(i + 1, j) + activity.get(i, j - 1) + activity.get(i, j + 1) +
							activity.get(i - 1, j - 1) + activity.get(i + 1, j + 1) + activity.get(i + 1, j - 1) + activity.get(i - 1, j + 1);

						if (activity.get(i, j) == 1.0f && sum == 8.0f)
						{
							output.set(i, j, 0.0f);
						}
					}
				break;

			}
			break;

		case MEDIAN:
			for (int i = 0; i < nFrameWidth; i++)
				for (int j = 0; j < nFrameHeight; j++)
				{
					std::vector<float> v;

					for (int n = -2; n < +3; n++)
						for (int m = -2; m < +3; m++)
							v.push_back(input.get(i + n, j + m));

					std::sort(v.begin(), v.end(), std::greater<float>());
					output.set(i, j, v[12]);
				}
			break;

		case ADAPTIVE:
			// Respond to user input
			if (GetKey(olc::Key::Z).bHeld) fAdaptiveBias -= 0.1f * dTimeBetweenFrames;
			if (GetKey(olc::Key::X).bHeld) fAdaptiveBias += 0.1f * dTimeBetweenFrames;
			if (fAdaptiveBias > 1.5f) fAdaptiveBias = 1.5f;
			if (fAdaptiveBias < 0.5f) fAdaptiveBias = 0.5f;


			for (int i = 0; i < nFrameWidth; i++)
				for (int j = 0; j < nFrameHeight; j++)
				{
					float fRegionSum = 0.0f;

					for (int n = -2; n < +3; n++)
						for (int m = -2; m < +3; m++)
							fRegionSum += input.get(i + n, j + m);

					fRegionSum /= 25.0f;
					output.set(i, j, input.get(i, j) > (fRegionSum * fAdaptiveBias) ? 1.0f : 0.0f);
				}
			break;

		case GURKAN:
			// Respond to user input
			if (GetKey(olc::Key::Z).bHeld) fGCThreshold -= 10.0f * dTimeBetweenFrames;
			if (GetKey(olc::Key::X).bHeld) fGCThreshold += 10.0f * dTimeBetweenFrames;
			if (GetKey(olc::Key::SPACE).bHeld) fGCThreshold = 12.5f;
			int nMouseX = GetMouseX()- 20; //get mouse position in Frame relative coordinates
			int nMouseY = GetMouseY() - 180;
			
			if (fGCThreshold > 25.0f) fGCThreshold = 25.0f;
			if (fGCThreshold < 0.0f) fGCThreshold = 0.0f;

			// Handle Overlay
			for (int i = 0; i < nFrameWidth; i++)
				for (int j = 0; j < nFrameHeight; j++)
				{
					float fRegionSum = 0.0f;
					for (int n = -2; n < +3; n++)
						for (int m = -2; m < +3; m++)
							fRegionSum += input.get(i + n, j + m);
					output.set(i, j, input.get(i, j) > (fRegionSum > fGCThreshold) ? 0.0f : 0.5f);
				}
			for (int k = 0; k < 6; k++) //all string
				for (int i = 0; i < sqrt(GCTEXT[k].size()); i++) //draw each (point of each) character
					for (int j = 0; j < sqrt(GCTEXT[k].size()); j++)
					{
						wchar_t FontPixel = GCTEXT[k][i + j * 8];
						if (FontPixel == L'X') {
							output.set(k * 20 + i*2, j*2, 1.0f);
							output.set(k * 20 + i*2, j*2+1, 1.0f);
							output.set(k * 20 + i*2+1, j*2, 1.0f);
							output.set(k * 20 + i*2+1, j*2+1, 1.0f);
						}
					}
			
			for (int i = 0; i < 10; i++)
				output.set(nMouseX - 5 + std::rand() %10, nMouseY - 5 + std::rand() %10, 1.0f);

			break;
		}
		prev_input = input;

		// DRAW STUFF ONLY HERE
		Clear(olc::DARK_BLUE);
		// draw previews of all cameras on the top line
		for (int i = 0; i < nCameras; i++) {
			DrawFrame(mCapture[i].inputFrame, 10+i*80, 10, 4);
			DrawRect(10 + i * 80, 10, 160, 160, olc::GREEN);
		}
		
		DrawFrame(input, 10, 180, 1);
		DrawFrame(output, nFrameWidth + 20, 180, 1);

		DrawString(10, 180, "INPUT");
		DrawString(nFrameWidth + 20, 180, "OUTPUT");

		DrawString(10, nFrameHeight + 210, "1) Threshold");
		DrawString(10, nFrameHeight + 220, "2) Absolute Motion");
		DrawString(10, nFrameHeight + 230, "3) Low-Pass Temporal Filtering");
		DrawString(10, nFrameHeight + 240, "4) Convolution (Blurring/Sharpening)");
		DrawString(10, nFrameHeight + 250, "5) Sobel Edge Detection");
		DrawString(10, nFrameHeight + 260, "6) Binary Morphological Operations (Erosion/Dilation)");
		DrawString(10, nFrameHeight + 270, "7) Median Filter");
		DrawString(10, nFrameHeight + 280, "8) Adaptive Threshold");
		DrawString(10, nFrameHeight + 290, "9) GURKAN");

		bVideoBufferExists = false;

		switch (algo)
		{
		case THRESHOLD:
			DrawString(10, 375, "Change threshold value with Z and X keys");
			DrawString(10, 385, "Current value = " + std::to_string(fThresholdValue));
			break;

		case LOWPASS:
			DrawString(10, 375, "Change RC constant value with Z and X keys");
			DrawString(10, 385, "Current value = " + std::to_string(fLowPassRC));
			break;

		case CONVOLUTION:
			DrawString(10, 375, "Change convolution kernel with Z and X keys");
			DrawString(10, 385, "Current kernel = " + std::string((pConvoKernel == kernel_blur) ? "Blur" : "Sharpen"));
			break;

		case MORPHO:
			DrawString(10, 375, "Change operation with Z and X and C keys");
			if (morph == DILATION) DrawString(10, 385, "Current operation = DILATION");
			if (morph == EROSION) DrawString(10, 385, "Current operation = EROSION");
			if (morph == EDGE) DrawString(10, 385, "Current operation = EDGE");
			DrawString(10, 395, "Change Iterations with A and S keys");
			DrawString(10, 405, "Current iteration count = " + std::to_string(nMorphCount));
			break;

		case ADAPTIVE:
			DrawString(10, 375, "Change adaptive threshold bias with Z and X keys");
			DrawString(10, 385, "Current value = " + std::to_string(fAdaptiveBias));
			break;

		case GURKAN:
			DrawString(10, 375, "Change adaptive GURKAN threshold with Z and X keys");
			DrawString(10, 385, "Current value = " + std::to_string(fGCThreshold));
			break;

		default:
			break;
		}

		bVideoBufferExists = false;
		return true;
	}
};

int main()
{
	WIP_ImageProcessing demo;
	

	if (demo.Construct(1400, 800, 1, 1)) {
		initCameraForCapture();
		std::thread thrCapture(CaptureVideo);
		demo.Start();
		thrCapture.join();
	}
		
	return 0;
}


/*
	8-Bits Of Image Processing You Should Know
	"Colin, at least you'll always get 700s now..." - javidx9

	License (OLC-3)
	~~~~~~~~~~~~~~~

	Copyright 2018-2019 OneLoneCoder.com

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions
	are met:

	1. Redistributions or derivations of source code must retain the above
	copyright notice, this list of conditions and the following disclaimer.

	2. Redistributions or derivative works in binary form must reproduce
	the above copyright notice. This list of conditions and the following
	disclaimer must be reproduced in the documentation and/or other
	materials provided with the distribution.

	3. Neither the name of the copyright holder nor the names of its
	contributors may be used to endorse or promote products derived
	from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
	A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
	HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	Instructions:
	~~~~~~~~~~~~~
	Choose algorithm 1-8, instructions on screen


	Relevant Video: https://youtu.be/mRM5Js3VLCk

	Links
	~~~~~
	YouTube:	https://www.youtube.com/javidx9
				https://www.youtube.com/javidx9extra
	Discord:	https://discord.gg/WhwHUMV
	Twitter:	https://www.twitter.com/javidx9
	Twitch:		https://www.twitch.tv/javidx9
	GitHub:		https://www.github.com/onelonecoder
	Patreon:	https://www.patreon.com/javidx9
	Homepage:	https://www.onelonecoder.com

	Author
	~~~~~~
	David Barr, aka javidx9, �OneLoneCoder 2019
*/

