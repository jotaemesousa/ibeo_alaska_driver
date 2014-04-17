#ifndef ALASCA_XT_HT
#define  ALASCA_XT_HT

#ifdef __cplusplus
extern "C" {
#endif
#ifndef GENOM
#include <stdlib.h>
#include <stdio.h>
#endif

/*! Maximum number of points possible for Alasca XT */
#define AXT_MAX_SCAN_POINTS (8648)

/*! Default TCP/IP port */
#define AXT_DEFAULT_PORT (12000)

#define AXT_DEFAULT_IP "10.152.10.162"

/*! To be verified */
#define AXT_MAX_DISTANCE (300.00)

 
#define AXT_PT_STATUS_OK (0);
#define AXT_PT_STATUS_INVALID (1);
#define AXT_PT_STATUS_RAIN (2);
#define AXT_PT_STATUS_GROUND (3);
#define AXT_PT_STATUS_DIRT (4);	

/*! Header of a Scan Message */ 
typedef struct _AXT_SCAN_HDR_STR{
	unsigned int version; ///< Version Number of the protocol specification
	unsigned int scanner_type; ///< Alasca XT is type 3
	unsigned int ecu_id; 
	unsigned int time_stamp; /// Timestamp when the scanner has measured to 0
	double timestamp_sinc; // Timestamp of the hosting computer	
	int start_angle_int; ///< raw as got from scanner
	int end_angle_int; ///< raw as got from scanner
	float start_angle; ///< in scanner coordinates
	float end_angle; ///< in scanner coordinates
	unsigned int scan_counter;
	unsigned int num_points;
}AXT_SCAN_HDR_STR;

typedef struct _AXT_SCAN_POINT_STR{
	unsigned int scanner_id;
	unsigned int channel; ///< Scan Layer
	unsigned int sub_channel; ///< echo 1 or 2
	unsigned int point_status;  
	float x; ///< in scanner coordinates
	float y; ///< in scanner coordinates
	float z; ///< in scanner coordinates
	unsigned int echo_pulse_width; 
}AXT_SCAN_POINT_STR;

typedef struct _AXT_SCAN_STR{
	AXT_SCAN_HDR_STR header;
	AXT_SCAN_POINT_STR points[AXT_MAX_SCAN_POINTS];
}AXT_SCAN_STR;

#ifndef GENOM
	/*! \brief Parses the next message */
	int axt_parse(int sd, AXT_SCAN_STR *scan);
	
	/*! \brief Connects to the scanner */
	int axt_connect(char *host);
	
	/*! \brief Closes connection to the scanner */
	void axt_close(int sd);

	void axt_dump_scan(FILE* f, AXT_SCAN_STR *scan);

	void axt_dump_scan_line(FILE* f, AXT_SCAN_STR *scan);

	int axt_get_scan_from_file(FILE* f, AXT_SCAN_STR *scan);

#endif //GENOM

#ifdef __cplusplus
}
#endif


#endif
