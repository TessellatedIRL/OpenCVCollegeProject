
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include<iostream>

double GetNormalizedHeight(double Y1, double Y2);
double GetNormalizedLength(double X1, double X2);

double ImageHeight = 0.0, ImageLength = 0.0;
double VerticalScaling=1.0, HorizontalScaling=1.0;

int main()
{
	// Required Variables
	double Length = 0.0, Height = 0.0, Radius = 0.0, Volume = 0.0, Area = 0.0, X1 = 0.0, X2 = 0.0, Y1 = 0.0, Y2 = 0.0;

	// Importing File with its Dimensions
	cv::Mat target = cv::imread("target.jpg");
	ImageHeight = target.size().height;
	ImageLength = target.size().width;

	// Intermediate Processing Windows
	cv::Mat MedianBlurred; cv::Mat DThreshed; cv::Mat SplitChannels[3]; cv::Mat WithRect;

	// Color Space Switch
	cv::cvtColor(target, target, cv::COLOR_BGR2HSV);

	// Splitting of three channels
	cv::split(target, SplitChannels);

	// Storing the three seperate channels in three windows
	cv::imshow("Hue", SplitChannels[0]);
	cv::imshow("Sat", SplitChannels[1]);
	cv::imshow("Val", SplitChannels[2]);
	cv::imshow("Output", target);

	// Performing Median Blur on the best available channel
	cv::medianBlur(SplitChannels[1], MedianBlurred, 11);
	cv::imshow("MedBlur", MedianBlurred);

	// Performing Double Thresholding on Blurred Image
	cv::inRange(MedianBlurred, cv::Scalar(35), cv::Scalar(255), DThreshed);
	cv::imshow("Dthresh", DThreshed);

	//Copying 
	DThreshed.copyTo(WithRect);

	// To store the objects detected
	std::vector<std::vector<cv::Point>> Countours;

	// Finding Contours
	cv::findContours(DThreshed, Countours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	// Stores the Rectangle Parameters for each object detection iteration
	cv::Rect BoundRect;

	// Finding objects
	for (size_t i = 0; i < Countours.size(); i++)
	{
		// Drawing Rectangles
		BoundRect = cv::boundingRect(Countours[i]);
		cv::rectangle(WithRect, BoundRect.tl(), BoundRect.br(), (255, 255, 255), 1);

		// Storing Required Parameters of the largest Rectangle(Pothole)
		if (BoundRect.area() > Area)
		{
			Area = BoundRect.area();
			Length = BoundRect.width;
			Height = BoundRect.height;
			X1 = BoundRect.tl().x;
			Y1 = BoundRect.tl().y;
			X2 = BoundRect.br().x;
			Y2 = BoundRect.br().y;
		}
	}

	//Displaying dimensions of the Rectangle before Scaling
	printf("\n \n Height: %f", Height);
	printf("\n Length: %f", Length);
	printf("\n \n");
	printf("\n X1: %f", X1);
	printf("\n X2: %f", X2);
	printf("\n Y1: %f", Y1);
	printf("\n Y2: %f", Y2);

	// Displaying the window with the Pothole outlined by the rectangle
	cv::imshow("Countours", WithRect);

	// Scaling the Rectangle
	Length = GetNormalizedLength(X1, X2);
	Height = GetNormalizedHeight(Y1, Y2);

	// Calculating the Volume
	Radius = Length > Height ? Length : Height;
	Radius = Radius / 2;
	Volume = 2.0 / 3.0 * 22.0 / 7.0 * Radius * Radius * Radius;

	// Displaying the Volume
	printf("\n \n Volume: %f", Volume);

	// Prevents the display windows from closing after execution
	cv::waitKey(0);
}

double GetNormalizedHeight(double Y1, double Y2)
{
	double Norm = 0.0, LowerRatio = 0.0, UpperRatio = 0.0;;

	LowerRatio = Y1 / ImageHeight;
	UpperRatio = Y2 / ImageHeight;

	Y1 = Y1 * LowerRatio * VerticalScaling;
	Y2 = Y2 * UpperRatio * VerticalScaling;
	Norm = Y2 - Y1;

	return Norm;
}
double GetNormalizedLength(double X1, double X2)
{
	double Norm = 0.0, LeftRatio = 0.0, RightRatio = 0.0;

	LeftRatio = abs((ImageLength / 2 - X1)) / (ImageLength / 2);
	RightRatio = abs((ImageLength / 2 - X2)) / (ImageLength / 2);
		
	X1 = X1 * LeftRatio * HorizontalScaling;
	X2 = X2 * RightRatio * HorizontalScaling;
	Norm = X2 - X1;

	return Norm;
}
