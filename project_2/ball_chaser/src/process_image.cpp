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
    int white_pixel_ht = -1;
    int white_pixel_stp = -1;
    float lin_x = 0.0;
    float ang_z = 0.0;

    for (int i = 0; i< img.height * img.step; i++)
    {
      if(img.data[i] == 255)
      {
        white_pixel_ht = i/img.step;
        white_pixel_stp = i % img.step;
        break;
      }
    }

    if (white_pixel_stp!= -1)
    {
      lin_x = 0.25;
    }
    else if (white_pixel_stp <= img.step * 0.3 && white_pixel_stp >=0)
    {
      ang_z = 0.25;
    }
    else if (white_pixel_stp <= img.step * 1 && white_pixel_stp > img.step * 0.7)
    {
      ang_z = 0.25;
    }
    
    
    drive_robot(lin_x, ang_z);
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
