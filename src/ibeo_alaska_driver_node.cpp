
#include <ros/ros.h>
#include "LaserIbeo.h"

int main(int argc, char *argv[])
{
    ros::init(argc, argv, "ibeo_alaska_driver_node");

    ROS_INFO("IBEO_ALASKA_DRIVER for ROS v0.1");

    LaserIbeo laser;

    if(!laser.open())
    {
        ROS_FATAL("IBEO_ALASKA_DRIVER -- Cannot establish a connection with the laser!");
        ROS_BREAK();
    }
    else
    {
        ROS_INFO("IBEO_ALASKA_DRIVER: Successfuly connected");
    }



}
