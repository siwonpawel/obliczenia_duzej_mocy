#include <iostream>
#include <stdio.h>
#include <math.h>
#include <thread>
#include <chrono>

#define THREADS 12

using namespace std;
using namespace chrono;

const int iXmax = 3400;
const int iYmax = 3400;

const double CxMin = -2.5;

const double CxMax = 1.5;
const double CyMin = -2.0;
const double CyMax = 2.0;

double PixelWidth = (CxMax - CxMin) / iXmax;
double PixelHeight = (CyMax - CyMin) / iYmax;

const int MaxColorComponentValue = 255;
const int IterationMax = 200;
const double EscapeRadius = 2;
const double ER2 = EscapeRadius * EscapeRadius;

int sumi[THREADS] = {0};

unsigned char color[iYmax][iXmax][3];

void calt(int tid) {
    double Cx, Cy;
    int Iteration;
    int iX, iY;
    double Zx, Zy;
    double Zx2, Zy2; /* Zx2=Zx*Zx;  Zy2=Zy*Zy  */

    int chunk = iYmax / THREADS;
    int lb = tid * chunk;
    int ub = lb + chunk;

    for (iY = lb; iY < ub; iY++) {
        Cy = CyMin + iY * PixelHeight;
        if (fabs(Cy) < PixelHeight / 2) Cy = 0.0; /* Main antenna */
        for (iX = 0; iX < iXmax; iX++) {
            Cx = CxMin + iX * PixelWidth;
            /* initial value of orbit = critical point Z= 0 */
            Zx = 0.0;
            Zy = 0.0;
            Zx2 = Zx * Zx;
            Zy2 = Zy * Zy;
            /* */
            for (Iteration = 0; Iteration < IterationMax && ((Zx2 + Zy2) < ER2); Iteration++) {
                Zy = 2 * Zx * Zy + Cy;
                Zx = Zx2 - Zy2 + Cx;
                Zx2 = Zx * Zx;
                Zy2 = Zy * Zy;
            }

            sumi[tid] += Iteration;

            /* compute  pixel color (24 bit = 3 bytes) */
            if (Iteration == IterationMax) { /*  interior of Mandelbrot set = black */
                color[iY][iX][0] = 0;
                color[iY][iX][1] = 0;
                color[iY][iX][2] = 0;
            } else { /* exterior of Mandelbrot set = white */
                color[iY][iX][0] = 255; /* Red*/
                color[iY][iX][1] = 255; /* Green */
                color[iY][iX][2] = 255; /* Blue */
            }
        }
    }
}

int main() {
    FILE *fp;
    const char *filename = "new1.ppm";
    const char *comment = "# ";/* comment should start with # */
    /* Z=Zx+Zy*i  ;   Z0 = 0 */

    fp = fopen(filename, "wb"); /* b -  binary mode */
    /*write ASCII header to the file*/
    fprintf(fp, "P6\n %s\n %d\n %d\n %d\n", comment, iXmax, iYmax, MaxColorComponentValue);
    /* compute and write image data bytes to the file*/


    steady_clock::time_point start = high_resolution_clock::now();
    thread ekipa[THREADS];
    for (int i = 0; i < THREADS; i++) {
        ekipa[i] = thread(calt, i);
    }

    for (int i = 0; i < THREADS; i++) {
        ekipa[i].join();
    }

    for (int i = 0; i < THREADS; i++) {
        cout << sumi[i] << endl;
    }

    auto end = std::chrono::steady_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << elapsedTime << std::endl;


    fwrite(color, 1, 3 * iYmax * iXmax, fp);
    fclose(fp);
    return 0;
}
