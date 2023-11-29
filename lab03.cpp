#include <omp.h>
#include <iostream>
#include <math.h>

#define N 10000

#define COLS 1
#define ROWS 128

using namespace std;
int tab[N][N];
int ulam[N][N];
char color[N][N];

unsigned char img[N][N][3];
const int MaxColorComponentValue = 255;

int ulam_get_map(int x, int y)
{
    x -= (N - 1) / 2;
    y -= N / 2;
    int mx = abs(x), my = abs(y);
    int l = 2 * max(mx, my);
    int d = y >= x ? l * 3 + x + y : l - x - y;
    return pow(l - 1, 2) + d;
}

int isprime(int n)
{
    int p;
    for (p = 2; p * p <= n; p++)
        if (n % p == 0) return 0;
    return n > 2;
}

int main()
{
    char filename[100];
    snprintf(filename, sizeof(filename), "ulam_%d_%d_%d.ppm", N, COLS, ROWS);


    int j, i, idx, idy;
    omp_set_nested(1);

    double start = omp_get_wtime();

#pragma omp parallel private(idy) num_threads(ROWS)
    {
        idy = omp_get_thread_num();
#pragma omp for private(j)
        for (j = 0; j < N; j++)
        {
            for (i = 0; i < N; i++)
            {
                int c, prime, col;
                tab[j][i] = idy * 11 + idx;
                // ulam_get_map
                c = ulam[j][i] = ulam_get_map(i, j);
                prime = isprime(c);  // 1 - pierwsza else 0
                col = (!prime) * 255;
                color[j][i] = prime ? '@' : ' ';
            }
        }
    }

    double end = omp_get_wtime();

    double diff = end - start;
    cout << filename << " >>> " << diff << " sec." << endl;

//    for (j = 0; j < N; j++)
//    {
//        for (i = 0; i < N; i++)
//            cout << tab[j][i] << " ";
//        cout << endl;
//    }
//
//    cout << endl << endl;
//
//    for (j = 0; j < N; j++)
//    {
//        for (i = 0; i < N; i++)
//        {
//            cout << ulam[j][i] << color[j][i] << "\t";
//        }
//        cout << endl;
//    }
//
//    cout << endl << endl;
//
//    for (j = 0; j < N; j++)
//    {
//        for (i = 0; i < N; i++)
//            cout << color[j][i] << " ";
//        cout << endl;
//    }

    for (j = 0; j < N; j++)
    {
        for (i = 0; i < N; i++)
        {
            if (color[j][i] != '@')
            {
                img[j][i][0] = (tab[j][i] * 103) % MaxColorComponentValue;
                img[j][i][1] = (tab[j][i] * 153) % MaxColorComponentValue;
                img[j][i][2] = (tab[j][i] * 196) % MaxColorComponentValue;
            } else
            {
                img[j][i][0] = MaxColorComponentValue;
                img[j][i][1] = MaxColorComponentValue;
                img[j][i][2] = MaxColorComponentValue;
            }
        }
    }


    FILE *fp;
    fp = fopen(filename, "wb");

    const char *comment = "# ";
    fprintf(fp, "P6\n %s\n %d\n %d\n %d\n", comment, N, N, MaxColorComponentValue);

    fwrite(img, 1, 3 * N * N, fp);
    fclose(fp);

    return 0;
}