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

    return numberOfCollisions;
}

int displayMatrix(Matrix &tau) {
    cout << endl;
    cout << tau;
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

    cout << "countT failed: " << "random: " << random << " border: " << border << " modT: " << modT << endl;
    return countT(propabilisticsEvent, modT);

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

    int modE, modT, antQuantity, singleSlaveIterationSeconds;
    double rho;
    int i = pvm_recv( -1, 1);
    if ( i < 0 )
    {
        perror("Nieoczekiwany błąd.");
        exit(0);
    }
    pvm_upkint( &modE, 1, 1);
    pvm_upkint( &modT, 1, 1);
    pvm_upkint( &antQuantity, 1, 1);
    pvm_upkdouble( &rho, 1, 1);
    pvm_upkint(&singleSlaveIterationSeconds, 1, 1);
    Matrix collisions(modE, modE);
    unpackMatrix(modE, modE, collisions);

    Matrix tau(modE, modT);
    unpackMatrix(modE, modT, tau);

    //  Cgb
    Matrix globalBestSolution(modE, modT);
    globalBestSolution.fillMatrix(1);

    time_t start = time(nullptr);

    while (time(nullptr) - start < singleSlaveIterationSeconds) {

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

        if (countCollisions(iterationBestSolution, collisions) < countCollisions(globalBestSolution, collisions)) {
            globalBestSolution = iterationBestSolution;
        }

        tau = (1 - rho) * tau + antQuantity * globalBestSolution;

    }

    pvm_initsend(PvmDataDefault); // tworzenie buffora
    packMatrix(modE, modT, globalBestSolution);
    packMatrix(modE, modT, tau);

    pvm_send(pvm_parent(), 2); // wyslanie messega

	pvm_exit();
}
