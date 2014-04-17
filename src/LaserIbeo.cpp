//
//
//  @ Project : Untitled
//  @ File Name : LaserIbeo.cpp
//  @ Date : 01-03-2012
//  @ Author : Luis Garrote, Joao Sousa
//
//


#include "LaserIbeo.h"

#include "fusion.h"

LaserIbeo::LaserIbeo(std::string ip){

    IP=ip;

    version=0; ///< Version Number of the protocol specification
    scanner_type=0; ///< Alasca XT is type 3
    ecu_id=0;
    time_stamp=0; /// Timestamp when the scanner has measured to 0
    start_angle_int=0; ///< raw as got from scanner
    end_angle_int=0; ///< raw as got from scanner
    start_angle=0; ///< in scanner coordinates
    end_angle=0; ///< in scanner coordinates
    scan_counter=0;
    num_points=0;
    n=0;
    distancia_STOP = 10000;
}

LaserIbeo::LaserIbeo(){

    IP="10.152.10.247";
    distancia_STOP = 10000;
    version=0; ///< Version Number of the protocol specification
    scanner_type=0; ///< Alasca XT is type 3
    ecu_id=0;
    time_stamp=0; /// Timestamp when the scanner has measured to 0
    start_angle_int=0; ///< raw as got from scanner
    end_angle_int=0; ///< raw as got from scanner
    start_angle=0; ///< in scanner coordinates
    end_angle=0; ///< in scanner coordinates
    scan_counter=0;
    num_points=0;
    n=0;
    distancia_STOP = 10000;
}
LaserIbeo::~LaserIbeo(){

    IP="";

}


bool LaserIbeo::open() {
    sd=ConnectLaser(IP.c_str());
    if(sd!=-1){
        return true;
    }else{
        return false;
    }
}

bool LaserIbeo::isOpen() {


    if(sd<0){
        return false;

    }else{
        return true;
    }
}

bool LaserIbeo::Close( ) {
    closesocket(sd);
    return true;
}



unsigned int LaserIbeo::getParam(){
    int data_type=0;
    unsigned int size;
    while(data_type!=15){


        axt_skip(sd);
        n=recv(sd, buf, 4, 0);

        size = buf_to_uint32(buf);

        n+=recv(sd, buf, 4, 0);
        data_type = buf_to_uint32(buf);

        n+=recv(sd, buf, 4, 0);
        time_stamp = buf_to_uint32(buf);
    }


    //======================= Processing laser stream =======================


    n += recv(sd, buf, 4, 0);

    version = (unsigned int) buf[0];
    scanner_type = (unsigned int) buf[1];
    ecu_id = (unsigned int) buf[2];
    
    time_stamp = axt_get_uint32(sd, buf, &n);

    start_angle_int = axt_get_int16(sd, buf, &n);
    start_angle = axt_decode_angle(start_angle_int);
    end_angle_int = axt_get_int16(sd, buf, &n);
    end_angle = axt_decode_angle(end_angle_int);
    scan_counter = axt_get_uint16(sd, buf, &n);
    num_points = axt_get_uint16(sd, buf, &n);


    return num_points;

}

double LaserIbeo::get(LaserRead scan[],/*ISRobotCarMessages::laser *laserstructure,*/unsigned int num) {

    //ISRobotCarMessages::LaserPoint point;     //falta msg
    //laserstructure->number=num;
    //SCAN_IBEO scan[num_points];

    for (unsigned int i=0;i<num;i++)
    {
        scanner_id = axt_get_uint8(sd, buf, &n);
        scan[i].channel = axt_get_uint8(sd, buf, &n);
        scan[i].sub_channel = axt_get_uint8(sd, buf, &n);
        scan[i].point_status = axt_get_uint8(sd, buf, &n);
        scan[i].x = axt_get_int16(sd, buf, &n);
        scan[i].y = axt_get_int16(sd, buf, &n);
        lixoz = axt_get_int16(sd, buf, &n);
        echo_pulse_width = axt_get_uint16(sd, buf, &n);

//        point.x=scan[i].x;
//        point.y=scan[i].y;
//        point.channel=scan[i].channel;

//        laserstructure->points.push_back(point);

    }

    return distancia_STOP;

}

