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
	zlabel = "Z [unità di Z]";
	title = "Titolo";
	raise = false;
	fitting=false;
	persist = false;
	matrice=false;
	logX=false;
	logY=false;
	suFile=false;
	funzioneOverlay=false;
	griglia=true;
	daEseguire=false;
	legenda=false;
	auto_color=true;
	trace_color = "blue";
	format="pdf";
	background_color = "white";
	limitx="";
	limity="";
	limitz="";
	limitx_fit="";
	limity_fit="";
	nomefile="gpd_fig";
	format="pdf";
	stileRiga="linespoints";
	titoloRiga = "";
	posLegenda = "top right";
	gp = NULL;
	num_linea=0;
	max_linee=64;
	codici_colore = new std::string[4];
	codici_colore[0]="3F";
	codici_colore[1]="7F";
	codici_colore[2]="BF";
	codici_colore[3]="FF";
}  
GnuplotDriver::~GnuplotDriver() {
	int stream_legale=config_stream();
	if(gp){
		if(stream_legale){
			std::stringstream string_final;
			string_final << config_stream_string.str();
			if(!matrice && !fitting){
					if(nColonne==1 || nColonne==2){ //non sto facendo un plot 3d
						string_final << "plot ";
					}else{ //sto facendo un plot 3d
						string_final << "splot ";
					}
					string_final << buf.str() << endl;
					string_final << data.str();
			}else{
				string_final << buf.str() << data.str();
			}
			fprintf(gp, "%s",string_final.str().c_str());
			// printf("%s",string_final.str().c_str());
		}else{
			cerr << "non è possibile creare una stream valida con le opzioni specificate"<< endl;
		}
		fclose(gp);
	}
}  

int GnuplotDriver::conf(string opzione, string argomento){
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
	if(opzione=="z"){
		zlabel=argomento;
		return 0;
	}
	if(opzione=="c"){
		trace_color=argomento;
		auto_color=false;
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
	if(opzione=="limX"){
		limitx=argomento;
		return 0;
	}
	if(opzione=="limY"){
		limity=argomento;
		return 0;
	}	
	if(opzione=="limXFit"){
		limitx_fit=argomento;
		return 0;
	}
	if(opzione=="limYFit"){
		limity_fit=argomento;
		return 0;
	}
	if(opzione=="limZ"){
		limitz=argomento;
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
	if(opzione=="lt"){
		legenda=true;
		titoloRiga=argomento;
	}	
	if(opzione=="keyLoc"){
		posLegenda=argomento;
	}
	return -1;
}

int GnuplotDriver::conf(string opzione){
	if(opzione=="h"){
		cerr << "Usage: [t (chart title, default \"" << title << "\"]" << endl;
		cerr << "       [x (x axis title, default \"" << xlabel << "\")]" << endl;
		cerr << "       [y (y axis title, default \"" << ylabel << "\")]" << endl;
		cerr << "       [z (z axis title, default \"" << zlabel << "\")]" << endl;
		cerr << "       [p (persist)]" <<endl;
		cerr << "       [c (trace color, default \"" << trace_color << "\")]" << endl;
		cerr << "		[ls (stile della linea, default )\"" << stileRiga << "\")]" << endl;
		cerr << "       [m (plotta la heatmap della matrice di dati)]" << endl;
		cerr << "       [logX (scala logaritmica sull'asse x, esclusa da -m)]" << endl;
		cerr << "       [logY (scala logaritmica sull'asse y, esclusa da -m)]" << endl;
		cerr << "       [fPath (Percorso in cui viene salvato il file, default gpd_fig. Non può coesistere con p)]" << endl;
		cerr << "		[fExt (estensione della figura salvata, default .pdf. Non può coesistere con p)" << endl;
		cerr << "       [limX ( segue [xmin:xmax])" << endl;
		cerr << "       [limY ( segue [ymin:ymax])" << endl;
		cerr << "       [limZ ( segue [zmin:zmax] <valido solo se splot>)" << endl;
		cerr << "       [limXFit ( segue [xmin:xmax] per i dati da fittare <valido solo se fitting>)" << endl;
		cerr << "       [limYFit ( segue [ymin:ymax] per i dati da fittare <valido solo se fitting>)" << endl;
		cerr << "       [func (prossimo argomento è una funzione con la sintassi di gnuplot che verrà plottata nel grafico insieme ai dati. Non funziona con -m)" << endl;		
		cerr << "       [noGrid (rimuove la griglia dal grafico)]" << endl;		
		cerr << "		[lt (line title, specifica il titolo della linea successiva nella legenda.)]" << endl;
		cerr <<"		[keyLoc (key location: top right, top left, bottom right, bottom left.)]";
		return 0;
	}
	if(opzione=="p"){
		persist=true;
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
	if(opzione=="noGrid"){
		griglia=false;
		return 0;
	}
	return -1;
}

bool GnuplotDriver::config_stream(){
	if(!daEseguire && !fitting){
		return false;
	}
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
	// controllo di non aver settato i limiti dell'asse z in grafici 2d
	if(nColonne<3 && (limitz != "" || matrice)){
		cerr << "non sono consentiti limiti sull'asse z per grafici 2d" << endl;
		return false;
	}
	if(fitting == false && (limitx_fit!="" || limity_fit!="")){
		cerr << "non sono consentiti limiti sui fit se non vengono fatti fit" << endl;
	}

	//setto le prime cose che posso settare indipendente dal resto
	// stringstream buf;
	config_stream_string << "set title '" << title << "'" << endl;
	config_stream_string << "set xlabel '" << xlabel << "'" << endl;
	config_stream_string << "set ylabel '" << ylabel << "'" << endl;
	config_stream_string << "set zlabel '" << zlabel << "'" << endl;
	config_stream_string << "unset cblabel" << endl;
	config_stream_string << "unset key" << endl;
	//setto i comandi di gnuplot in base ai comandi dati
	//legenda
	if(legenda){
		config_stream_string << "set key " << posLegenda << endl;
	}
	// griglia
	if(griglia){
		config_stream_string << "set grid" << endl; 
	}     
	//scale log
	if(logX){
		config_stream_string<<"set logscale x"<<endl;
	}
	if(logY){
		config_stream_string<<"set logscale y"<<endl;
	}
	// limiti
	if(fitting){
		if(limitx_fit!=""){
			config_stream_string << "set xrange " << limitx_fit << endl;
		}
		if(limity_fit!=""){
			config_stream_string << "set yrange " << limity_fit << endl;
		}
	}else{
		if(limitx!=""){
			config_stream_string << "set xrange " << limitx << endl;
		}
		if(limity!=""){
			config_stream_string << "set yrange " << limity << endl;
		}
		if(limitz!=""){
			config_stream_string << "set zrange " << limitz << endl;
		}
	}
	//formato di output
	if(persist){
		config_stream_string << "set terminal qt persist" << endl;
	}
	if(suFile){
		if(format=="pdf"){
			config_stream_string<<"set terminal pdfcairo"<<endl;
			config_stream_string<<"set out \""<<nomefile<<".pdf\""<<endl;
		}else if(format=="jpg"){
			config_stream_string<<"set terminal jpeg"<<endl;
			config_stream_string<<"set out \""<<nomefile<<".jpg\""<<endl;
		}else{
			cerr<<"il formato inserito per l'output non è supportato";
			return false;
		}
	}
	//dichiaro la funzione
	if(funzioneOverlay){
		config_stream_string << funzione << endl;
	}
	return true;
}

// funzione che crea il comando per plottare i dati
int GnuplotDriver::comandoplot(){
	daEseguire=true;
	try{
		if(matrice==false){ //vuol dire che sto plottando un grafico "normale"
			buf << " \"-\"";
			//se do una colonna allora questa è la colonna delle y e le x sono gli indici, se do due colonne allora le colonne sono x e y mentre se do tre colonne queste sono x,y,z
			switch(nColonne){
				case 1:
					buf << " u 0:1 w ";
				break;
				case 2:
					buf << " u 1:2 w ";
				break;
				case 3:
					buf << " u 1:2:3 w ";
				break;
			}
			buf << stileRiga;
			if(!legenda || titoloRiga == ""){
				buf << " notitle";
			}else{
				buf << " t \"" << titoloRiga << "\"";
				titoloRiga = ""; // se non diversamente specificato il titolo della riga successiva deve essere vuoto
			}
			if(auto_color){
				int b=num_linea%4;
				int g=(num_linea/4)%4;
				int r=(num_linea/16)%4;
				std::string coloretemp= "#"+codici_colore[r]+codici_colore[g]+codici_colore[b];
				buf << " lc rgb \"" << coloretemp << "\"";
				num_linea < 63 ? num_linea++ : num_linea = 0;	
			}else{
				buf << " lc rgb \"" << trace_color << "\"";
				auto_color=true;
			}
			if(funzioneOverlay){
				int end=funzione.find("=");
				buf << ", " << funzione.substr(0, end);
			}
			buf << ", ";
			//finisco di mandare i dati alla streamstring
			data << "e" << endl;
		}else{ //se non sto plottando un grafico "normale" allora sto plottando una heatmap
			buf << "plot " <<" \"-\" matrix with image" << endl;
			data << "e" << endl;
		}
	}
	catch (ios::failure const &problem) { //se succedono errori me lo segno
		cerr << "gnuplot_driver: " << problem.what() << endl;
	}
	return 0;
}

//funzione principale con i valori normali
int GnuplotDriver::plot(double * dati, int numRighe, int numColonne){
	bool datiLegali=true;	
	nColonne=numColonne;
	nRighe=numRighe;	
	if(matrice==false && nColonne>3){
		cerr<<"non si possono fare grafici con dimensione maggiore di 3"<<endl;
		datiLegali=false;
	}
	if(datiLegali){	
		//costruisco la stream di dati
		for(int i=0;i<nRighe;i++){
			for(int j=0;j<nColonne;j++){
				data<<dati[i*nColonne+j] << " ";
			}
			data<<endl;
		}	
		comandoplot();	
		return 0;
	}else{
		cerr << "non è possibile creare una stream valida con le opzioni specificate"<< endl;
		return -1;
	}
}
int GnuplotDriver::plot(double * dati1, double * dati2, int numRighe){
	bool datiLegali=true;
	nRighe=numRighe;
	nColonne=2;
	if(datiLegali){	
		//costruisco la stream di dati
		for(int i=0;i<nRighe;i++){
			data<< dati1[i] << " " << dati2[i] << endl;
		}	
		comandoplot();	
		return 0;
	}else{
		cerr << "non è possibile creare una stream valida con le opzioni specificate"<< endl;
		return -1;
	}
}
int GnuplotDriver::plot(double * dati1, double * dati2, double * dati3, int numRighe){
	bool datiLegali=true;
	nRighe=numRighe;
	nColonne=3;	
	if(datiLegali){	
		//costruisco la stream di dati
		for(int i=0;i<nRighe;i++){
			data<< dati1[i] << " " << dati2[i] << " " << dati3[i] << endl;
		}	
		comandoplot();	
		return 0;
	}else{
		cerr << "non è possibile creare una stream valida con le opzioni specificate"<< endl;
		return -1;
	}
}

int GnuplotDriver::comandofit(string funzione, string parametri){
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
	try{
		buf << "fitFunc" << fitFunc << endl;
		buf << "set fit logfile \"" << nomefile << "_fitlog.txt\" " << endl;
		buf << "set fit quiet" << endl;
		buf << "fit " << " fitFunc" << varFunc << " \"-\" ";
		if(nVariabili=1 && nColonne==1){
			buf << "u 0:1 "; 
		}else if(nVariabili=1 && nColonne==2){
			buf << "u 1:2 ";
		}else if(nVariabili=2 && nColonne==2){
			buf << "u 0:1:2 ";
		}else if(nVariabili=2 && nColonne==3){
			buf << "u 1:2:3 ";
		}
		buf << "via " << parametri << endl;
		buf << data.str();
		buf << "e" << endl;

		if(nVariabili==1){
			buf << "unset xrange" << endl << "unset yrange" << endl;
			if(limitx!=""){
				buf << "set xrange " << limitx << endl;
			}
			if(limity!=""){
				buf << "set yrange " << limity << endl;
			}
			buf << "set key " << posLegenda << endl;
			buf << "plot " << " \"-\" ";
			if(nColonne==1){
				buf << "u 0:1 ";
			}else if(nColonne==2){
				buf << "u 1:2 ";
			}
			buf << "w " << stileRiga << " t \"dati\", fitFunc" << varFunc << " t \"fit\" "<<endl;
			data << "e" << endl;
		}
	}
	catch (ios::failure const &problem) { //se succedono errori me lo segno
		cerr << "gnuplot_driver: " << problem.what() << endl;
	}
	return 0;
}
int GnuplotDriver::fit(double * dati, int numRighe, int numColonne, string funzione, string parametri){
	fitting=true;
	bool datiLegali=true;
	nRighe=numRighe;
	nColonne=numColonne;
	if(nColonne>3){
		cerr<<"non si possono fare grafici con dimensione maggiore di 3"<<endl;
		datiLegali=false;
	}
	if(datiLegali){	
		//costruisco la stream di dati
		for(int i=0;i<nRighe;i++){
			for(int j=0;j<nColonne;j++){
				data<<dati[i*nColonne+j] << " ";
			}
			data<<endl;
		}	
		comandofit(funzione, parametri);
		return 0;
	}else{
		cerr << "non è possibile creare una stream valida con le opzioni specificate"<< endl;
		return -1;
	}
}
int GnuplotDriver::fit(double * dati1, double * dati2, int numRighe, string funzione, string parametri){
	fitting=true;
	bool datiLegali=true;
	nRighe=numRighe;
	nColonne=2;
	if(datiLegali){	
		//costruisco la stream di dati
		for(int i=0;i<nRighe;i++){
			data<< dati1[i] << " " << dati2[i] << endl;
		}		
		comandofit(funzione, parametri);
		return 0;
	}else{
		cerr << "non è possibile creare una stream valida con le opzioni specificate"<< endl;
		return -1;
	}
}
int GnuplotDriver::fit(double * dati1, double * dati2, double * dati3, int numRighe, string funzione, string parametri){
	fitting=true;
	bool datiLegali=true;
	nRighe=numRighe;
	nColonne=3;
	if(datiLegali){	
		//costruisco la stream di dati
		for(int i=0;i<nRighe;i++){
			data<< dati1[i] << " " << dati2[i] << " " << dati3[i] << endl;
		}	
		comandofit(funzione, parametri);
		return 0;
	}else{
		cerr << "non è possibile creare una stream valida con le opzioni specificate"<< endl;
		return -1;
	}
}