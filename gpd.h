#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <cassert>
#include <cstdlib>

/*gnuplot driver per fare i grafici di gnuplot*/
class GnuplotDriver{
    std::string xlabel,ylabel,zlabel, title, trace_color,background_color, nomefile, format, limitx, limity, limitz, limitx_fit, limity_fit, funzione, stileRiga, titoloRiga, posLegenda, printDataName;
    std::string * codici_colore;
    bool inverse,raise,persist,logX,logY,matrice, suFile, funzioneOverlay,fitting, griglia, daEseguire, legenda, auto_color;
    std::stringstream config_stream_string, buf, data;
    int nColonne, nRighe, num_linea, max_linee, printData;
    FILE *gp, *outFile;
    public:
    GnuplotDriver();
    ~GnuplotDriver();
    int conf(std::string opzione, std::string argomento);
    int conf(std::string opzione);
    bool config_stream();
    int comandoplot();
    int plot(double * dati, int numRighe, int numColonne);
    int plot(double * dati1, double * dati2, int numRighe);
    int plot(double * dati1, double * dati2, double * dati3, int numRighe);
    int comandofit(std::string funzione, std::string parametri);
    int fit(double * dati, int numRighe, int numColonne, std::string funzione, std::string parametri);
    int fit(double * dati1, double * dati2, int numRighe, std::string funzione, std::string parametri);
    int fit(double * dati1, double * dati2, double * dati3, int numRighe, std::string funzione, std::string parametri);
};
