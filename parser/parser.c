#include <stdio.h>
#include <string.h>
#include "parser.h"

int main(){
	//input and output files
	FILE *ifp;
	FILE *rfp;

	ifp = fopen("../output001.txt", "r");
	rfp = fopen("../parsedOutput001.txt","w");

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

	//data for mapping algorithm
	for(int i = 0; i < 20; ++i){
		for(int j = 0; j < i; ++j){
			if(rdata[i][j].set == 1){
				if((rdata[i][j].set == 1) && (rdata[j][i].set == 1)){
					fprintf(rfp, "%02d,", rdata[i][j].rmote);
					fprintf(rfp, "%02d,", rdata[i][j].smote);
					float average = ((float) rdata[i][j].dist + (float) rdata[j][i].dist) / 2;
					fprintf(rfp, "%04.1f\n", average);
				}
			}
		}
	}

	fclose(ifp);
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
		for(int i = 0; i < 6; ++i){
			++n;
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
	int rD;
	for(int i = 0; i < 20; ++i){
		for(int j = 0; j < 20; ++j){
			rD = (int) (rdata[i][j].avgrssi + 0.5);
			if(rD >= 37)
				rdata[i][j].dist = 1;
			else if((rD >= 30) && (rD <= 36))
				rdata[i][j].dist = 2;
			else if((rD >= 26) && (rD <= 29))
				rdata[i][j].dist = 3;
			else if((rD >= 20) && (rD <= 25))
				rdata[i][j].dist = 4;
			else if((rD >= 18) && (rD <= 19))
				rdata[i][j].dist = 5;
			else if((rD >= 16) && (rD <= 17))
				rdata[i][j].dist = 6;
			else if((rD >= 10) && (rD <= 15))
				rdata[i][j].dist = 7;
			else if((rD >= 5) && (rD <= 9))
				rdata[i][j].dist = 8;
			else if((rD >= 2) && (rD <= 4))
				rdata[i][j].dist = 9;
			else if((rD >= 0) && (rD <= 1))
				rdata[i][j].dist = 10;
			else if((rD >= -3) && (rD <= -1))
				rdata[i][j].dist = 11;
			else if((rD >= -4) && (rD <= -4))
				rdata[i][j].dist = 12;
			else if((rD >= -7) && (rD <= -5))
				rdata[i][j].dist = 13;
			else if((rD >= -10) && (rD <= -8))
				rdata[i][j].dist = 14;
			else if((rD >= -11) && (rD <= -11))
				rdata[i][j].dist = 15;
			else if((rD >= -12) && (rD <= -12))
				rdata[i][j].dist = 16;
			else if((rD >= -13) && (rD <= -13))
				rdata[i][j].dist = 17;
			else
				rdata[i][j].dist = 20;
		}
	}
	return;
}
