    #include "planner.h"

using namespace std;

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

int countT(float *propabilisticsEvent, int modT) {

    float random = 1.0;
    while (random >= 1.0 || random >= 1) {
        random = ( (float) rand() / RAND_MAX);
    }

    float border = 0;
    for (int i = 0; i < modT; i++) {
        border += propabilisticsEvent[i];
        if (random <= border) {
            return i;
        }
    }

    cout << "random: " << random << " border: " << border << " modT: " << modT << endl;
    return countT(propabilisticsEvent, modT);

    throw 0.1;
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

int main(){
    srand(time(NULL));

    int modE, modT, antQuantity, numberOfIterations;
    int i = pvm_recv( -1, 1);
    if ( i < 0 )
    {
        perror("Nieoczekiwany błąd.");
        exit(0);
    }
    pvm_upkint( &modE, 1, 1);
    pvm_upkint( &modT, 1, 1);
    pvm_upkint( &antQuantity, 1, 1);
    pvm_upkint(  &numberOfIterations, 1, 1);
    Matrix collisions(modE, modE);
    unpackMatrix(modE, modE, collisions);

    Matrix tau(modE, modT);
    unpackMatrix(modE, modT, tau);

    int iterationNr = 0;

    //  Cgb
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

        // cout << "iterationBestSolution; " << "collisions: " << countCollisions(iterationBestSolution, collisions);
        // displayMatrix(iterationBestSolution);

        if (countCollisions(iterationBestSolution, collisions) < countCollisions(globalBestSolution, collisions)) {
            globalBestSolution = iterationBestSolution;
        }

        tau = tau + globalBestSolution;

        // cout << "feromony:";
        // displayMatrix(tau);

        iterationNr++;
    }

    cout << "feromony:";
    displayMatrix(tau);

    cout << "Worker Best Solution; " << "collisions: " << countCollisions(globalBestSolution, collisions) << endl;
    displayMatrix(globalBestSolution);

    pvm_initsend(PvmDataDefault); // tworzenie buffora
    packMatrix(modE, modT, globalBestSolution);
    packMatrix(modE, modT, tau);

//    int mytid = pvm_mytid();
//    pvm_pkint(&mytid, 1, 1);
    pvm_send(pvm_parent(), 2); // wyslanie messega

	pvm_exit();
}