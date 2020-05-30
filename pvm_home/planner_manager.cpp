#include "pvm3.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "matrix.h"
#include <string>
#include <sstream>

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


//void displayEvents(Event * eventObj, int size) {
//    for (int i = 0; i < arrSize; ++i) {
//        cout << eventObj[i]->toString() << endl;
//    }
//}




int main() {
    float ro = 0.3;
    float tauMax = 1 / ro;
    int modE = 5;
    int modT = 10;
    Matrix tau(modE, modT);
    tau.fillMatrix(tauMax);

    // m
    int antQuantity = 10;

    Matrix collisions(modE, modE);

    collisions(0, 0) = 1;
    collisions(2, 2) = 1;
    collisions(1, 1) = 1;
    collisions(3, 3) = 1;
    collisions(4, 4) = 1;
    collisions(0, 1) = 1;
    collisions(1, 0) = 1;
    collisions(2, 1) = 1;
    collisions(1, 2) = 1;
    collisions(3, 2) = 1;
    collisions(2, 3) = 1;

    // Tablica przechowująca liczbę kolizji z innymi eventami dla każdego eventu
    Event d[modE];

    initializeAndCountCollisionsForEvent(d, collisions);

    qsort(d, modE, sizeof(*d), compare);

//    displayArray(d, modE);
    Event::print(d, modE);


    int numberOfIterations = 1;
    int iterationNr = 0;

    // Cgb
    Matrix globalBestSolution(modE, modT);

    while (iterationNr < numberOfIterations) {

        // Cib
        Matrix iterationBestSolution(modE, modT);

        for (int a = 1; a <= antQuantity; a++) {

            // A
            Matrix partialSolution(modE, modT);
            partialSolution.fillMatrix(0);

            for (int e = 0; e < modE; e++) {

                float Pe[modT];

                // Wypełnia tablicę prawdopodobieństw Pe
                countPe(e, tau, Pe);

                // Zwraca wylosowany identyfikator timeslotu w oparciu o tablice prawdopodobieństw Pe
                int t = countT(Pe);
                partialSolution(e, t) = 1;

                // 0: 23.5654%; 1: 30%;
                // rand(0,1): (0;0.5) -> 0 ; (0.5;0.8) -> 1     ; (0.8;1) -> 2

            }

            if (countCollisions(partialSolution, collisions) < countCollisions(iterationBestSolution, collisions)) {
                iterationBestSolution = partialSolution;
            }

        }

        if (countCollisions(iterationBestSolution, collisions) < countCollisions(globalBestSolution, collisions)) {
            globalBestSolution = iterationBestSolution;
        }

        iterationNr++;
    }

    pvm_exit();
    return 0;
}


