

#ifndef unix
#define WIN32
#include <windows.h>
#include <winsock.h>
#else
#define closesocket close
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

#include <stdio.h>
#include <string.h>

#include "alascaxt.h"

/*! \brief returns four data bytes as an unsigned int32 
	@param *buf Buffer to read from 
*/
inline unsigned int buf_to_uint32(unsigned char *buf)
{
	return (((unsigned int) buf[0]) << 24) 
		+ (((unsigned int) buf[1]) << 16)
		+ (((unsigned int) buf[2]) << 8)  
		+ (((unsigned int) buf[3]));  
}

/*! \brief returns two data bytes as an unsigned int16  
	@param *buf Buffer to read from 
*/
inline unsigned short int buf_to_uint16(unsigned char *buf)
{
	return (((unsigned int) buf[0]) << 8)
		+ (((unsigned int) buf[1]));  
}

/*! \brief returns two data bytes as a signed int16  
	@param *buf Buffer to read from 
*/
inline short int buf_to_int16(unsigned char *buf)
{
	short int myint;
	myint= (((unsigned int) buf[0]) << 8)
		+ (((unsigned int) buf[1]));
	return myint;
}

/*! \brief returns a data bytes as an unsigned char  
	@param *buf Buffer to read from 
*/
inline unsigned char buf_to_uint8(unsigned char *buf)
{
	return buf[0];
}

/*! Gets an unsigned char as an unsigned int from the socket 
	@param sd Socket handle
	@param *buf Buffer to read from
	@param *report Number of bytes read 
*/
inline unsigned int axt_get_uint8(int sd, unsigned char *buf, int *report)
{
	*report += recv(sd, buf, 1, 0);
	return (unsigned int) buf_to_uint8(buf);
}

/*! Gets an int16 from the socket  
	@param sd Socket handle
	@param *buf Buffer to read from
	@param *report Number of bytes read 
*/
inline int axt_get_int16(int sd, unsigned char *buf, int *report)
{
	*report += recv(sd, buf, 2, 0);
	return (int) buf_to_int16(buf);  ///////////////////////////////7777777
}

/*! Gets an unsigned int16 from the socket 
	@param sd Socket handle
	@param *buf Buffer to read from
	@param *report Number of bytes read 
*/
inline unsigned int axt_get_uint16(int sd, unsigned char *buf, int *report)
{
	*report += recv(sd, buf, 2, 0);
	return  (unsigned int) buf_to_uint16(buf); ////////////////////////////////7777777777777
	
}

/*! Gets an unsigned int32 from the socket
	@param sd Socket handle  
	@param *buf Buffer to read from
	@param *report Number of bytes read 
*/
inline unsigned int axt_get_uint32(int sd, unsigned char *buf, int *report)
{
	*report += recv(sd, buf, 4, 0);
	return  buf_to_uint32(buf);
}

/*! Decodes the angle into scanner coordinates 
 @param angle Angle as comming from socket 
*/
inline float axt_decode_angle(int angle){
	//printf("angle %d becomes %f\n", angle, angle/10000.0);
	return angle/10000.0;
}

/*! Decodes distances into scanner coordinates 
	@param n Distance as comming from socket
*/
inline float axt_decode_distance(int n){
	if (n<-10000)
		return n*0.1 + 900.0;
	else if (n>10000)
		return n*0.1 - 900.0;
	else return n* 0.01;
}

/*! Skips the remaining message 
	@param sd Socket handle
	@returns number of bytes skipped 
*/
unsigned int axt_skip(int sd)
{
	unsigned char    buf[4];       /* buffer for data from the server     */
	int foundmagic=0;
	int i=0;
	int n=0;

	while (!foundmagic) {
		n = recv(sd, buf, 4, 0);
		// magic word: affe c0c0
		if ((buf[0]==0xaf)&&(buf[1]==0xfe)&&(buf[2]==0xc0)&&(buf[3]==0xc0)) 
			foundmagic=1;
		i+=n;
	}
	
	//printf("... AXT_Buffer size= %d ...", i-4);
	return i-4;
}

/*! Connects to the Alasca XT 
	@param *host Hostname or IP of scanner
	@param *sd Handle to socket
	@returns 1 if successfull
*/
int axt_connect(char *host)
{
	struct  sockaddr_in sad; /* structure to hold an IP address     */
	struct  hostent  *ptrh;  /* pointer to a host table entry       */
	struct  protoent *ptrp;  /* pointer to a protocol table entry   */
	
	int port;
	int sd;
	
	memset((char *)&sad,0,sizeof(sad)); /* clear sockaddr structure */
	sad.sin_family = AF_INET;         /* set family to Internet     */
	port = AXT_DEFAULT_PORT;
	sad.sin_port =  htons((u_short)port);

	printf("Trying to connect to host %s on port %d\n", host,sad.sin_port); 

	ptrh = gethostbyname(host);
	if ( ((char *)ptrh) == NULL ) {
		fprintf(stderr,"Invalid host: %s\n", host);
		exit(1);
	}
	memcpy(&sad.sin_addr, ptrh->h_addr, ptrh->h_length);
	
	/* Map TCP transport protocol name to protocol number. */
	if ( ((ptrp = getprotobyname("tcp"))) == NULL) {
		fprintf(stderr, "Cannot map \"tcp\" to protocol number");
		exit(1);
	}

	/* Create a socket. */
	sd = socket(AF_INET, SOCK_STREAM, ptrp->p_proto);
	if (sd < 0) {
		fprintf(stderr, "Socket creation failed\n");
		exit(1);
	}

	/* Connect the socket to the specified server. */
	if (connect(sd, (struct sockaddr *)&sad, sizeof(sad)) < 0) {
		fprintf(stderr,"Connect failed\n");
		exit(1);
	}
	return sd;
}
/*! Close the socket. 
	@param sd Handle to socket
*/
void axt_close(int sd)
{
	closesocket(sd);
}

/*! Dumps the scan 
	@param *f FILE_ptr to dump to
	@param *scan Scan to be dumped
*/
/*
void axt_dump_scan(FILE* f, AXT_SCAN_STR *scan)
{
	int i;
	fprintf(f, "# %d  %d %d %d %d %d %d %d \n"
					, scan->header.version
					, scan->header.scanner_type
					, scan->header.ecu_id
					, scan->header.time_stamp
					, scan->header.start_angle_int
					, scan->header.end_angle_int
					, scan->header.scan_counter
					, scan->header.num_points);

			for (i=0;i<scan->header.num_points;i++)
				{
					//if (scan->points[i].scanner_id == 67) 
					fprintf(f, "%d %d %d %d %f %f %f %d\n"
						,scan->points[i].scanner_id 
						,scan->points[i].channel 
						,scan->points[i].sub_channel 
						,scan->points[i].point_status 
						,scan->points[i].x 
						,scan->points[i].y 
						,scan->points[i].z 
						,scan->points[i].echo_pulse_width); 
				}
}
*/
/*! Dumps the scan 
	@param *f FILE_ptr to dump to
	@param *scan Scan to be dumped
*/
/*void axt_dump_scan_line(FILE* f, AXT_SCAN_STR *scan)
{
	int i;
	fprintf(f, "%d %d %d %d %d %d %d %d "
					, scan->header.version
					, scan->header.scanner_type
					, scan->header.ecu_id
					, scan->header.time_stamp
					, scan->header.start_angle_int
					, scan->header.end_angle_int
					, scan->header.scan_counter
					, scan->header.num_points);

			for (i=0;i<scan->header.num_points;i++)
				{
					//if (scan->points[i].scanner_id == 67)
					fprintf(f, "%d %d %d %d %f %f %f %d "
									,scan->points[i].scanner_id 
									,scan->points[i].channel 
									,scan->points[i].sub_channel 
									,scan->points[i].point_status 
									,scan->points[i].x 
									,scan->points[i].y 
									,scan->points[i].z 
									,scan->points[i].echo_pulse_width); 
				}
			fprintf(f, "\n");
}

int axt_get_scan_from_file(FILE* f, AXT_SCAN_STR *scan)
{
	int i;
	
	fscanf(f, "# %d  %d %d %d %d %d %d %d \n"
				 , &scan->header.version
				 , &scan->header.scanner_type
				 , &scan->header.ecu_id
				 , &scan->header.time_stamp
				 , &scan->header.start_angle_int
				 , &scan->header.end_angle_int
				 , &scan->header.scan_counter
				 , &scan->header.num_points);

			for (i=0;i<scan->header.num_points;i++)
				{
					fscanf(f, "%d %d %d %d %f %f %f %d\n"
								 ,&scan->points[i].scanner_id 
								 ,&scan->points[i].channel 
								 ,&scan->points[i].sub_channel 
								 ,&scan->points[i].point_status 
								 ,&scan->points[i].x 
								 ,&scan->points[i].y 
								 ,&scan->points[i].z 
								 ,&scan->points[i].echo_pulse_width); 
				}
			return 0;
}*/

int axt_handle_message(int type, int sd, AXT_SCAN_STR *scan, double timestamp_cur)
{
  unsigned char buf[4];
  int n=0;
 // int i=0;
  
 // int temp_x;
 // int temp_y;
 // int temp_z;
  
  if (type==15)	
    {
  //     printf("Got a scan "); 
      n += recv(sd, buf, 4, 0);
perror("recv");
printf("n=%d\n",n);
     // printf("%x \n", *buf);
      scan->header.version = (unsigned int) buf[0];
  //    scan->header.scanner_type = (unsigned int) buf[1];
  //    scan->header.ecu_id = (unsigned int) buf[2];
perror("recv2");
   //    printf("version %d scanner_type %d ecu_id %d\n" 
 //	     ,scan->header.version  
//	     ,scan->header.scanner_type 
 //	     ,scan->header.ecu_id); 
      
      scan->header.time_stamp = axt_get_uint32(sd, buf, &n);

       printf("ts: %d \n", scan->header.time_stamp); 
	
      scan->header.timestamp_sinc = timestamp_cur;	
	
      scan->header.start_angle_int = axt_get_int16(sd, buf, &n);
    //   printf("%2x%2x ", buf[0], buf[1]); 
    //   printf("start_angle_int: %x %d \n", scan->header.start_angle_int,scan->header.start_angle_int); 
      scan->header.start_angle = axt_decode_angle(scan->header.start_angle_int);
      scan->header.end_angle_int = axt_get_int16(sd, buf, &n);
      scan->header.end_angle = axt_decode_angle(scan->header.end_angle_int);
   //    printf("end_angle_int: %d \n", scan->header.end_angle_int); 
      scan->header.scan_counter = axt_get_uint16(sd, buf, &n);
      scan->header.num_points = axt_get_uint16(sd, buf, &n);
   //    printf("num_points: %d \n",scan->header.num_points);  
      
    /*  for (i=0;i<scan->header.num_points;i++)
	{
	  scan->points[i].scanner_id = axt_get_uint8(sd, buf, &n);
	  scan->points[i].channel = axt_get_uint8(sd, buf, &n);
	  scan->points[i].sub_channel = axt_get_uint8(sd, buf, &n);
	  scan->points[i].point_status = axt_get_uint8(sd, buf, &n);
	  // for correct reading of the alasca message 
	  temp_x = axt_get_int16(sd, buf, &n);
	  temp_y = axt_get_int16(sd, buf, &n);
	  temp_z = axt_get_int16(sd, buf, &n);
	  scan->points[i].x = axt_decode_distance(temp_x);
	  scan->points[i].y = axt_decode_distance(temp_y);
	  scan->points[i].z = axt_decode_distance(temp_z);
	  scan->points[i].echo_pulse_width = axt_get_uint16(sd, buf, &n);
	}
      axt_dump_scan(stdout, scan);*/
         }
  else
     printf("For now only messages of type 15 (scan) are parsed\n"); 
	
  return n;
}

/*! Parses the socket 
	@param sd Socket handle
	@param *scan new Scan
	@returns number of bytes read
*/

int axt_parse(int sd, AXT_SCAN_STR *scan)
{
  int n=0;
  unsigned char    buf[4];       /* buffer for data from the server     */
  
  unsigned int size;
//  unsigned int type;
  
	//printf("Looking for next Header, found after %d byte\n",axt_skip(sd)); 
  axt_skip(sd);  
  // got the header
  // take current time
  struct timeval ntv;
  gettimeofday(&ntv,  NULL); 		 
  double timestamp_cur = ntv.tv_sec + ntv.tv_usec/1000000.0;	 

  n=recv(sd, buf, 4, 0);
	  
  size = buf_to_uint32(buf);
   printf("Got message of size %d ", size); 
  
  n+=recv(sd, buf, 4, 0);
  unsigned int data_type = buf_to_uint32(buf);
   printf("of TYPE %d ", data_type); 
  
  n+=recv(sd, buf, 4, 0);
  unsigned int time_stamp = buf_to_uint32(buf);
   printf("at time %d\n",time_stamp);  
  
  n+=axt_handle_message(data_type, sd, scan, timestamp_cur);
//  printf("Found next header after %d byte\n", axt_skip(sd));
  
  return n;
}






