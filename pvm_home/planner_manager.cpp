#include "pvm3.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "matrix.h"
#include <string>
#include <sstream>
#include <cstdlib>

using namespace std;


int countTimeSlotProbability();

class Event {
public:
    int id;
    int colissions;
    int timeSlot;

    Event() {

    }

    Event(int _id) {
        id = _id;
    }

    bool operator<(const Event &eventObj) const {
        return colissions < eventObj.colissions;
    }


    string toString() {
        stringstream s;
        s << "id:" << id << "; colissions:" << colissions;
        return s.str();
    }

    static void print(Event *eventObj, int size) {
        for (int i = 0; i < size; ++i) {
            cout << eventObj[i].toString() << endl;
        }
    }

};

int compare(const void *e1, const void *e2) {
    const Event *ee1 = (Event *) e1;
    const Event *ee2 = (Event *) e2;

    if (ee1->colissions < ee2->colissions) {
        return 1;
    } else if (ee1->colissions > ee2->colissions) {
        return -1;
    } else {
        return 0;
    }
}

int displayMatrix(Matrix &tau) {
    cout << endl;
    cout << tau;
}

void initializeAndCountCollisionsForEvent(Event *d, Matrix &collisions) {

    for (int i = 0; i < collisions.getRows(); ++i) {
        d[i].id = i;
        d[i].colissions = 0;
        for (int j = 0; j < collisions.getCols(); ++j) {
            if (collisions(i, j) == 1) {
                d[i].colissions++;
            }
        }

    }

}

void displayArray(int *arr, int arrSize) {
    for (int i = 0; i < arrSize; ++i) {
        cout << i << ": " << arr[i] << "; " << endl;
    }
}


void displayArray(float *arr, int arrSize) {
    for (int i = 0; i < arrSize; ++i) {
        cout << i << ": " << arr[i] << "; " << endl;
    }
}


//void displayEvents(Event * eventObj, int size) {
//    for (int i = 0; i < arrSize; ++i) {
//        cout << eventObj[i]->toString() << endl;
//    }
//}


float countPe(int e, Matrix &tau, float *Pe, int modT) {

    float tauSum = 0;
    for (int t = 0; t < modT; t++) {
        tauSum += tau(e, t);
    }

    for (int t = 0; t < modT; t++) {
        Pe[t] = tau(e, t) / tauSum;
    }

//    cout << "FOR event: " << e << endl;
//    displayArray(Pe, modT);

}

int countT(float *propabilisticsEvent, int modT) {

    double random = ((double) rand() / RAND_MAX);
    double border = 0;
    for (int i = 0; i < modT; i++) {
        border += propabilisticsEvent[i];
        if (random <= border) {
//            cout << random << " return: " << i << endl;
            return i;
        }
    }

    throw "countT nie zadziałał bo nie policzył które t";
}


int countCollisions(Matrix &partialSolution, Matrix &collisions) {

    // --- INITIALIZATION---
    int numberOfCollisions = 0;
    // # kolizja - dwa takie same eventy są w tym samym slocie czasowym
    // # kolizja - event odbywa się równocześnie z innym eventem

//    // --- SHOW PARTIAL MATRIX ---
//    cout << "partial" << endl;
//    cout << partialSolution;
//
//    // --- SHOW COLLISION MATRIX ---
//    cout << "collisions" << endl;
//    cout << collisions;
    for (int t = 0; t < partialSolution.getCols(); t++) {
        for (int e = 0; e < partialSolution.getRows(); e++) {
//            cout << "partialSolution(" << e << "," << t << "): " << partialSolution(e, t) << endl;
            if (partialSolution(e, t) == 0) {
                continue;
            }

            for (int ee = e; ee < partialSolution.getRows(); ee++) {

//                cout << "collisions(" << e << "," << ee << "): " << collisions(e, ee) << " partialSolution(" << ee << "," << t << "): " << partialSolution(ee, t) << endl;
                if (collisions(e, ee) > 0 && partialSolution(ee, t) > 0) {
                    numberOfCollisions++;
                }
            }
        }

    }

//    cout << "num of collisions: " << numberOfCollisions << endl;
    return numberOfCollisions;
}


int main() {

    srand(time(NULL));

    float rho = 0.3;
    float tauMax = 1 / rho;
    int numberOfIterations = 100;
    // m
    int antQuantity = 1;


    int modE = 5;
    int modT = 5;


    Matrix tau(modE, modT);
    tau.fillMatrix(tauMax);


    Matrix collisions(modE, modE);

    collisions(0, 1) = 1;
    collisions(1, 0) = 1;
    collisions(2, 1) = 1;
    collisions(1, 2) = 1;
    collisions(3, 2) = 1;
    collisions(2, 3) = 1;
    collisions(1, 3) = 1;
    collisions(3, 1) = 1;
    collisions(1, 4) = 1;
    collisions(4, 1) = 1;
    collisions(0, 4) = 1;
    collisions(4, 0) = 1;
    collisions(3, 4) = 1;
    collisions(4, 3) = 1;
    collisions(0, 3) = 1;
    collisions(3, 0) = 1;
    collisions(0, 2) = 1;
    collisions(2, 0) = 1;
    collisions(2, 4) = 1;
    collisions(4, 2) = 1;


    // Tablica przechowująca liczbę kolizji z innymi eventami dla każdego eventu
    Event d[modE];

    initializeAndCountCollisionsForEvent(d, collisions);

    qsort(d, modE, sizeof(*d), compare);

    Event::print(d, modE);

    int iterationNr = 0;

    // Cgb
    Matrix globalBestSolution(modE, modT);
    globalBestSolution.fillMatrix(1);

    while (iterationNr < numberOfIterations) {

        // Cib
        Matrix iterationBestSolution(modE, modT);
        iterationBestSolution.fillMatrix(1);

        for (int a = 1; a <= antQuantity; a++) {

            // A
            Matrix partialSolution(modE, modT);
            partialSolution.fillMatrix(0);

            for (int e = 0; e < modE; e++) {

                float Pe[modT];

                // Wypełnia tablicę prawdopodobieństw Pe
                countPe(e, tau, Pe, modT);

                // Zwraca wylosowany identyfikator timeslotu w oparciu o tablice prawdopodobieństw Pe
                int t = countT(Pe, modT);
                partialSolution(e, t) = 1;

            }

            if (countCollisions(partialSolution, collisions) < countCollisions(iterationBestSolution, collisions)) {
                iterationBestSolution = partialSolution;
            }

        }

        cout << "iterationBestSolution; " << "collisions: " << countCollisions(iterationBestSolution, collisions);
        displayMatrix(iterationBestSolution);

        if (countCollisions(iterationBestSolution, collisions) < countCollisions(globalBestSolution, collisions)) {
            globalBestSolution = iterationBestSolution;
        }

        tau = tau + globalBestSolution;

        cout << "feromony:";
        displayMatrix(tau);

        iterationNr++;
    }

    cout << "globalBestSolution; " << "collisions: " << countCollisions(globalBestSolution, collisions) << endl;
    displayMatrix(globalBestSolution);

    pvm_exit();
    return 0;
}


