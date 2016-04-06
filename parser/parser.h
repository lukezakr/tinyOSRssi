#ifndef PARSER_H_
#define PARSER_H_

typedef struct {
	int smote; 		//sending mote id
	int rmote;		//receiving mote id
	int rvals[4096]; 	//rssi data values
	int counter;
	int set;		//1 = atleast 1 rssi value was taken, 0 = none were taken
	float avgrssi;	//average rssi value
}rssi;

//remove all whitespace in output file from java Listen and dump into char array
int rem_whitespace(FILE* ifp, char* data);
//remove the leading 8 bytes  from output, leaving only RSSI and two node IDs
int rem_buffer(char* data);
//initialize all rssi values to arbitrary number 4096 and edges set off
void init_rdata(rssi rdata[20][20]);
//extract data out rssi data from text string in cdata
void cdata_to_rdata(char* cdata, rssi rdata[20][20]);

#endif
