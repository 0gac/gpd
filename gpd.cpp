#include "gpd.h"

using std::cerr;
using std::string;
using std::endl;
using std::stringstream;
using std::ios;
/*--------------------------------------------------------------------------------------------------
												gnuplot driver per fare i grafici di gnuplot
--------------------------------------------------------------------------------------------------*/


GnuplotDriver::GnuplotDriver() {
	ylabel = "Y [unità di Y]";
	xlabel = "X [unità di X]";
	title = "Titolo";
	raise = false;
	persist = false;
	matrice=false;
	logX=false;
	logY=false;
	suFile=false;
	funzioneOverlay=false;
	trace_color = "blue";
	format="pdf";
	background_color = "white";
	limits="";
	nomefile="gpd_fig";
	format="pdf";
	stileRiga="linespoints";
	gp = NULL;
}  
GnuplotDriver::~GnuplotDriver() {
	if(gp) {
		fclose(gp);
	}
}  

int GnuplotDriver::conf(string opzione, string argomento){
	string delimitatore="__";
	if(opzione=="h"){
		cerr << "Usage: [t (chart title, default \"" << title << "\"]" << endl;
		cerr << "       [x (x axis title, default \"" << xlabel << "\")]" << endl;
		cerr << "       [y (y axis title, default \"" << ylabel << "\")]" << endl;
		cerr << "       [p (persist)]" <<endl;
		cerr << "       [c (trace color, default \"" << trace_color << "\")]" << endl;
		cerr << "		[ls (stile della linea, default )\"" << stileRiga << "\")]" << endl;
		cerr << "       [m (plotta la heatmap della matrice di dati)]" << endl;
		cerr << "       [logX (scala logaritmica sull'asse x, esclusa da -m)]" << endl;
		cerr << "       [logY (scala logaritmica sull'asse y, esclusa da -m)]" << endl;
		cerr << "       [fPath (Percorso in cui viene salvato il file, default gpd_fig. Non può coesistere con p)]" << endl;
		cerr << "		[fExt (estensione della figura salvata, default .pdf. Non può coesistere con p)" << endl;
		cerr << "       [lim (fissa gli assi, segue [xmin:xmax][ymin:ymax]<[zmin:zmax] se splot>)" << endl;
		cerr << "       [func (prossimo argomento è una funzione con la sintassi di gnuplot che verrà plottata nel grafico insieme ai dati. Non funziona con -m)" << endl;
		return 0;
	}
	if(opzione=="t"){
		title=argomento;
		return 0;
	}
	if(opzione=="x"){
		xlabel=argomento;
		return 0;
	}
	if(opzione=="y"){
		ylabel=argomento;
		return 0;
	}
	if(opzione=="p"){
		persist=true;
		return 0;
	}
	if(opzione=="c"){
		trace_color=argomento;
		return 0;
	}
	if(opzione=="m"){
		matrice=true;
		return 0;
	}
	if(opzione=="logX"){
		logX=true;
		return 0;
	}
	if(opzione=="logY"){
		logY=true;
		return 0;
	}
	if(opzione=="fPath"){		
		suFile=true;
		nomefile=argomento;
		return 0;
	}
	if(opzione=="fExt"){
		suFile=true;
		format=argomento;
	}
	if(opzione=="lim"){
		limits=argomento;
		return 0;
	}
	if(opzione=="func"){
		funzioneOverlay=true;
		funzione=argomento;
		return 0;
	}
	if(opzione=="ls"){
		stileRiga=argomento;
		return 0;
	}
	return -1;

}

bool GnuplotDriver::config_stream(){
	//apro la stream verso Gnuplot e sistemo tutte le configurazioni globali
	if(!gp) {
		//costruisco la stringa per chiamare gnuplot con le richieste proprietà
		string com = (string) "gnuplot -background " + background_color + "";
		//apro la stream che finisce in gnuplot
		gp = popen(com.c_str(),"w");
	}
	/*
	verifico la legalità della stream richiesta
	*/
	//non ha senso fare una heatmap con le scale logaritmiche
	if((matrice==true)&&((logX==true)||(logY==true))){
		cerr<<"-m e -z/-w non possono coesistere, vedi -h"<<endl;
		return false;
	}
	//il terminale pdfcairo non accetta -persist come comando
	if(persist==true && suFile==true){
		cerr<<"-f e -p non possono coesistere, vedi -h"<<endl;
		return false;
	} 
	//-o e -m non possono coesistere
	if(matrice==true && funzioneOverlay==true){
		cerr<<"non si può sovrapporre una funzione ad una heatmap: -m e -o non possono coesistere"<< endl;
		return false;
	}
	//controllo che lo stile della linea sia legale
	if(stileRiga != "line" && stileRiga != "points" && stileRiga != "linespoints"){
		cerr <<" stile della riga specificato non esiste" << endl;
		return false;
	}

	//setto le prime cose che posso settare indipendente dal resto
	stringstream buf;
	buf << "set title '" << title << "'" << endl;
	buf << "set xlabel '" << xlabel << "'" << endl;
	buf << "set ylabel '" << ylabel << "'" << endl;
	buf << "unset cblabel" << endl;
	buf << "unset key" << endl;
	buf << "set grid" << endl;      
	//setto i comandi di gnuplot in base ai comandi dati
	//scale log
	if(logX){
		buf<<"set logscale x"<<endl;
	}
	if(logY){
		buf<<"set logscale y"<<endl;
	}
	//formato di output
	if(persist){
		buf << "set terminal qt persist" << endl;
	}
	if(suFile){
		if(format=="pdf"){
			buf<<"set terminal pdfcairo"<<endl;
			buf<<"set out \""<<nomefile<<".pdf\""<<endl;
		}else if(format=="jpg"){
			buf<<"set terminal jpeg"<<endl;
			buf<<"set out \""<<nomefile<<".jpg\""<<endl;
		}else{
			cerr<<"il formato inserito per l'output non è supportato";
			return false;
		}
	}
	//dichiaro la funzione
	if(funzioneOverlay){
		buf << funzione << endl;
	}

	fprintf(gp,"%s",buf.str().c_str());
	return true;
}

//funzione principale con i valori normali
//void GnuplotDriver::process(double *dati, int numRighe, int numColonne, string comandi) {
int GnuplotDriver::plot(double * dati, int numRighe, int numColonne) {
	bool datiLegali=true;
	if(matrice==false && numColonne>3){
		cerr<<"non si possono fare grafici con dimensione maggiore di 3"<<endl;
		datiLegali=false;
	}
	if(config_stream() && datiLegali){
		
		try {
			stringstream buf;
			//costruisco la stream di dati
			stringstream data;
			for(int i=0;i<numRighe;i++){
				for(int j=0;j<numColonne;j++){
					data<<dati[i*numColonne+j] << " ";
				}
				data<<endl;
			}
			if(matrice==false){ //vuol dire che sto plottando un grafico "normale"
				if(numColonne==1 || numColonne==2){ //non sto facendo un plot 3d
					buf << "plot " << limits;
				}else{ //sto facendo un plot 3d
					buf << "splot " << limits;
				}
				buf << " \"-\"";
				//se do una colonna allora questa è la colonna delle y e le x sono gli indici, se do due colonne allora le colonne sono x e y mentre se do tre colonne queste sono x,y,z
				switch(numColonne){
					case 1:
						buf << " u 0:1 w "<< stileRiga << " notitle lc rgb \"" << trace_color << "\"";
					break;
					case 2:
						buf << " u 1:2 w "<< stileRiga << " notitle lc rgb \"" << trace_color << "\"";
					break;
					case 3:
						buf << " u 1:2:3 w "<< stileRiga << " notitle lc rgb \"" << trace_color << "\"";
					break;
				}
				if(funzioneOverlay){
					int end=funzione.find("=");
					buf << ", " << funzione.substr(0, end);
				}
				buf << endl;
				//finisco di mandare i dati alla streamstring
				buf << data.str();
				buf << "e" << endl;
			}else{ //se non sto plottando un grafico "normale" allora sto plottando una heatmap
				buf << "plot "<< limits <<" \"-\" matrix with image" << endl;
				buf << data.str();
				buf << "e" << endl;
			}
			fprintf(gp,"%s",buf.str().c_str());
		}
		catch (ios::failure const &problem) { //se succedono errori me lo segno
			cerr << "gnuplot_driver: " << problem.what() << endl;
		}
		return 0;
	}else{
		cerr << "non è possibile creare una stream valida con le opzioni specificate"<< endl;
		return -1;
	}
}

int GnuplotDriver::fit(double * dati, int numRighe, int numColonne, string funzione, string parametri){
	bool datiLegali=true;
	if(numColonne>3){
		cerr<<"non si possono fare grafici con dimensione maggiore di 3"<<endl;
		datiLegali=false;
	}
	int nVariabili=0;
	int posUguale=funzione.find("=");
	string nomeFunc=funzione.substr(0, posUguale);
	int apertaTonda=nomeFunc.find("(");
	int chiusaTonda=nomeFunc.find(")");
	if(chiusaTonda-apertaTonda==2){
		nVariabili=1;
	}else if(chiusaTonda-apertaTonda==4){
		nVariabili=2;
	}
	string fitFunc=funzione.substr(apertaTonda, funzione.length());
	string varFunc=funzione.substr(apertaTonda, chiusaTonda);
	if(config_stream() && datiLegali){
		try{
			stringstream buf;
			buf << "fitFunc" << fitFunc << endl;
			buf << "set fit logfile \"" << nomefile << "_fitlog.txt\" " << endl;
			buf << "set fit quiet" << endl;
			//costruisco la stream di dati
			stringstream data;
			for(int i=0;i<numRighe;i++){
				for(int j=0;j<numColonne;j++){
					data<<dati[i*numColonne+j] << " ";
				}
				data<<endl;
			}
			buf << "fit " << limits << " fitFunc" << varFunc << " \"-\" ";
			if(nVariabili=1 && numColonne==1){
				buf << "u 0:1 "; 
			}else if(nVariabili=1 && numColonne==2){
				buf << "u 1:2 ";
			}else if(nVariabili=2 && numColonne==2){
				buf << "u 0:1:2 ";
			}else if(nVariabili=2 && numColonne==3){
				buf << "u 1:2:3 ";
			}
			buf << "via " << parametri << endl;
			buf << data.str();
			buf << "e" << endl;

			if(nVariabili==1){
				buf << "set key" << endl;
				buf << "plot " << limits << " \"-\" ";
				if(numColonne==1){
					buf << "u 0:1 ";
				}else if(numColonne==2){
					buf << "u 1:2 ";
				}
				buf << "w " << stileRiga << " t \"dati\", fitFunc" << varFunc << " t \"fit\" "<<endl;
				buf << data.str();
				buf << "e" << endl;
			}
			fprintf(gp,"%s",buf.str().c_str());
		}
		catch (ios::failure const &problem) { //se succedono errori me lo segno
			cerr << "gnuplot_driver: " << problem.what() << endl;
		}
		return 0;
	}else{
		cerr << "non è possibile creare una stream valida con le opzioni specificate"<< endl;
		return -1;
	}
}

#ifdef GPDRIVER_ARMADILLO
int GnuplotDriver::plot(arma::Mat<double> dati) {
	int numRighe=dati.n_rows;
	int numColonne=dati.n_cols;
	bool datiLegali=true;
	if(matrice==false && numColonne>3){
		cerr<<"non si possono fare grafici con dimensione maggiore di 3"<<endl;
		datiLegali=false;
	}
	if(config_stream() && datiLegali){
		
		try {
			stringstream buf;
			//costruisco la stream di dati
			stringstream data;
			for(int i=0;i<numRighe;i++){
				for(int j=0;j<numColonne;j++){
					data<<dati(i*numColonne+j, 0) << " ";
				}
				data<<endl;
			}
			if(matrice==false){ //vuol dire che sto plottando un grafico "normale"
				if(numColonne==1 || numColonne==2){ //non sto facendo un plot 3d
					buf << "plot " << limits;
				}else{ //sto facendo un plot 3d
					buf << "splot " << limits;
				}
				buf << " \"-\"";
				//se do una colonna allora questa è la colonna delle y e le x sono gli indici, se do due colonne allora le colonne sono x e y mentre se do tre colonne queste sono x,y,z
				switch(numColonne){
					case 1:
						buf << " u 0:1 w "<< stileRiga << " notitle lc rgb \"" << trace_color << "\"";
					break;
					case 2:
						buf << " u 1:2 w "<< stileRiga << " notitle lc rgb \"" << trace_color << "\"";
					break;
					case 3:
						buf << " u 1:2:3 w "<< stileRiga << " notitle lc rgb \"" << trace_color << "\"";
					break;
				}
				if(funzioneOverlay){
					int end=funzione.find("=");
					buf << ", " << funzione.substr(0, end);
				}
				buf << endl;
				//finisco di mandare i dati alla streamstring
				buf << data.str();
				buf << "e" << endl;
			}else{ //se non sto plottando un grafico "normale" allora sto plottando una heatmap
				buf << "plot "<< limits <<" \"-\" matrix with image" << endl;
				buf << data.str();
				buf << "e" << endl;
			}
			fprintf(gp,"%s",buf.str().c_str());
		}
		catch (ios::failure const &problem) { //se succedono errori me lo segno
			cerr << "gnuplot_driver: " << problem.what() << endl;
		}
		return 0;
	}else{
		cerr << "non è possibile creare una stream valida con le opzioni specificate"<< endl;
		return -1;
	}
}

int GnuplotDriver::fit(arma::Mat<double> dati, int numRighe, int numColonne, string funzione, string parametri){
	bool datiLegali=true;
	if(numColonne>3){
		cerr<<"non si possono fare grafici con dimensione maggiore di 3"<<endl;
		datiLegali=false;
	}
	int nVariabili=0;
	int posUguale=funzione.find("=");
	string nomeFunc=funzione.substr(0, posUguale);
	int apertaTonda=nomeFunc.find("(");
	int chiusaTonda=nomeFunc.find(")");
	if(chiusaTonda-apertaTonda==2){
		nVariabili=1;
	}else if(chiusaTonda-apertaTonda==4){
		nVariabili=2;
	}
	string fitFunc=funzione.substr(apertaTonda, funzione.length());
	string varFunc=funzione.substr(apertaTonda, chiusaTonda);
	if(config_stream() && datiLegali){
		try{
			stringstream buf;
			buf << "fitFunc" << fitFunc << endl;
			buf << "set fit logfile \"" << nomefile << "_fitlog.txt\" " << endl;
			buf << "set fit quiet" << endl;
			//costruisco la stream di dati
			stringstream data;
			for(int i=0;i<numRighe;i++){
				for(int j=0;j<numColonne;j++){
					data<<dati(i*numColonne+j, 0) << " ";
				}
				data<<endl;
			}
			buf << "fit " << limits << " fitFunc" << varFunc << " \"-\" ";
			if(nVariabili=1 && numColonne==1){
				buf << "u 0:1 "; 
			}else if(nVariabili=1 && numColonne==2){
				buf << "u 1:2 ";
			}else if(nVariabili=2 && numColonne==2){
				buf << "u 0:1:2 ";
			}else if(nVariabili=2 && numColonne==3){
				buf << "u 1:2:3 ";
			}
			buf << "via " << parametri << endl;
			buf << data.str();
			buf << "e" << endl;

			if(nVariabili==1){
				buf << "set key" << endl;
				buf << "plot " << limits << " \"-\" ";
				if(numColonne==1){
					buf << "u 0:1 ";
				}else if(numColonne==2){
					buf << "u 1:2 ";
				}
				buf << "w " << stileRiga << " t \"dati\", fitFunc" << varFunc << " t \"fit\" "<<endl;
				buf << data.str();
				buf << "e" << endl;
			}
			fprintf(gp,"%s",buf.str().c_str());
		}
		catch (ios::failure const &problem) { //se succedono errori me lo segno
			cerr << "gnuplot_driver: " << problem.what() << endl;
		}
		return 0;
	}else{
		cerr << "non è possibile creare una stream valida con le opzioni specificate"<< endl;
		return -1;
	}
}
#endif

int GnuplotDriver::genComm(string comando){
	if(config_stream()){
		stringstream buf;
		buf << comando;
		fprintf(gp,"%s",buf.str().c_str());
	}else{
		cerr << "non è possibile creare una stream valida con le opzioni specificate"<< endl;
		return -1;
	}
	return 0;
}
