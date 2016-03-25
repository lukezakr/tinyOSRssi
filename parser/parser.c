#include <stdio.h>
#include <string.h>
#include "parser.h"

int main(){
	//input and output files
	FILE *ifp;
	FILE *ofp;
	FILE *rfp;

	char outputf[256];
	char rssidumpf[256];
	char indexf[3];

	scanf("%s", indexf);
	strcat(outputf, "../output");
	strcat(outputf, indexf);
	strcat(outputf,".txt");

	strcat(rssidumpf, "../rssiDump");
	strcat(rssidumpf, indexf);
	strcat(rssidumpf,".txt");

	//ifp = fopen("../output%s.txt", "r");
	ifp = fopen(outputf, "r");
	ofp = fopen("../charStringOutput01.txt","w");
	//rfp = fopen("../rssiDump01.txt","w");
	rfp = fopen(rssidumpf,"w");

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

	//dump raw character data into output file
	for(int i = 0; i < size - 1; ++i){
		putc(cdata[i], ofp);
	}

	//initalize the 400 element array
	init_rdata(rdata);

	//extract rssi data from cdata and dump into rdata
	cdata_to_rdata(cdata, rdata);
	//print all avg rssi values from edges that hold rssi values
	printf("\n");
	for(int i = 0; i < 20; ++i){
		for(int j = 0; j < 20; ++j){
			if(rdata[i][j].set == 1){
				printf("R = %2d, ", rdata[i][j].rmote);
				printf("S = %2d, ", rdata[i][j].smote);
				printf("Avg = %f\n", rdata[i][j].avgrssi);
			}
		}
	}
	//dump all rssi data to output file
	for(int i = 0; i < 20; ++i){
		for(int j = 0; j < 20; ++j){
			if(rdata[i][j].set == 1){
				fprintf(rfp, "R = %2d, ", rdata[i][j].rmote);
				fprintf(rfp, "S = %2d, ", rdata[i][j].smote);
				fprintf(rfp, "Avg = %f\n", rdata[i][j].avgrssi);
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

	//print out average of two rssi values between given nodes i and j
	for(int i = 0; i < 20; ++i){
		for(int j = 0; j < i; ++j){
			if((rdata[i][j].set == 1) && (rdata[j][i].set == 1)){
				float average = ((float) rdata[i][j].avgrssi + (float) rdata[j][i].avgrssi) / 2;
				printf("Avg. of %2d %2d: %f\n", i, j, average);
			}
		}
	}

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
