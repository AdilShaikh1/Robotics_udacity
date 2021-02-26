#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "ball_chaser/DriveToTarget.h"

// ROS::Publisher motor commands;
ros::Publisher motor_command_publisher;

bool drive_request_callback(ball_chaser::DriveToTarget::Request& req, ball_chaser::DriveToTarget::Response& res)
{
    ROS_INFO("DriveToTarget Request receieved: linear_x = %1.2f, angular_z = %1.2f", (float)req.linear_x, (float)req.angular_z);
    
    geometry_msgs::Twist vel;

    vel.linear.x = req.linear_x;
    vel.angular.z = req.angular_z;

    motor_command_publisher.publish(vel);

    res.msg_feedback = "cmd sent: linear_x = " + std::to_string(vel.linear.x) + ", angular_z = " + std::to_string(vel.angular.z);

    ROS_INFO_STREAM(res.msg_feedback);

    return true;
}

int main(int argc, char** argv)
{
    // Initialize a ROS node
    ros::init(argc, argv, "drive_bot");

    // Create a ROS NodeHandle object
    ros::NodeHandle n;

    // Inform ROS master that we will be publishing a message of type geometry_msgs::Twist on the robot actuation topic with a publishing queue size of 10
    motor_command_publisher = n.advertise<geometry_msgs::Twist>("/cmd_vel", 10);

    ros::ServiceServer service = n.advertiseService("/ball_chaser/command_robot", drive_request_callback);
    ROS_INFO("Ready !!");    

    ros::spin();

    return 0;
}
