/*
Copyright (C) 2006 Pedro Felzenszwalb

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
*/

// Modified by Benjamin Melone

#ifndef SEGMENT_IMAGE
#define SEGMENT_IMAGE

#include <cstdlib>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <vector>
#include "segment-graph.h"


inline uchar square(const uchar& x) {
	return x * x;
};

// dissimilarity measure between pixels
// SHOULD BE MADE INLINE LATER FOR PERFORNACE OPT
float diff(cv::Mat img, int x1, int y1, int x2, int y2) {
	cv::Vec3b bgrPixel = img.at<cv::Vec3b>(y1, x1);
	cv::Vec3b bgrPixel2 = img.at<cv::Vec3b>(y2, x2);
	return std::sqrt(square(bgrPixel[2] - bgrPixel2[2]) +
		square(bgrPixel[1] - bgrPixel2[1]) +
		square(bgrPixel[0] - bgrPixel2[0]));
}

/*
 * Segment an image
 *
 * Returns a color image representing the segmentation.
 *
 * img: image to segment.
 * sigma: to smooth the image.
 * c: constant for treshold function.
 * min_size: minimum component size (enforced by post-processing stage).
 * num_ccs: number of connected components in the segmentation.
 */
cv::Mat segment_image(cv::Mat& img, float sigma, float c, int min_size, int *num_ccs, universe*& u) {
	cv::Mat blurred;

	int size = (int)ceil(sigma * 4.0);
	size = size % 2 == 0 ? size + 1 : size; //ensure that size it odd
	cv::Size ksize(size, size);
	// You can try more different parameters
	cv::GaussianBlur(img, blurred, ksize, sigma, sigma, cv::BORDER_DEFAULT);
	//cv::imshow("blurred", blurred);

	int width = blurred.cols;
	int height = blurred.rows;
	// build graph
	edge *edges = new edge[width*height * 4];
	int num = 0;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			if (x < width - 1) {
				edges[num].a = y * width + x;
				edges[num].b = y * width + (x + 1);
				edges[num].w = diff(blurred, x, y, x + 1, y);
				num++;
			}

			if (y < height - 1) {
				edges[num].a = y * width + x;
				edges[num].b = (y + 1) * width + x;
				edges[num].w = diff(blurred, x, y, x, y + 1);
				num++;
			}

			if ((x < width - 1) && (y < height - 1)) {
				edges[num].a = y * width + x;
				edges[num].b = (y + 1) * width + (x + 1);
				edges[num].w = diff(blurred, x, y, x + 1, y + 1);
				num++;
			}

			if ((x < width - 1) && (y > 0)) {
				edges[num].a = y * width + x;
				edges[num].b = (y - 1) * width + (x + 1);
				edges[num].w = diff(blurred, x, y, x + 1, y - 1);
				num++;
			}
		}
	}

	// segment
	u = segment_graph(width*height, num, edges, c);

	// post process small components
	for (int i = 0; i < num; i++) {
		int a = u->find(edges[i].a);
		int b = u->find(edges[i].b);
		if ((a != b) && ((u->size(a) < min_size) || (u->size(b) < min_size)))
			u->join(a, b);
	}
	delete[] edges;
	*num_ccs = u->num_sets();

  cv::Mat output = cv::Mat::zeros(cv::Size(width, height), img.type());

  // pick random colors for each component
  std::vector<cv::Vec3b> colors;
  colors.resize(width*height);
  for (int i = 0; i < width*height; i++) {
	  colors[i][0] = rand() % 255;
	  colors[i][1] = rand() % 255;
	  colors[i][2] = rand() % 255;
  }
  
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int comp = u->find(y * width + x);
	  output.at<cv::Vec3b>(y, x) = colors[comp];
    }
  }

  return output;
}

#endif
