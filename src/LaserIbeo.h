//
//
//  @ Project : Untitled
//  @ File Name : LaserIbeo.h
//  @ Date : 01-03-2012
//  @ Author : Luis Garrote, Joao Sousa
//
//


#ifndef _LASERIBEO_H
#define _LASERIBEO_H
#include <iostream>

#include "LaserRead.h"
//#include "ISRobotCarMessages/laser.h"
//#include "ISRobotCarMessages/LaserPoint.h"

class LaserIbeo
{

    int sd;
    std::string IP;
    unsigned char    buf[4];

    unsigned int version; ///< Version Number of the protocol specification
    unsigned int scanner_type; ///< Alasca XT is type 3
    unsigned int ecu_id;
    unsigned int time_stamp; /// Timestamp when the scanner has measured to 0
    int start_angle_int; ///< raw as got from scanner
    int end_angle_int; ///< raw as got from scanner
    float start_angle; ///< in scanner coordinates
    float end_angle; ///< in scanner coordinates
    unsigned int scan_counter;
    unsigned int num_points;
    double distancia_STOP;
    double distancia_STOP_actual;
    unsigned int scanner_id;
    unsigned int echo_pulse_width;
    int lixoz;
    int n;


    int coord_x;
    int coord_y;

public:

    LaserIbeo(std::string ip);
    LaserIbeo();
    ~LaserIbeo();

    bool open();
    bool isOpen();
    bool Close();
    unsigned int getParam();
    double get(LaserRead scan[],/*ISRobotCarMessages::laser *laserstructure*/unsigned int num); // falta alterar a mensagem

};

#endif  //_LASERIBEO_H
