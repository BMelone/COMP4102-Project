//
// Created by ryan on 4/11/20.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include <cmath>

#ifndef COMP4102_PROJECT_PIXELATE_H
#define COMP4102_PROJECT_PIXELATE_H

cv::Mat restrict_color_palette(cv::Mat& im1)

{
    // Converts the image from 24 bit RGB to b bit BGR back to 24 bit.
    // Colors not in the palette are lost in the conversion and rounded
    // to the nearest 15 bit color: https://wiki.superfamicom.org/palettes
    int width = im1.cols;
    int height = im1.rows;

    cv::Mat out = cv::Mat::zeros(cv::Size(width, height), im1.type());
    int p =  8;
    int bs = 1024;
    int bss = 32;
    std::cout << p << std::endl;
    std::cout << bs << std::endl;
    std::cout << bss << std::endl;
    for(int i = 0; i < width; i++)
    {
        for(int j = 0; j < height; j++)
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

cv::Mat pixelate(cv::Mat& img, int factor){
    int width = img.cols;
    int height = img.rows;
    // smooth image
    cv::Mat smoothed;
    float sigma = 0.5;
    int size = 7;
    cv::Size ksize(size, size);
    cv::GaussianBlur(img, smoothed, ksize, sigma, sigma, cv::BORDER_DEFAULT);

    // resize
    cv::Mat s_mat;
    int s_width = (int)floor(width/factor);
    int s_height = (int)floor(height/factor);
    cv::Size s_size(s_width, s_height);
    cv::resize(smoothed, s_mat, s_size, cv::INTER_LINEAR_EXACT);
    // restrict colors
    cv::Mat out = restrict_color_palette(s_mat);
    return out;
}

#endif //COMP4102_PROJECT_PIXELATE_H
