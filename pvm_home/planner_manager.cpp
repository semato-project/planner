#include "planner.h"

using namespace std;

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

int displayMatrix(Matrix &tau) {
    cout << endl;
    cout << tau;
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

//int countT(float *propabilisticsEvent, int modT) {
//
//    double random = ((double) rand() / RAND_MAX);
//    double border = 0;
//    for (int i = 0; i < modT; i++) {
//        border += propabilisticsEvent[i];
//        if (random <= border) {
////            cout << random << " return: " << i << endl;
//            return i;
//        }
//    }
//
//    throw 9;
//}


int countCollisions(Matrix &partialSolution, Matrix &collisions) {

    // --- INITIALIZATION---
    int numberOfCollisions = 0;

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

void unpackMatrix(int cols, int rows, Matrix &matrix) {
     for(int i=0 ; i < cols ; i++){
        for(int j=0 ; j < rows ; j++){
            pvm_upkdouble( &matrix(i, j), 1, 1); // wsadzenie colizji do bufora
        } 
    }
}

void packMatrix(int cols, int rows, Matrix &matrix) {
     for(int i=0 ; i < cols ; i++){
        for(int j=0 ; j < rows ; j++){
            pvm_pkdouble(&matrix(i, j), 1, 1); // wsadzenie colizji do bufora
        } 
    }
}

void initializeConstants(ifstream &inputDataFile, float &rho, int &numberOfIterations, int &singleSlaveIterationSeconds, int &antQuantity, int &modE, int &modT) {

    string line;
    getline(inputDataFile, line);
    getline(inputDataFile, line);
    rho = stof(line);

    cout << "rho: " << rho << endl;

    getline(inputDataFile, line);
    getline(inputDataFile, line);
    numberOfIterations = stoi(line);

    cout << "numberOfIterations: " << numberOfIterations << endl;

    getline(inputDataFile, line);
    getline(inputDataFile, line);
    singleSlaveIterationSeconds = stoi(line);

    cout << "singleSlaveIterationSeconds: " << singleSlaveIterationSeconds << endl;

    getline(inputDataFile, line);
    getline(inputDataFile, line);
    antQuantity = stoi(line);

    cout << "antQuantity: " << antQuantity << endl;

    getline(inputDataFile, line);
    getline(inputDataFile, line);
    modE = stoi(line);

    cout << "modE: " << modE << endl;

    getline(inputDataFile, line);
    getline(inputDataFile, line);
    modT = stoi(line);

    cout << "modT: " << modT << endl;

}

void initializeCollisions(ifstream &inputDataFile, Matrix &collisions, int modE) {

    string line;
    getline(inputDataFile, line);
    while (!inputDataFile.eof()) {
        int eventsForGroupQuantity = 0;
        int eventsForGroup[modE];

        int i = 0;
        while (line.front() != '#') {

            eventsForGroupQuantity++;

            cout << line << '\n';

            eventsForGroup[i] = stoi(line);
            i++;

            getline(inputDataFile, line);
        }

        for (int e = 0; e < eventsForGroupQuantity; e++) {
            for (int ee = 0; ee < eventsForGroupQuantity; ee++) {
                if (eventsForGroup[e] == eventsForGroup[ee]) {
                    continue;
                }
                collisions(eventsForGroup[e], eventsForGroup[ee]) = 1;
            }
        }
        getline(inputDataFile, line);
    }
}


int main() {
    float rho, tauMax;
    int numberOfIterations, antQuantity, modE, modT, singleSlaveIterationSeconds;

    ifstream inputDataFile;
    string inputFilePath = "/home/pvm/data/input.txt";
//    inputFilePath = getenv("PVM_PATH") + inputFilePath;
    inputDataFile.open(inputFilePath);

    inputDataFile.clear(); //clear the failure flag
    inputDataFile.seekg (0, ios::beg);


    if (!inputDataFile.is_open()) {
        throw "input.txt file can not be read.";
    }

    initializeConstants(inputDataFile, rho, numberOfIterations, singleSlaveIterationSeconds, antQuantity, modE, modT);

    tauMax = (double)1 / rho;

    Matrix collisions(modE, modE);

    initializeCollisions(inputDataFile, collisions, modE);

    inputDataFile.close();

    displayMatrix(collisions);

    Matrix tau(modE, modT);
    tau.fillMatrix(tauMax);

    // displayMatrix(tau);
    // return 0;

    // // Tablica przechowująca liczbę kolizji z innymi eventami dla każdego eventu
    // // Event d[modE];

    // // initializeAndCountCollisionsForEvent(d, collisions);

    // // qsort(d, modE, sizeof(*d), compare);

    // // Event::print(d, modE);

    // // int iterationNr = 0;

    // // do przekazania: tau, antQuantity, collisions, modE, modT
    int tids[5]; // identyfikatory zadań slave
    int proc = pvm_spawn((char*) "planner_worker", NULL, PvmTaskDefault, (char*) "", 5, tids);

    for(int i=0; i<proc; i++ ){
        pvm_initsend(PvmDataDefault); // tworzenie buffora
        pvm_pkint(&modE, 1, 1); // wsadzenie inta do buffora
        pvm_pkint(&modT, 1, 1); // wsadzenie inta do buffora
        pvm_pkint(&antQuantity, 1, 1); 
        pvm_pkint(&numberOfIterations, 1, 1);
        packMatrix(modE, modE, collisions);
        packMatrix(modE, modT, tau);
        pvm_send(tids[i], 1); // wyslanie messega
    }

    Matrix masterBestSolution(modE, modT);
    masterBestSolution.fillMatrix(1);

    for(int i=0; i<proc; i++ ){
        pvm_recv( -1, 2);
        Matrix result(modE, modT);
        unpackMatrix(modE, modT, result);
        if (countCollisions(result, collisions) < countCollisions(masterBestSolution, collisions)) {
            masterBestSolution = result;
        }
    }

    cout << "Best Solution: (" << countCollisions(masterBestSolution, collisions) << " collisions) " << endl << masterBestSolution << endl;
    pvm_exit();
    return 0;
}


