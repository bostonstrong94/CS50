/****************************************************************************************
*whodunit.c
*
*Computer science 50
*
*
*copies a BMP piece by piece with a few modifications
*****************************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char* argv[])
{
    //ensure proper usage
    if (argc != 3)
    {
        printf("Usage: whodunit infile outfile\n");
        return 1;
    }

    //remember filenames
    char* infile = argv[1];
    char* outfile = argv[2];

    //open input file
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    //open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        printf("Could not create %s.\n", outfile);
        return 3;
    }

    //read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    //read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    //ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format. \n");
        return 4;
    }

    //write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    //write outfile's BITINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    //determine padding for scanlines
    int padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4 ;

    //iterate over infile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {

        //iterate over pizels in scanline
        for (int j = 0; j < bi.biWidth; j++)
        {
            //temporary storage
            RGBTRIPLE triple;

            //read RGB triple from infile
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

            //modify RGB triple to take out red
            if (triple.rgbtRed == 255 && triple.rgbtGreen == 0 && triple.rgbtBlue == 0)
            {
                triple.rgbtRed = 0;
            }
            if (triple.rgbtRed == 255 && triple.rgbtGreen == 255 && triple.rgbtBlue == 255)
            {
                triple.rgbtRed = 0;
                triple.rgbtGreen = 0;
                triple.rgbtBlue = 0;
            }

            //write RGB triple to outfile
            fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
        }

        //skip over padding, if any
        fseek(inptr, padding, SEEK_CUR);

        //write padding to outfile
        for (int k = 0; k < padding; k++)
        {
            fputc(0x00, outptr);
        }

    }

    //close infile
    fclose(inptr);

    //close outfile
    fclose(outptr);

    return 0;

}
