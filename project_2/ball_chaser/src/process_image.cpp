#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    ball_chaser::DriveToTarget serv;

    serv.request.linear_x = lin_x;
    serv.request.angular_z = ang_z;

    if (!client.call(serv))
    {
      ROS_ERROR("Error calling service !!");
    }
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;
    int white_pixel_stp = -1;
    auto pose = "no ball";

    for (int i = 0; i< img.height * img.step; i+=3)
    {
      int red = img.data[i], green = img.data[i+1], blue = img.data[i+2];
      white_pixel_stp = i % img.step;
     

      if(red == 255 && green == 255 && blue == 255)
      {
      
        if (white_pixel_stp <= img.step * 0.3 && white_pixel_stp >=0)
        {
          pose = "left";
        }
        else if (white_pixel_stp <= img.step * 0.7 && white_pixel_stp > img.step * 0.3)
        {
          pose = "mid";
        }
        else if (white_pixel_stp <= img.step * 1 && white_pixel_stp > img.step * 0.7)
        {
          pose = "right";
        }
        break;
      }
    }
    
    if (pose == "left")
    {
      drive_robot(0.25,0.25);
    }
    else if (pose == "right")
    {
      drive_robot(0.25,-0.25);
    }
    else if (pose == "mid")
    {
      drive_robot(0.25,0.25);
    }
    else
    {
      drive_robot(0.0,0.0);
    }
    
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
