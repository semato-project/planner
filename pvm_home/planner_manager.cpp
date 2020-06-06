#include "planner.h"

using namespace std;

//class Event {
//public:
//    int id;
//    int colissions;
//    int timeSlot;
//
//    Event() {
//
//    }
//
//    Event(int _id) {
//        id = _id;
//    }
//
//    bool operator<(const Event &eventObj) const {
//        return colissions < eventObj.colissions;
//    }
//
//
//    string toString() {
//        stringstream s;
//        s << "id:" << id << "; colissions:" << colissions;
//        return s.str();
//    }
//
//    static void print(Event *eventObj, int size) {
//        for (int i = 0; i < size; ++i) {
//            cout << eventObj[i].toString() << endl;
//        }
//    }
//
//};

//int compare(const void *e1, const void *e2) {
//    const Event *ee1 = (Event *) e1;
//    const Event *ee2 = (Event *) e2;
//
//    if (ee1->colissions < ee2->colissions) {
//        return 1;
//    } else if (ee1->colissions > ee2->colissions) {
//        return -1;
//    } else {
//        return 0;
//    }
//}

//void initializeAndCountCollisionsForEvent(Event *d, Matrix &collisions) {
//
//    for (int i = 0; i < collisions.getRows(); ++i) {
//        d[i].id = i;
//        d[i].colissions = 0;
//        for (int j = 0; j < collisions.getCols(); ++j) {
//            if (collisions(i, j) == 1) {
//                d[i].colissions++;
//            }
//        }
//
//    }
//
//}

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

float countPe(int e, Matrix &tau, float *Pe, int modT) {

    float tauSum = 0;
    for (int t = 0; t < modT; t++) {
        tauSum += tau(e, t);
    }

    for (int t = 0; t < modT; t++) {
        Pe[t] = tau(e, t) / tauSum;
    }
}

int countCollisions(Matrix &partialSolution, Matrix &collisions) {

    // --- INITIALIZATION---
    int numberOfCollisions = 0;

    for (int t = 0; t < partialSolution.getCols(); t++) {
        for (int e = 0; e < partialSolution.getRows(); e++) {
            if (partialSolution(e, t) == 0) {
                continue;
            }

            for (int ee = e; ee < partialSolution.getRows(); ee++) {

                if (collisions(e, ee) > 0 && partialSolution(ee, t) > 0) {
                    numberOfCollisions++;
                }
            }
        }

    }

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

void initializeConstants(ifstream &inputDataFile, double &rho, double &tauMin, int &numberOfIterations, int &singleSlaveIterationSeconds, int &antQuantity, int &modE, int &modT) {

    string line;
    getline(inputDataFile, line);
    getline(inputDataFile, line);
    rho = stod(line);

    cout << "rho: " << rho << endl;

    getline(inputDataFile, line);
    getline(inputDataFile, line);
    tauMin = stod(line);

    cout << "tauMin: " << tauMin << endl;

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
    float tauMax;
    double rho, tauMin;
    int numberOfIterations, antQuantity, modE, modT, singleSlaveIterationSeconds;

    time_t start = time(nullptr);

    ifstream inputDataFile;
    string inputFilePath = "/home/pvm/data/input.txt";
    inputDataFile.open(inputFilePath);

    inputDataFile.clear(); //clear the failure flag
    inputDataFile.seekg (0, ios::beg);


    if (!inputDataFile.is_open()) {
        throw "input.txt file can not be read.";
    }

    initializeConstants(inputDataFile, rho, tauMin, numberOfIterations, singleSlaveIterationSeconds, antQuantity, modE, modT);

    tauMax = (double)1 / rho;

    Matrix collisions(modE, modE);

    initializeCollisions(inputDataFile, collisions, modE);

    inputDataFile.close();

    displayMatrix(collisions);

    Matrix tau(modE, modT);
    tau.fillMatrix(tauMax);

    Matrix masterBestSolution(modE, modT);
    masterBestSolution.fillMatrix(1);

    bool optimalSolutionFound = false;

    int iterationPerformed = 0;

    for (int ii = 0; ii < numberOfIterations; ii++) {

        cout << "Current duration: " << (time(nullptr) - start) << "[s]" << endl;

        int tids[5]; // identyfikatory zadań slave
        int proc = pvm_spawn((char *) "planner_worker", NULL, PvmTaskDefault, (char *) "", 5, tids);

        for (int i = 0; i < proc; i++) {
            pvm_initsend(PvmDataDefault); // tworzenie buffora
            pvm_pkint(&modE, 1, 1); // wsadzenie inta do buffora
            pvm_pkint(&modT, 1, 1); // wsadzenie inta do buffora
            pvm_pkint(&antQuantity, 1, 1);
            pvm_pkdouble(&rho, 1, 1);
            pvm_pkdouble(&tauMin, 1, 1);
            pvm_pkint(&singleSlaveIterationSeconds, 1, 1);
            packMatrix(modE, modE, collisions);
            packMatrix(modE, modT, tau);
            pvm_send(tids[i], 1); // wyslanie messega
        }

        for (int i = 0; i < proc; i++) {
            pvm_recv(-1, 2);
            Matrix result(modE, modT);
            unpackMatrix(modE, modT, result);


            if (countCollisions(result, collisions) < countCollisions(masterBestSolution, collisions)) {
                unpackMatrix(modE, modT, tau);
                masterBestSolution = result;
            }

            if (countCollisions(masterBestSolution, collisions) <= 0) {
                optimalSolutionFound = true;
                break;
            }
        }

        if (optimalSolutionFound) {
            break;
        }

        iterationPerformed++;
    }

    cout << "Master Best Solution (Total): (" << countCollisions(masterBestSolution, collisions) << " collisions) " << endl << masterBestSolution
        << endl << "Final feromones matrix: " << endl << tau << endl <<  "Total Duration: " << (time(nullptr) - start)
        << endl << "After iterations: " << iterationPerformed << endl << "Number of collisions: " << countCollisions(masterBestSolution, collisions) << endl;
    pvm_exit();
    return 0;
}
