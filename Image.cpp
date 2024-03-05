//*****************************************************************************
//
// Image.cpp : Defines the class operations on images
//
// Author - Parag Havaldar
// Code used by students as starter code to display and modify images
//
//*****************************************************************************
#define NOMINMAX

#include "Image.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <ctime>
#include <vector>
#include <stack>
#include <random>
#include <windows.h>

const int WIDTH = 640;
const int HEIGHT = 480;
const double DIST_THRESHOLD = 150;  

// Constructor and Desctructors
MyImage::MyImage() 
{
	Data = NULL;
	Width = -1;
	Height = -1;
	ImagePath[0] = 0;
}

MyImage::~MyImage()
{
	if ( Data )
		delete Data;
}


// Copy constructor
MyImage::MyImage( MyImage *otherImage)
{
	Height = otherImage->Height;
	Width  = otherImage->Width;
	Data   = new char[Width*Height*3];
	strcpy(otherImage->ImagePath, ImagePath );

	for ( int i=0; i<(Height*Width*3); i++ )
	{
		Data[i]	= otherImage->Data[i];
	}


}



// = operator overload
MyImage & MyImage::operator= (const MyImage &otherImage)
{
	Height = otherImage.Height;
	Width  = otherImage.Width;
	Data   = new char[Width*Height*3];
	strcpy( (char *)otherImage.ImagePath, ImagePath );

	for ( int i=0; i<(Height*Width*3); i++ )
	{
		Data[i]	= otherImage.Data[i];
	}

	
	return *this;

}


void DrawTextOnImage(char* imageData, int width, int height, int x, int y, const char* text, char* fontColor) {
	// 1. Create a device context (DC) for drawing
	HDC hdc = CreateCompatibleDC(NULL);
	if (!hdc) {
		return;
	}

	// 2. Load your image into a bitmap
	BITMAPINFO bmi = {};
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = width;
	bmi.bmiHeader.biHeight = -height;  // top-down DIB
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 24;     // 24-bit RGB
	bmi.bmiHeader.biCompression = BI_RGB;

	HBITMAP hBitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, NULL, NULL, 0);
	if (!hBitmap) {
		DeleteDC(hdc);
		return;
	}

	// Set the bitmap data
	SetDIBits(hdc, hBitmap, 0, height, imageData, &bmi, DIB_RGB_COLORS);

	// 3. Select the bitmap into the DC
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdc, hBitmap);

	// 4. Draw the text onto the DC
	SetTextColor(hdc, RGB((unsigned char)fontColor[2], (unsigned char)fontColor[1], (unsigned char)fontColor[0]));  // Convert char values to RGB
	SetBkMode(hdc, TRANSPARENT);            // Background mode to transparent
	TextOutA(hdc, x, y, text, strlen(text));

	// Deselect the bitmap
	SelectObject(hdc, hOldBitmap);

	// 5. Extract the modified image data from the bitmap
	GetDIBits(hdc, hBitmap, 0, height, imageData, &bmi, DIB_RGB_COLORS);

	// Cleanup
	DeleteObject(hBitmap);
	DeleteDC(hdc);
}

char* generateRandomColor() {
	std::random_device rd;  // Used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); // Mersenne Twister random number generator
	std::uniform_int_distribution<> distrib(0, 255); // Uniform distribution between 0 and 255

	char* color = new char[3];

	for (int i = 0; i < 3; ++i) {
		color[i] = static_cast<char>(distrib(gen));
	}

	return color;
}

void MyImage::drawBoundingBox(char* image, int x, int y, int w, int h, char* boundingBoxOutlineColor) {
	if (x < 0 || y < 0 || x + w > Width || y + h > Height) {
		std::cerr << "Invalid bounding box coordinates" << std::endl;
		return;
	}

	// Define the bounding box color (red in RGB)

	for (int col = x; col < x + w; ++col) {
		int topPixelIndex = (y * Width + col) * 3;
		int bottomPixelIndex = ((y + h - 1) * Width + col) * 3;

		image[topPixelIndex] = boundingBoxOutlineColor[0];
		image[topPixelIndex + 1] = boundingBoxOutlineColor[1];
		image[topPixelIndex + 2] = boundingBoxOutlineColor[2];

		image[bottomPixelIndex] = boundingBoxOutlineColor[0];
		image[bottomPixelIndex + 1] = boundingBoxOutlineColor[1];
		image[bottomPixelIndex + 2] = boundingBoxOutlineColor[2];
	}

	// Draw the left and right vertical lines of the bounding box
	for (int row = y; row < y + h; ++row) {
		int leftPixelIndex = (row * Width + x) * 3;
		int rightPixelIndex = (row * Width + x + w - 1) * 3;

		image[leftPixelIndex] = boundingBoxOutlineColor[0];
		image[leftPixelIndex + 1] = boundingBoxOutlineColor[1];
		image[leftPixelIndex + 2] = boundingBoxOutlineColor[2];

		image[rightPixelIndex] = boundingBoxOutlineColor[0];
		image[rightPixelIndex + 1] = boundingBoxOutlineColor[1];
		image[rightPixelIndex + 2] = boundingBoxOutlineColor[2];
	}
}

void BGRtoHSV(int32_t b, int32_t g, int32_t r, double& h, double& s, double& v) {
	// Normalize the BGR values to the range [0, 1]
	double blue = static_cast<double>(b) / 255.0;
	double green = static_cast<double>(g) / 255.0;
	double red = static_cast<double>(r) / 255.0;


	// Find the maximum and minimum of the three RGB components
	v = std::max(std::max(red, green), blue);
	double minVal = std::min(std::min(red, green), blue);

	
	if (v > 0.0) {
		s = (v - minVal) / v;
	}

	// Calculate the hue component (H)
	if (s > 0.0) {
		if (v == red) {
			h = (green - blue) / (v - minVal);
		}
		else if (v == green) {
			h = 2.0 + (blue - red) / (v - minVal);
		}
		else {
			h = 4.0 + (red - green) / (v - minVal);
		}

		// Convert hue to degrees
		h *= 60.0;

		// Ensure the hue is within [0, 360] degrees
		if (h < 0.0) {
			h += 360.0;
		}
	}
	else {
		h = 0.0;
	}
}

void BGRtoYUV(int32_t b, int32_t g, int32_t r, double& y, double& u, double& v) {
	// Normalize the BGR values to the range [0, 1]
	double blue = static_cast<double>(b) / 255.0;
	double green = static_cast<double>(g) / 255.0;
	double red = static_cast<double>(r) / 255.0;

	// Calculate Y, U, and V components
	y = 0.299 * red + 0.587 * green + 0.114 * blue;
	u = -0.14713 * red - 0.28886 * green + 0.436 * blue;
	v = 0.615 * red - 0.51498 * green - 0.10001 * blue;
}

std::vector<double> createHistogram(const double* hueArray, int arraySize, bool ignoreHue) {
	const int numBins = 360;  // Number of bins for the histogram (one for each degree)
	std::vector<double> histogram(numBins, 0);
	double green_hue = 120.0;
	for (int i = 0; i < arraySize; i++) {
		double hue = hueArray[i];
		// Ignore pixels with the specified hue
		if (ignoreHue) {
			if (abs(hue - green_hue) > 1) {
				int bin = static_cast<int>(hue);
				if (bin >= 0 && bin < numBins) {
					histogram[bin]++;
				}
			}
		}
		else {
			int bin = static_cast<int>(hue);
			if (bin >= 0 && bin < numBins) {
				histogram[bin]++;
			}
		}
	}

	return histogram;
}

//std::vector<double> createHistogram(const double* hueArray, int arraySize, bool ignoreHue) {
//	const int numBins = 360;  // Number of bins for the histogram (one for each degree)
//	std::vector<double> histogram(numBins, 0);
//	double green_hue = 120.0;
//	for (int i = 0; i < arraySize; i++) {
//		double hue = hueArray[i];
//		// Ignore pixels with the specified hue
//		if (ignoreHue) {
//			if (abs(hue - green_hue) > 1) {
//				int bin = static_cast<int>(hue);
//				if (bin >= 0 && bin < numBins) {
//					histogram[bin]++;
//				}
//			}
//		}
//		else {
//			int bin = static_cast<int>(hue);
//			if (bin >= 0 && bin < numBins) {
//				histogram[bin]++;
//			}
//		}
//	}
//
//	return histogram;
//}

// MyImage::ReadImage
// Function to read the image given a path
bool MyImage::ReadImage()
{

	// Verify ImagePath
	if (ImagePath[0] == 0 || Width < 0 || Height < 0 )
	{
		fprintf(stderr, "Image or Image properties not defined");
		fprintf(stderr, "Usage is `Image.exe Imagefile w h`");
		return false;
	}
	
	// Create a valid output file pointer
	FILE *IN_FILE;
	IN_FILE = fopen(ImagePath, "rb");
	if ( IN_FILE == NULL ) 
	{
		fprintf(stderr, "Error Opening File for Reading");
		return false;
	}

	// Create and populate RGB buffers
	int i;
	char *Rbuf = new char[Height*Width]; 
	char *Gbuf = new char[Height*Width]; 
	char *Bbuf = new char[Height*Width]; 

	for (i = 0; i < Width*Height; i ++)
	{
		Rbuf[i] = fgetc(IN_FILE);
	}
	for (i = 0; i < Width*Height; i ++)
	{
		Gbuf[i] = fgetc(IN_FILE);
	}
	for (i = 0; i < Width*Height; i ++)
	{
		Bbuf[i] = fgetc(IN_FILE);
	}
	
	// Allocate Data structure and copy
	Data = new char[Width*Height*3];
	for (i = 0; i < Height*Width; i++)
	{
		Data[3*i]	= Bbuf[i];
		Data[3*i+1]	= Gbuf[i];
		Data[3*i+2]	= Rbuf[i];
	}

	// Clean up and return
	delete Rbuf;
	delete Gbuf;
	delete Bbuf;
	fclose(IN_FILE);

	return true;

}



// MyImage functions defined here
bool MyImage::WriteImage()
{
	// Verify ImagePath
	// Verify ImagePath
	if (ImagePath[0] == 0 || Width < 0 || Height < 0 )
	{
		fprintf(stderr, "Image or Image properties not defined");
		return false;
	}
	
	// Create a valid output file pointer
	FILE *OUT_FILE;
	OUT_FILE = fopen(ImagePath, "wb");
	if ( OUT_FILE == NULL ) 
	{
		fprintf(stderr, "Error Opening File for Writing");
		return false;
	}

	// Create and populate RGB buffers
	int i;
	char *Rbuf = new char[Height*Width]; 
	char *Gbuf = new char[Height*Width]; 
	char *Bbuf = new char[Height*Width]; 


	for (i = 0; i < Height*Width; i++)
	{
		Bbuf[i] = Data[3*i];
		Gbuf[i] = Data[3*i+1];
		Rbuf[i] = Data[3*i+2];
	}

	
	// Write data to file
	for (i = 0; i < Width*Height; i ++)
	{
		fputc(Rbuf[i], OUT_FILE);
	}
	for (i = 0; i < Width*Height; i ++)
	{
		fputc(Gbuf[i], OUT_FILE);
	}
	for (i = 0; i < Width*Height; i ++)
	{
		fputc(Bbuf[i], OUT_FILE);
	}
	
	// Clean up and return
	delete Rbuf;
	delete Gbuf;
	delete Bbuf;
	fclose(OUT_FILE);

	return true;

}

void extractRectangularRegion(const char* inputImage, int imageWidth, int x, int y, int w, int h, char* outputRegion) {
	for (int row = y; row < y + h; ++row) {
		for (int col = x; col < x + w; ++col) {
			// Calculate the index in the input image
			int inputIndex = (row * imageWidth + col) * 3;

			// Calculate the index in the output region
			int outputIndex = ((row - y) * w + (col - x)) * 3;

			// Copy RGB values from the input image to the output region
			outputRegion[outputIndex] = inputImage[inputIndex];         // Blue
			outputRegion[outputIndex + 1] = inputImage[inputIndex + 1]; // Green
			outputRegion[outputIndex + 2] = inputImage[inputIndex + 2]; // Red
		}
	}
}

double chiSquaredDistance(const std::vector<double>& hist1, const std::vector<double>& hist2) {
	// Check if the input histograms have the same size (number of bins)
	if (hist1.size() != hist2.size()) {
		throw std::invalid_argument("Histograms must have the same number of bins.");
	}

	double chiSquared = 0.0;

	for (size_t i = 0; i < hist1.size(); ++i) {
		// Calculate the chi-squared contribution for each bin
		double diff = hist1[i] - hist2[i];
		double sum = hist1[i] + hist2[i];

		if (sum != 0) {
			chiSquared += (diff * diff) / sum;
		}
	}

	return chiSquared;
}

void getHue(char* img_data, int w, int h, double* hue_image, double* saturation_image, double* value_image) {

	for (int j = 0; j < h; j++) {
		for (int i = 0; i < w; i++) {
			int32_t b = (int32_t)img_data[(j * w + i) * 3] & 0xFF;
			int32_t g = (int32_t)img_data[(j * w + i) * 3 + 1] & 0xFF;
			int32_t r = (int32_t)img_data[(j * w + i) * 3 + 2] & 0xFF;
			BGRtoHSV(b, g, r, hue_image[j * w + i], saturation_image[j * w + i], value_image[j * w + i]);
		}
	}

}

void getYUV(char* img_data, int w, int h, double* Y_image, double* U_image, double* V_image) {
	for (int j = 0; j < h; j++) {
		for (int i = 0; i < w; i++) {
			int32_t b = (int32_t)img_data[(j * w + i) * 3] & 0xFF;
			int32_t g = (int32_t)img_data[(j * w + i) * 3 + 1] & 0xFF;
			int32_t r = (int32_t)img_data[(j * w + i) * 3 + 2] & 0xFF;
			BGRtoHSV(b, g, r, Y_image[j * w + i], U_image[j * w + i], V_image[j * w + i]);
		}
	}
}


struct Point {
	int x, y;
};



struct BoundingBox {
	Point topLeft, bottomRight;

	bool overlapsOrIsCloseTo(const BoundingBox& other) const {
		// Define the threshold distance
		const int threshold = 50;

		return !(other.topLeft.x - threshold > bottomRight.x ||
			other.bottomRight.x + threshold < topLeft.x ||
			other.topLeft.y - threshold > bottomRight.y ||
			other.bottomRight.y + threshold < topLeft.y);
	}

	BoundingBox combine(const BoundingBox& other) const {
		BoundingBox combined;
		combined.topLeft.x = std::min(topLeft.x, other.topLeft.x);
		combined.topLeft.y = std::min(topLeft.y, other.topLeft.y);
		combined.bottomRight.x = std::max(bottomRight.x, other.bottomRight.x);
		combined.bottomRight.y = std::max(bottomRight.y, other.bottomRight.y);
		return combined;
	}
};

// Check if a point is within the bounds of the grid
bool isValid(int x, int y) {
	return x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT;
}

void dfs(Point start, const std::vector<int>& array, std::vector<std::vector<bool>>& visited, BoundingBox& bbox) {
	std::stack<Point> s;
	s.push(start);

	// Possible moves: diagonally, below, above, left, right
	std::vector<Point> moves = { {1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {-1, -1}, {1, -1}, {-1, 1} };

	while (!s.empty()) {
		Point p = s.top();
		s.pop();

		for (const Point& move : moves) {
			int newX = p.x + move.x;
			int newY = p.y + move.y;

			if (isValid(newX, newY) && !visited[newY][newX] && array[newY * WIDTH + newX] == 1) {
				visited[newY][newX] = true;
				s.push({ newX, newY });

				// Update bounding box
				bbox.topLeft.x = std::min(bbox.topLeft.x, newX);
				bbox.topLeft.y = std::min(bbox.topLeft.y, newY);
				bbox.bottomRight.x = std::max(bbox.bottomRight.x, newX);
				bbox.bottomRight.y = std::max(bbox.bottomRight.y, newY);
			}
		}
	}
}


// Here is where you would place your code to modify an image
// eg Filtering, Transformation, Cropping, etc.

int area(int x1, int y1, int x2, int y2) {
	return abs(x1-x2) * abs(y1-y2);
}

bool MyImage::Modify(const std::vector<std::unique_ptr<MyImage>>& images)
{
	double* hue_img = new double[Height * Width];
	double* sat_img = new double[Height * Width];
	double* val_img = new double[Height * Width];
	getHue(Data, Width, Height, hue_img, sat_img, val_img);

	std::vector<double> img_histogram = createHistogram(hue_img, Height * Width, false);

	for (const auto& imagePtr : images) {
		char* boundingBoxOutlineColor = generateRandomColor();

		char* obj_data = imagePtr->getImageData();
		double* hue_obj = new double[Height * Width];
		double* sat_obj = new double[Height * Width];
		double* val_obj = new double[Height * Width];

		getHue(obj_data, Width, Height, hue_obj, sat_obj, val_obj);


		std::vector<double> obj_histogram = createHistogram(hue_obj, Height * Width, true);
		auto max_it = std::max_element(obj_histogram.begin(), obj_histogram.end());


		int max_value = *max_it;

		int index = std::distance(obj_histogram.begin(), max_it);

		std::vector<int> favorable_colors;
		for (int i = 0; i < 360; i++) {
			if (obj_histogram[i] > (0.1 * max_value))
				favorable_colors.push_back(i);
		}


		//std::cout << "IMG HISTOGRAM AFTER THIS \n\n\n";
		//for (int i = 0; i < 360; i++) {
		//	std::cout << i << ": " << val_img[i] << '\n';
		//}

		std::vector<int> cluster_arr(Height * Width, 0); // The given 1D array

		for (int y = 0; y < Height; y++) {
			for (int x = 0; x < Width; x++) {
				int idx = y * Width + x;
				//int32_t b = (int32_t)Data[idx * 3] & 0xFF;
				//int32_t g = (int32_t)Data[idx * 3 + 1] & 0xFF;
				//int32_t r = (int32_t)Data[idx * 3 + 2] & 0xFF;
				//double hue_value;
				//double sat_value;
				//double val_value;
				//BGRtoHSV(b, g, r, hue_value, sat_value, val_value);
				//if ((sat_value < 0.10 && val_value < 0.10) || (sat_value > 0.90 && val_value < 0.10) ||
				//	(sat_value > 0.90 && val_value > 0.90) || (sat_value < 0.10 && val_value>0.90)) {
				//	continue;
				//}

				if (abs(sat_img[idx] - sat_obj[idx]) > 0.3 || abs(val_img[idx] - val_obj[idx]) > 0.7) {
					continue;
				}

				if (hue_img[idx] > 45 && hue_img[idx] < 74) {
					if (sat_img[idx] < 0.90)
						continue;
				}

				bool found = false;
				for (int num : favorable_colors) {
					if (abs(hue_img[idx] - num) < 5) {
						found = true;
					}
				}
				if (found) {
					cluster_arr[idx] = 1;
					//std::cout << sat_img[idx];
		/*			Data[idx * 3] = 0;
					Data[idx * 3+2] = 0;
					Data[idx * 3+1] = 0;*/

				}

			}
		}

		// ... (initialize your array with values)
		std::vector<std::vector<bool>> visited(HEIGHT, std::vector<bool>(WIDTH, false));
		std::vector<BoundingBox> boxes;

		for (int y = 0; y < HEIGHT; y++) {
			for (int x = 0; x < WIDTH; x++) {
				if (!visited[y][x] && cluster_arr[y * WIDTH + x] == 1) {
					BoundingBox bbox = { {x, y}, {x, y} };
					dfs({ x, y }, cluster_arr, visited, bbox);
					if (area(bbox.topLeft.x, bbox.topLeft.y, bbox.bottomRight.x, bbox.bottomRight.y) > 3000)
						boxes.push_back(bbox);
				}
			}
		}

		bool combined;
		do {
			combined = false;

			for (size_t i = 0; i < boxes.size(); ++i) {
				for (size_t j = i + 1; j < boxes.size(); ++j) {
					if (boxes[i].overlapsOrIsCloseTo(boxes[j])) {
						boxes[i] = boxes[i].combine(boxes[j]);
						boxes.erase(boxes.begin() + j);
						combined = true;
						break; // Need to restart the inner loop because the vector was modified
					}
				}
				if (combined) {
					break; // Restart the outer loop
				}
			}
		} while (combined);

		char* obj_name = imagePtr->getImagePath();
		char* lastSlash = strrchr(obj_name, '\\');

		// If found, adjust the pointer to the character after the slash
		if (lastSlash) {
			obj_name = lastSlash + 1;
		}
		// Print bounding boxes for each cluster
		for (size_t i = 0; i < boxes.size(); i++) {
			drawBoundingBox(Data, boxes[i].topLeft.x, boxes[i].topLeft.y,
				abs(boxes[i].topLeft.x - boxes[i].bottomRight.x),
				abs(boxes[i].topLeft.y - boxes[i].bottomRight.y), boundingBoxOutlineColor);
				DrawTextOnImage(Data, Width, Height, boxes[i].topLeft.x, boxes[i].topLeft.y, obj_name, boundingBoxOutlineColor);
		}

	}

	return true;
}