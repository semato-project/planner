#include "pvm3.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "matrix.h"

using namespace std;

int displayMatrix(Matrix & tau) {
    cout << endl;
    cout << tau;
}

void countCollisionsForEvent(int * d, Matrix & collisions) {

    for (int i = 0; i < collisions.getRows(); ++i) {
        d[i] = 0;
        for (int j = 0; j < collisions.getCols(); ++j) {
            if (collisions(i,j) == 1) {
                d[i]++;
            }
        }

    }

}

void displayArray(int * arr, int arrSize) {
    for (int i = 0; i < arrSize; ++i) {
        cout << i << ": " << arr[i] << "; " << endl;
    }
}


int main()
{
    float ro = 0.3;
    float tauMax = 1 / ro;
    int modE = 5;
    int modT = 10;
    Matrix tau(modE, modT);

    Matrix collisions(modE, modE);

    collisions(0,0) = 1;
    collisions(2,2) = 1;
    collisions(1,1) = 1;
    collisions(3,3) = 1;
    collisions(4,4) = 1;
    collisions(0,1) = 1;
    collisions(1,0) = 1;
    collisions(2,1) = 1;
    collisions(1,2) = 1;
    collisions(3,2) = 1;
    collisions(2,3) = 1;


    int d[modE];

    countCollisionsForEvent(d, collisions);
    displayArray(d, modE);




//    displayMatrix(collisions);





//    tau.fillMatrix(tauMax);







//    displayMatrix(tau);







        pvm_exit();
        return 0;
}


