// Last update: 15-July (final current version)
//
#ifndef _FUSION_H
#define _FUSION_H

#include <ros/ros.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ctype.h>
//#include </usr/local/include/opencv/cv.h>
//#include </usr/local/include/opencv/highgui.h>
#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <sys/time.h>
#include <sys/ioctl.h>



#include "capser.c"
#include "libalascaxt.c"


typedef struct SCAN_IBEO{

    unsigned int channel; ///< Scan Layer
    unsigned int sub_channel; ///< echo 1 or 2
    unsigned int point_status;
    float x; ///< in scanner coordinates
    float y; ///< in scanner coordinates
} SCAN_IBEO;



//----------------------------------------------------------------------------------------
int Buffer_socket(int sd){

    //int ibuffer = 0;
    //int bufsize = 20480; //40960
    int bufsize = 22000;
    //char *buf = malloc(bufsize);
    unsigned char    buf[bufsize];
    int foundmagic=0;
    int i=0;
    int n=0;
    int k=0;

    n = recv(sd, buf, bufsize, MSG_PEEK);

    /*for (i=bufsize;i==3;i--)
 {
// magic word: (buf[0]==0xaf)&&(buf[1]==0xfe)&&(buf[2]==0xc0)&&(buf[3]==0xc0)

    if ((buf[i]==0xc0)&&(buf[i-1]==0xc0)&&(buf[i-2]==0xfe)&&(buf[i-3]==0xaf)) {
    foundmagic=1;
    //ibuffer = i;
    bufsize = i-3;
    exit(1);
     }
 }*/

    i=bufsize;
    while(!k) {

        if ((buf[i]==0xc0)&&(buf[i-1]==0xc0)&&(buf[i-2]==0xfe)&&(buf[i-3]==0xaf)) {
            foundmagic=1;
            bufsize = i-3;
        }

        if ( (i==3)||(foundmagic==1) ){
            //if ( i==3 ){
            k=1;
        }

        i = i - 1;

    }

    //printf("--> Buffer Size = %d <--", bufsize);
    n = recv(sd, buf, bufsize, 0);

    return bufsize;

}
//----------------------------------------------------------------------------------------

/*
CvCapture* initcam(){

 CvCapture* capture = 0;
 capture = cvCaptureFromCAM(CV_CAP_ANY); // capture from video device #0

cvSetCaptureProperty(capture, CV_CAP_PROP_FORMAT, 0);
cvSetCaptureProperty(capture, CV_CAP_PROP_FPS, 30);
//cvSetCaptureProperty(capture, CV_CAP_PROP_ISO_SPEED, 200);
//cvSetCaptureProperty(capture, CV_CAP_PROP_FORMAT, FORMAT_SVGA_NONCOMPRESSED_1);

cvInitSystem(0,NULL);//isto nao eh estritamente necessario, se nao for aqui explicitado, sera automaticamente chamado ao mostrar a primeira tela

   if( capture )
    {
    fprintf(stderr,"\n\n ====== The camera has been initialized properly !!! ====== \n\n");
    // return 1 to indicate successfull execution of the program
        //exit(1);

    }else{//if not loaded succefully
    fprintf(stderr,"\n\nFailure on load ! CHECK IF THE CAMERA IS CONECTED and if you are capturing from the right device 0? 1? 2?  \n\n");
    perror("PERROR:");
    exit(1);
    }
usleep(100);

return capture;

}



//CvCapture* CaptureFrame( CvCapture* capture, struct timeval first, struct timezone tzp){
void CaptureFrame( CvCapture* capture, struct timeval first, struct timezone tzp){


    IplImage *frame = 0;
    int hr,min,sec;
    char aux[50];


    hr=(first.tv_sec/3600 % 24 - tzp.tz_minuteswest/60) % 24;
    min=(first.tv_sec % 3600)/60;
    sec=first.tv_sec % 60;

     if( !cvGrabFrame( capture )){
        //if( !cvQueryFrame( capture )){
        printf("\n ************* Não capturou o frame!! *************\n");
        //break;
        }
            frame = cvRetrieveFrame( capture );

            // If the frame does not exist, quit the loop
            if( !frame ){
        printf("\n ************* Não existe frame! *************\n");
        //break;
        }



       //printf("\n frame ok");

       sprintf (aux, "images/im%02d_%02d_%02d_%04ld.bmp",hr,min,sec,first.tv_usec/1000);

       cvSaveImage(aux, frame);

//return capture;

}

*/

int ConnectLaser(const char *host){


    int sd;
    int port;



    struct  sockaddr_in sad; /* structure to hold an IP address     */
    struct  hostent  *ptrh;  /* pointer to a host table entry       */
    struct  protoent *ptrp;  /* pointer to a protocol table entry   */


    memset((char *)&sad,0,sizeof(sad)); /* clear sockaddr structure */
    sad.sin_family = AF_INET;         /* set family to Internet     */
    port = AXT_DEFAULT_PORT;
    sad.sin_port =  htons((u_short)port);

    ROS_INFO("Trying to connect to host %s on port %d", host,port);

    ptrh = gethostbyname(host);
    if ( ((char *)ptrh) == NULL ) {
        ROS_ERROR("Invalid host: %s", host);
        //fprintf(stderr,"Invalid host: %s\n", host);
        ROS_BREAK();
        //exit(1);
    }
    memcpy(&sad.sin_addr, ptrh->h_addr, ptrh->h_length);

    /* Map TCP transport protocol name to protocol number. */
    if ( ((ptrp = getprotobyname("tcp"))) == NULL) {
        ROS_ERROR("Cannot map \"tcp\" to protocol number");
        //fprintf(stderr, "Cannot map \"tcp\" to protocol number");
        ROS_BREAK();
        //exit(1);
    }

    /* Create a socket. */
    sd = socket(AF_INET, SOCK_STREAM, ptrp->p_proto);
    if (sd < 0) {
        ROS_ERROR("Socket creation failed");
        //fprintf(stderr, "Socket creation failed\n");
        ROS_BREAK();
        //exit(1);
    }

    /* Connect the socket to the specified server. */
    if (connect(sd, (struct sockaddr *)&sad, sizeof(sad)) < 0) {
        ROS_ERROR("Connect failed");
        //fprintf(stderr,"Connect failed\n");
        ROS_BREAK();
        //exit(1);
    }

    ROS_INFO("Laser connection OK");
    //printf("Laser connection OK\n");

    return sd;

}




int initport(int fd) {
    struct termios options;
    // Get the current options for the port...
    tcgetattr(fd, &options);
    // Set the baud rates to 19200...
    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);
    // Enable the receiver and set local mode...
    options.c_cflag |= (CLOCAL | CREAD);

    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    // Set the new options for the port...
    tcsetattr(fd, TCSANOW, &options);
    return 1;
}

int initportBat(int fd) {
    struct termios options;
    // Get the current options for the port...
    tcgetattr(fd, &options);
    // Set the baud rates to 19200...
    cfsetispeed(&options, B57600);
    cfsetospeed(&options, B57600);
    // Enable the receiver and set local mode...
    options.c_cflag |= (CLOCAL | CREAD);

    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    // Set the new options for the port...
    tcsetattr(fd, TCSANOW, &options);
    return 1;
}



int initGPS() {


    int fdx=0;
    unsigned char sResult[254];
    int nchar=0;


    fdx = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);

    if ( fdx==-1) {
        perror("open_port: Unable to open device ttyUSB - ");
        //return 1;
        exit(1);
    } else {
        fcntl(fdx, F_SETFL, 0);
    }



    printf("baud=%d\n", getbaud(fdx));
    initport(fdx);
    printf("baud=%d\n", getbaud(fdx));

    //char sCmd[254]="dm,,/msg/nmea/{GGA,ZDA,GSV,GLL}";

    char sCmd[254]="dm";

    if (!writeport(fdx, sCmd)) {
        printf("write failed\n");
        close(fdx);
        return 1;
    }

    printf("written:%s\n", sCmd);


    char sCmd1[254]="em,,/msg/nmea/{GGA}:0";

    if (!writeport(fdx, sCmd1)) {
        printf("write failed\n");
        close(fdx);
        return 1;
    }

    printf("written:%s\n", sCmd1);

    fcntl(fdx, F_SETFL, FNDELAY); // don't block serial read
    nchar=read(fdx, sResult, 254);
    //printf("ESVAZIAR GPS, ID; %d\n", nchar);
    while(nchar>0){
        nchar=read(fdx, sResult, 254);
        printf("ESVAZIAR GPS, ID; %d\n", nchar);

    }

    return (fdx);
}



int initBat() {

    int fd;
    unsigned char sResult[254];

    fd = open("/dev/ttyUSB1", O_RDWR | O_NOCTTY | O_NDELAY);
    if ( fd==-1) {
        perror("open_port: Unable to open device ttyUSB - ");
        return 1;
    } else {
        fcntl(fd, F_SETFL, 0);
    }

    printf("baud=%d\n", getbaud(fd));
    initportBat(fd);
    printf("baud=%d\n", getbaud(fd));
    //	char sCmd[254]="dm,,/msg/nmea/{GGA,ZDA,GSV,GLL}";
    //	char sCmd[254]="em,,/msg/nmea/{GGA}:0";
    /*	if (!writeport(fd, sCmd)) {
        printf("write failed\n");
        close(fd);
        return 1;
    }
    printf("written:%s\n", sCmd);
*/

    fcntl(fd, F_SETFL, FNDELAY); // don't block serial read
    while((read(fd, sResult, 254))>0);



    return fd;
}




int _kbhit() {
    static const int STDIN = 0;
    static bool initialized = false;

    if (!initialized) {
        // Use termios to turn off line buffering
        termios term;
        tcgetattr(STDIN, &term);
        term.c_lflag &= ~ICANON;
        tcsetattr(STDIN, TCSANOW, &term);
        setbuf(stdin, NULL);
        initialized = true;
    }

    int bytesWaiting;
    ioctl(STDIN, FIONREAD, &bytesWaiting);
    return bytesWaiting;
}


int getch_linux()
{
    int ch;
    struct termios oldt;
    struct termios newt;
    tcgetattr(STDIN_FILENO, &oldt); /*store old settings*/
    newt = oldt; /* copy old settings to new settings */
    newt.c_lflag &= ~(ICANON | ECHO); /* change settings*/
    tcsetattr(STDIN_FILENO, TCSANOW, &newt); /*apply the new settings immediatly */
    ch = getchar(); /* standard getchar call */
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); /* reapply the old settings */
    return ch; /* return received char */
}

#endif

