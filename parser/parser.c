#include <stdio.h>
#include "parser.h"

int main(){
	//input and output files
	FILE *ifp;
	FILE *ofp;
	FILE *rfp;
	ifp = fopen("../output00.txt", "r");
	ofp = fopen("../charStringOutput00.txt","w");
	rfp = fopen("../rssiDump.txt","w");

	//raw character string to hold parsed text from java Listen output
	char cdata[65536];
	//array to hold all 400 possible edge combinations, and associated information
	rssi rdata[20][20];
	
	//make sure input file exists
	if(ifp == NULL){
		printf("No Input File Found.\n");
		return 0;
	}

	//remove whitespace and leading bytes
	int size = rem_whitespace(ifp, cdata);
	size = rem_buffer(cdata);


	//initalize the 400 element array
	init_rdata(rdata);

	//extract rssi data from cdata and dump into rdata
	cdata_to_rdata(cdata, rdata);
	//change rssi decibel values to corresponding distance values
	decibel_to_distance(rdata);
	//print all avg rssi values from edges that hold rssi values
	printf("\n");
	for(int i = 0; i < 20; ++i){
		for(int j = 0; j < 20; ++j){
			if(rdata[i][j].set == 1){
				printf("R = %2d, ", rdata[i][j].rmote);
				printf("S = %2d, ", rdata[i][j].smote);
				printf("Avg = %0.1f\n", rdata[i][j].avgrssi);
			}
		}
	}
	//dump all rssi data to output file
	for(int i = 0; i < 20; ++i){
		for(int j = 0; j < 20; ++j){
			if(rdata[i][j].set == 1){
				fprintf(rfp, "R = %2d, ", rdata[i][j].rmote);
				fprintf(rfp, "S = %2d, ", rdata[i][j].smote);
				fprintf(rfp, "Avg = %0.1f\n", rdata[i][j].avgrssi);
				for(int k = 0; k < rdata[i][j].counter; ++k){
					fprintf(rfp, "\t%2d", rdata[i][j].rvals[k]);
					if((k % 5 == 4) && (k < rdata[i][j].counter - 1)){
						fprintf(rfp, "\n");
					}
				}
				fprintf(rfp, "\n");
			}
		}
	}

	//data for mapping algorithm
	for(int i = 0; i < 20; ++i){
		for(int j = 0; j < 20; ++j){
			if(rdata[i][j].set == 1){
				fprintf(ofp, "%2d, ", rdata[i][j].rmote);
				fprintf(ofp, "%2d, ", rdata[i][j].smote);
				fprintf(ofp, "%2d\n", rdata[i][j].dist);
			}
		}
	}
	fprintf(ofp, "$", rdata[i][j].dist);


	fclose(ifp);
	fclose(ofp);
	fclose(rfp);

	return 0;
}

int rem_whitespace(FILE* ifp, char* cdata)
{
	char c;
	int i = 0;

	while(!feof(ifp)){
		c = getc(ifp);
		if((c != '\n') && (c != ' ')){
			cdata[i] = c;
			++i;
		}
	}
	cdata[++i] = EOF;
	
	return i;
}

int rem_buffer(char* cdata)
{
	int n = 0;
	int m = 0;

	while(cdata[n] != EOF){
		for(int i = 0; i < 16; ++i){
			++n;
		}
		for(int i = 0; i < 8; ++i){
			cdata[m] = cdata[n];
			++n;
			++m;
		}
	}

	cdata[m++] = EOF;
		
	return m;

}

void init_rdata(rssi rdata[20][20])
{
	for(int i = 0; i < 20; ++i){
		for(int j = 0; j < 20; ++j){
			for(int k = 0; k < 256; ++k){
				rdata[i][j].rvals[k] = 4096;
			}
			rdata[i][j].set = 0;
			rdata[i][j].counter = 0;
			rdata[i][j].avgrssi = 0;
			rdata[i][j].smote = i;
			rdata[i][j].rmote = j;
		}
	}

	return;
}

void cdata_to_rdata(char* cdata, rssi rdata[20][20])
{
	int n = 0;
	int rssi = 0;
	int hex = 0;
	int r = 0;
	int s = 0;
	while(cdata[n] != EOF){
		for(int i = 0; i < 4; ++i){
			if(('A' <= cdata[n]) && (cdata[n] <= 'F')){
				hex = (cdata[n] - 'A') + 10;
			}
			else{
				hex = cdata[n] - '0';
			}
			rssi = 16*rssi + hex;
			++n;
		}
		for(int i = 0; i < 2; ++i){
			if(('A' <= cdata[n]) && (cdata[n] <= 'F')){
				hex = (cdata[n] - 'A') + 10;
			}
			else{
				hex = cdata[n] - '0';
			}
			r = 16*r + hex;
			++n;
		}
		for(int i = 0; i < 2; ++i){
			if(('A' <= cdata[n]) && (cdata[n] <= 'F')){
				hex = (cdata[n] - 'A') + 10;
			}
			else{
				hex = cdata[n] - '0';
			}
			s = 16*s + hex;
			++n;
		}
		rdata[s][r].smote = s;
		rdata[s][r].rmote = r;
		if(rssi >= 32768){
			rssi = rssi - 65536;
		}
		rdata[s][r].rvals[rdata[s][r].counter] = rssi;
		++rdata[s][r].counter;
		rdata[s][r].set = 1;
		rssi = 0;
		s = 0;
		r = 0;
	}

	int total = 0;
	for(int i = 0; i < 20; ++i){
		for(int j = 0; j < 20; ++j){
			if(rdata[i][j].set == 1){
				for(int k = 0; k < rdata[i][j].counter; ++k){
					total = rdata[i][j].rvals[k] + total;
				}
			}
			rdata[i][j].avgrssi = (float) total/ (float) rdata[i][j].counter;
			total = 0;
		}
	}
	return;
}

void decibel_to_distance(rssi rdata[20][20])
{
	int rssiDecibel;
	for(int i = 0; i < 20; ++i){
		for(int j = 0; j < 20; ++j){
			rssiDecibel = (int) (rdata[i][j].avgrssi + 0.5);
			switch(rssiDecibel){
				case 42:
					rdata[i][j].dist = 1;
					break;
				case 34:
					rdata[i][j].dist = 2;
					break;
				case 28:
					rdata[i][j].dist = 3;
					break;
				case 24:
					rdata[i][j].dist = 4;
					break;
				case 18:
					rdata[i][j].dist = 5;
					break;
				case 14:
					rdata[i][j].dist = 7;
					break;
				case 8:
					rdata[i][j].dist = 8;
					break;
				case 3:
					rdata[i][j].dist = 9;
					break;
				case 1:
					rdata[i][j].dist = 10;
					break;
				case -1:
					rdata[i][j].dist = 11;
					break;
				case -5:
					rdata[i][j].dist = 12;
					break;
				case -4:
					rdata[i][j].dist = 13;
					break;
				case -8:
					rdata[i][j].dist = 14;
					break;
				case -13:
					rdata[i][j].dist = 15;
					break;
				case -11:
					rdata[i][j].dist = 16;
					break;
				case -12:
					rdata[i][j].dist = 17;
					break;
				default:
					rdata[i][j].dist = 256;
					break;
			}
		}
	}
	return;
}
