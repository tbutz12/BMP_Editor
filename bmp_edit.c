//
//  main.c
//  ImageInversion
//
//  Created by tristin butz on 2/8/19.
//  Copyright © 2019 tristin butz. All rights reserved.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#pragma pack(1)
struct {
    char headerType[2];
    int size;
    short int reserved, reserved2;
    int offset;
}header;
struct {
    int size;
    int width;
    int height;
    short int colorPlanes;
    short int bitsPerPixel;
    int compression;
    int imageSize;
    int horizontalRes;
    int verticalRes;
    int colorsInPalette;
    int importantColors;
}DIBheader;
struct {
    unsigned char blue;
    unsigned char green;
    unsigned char red;
}pixel;


int main(int argc, char **argv) {
    if(argc < 3) {
       printf("Error, too few arguments\n");
       return 1;
    }
    FILE *myFile;
    int x, y;
    char *invert = "-invert";
    char *grayscale = "-grayscale";
    myFile = fopen(argv[2], "rb+");
    if(myFile == NULL){
        printf("No file to load\n");
        return(1);
    }
    fread(&header, sizeof(header), 1 , myFile);
    if(header.headerType[0] != 'B' || header.headerType[1] != 'M'){
        printf("We do not support the file format\n");
        exit(1);
    }
    fread(&DIBheader, sizeof(DIBheader), 1, myFile);
    fseek(myFile , header.offset, SEEK_SET);
        if(DIBheader.size != 40){
      printf("We do not support the file format\n");
       exit(1);
    }
    if(DIBheader.bitsPerPixel != 24){
       printf("We do not support the file format\n");
       exit(1);
   }
    
    if(strcmp(argv[1], invert) == 0) {
        int padding = 3 - ((DIBheader.width * 3 - 1) % 4);
        for (x = 0; x < DIBheader.height; x++) {
            for (y = 0; y < DIBheader.width; y++) {
                fread(&pixel, 3, 1, myFile);
                pixel.red = ~pixel.red;
                pixel.green = ~pixel.green;
                pixel.blue = ~pixel.blue;
                fseek(myFile, -3, SEEK_CUR);
                fwrite(&pixel, 3, 1, myFile);
            }
                fseek(myFile, padding, SEEK_CUR);
      }
    }


    else if(strcmp(argv[1], grayscale) == 0) {
        int padding = 3 - ((DIBheader.width * 3 - 1) % 4);
        for (x = 0; x < DIBheader.height; x++) {
            for (y = 0; y < DIBheader.width; y++) {
                fread(&pixel, 3, 1, myFile);
                float y = (0.2126*(float)pixel.red/255) + (0.7152*(float)pixel.green/255) + (0.0722*(float)pixel.blue/255);
                if(y <= 0.0031308){
                    pixel.blue = pixel.green = pixel.red = ((y*12.92)*255);
                }
                else if(y > 0.0031308){
                    pixel.red = pixel.blue = pixel.green = (((pow(y,1.0/2.4)*1.055) - 0.055)*255);
                }
                fseek(myFile, -3, SEEK_CUR);
                fwrite(&pixel, 3, 1, myFile);
            }
                fseek(myFile, padding , SEEK_CUR);
      }
    }



    fclose(myFile);
    return 0;
}

