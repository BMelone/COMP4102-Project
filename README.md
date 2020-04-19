# Developing an Application to Convert Real Images into Cartoons using De-pixelization, Segmentation and Rescaling with Restricted Colour Palette

By: Benjamin Melone, Eimhin McDonald, Joseph Dooley, Ryan Kane

## Final Report

The final report is available in the file FinalWriteUp.pdf on github, or in the CuLearn submission

## Project Demonstration

The demo video is on this url: https://drive.google.com/file/d/1mNl5U3CbNynxZYRsVIoP0Zib-HPWKIHM/view?usp=sharing

## Project Presentation

The project presentation video was submitted in CuLearn

## Code Demo and Execution Instructions

The code can be run by following the instructions below.
These instructions will describe how to compile and run using Windows, Visual Studio 2017, and OpenCV 3.4.9.
You can run on Linux, but that will not be explained in these instructions.
Versions other than Visual Studio 2017 and OpenCV 3.4.9 are not guaranteed to work.

An example of how to run the code is in the file CodeDemo.mp4

To compile and run the code:
1. Clone the repository to your computer
2. Create a new empty Visual Studio 2017 project
3. Configure the new project so that it links to OpenCV 3.4.9
4. Import the header files and source files found in COMP4102Project\\ and COMP4102Project\\cartoon\\
5. Build the solution in Release and in x64
6. Open a command prompt at Comp4102Project\\x64\\Release
7. Run the application by typing Comp4102Project.exe
8. The help text will be displayed
9. As said in the help text, you need to provide an image file.
10. You can test by executing Comp4102Project.exe ..\\..\\obama.png
11. This will open a window with 3 sliders, which can be adjusted to find the perfect segmentation
12. Once you are satisfied with the segments, use the mouse to click the segments that make up the foreground
13. Once you have clicked them all, press spacebar. This will display to you the image with the background segments removed
14. Press spacebar again. This will display the pixelized image
15. Press spacebar again. This will display the cartoonized image

## Project Proposal

The original project proposal is available in the file COMP4102_Proposal.pdf on github.

