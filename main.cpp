#include<iostream>
#include <fstream>
#include <string>
#include<ros/ros.h>
#include<rosbag/bag.h>
#include<rosbag/view.h>
#include<sensor_msgs/Image.h>
#include<std_msgs/Time.h>
#include<std_msgs/Header.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>

#include "Thirdparty/DLib/FileFunctions.h"


using namespace std;

int main(int argc, char **argv)
{
    ros::init(argc, argv, "BagFromImages");
    if(argc!=4) {
        cerr << "Usage: rosrun BagFromImages BagFromImages <path to image directory> <association.txt> <path to output bag>" << endl;
        return 0;
    }

    ros::start();

    // Vectors saving the filenames and times from association files.
    std::string line_str_temp;
    std::ifstream file(argv[2]);

    std::vector<double> image_time;
    std::vector<std::string> filenames;
    std::vector<std::string> rgbnames,depthnames; 

    if(file.is_open())
       while(file.good()){
          getline(file,line_str_temp);
	  const char* line_char_temp = line_str_temp.c_str();
	  char time_temp[18];
	  char rgb_name_temp[26];
	  char depth_name_temp[28];
	  char rgb_name_aug[40];
	  char depth_name_aug[40];

	  strncpy(time_temp,line_char_temp,17);
	  time_temp[17]='\0';
	  image_time.push_back(atof(time_temp));

	  strncpy(rgb_name_temp,line_char_temp+18+4,22);
	  rgb_name_temp[21]='\0';
	  strcpy(rgb_name_aug,argv[1]);
	  strcat(rgb_name_aug,rgb_name_temp);

	  rgbnames.push_back(rgb_name_aug);
	  //cout<<rgbnames.back()<<endl;

	  strncpy(depth_name_temp,line_char_temp+18+4+26+18+2,22);
	  depth_name_temp[21]='\0';
	  strcpy(depth_name_aug,"depth/");
	  strcat(depth_name_aug,depth_name_temp);
	  depthnames.push_back(depth_name_aug);

       }
    std::cout<<" Data length : "<<image_time.size()<<endl;
	
    // Output bag
    rosbag::Bag bag_out(argv[3],rosbag::bagmode::Write);

    ros::Time t(image_time[0]);
    cout<<t<<endl;
    for(size_t i=0;i<rgbnames.size()-3;i++) {
    	//ros::Time t(image_time[i]);

        if(!ros::ok())
            break;
	//std::cout<<rgbnames[i]<<std::endl;
	//cout<<depthnames[i]<<endl;
        cv::Mat im = cv::imread(rgbnames[i],CV_LOAD_IMAGE_COLOR);

        cv::Mat depth = cv::imread(depthnames[i],-1); // 16UC1
	cv::Mat depth2,color2;
	std::cout<< " depth : "<< depth.at<unsigned short>(300,200)/5000.0<<std::endl;
	//im.convertTo(color2,CV_8UC3);
	//cout<<depth2.size()<<endl;
	//cout<<depth2.type()<<endl;
  	//depth_o = cv::imread(depth_name,-1);
 	// depth.convertTo(depth,CV_64F);
 	// depth*=0.0002;
	//depth_32/=5000.0;
	//cout<<depth_32.size()<<endl;
	//cout<<depth_32.type()<<endl;
cv::Mat aa(4,4,CV_16UC1);
aa =cv::Scalar(3);
aa.convertTo(aa,CV_32FC1);

        cv_bridge::CvImage cvImage;
        cv_bridge::CvImage cvImage2;
        cvImage.image = im;
        cvImage.encoding = sensor_msgs::image_encodings::RGB8;
        cvImage.header.stamp = t;
	cvImage2.image = depth;
	cvImage2.encoding = sensor_msgs::image_encodings::TYPE_16UC1;
	cvImage2.header.stamp = t;
	

        bag_out.write("/camera/rgb/image_color",ros::Time(t),cvImage.toImageMsg());
        bag_out.write("/camera_depth_registered/sw_registered/image_rect",ros::Time(t),cvImage2.toImageMsg());
	ros::Duration d(image_time[i+1]-image_time[i]);	
t+=d;
        std::cout << i << " / " << rgbnames.size() << " , time : "<< t<< std::endl;
    }

    bag_out.close();

    ros::shutdown();

    return 0;
}
