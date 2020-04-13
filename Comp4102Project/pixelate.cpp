#include <iostream>
#include <opencv2/opencv.hpp>
#include <cmath>
#include <math.h>
#include <unordered_set>

enum QuantizeColor{BIT15, PALETTE, KMEANS};

/*
	restrict_color_15bit takes a 24bit image and outputs a 15bit version of that image.
	By Ryan
	@param cv::Mat& input image
	@return cv::Mat output image
*/
cv::Mat restrict_color_15bit(cv::Mat& im1)

{
	// Converts the image from 24 bit RGB to b bit BGR back to 24 bit.
	// Colors not in the palette are lost in the conversion and rounded
	// to the nearest 15 bit color: https://wiki.superfamicom.org/palettes
	int width = im1.cols;
	int height = im1.rows;

	cv::Mat out = cv::Mat::zeros(cv::Size(width, height), im1.type());
	int p = 8;
	int bs = 1024;
	int bss = 32;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{

			int R = (int)floor(im1.at<cv::Vec3b>(i, j)[0] / p);
			int G = (int)floor(im1.at<cv::Vec3b>(i, j)[1] / p);
			int B = (int)floor(im1.at<cv::Vec3b>(i, j)[2] / p);

			int val = B * bs + G * bss + R;

			R = ((val) % bss) * p;
			G = ((val / bss) % bss) * p;
			B = ((val / bs) % bss) * p;

			out.at<cv::Vec3b>(i, j)[0] = R;
			out.at<cv::Vec3b>(i, j)[1] = G;
			out.at<cv::Vec3b>(i, j)[2] = B;
		}
	}
	return out;
}

/*
	restrict_color_palette takes an image and a color palette, and outputs an image with only the colors in the palette.
	Pixels closest to a color in the palette will be replaced by that color.
	By Joseph
	@param cv::Mat& input image
	@param std::vector<cv::Vec3b> color palette
	@return cv::Mat output image
*/
cv::Mat restrict_color_palette(cv::Mat& im1, std::vector<cv::Vec3b> palette) {
	int width = im1.cols;
	int height = im1.rows;

	cv::Mat out = cv::Mat::zeros(cv::Size(width, height), im1.type());
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			float dist = 1000000;
			cv::Vec3b chosenColor = cv::Vec3b(0, 0, 0);
			cv::Vec3b pixelColor= im1.at<cv::Vec3b>(j, i);
			for (cv::Vec3b paletteColor : palette) {
				float newdist = sqrt(pow((paletteColor[0]-pixelColor[0]),2.0)+ pow((paletteColor[1] - pixelColor[1]), 2.0)+ pow((paletteColor[2] - pixelColor[2]), 2.0));
				if (newdist<dist) {
					dist = newdist;
					chosenColor = paletteColor;
				}
			}
			out.at<cv::Vec3b>(j, i) = chosenColor;

		}
	}
	printf("rows: %d\n", im1.rows);
	return out;
}

/*
	restrict_color_kMeans color quantizes an image using kmeans selection.
	By Joseph
	@param cv::Mat& input image
	@param int ksplit k^2 number of quantized colors in the image
	@return cv::Mat output image
*/
cv::Mat restrict_color_kMeans(cv::Mat& im1, int ksplits) {
	cv::Mat data,out;
	im1.convertTo(data, CV_32F);
	data = data.reshape(1, data.total());

	cv::Mat labels, centers;
	cv::kmeans(data, ksplits, labels, cv::TermCriteria(CV_TERMCRIT_ITER, 10, 1.0), 3, cv::KMEANS_PP_CENTERS, centers);

	centers = centers.reshape(3, centers.rows);
	data = data.reshape(3, data.rows);

	cv::Vec3f *p = data.ptr<cv::Vec3f>();
	for (size_t i = 0; i < data.rows; i++) {
		int center_id = labels.at<int>(i);
		p[i] = centers.at<cv::Vec3f>(center_id);
	}

	out = data.reshape(3, im1.rows);
	out.convertTo(out, CV_8U);
	return out;
}


/*
	Pixelate takes an image and returns a small, quantized colored image with emphasized high frequency.
	By Ryan and Joseph
	@param cv::Mat& input image
	@param int factor to scale down by
	@return cv::Mat output image
*/
cv::Mat pixelate(cv::Mat& img, int factor, QuantizeColor opt = KMEANS) {
	int width = img.cols;
	int height = img.rows;

	//get contours
	cv::Mat edge,contourImg,s_contourImg;
	contourImg = cv::Mat::zeros(cv::Size(width, height), img.type());
	cv::Canny(img, edge, 100, 200);
	std::vector<std::vector<cv::Point> > contours,s_contour;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(edge, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	//draw on contours
	for (int i = 0; i < contours.size(); i++)
	{
		cv::drawContours(img, contours, i, cv::Scalar(0, 0, 0), 1, 8, hierarchy, 0, cv::Point());
	}

	// smooth image
	cv::Mat smoothed;
	float sigma = 0.5;
	int size = 7;
	cv::Size ksize(size, size);
	cv::GaussianBlur(img, smoothed, ksize, sigma, sigma, cv::BORDER_DEFAULT);

	// resize
	cv::Mat s_mat;
	int s_width = (int)floor(width / factor);
	int s_height = (int)floor(height / factor);
	cv::Size s_size(s_width, s_height);
	cv::resize(smoothed, s_mat, s_size, cv::INTER_LINEAR_EXACT);

	
	// restrict colors
	//cv::Mat out = restrict_color_palette(s_mat);
	std::vector<cv::Vec3b> colors{cv::Vec3b(0,0,255),cv::Vec3b(106,182,255),cv::Vec3b(0,106,255),cv::Vec3b(0,51,127),cv::Vec3b(0,216,255),cv::Vec3b(0,255,182),cv::Vec3b(0,255,76),cv::Vec3b(33,255,0),cv::Vec3b(144,255,0),cv::Vec3b(255,255,0),cv::Vec3b(255,148,0),cv::Vec3b(0,38,255),cv::Vec3b(255,0,72),cv::Vec3b(255,0,178),cv::Vec3b(220,0,255),cv::Vec3b(110,0,255),cv::Vec3b(0,0,0),cv::Vec3b(255,255,255),cv::Vec3b(64,64,64)};
	cv::Mat out;
	switch (opt) 
	{
		case BIT15: out = restrict_color_15bit(s_mat); break;
		case PALETTE:  out = restrict_color_palette(s_mat, colors); break;
		case KMEANS: out = restrict_color_kMeans(s_mat, 8); break;
	}

	return out;
}
/*
int main() {
	cv::Mat img = cv::imread("smokin.png");
	cv::Mat thickEdges = pixelate(img, 4,PALETTE);
	cv::imshow("image", thickEdges);
	cv::imwrite("pixelted.png", thickEdges);
	cv::waitKey(0);
}*/