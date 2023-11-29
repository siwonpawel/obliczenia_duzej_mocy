#include <stdio.h>
#include <math.h>
#include <thread>
#include <chrono>
#include <iostream>
#include <random>

#include "omp.h"

/*
9600x9600
*/

const double CxMin = -2.5;
const double CxMax = 1.5;
const double CyMin = -2.0;
const double CyMax = 2.0;

const int iXmax = 9600;
const int iYmax = 9600;
unsigned char color[iYmax][iXmax][3];

double PixelWidth = (CxMax - CxMin) / iXmax;
double PixelHeight = (CyMax - CyMin) / iYmax;

const int MaxColorComponentValue = 255;
const int IterationMax = 200;

const double EscapeRadius = 2;
double ER2 = EscapeRadius * EscapeRadius;

void randomize_color(int rgb[3], int tid)
{
    srand((unsigned) tid + time(NULL));
    for (int i = 0; i < 3; i++)
    {
        rgb[i] = rand() % 256;
    }
}

using namespace std;

int main()
{
    FILE *fp;
    char *filename = "new1.ppm";
    char *comment = "# ";
    fp = fopen(filename, "wb");
    fprintf(fp, "P6\n %s\n %d\n %d\n %d\n", comment, iXmax, iYmax, MaxColorComponentValue);

    int iX, iY;
    double Cx, Cy;
    double Zx, Zy;
    double Zx2, Zy2;

    int Iteration;
    int tid;

    //omp_set_num_threads(12);

    double start = omp_get_wtime();
#pragma omp parallel private(tid)
    {
        tid = omp_get_thread_num();
        int rgb[3];
        randomize_color(rgb, tid);
#pragma omp for private(iY, iX, Iteration, Cx, Cy, Zx, Zy, Zx2, Zy2) schedule(dynamic, 1)
        for (iY = 0; iY < iYmax; iY++)
        {
            Cy = CyMin + iY * PixelHeight;
            if (fabs(Cy) < PixelHeight / 2)
                Cy = 0.0; /* Main antenna */
            for (iX = 0; iX < iXmax; iX++)
            {
                Cx = CxMin + iX * PixelWidth;
                /* initial value of orbit = critical point Z= 0 */
                Zx = 0.0;
                Zy = 0.0;
                Zx2 = Zx * Zx;
                Zy2 = Zy * Zy;
                /* */
                for (Iteration = 0; Iteration < IterationMax && ((Zx2 + Zy2) < ER2); Iteration++)
                {
                    Zy = 2 * Zx * Zy + Cy;
                    Zx = Zx2 - Zy2 + Cx;
                    Zx2 = Zx * Zx;
                    Zy2 = Zy * Zy;
                };
                /* compute  pixel color (24 bit = 3 bytes) */
                if (Iteration == IterationMax)
                { /*  interior of Mandelbrot set = black */
                    color[iY][iX][0] = 0;
                    color[iY][iX][1] = 0;
                    color[iY][iX][2] = 0;
                } else
                {                              /* exterior of Mandelbrot set = white */
                    color[iY][iX][0] = rgb[0]; /* Red*/
                    color[iY][iX][1] = rgb[1]; /* Green */
                    color[iY][iX][2] = rgb[2]; /* Blue */
                };
                /*write color to the file*/
            }
        }
    }

    double end = omp_get_wtime();

    double diff = end - start;

    cout << diff << " sec." << endl;

    fwrite(color, 1, 3 * iYmax * iXmax, fp);
    fclose(fp);
    return 0;
}