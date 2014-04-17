//#include </usr/local/include/dc1394/dc1394.h>
// gcc -Wall  -I/usr/local/include/opencv/ -lcxcore -lcv -lhighgui fusion.cpp MTComm.cpp -o fusion

#include "ros/ros.h"
#include <sstream>
#include "LaserRead.h"
#include "LaserIbeo.h"

int main(int argc, char **argv)
{

    ros::init(argc, argv, "talker");

    ros::NodeHandle no;
    //	ros::Publisher chatter_pub = no.advertise<std_msgs::String>("chatter", 1000);
    ros::Rate loop_rate(12.5); // hz

    LaserIbeo alasca;

    if(alasca.open()==false){
        return 0;
    }
    while (ros::ok())
    {
        unsigned int numberMeasurements=alasca.getParam();

        LaserRead *lr;
        lr=new LaserRead[numberMeasurements];



        unsigned int distance=alasca.get(lr,numberMeasurements);



        delete [] lr;

        printf( " ---- Distance %d-----\n", distance);



        //std::stringstream ss;
        //ss << " ---- POSIÇÃO X =" << coord_x << " Y =" << coord_y << "DIST_STOP = "<< distancia_STOP;

        ROS_INFO("%d\n",distance);

        ros::spinOnce();

        loop_rate.sleep();

    }



    return 1;
}

