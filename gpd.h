#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <cassert>
#include <cstdlib>
#ifdef GPDRIVER_ARMADILLO
    #include <armadillo>
#endif

/*gnuplot driver per fare i grafici di gnuplot*/
class GnuplotDriver{
    std::string xlabel,ylabel,title;
    bool inverse,raise,persist,logX,logY,matrice, suFile, funzioneOverlay,fitting;
    std::string trace_color,background_color, nomefile, format, limits, funzione, stileRiga;
    std::stringstream config_stream_string, buf, data;
    int nColonne, nRighe;
    FILE *gp;
    public:
    GnuplotDriver();
    ~GnuplotDriver();
    int conf(std::string opzione, std::string argomento);
    int conf(std::string opzione);
    bool config_stream();
    int plot(double * dati, int numRighe, int numColonne);
    int fit(double * dati, int numRighe, int numColonne, std::string funzione, std::string parametri);
    #ifdef GPDRIVER_ARMADILLO
        int plot(arma::Mat<double> dati);  
        int fit(arma::Mat<double> dati, int numRighe, int numColonne, std::string funzione, std::string parametri);  
    #endif
};
