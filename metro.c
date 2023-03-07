#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
//#include <unistd.h>
#define STAT 304
#define LIAISON 735
#define tps_chgmt 5
#define tps_dplmt 1 

/* structures metro station et voisin */
typedef struct voisin_s{ 
    int id_voisin;
    char ligne[5];
    struct voisin_s *suivant;
}voisin;

typedef struct {
    char nom[50];
    //int id;
    voisin *premier;
    int nb_voisins;
}station;

typedef struct {
    station tab[STAT];
    int nb_stations; 
}metro;

typedef struct {
  char ligne[5];
  int indice_pred ;
  int cout;
  int marque;
}predecesseur;


/* prototypes des fonctions */

/* fonctions pour metro station et voisin */
metro * creer_metro();
voisin *creer_voisin(int id_voisin, char *ligne);
void ajouter_voisin(metro *m, int dep, int arv, char *ligne);
/* fonction qui extrait les données stochées dans les deux fichiers et les stocke dans la structure métro */
void extraire_donnee(char* _stationFileName, char * _edgeFileName, metro * _metro);
void mettre_a_jour_voisins(station st, predecesseur *tab[], char * ligne_actuelle, int ind_station_actuelle);
void initialisation_algorithme();
void algorithme_plusCourtChemin(metro *m, int depart, int arrivee);
/* fonction de test */
void Test(); 
/* libération de mémoire de la structure metro */
void free_metro(metro * _metro);


/* algorithme */
int main() {
    initialisation_algorithme();
}


/* fonctions pour metro station et voisin */
metro * creer_metro(){
    metro * m = malloc(sizeof(metro));
    m->nb_stations = 0;
    for(int i = 0; i < STAT; i++) {
        m->tab[i].premier = NULL;
        m->tab[i].nb_voisins = 0;
    }
    m->nb_stations = STAT;
    return m;
}

voisin *creer_voisin(int id_voisin, char *ligne) {
    voisin *vs = malloc(sizeof(voisin));
    vs->id_voisin = id_voisin;
    strcpy(vs->ligne, ligne); 
    vs->suivant = NULL;
    return vs;
}

void ajouter_voisin(metro *m, int dep, int arv, char *ligne) { 
    voisin *vs = creer_voisin(arv, ligne);
    vs->suivant = m->tab[dep].premier; 
    m->tab[dep].premier = vs;
    m->tab[dep].nb_voisins++; 
}

/* extraire les données des fichiers */
void extraire_donnee(char* _stationFileName, char * _edgeFileName, metro * _metro) {
    //Ouverture du fichier _stationFileName, et extraction de ses données vers le tableau contenu dans _metro
    FILE * f_station = fopen(_stationFileName,"r");
    if(f_station == NULL){
        printf("extraire_donnee : Erreur lecture fichier f_station\n");
        assert(0);
    }
    
    char *name;
    char * id;
    char e;
    e = fscanf(f_station, "%*[^\n]\n");
    while(!feof(f_station)){
        char data[50];
        fgets(data,50,f_station);
        name = strtok(data, ",");
        id = strtok(NULL, ",");
        strcpy(_metro->tab[atoi(id)- 1].nom, data);
    }
 
    //Ouverture du fichier _edgeFileName et extraction de ses données vers des maillons voisions
    //puis ajoute les voisins aux stations
    FILE * f_edge = fopen(_edgeFileName, "r");
    if(f_edge == NULL){
        printf("extraire_donnee : Erreur lecture fichier f_edge\n");
        assert(0);
    }
    int start, end;
    char line[5];
    e = fscanf(f_edge, "%*[^\n]\n");
    while(!feof(f_edge)){
        e = fscanf(f_edge, "%d,%d,%s\n", &start, &end, line);
        //printf("start : %d, end : %d, ligne : %s\n", start, end, line);
        ajouter_voisin(_metro, start - 1, end - 1, line);
    }
    
    
    fclose(f_edge);
    fclose(f_station);
} 


void free_metro(metro * _metro){ 
    for(int i = 0; i < STAT; i++){
        voisin * vs = _metro->tab[i].premier, *tmp;
        while(vs != NULL){
            tmp = vs->suivant;           
            free(vs);
            vs = tmp;
        }
    }
    free(_metro);

}

void algorithme_plusCourtChemin(metro *m, int depart, int arrivee) {
    predecesseur *tab[STAT];
    for(int i = 0; i < STAT; i++) 
        tab[i] = NULL;
    tab[depart] = malloc(sizeof(predecesseur));
   
    char ligne_depart[4] = "null";
    tab[depart]->indice_pred = depart;
    tab[depart]->cout = 0;
    tab[depart]->marque = 1; // sommet de départ marqué
    strcpy(tab[depart]->ligne, ligne_depart);
    mettre_a_jour_voisins(m->tab[depart], tab, ligne_depart, depart);
    int min = 100000;
    int ind_min = depart;

    int fini = 0;
    
    while(!fini) {  
    	min = 100000; 
        for(int i = 0; i < STAT; i++) {
            if(tab[i] != NULL && tab[i]->marque == 0) {
                if(tab[i]->cout < min) {
                    min = tab[i]->cout;
                    ind_min = i; 
                }
            }
        }       

        tab[ind_min]->marque = 1;
        if(tab[arrivee] != NULL && tab[arrivee]->marque == 1)//modification condition : "marque != 1" => "marque == 1" 
        	fini = 1;                                        
        mettre_a_jour_voisins(m->tab[ind_min], tab, tab[ind_min]->ligne, ind_min);
    }
    printf("la durée du trajet est de : %d minute(s))\n", tab[arrivee]->cout);
    printf("|------------------------------------------------------------------|\n");
    /* affichage du trajet */
    int i = arrivee;
    while(i != depart) {
        printf("minute %d : arrivé à la station \"%s\" avec la ligne %s\n",tab[i]->cout,m->tab[i].nom, tab[i]->ligne);
    	i = tab[i]->indice_pred;   	
    }
    printf("minute 0 : Station de départ : %s\n", m->tab[i].nom);
    printf("|------------------------------------------------------------------|\n");
}

void mettre_a_jour_voisins(station st, predecesseur *tab[], char * ligne_actuelle, int ind_station_actuelle){
    voisin * vs = st.premier;
    char ligne_depart[4] = "null";
    // si nous sommes dans le premier cas (station de départ) 
    if(!strcmp(ligne_depart, ligne_actuelle)){
        while(vs != NULL){
            int id = vs->id_voisin;
            tab[id] = malloc(sizeof(predecesseur));
            
            strcpy(tab[id]->ligne,vs->ligne);
            tab[id]->indice_pred = ind_station_actuelle;
            tab[id]->cout = 1;
            tab[id]->marque = 0;
            vs = vs->suivant;
        }     
        
    }
    else {
        while(vs != NULL){
            int id = vs->id_voisin;
            if(tab[id] == NULL) { // si la case n'a jamais été initialisé
            	tab[id] = malloc(sizeof(predecesseur));
            	strcpy(tab[id]->ligne,vs->ligne);
            	tab[id]->indice_pred = ind_station_actuelle;
            	if(!strcmp(vs->ligne, ligne_actuelle))
                	tab[id]->cout = tab[ind_station_actuelle]->cout + tps_dplmt;
	            else    
	                tab[id]->cout = tab[ind_station_actuelle]->cout + tps_dplmt + tps_chgmt;
	            tab[id]->marque = 0;
            }
            else { // si la case a déjà été initialisé 
            	int cout1;
            	// on stocke le cout
            	if(!strcmp(vs->ligne, ligne_actuelle))
                	cout1 = tab[ind_station_actuelle]->cout + 1;
	            else    
	                cout1 = tab[ind_station_actuelle]->cout + 6;
	            // si le nouveau cout est inférieur a celui déjà stocké => on le met a jour
	            // il ne doit pas être mis a jour si déjà marqué
	            if(cout1 < tab[id]->cout && tab[id]->marque == 0){
	            	strcpy(tab[id]->ligne,vs->ligne);
	            	tab[id]->indice_pred = ind_station_actuelle;
	            	if(!strcmp(vs->ligne, ligne_actuelle))
	                	tab[id]->cout = tab[ind_station_actuelle]->cout + 1;
		            else    
		                tab[id]->cout = tab[ind_station_actuelle]->cout + 6;
		            tab[id]->marque = 0;   
	            }
            }
            vs = vs->suivant;
        }
        
    }
}



void initialisation_algorithme(){
    metro * _metro = creer_metro();
    int st1 = 0, st2 = 0;
    extraire_donnee("Metro Paris Data - Stations.csv", "Metro Paris Data - Aretes.csv", _metro);
    for(int i = 0; i <STAT ; i++){
        printf("\"%s\" - %d\n",_metro->tab[i].nom, i + 1);
    }

    printf("Veuillez saisir l'id des stations entre 1 et 304 inclus :\n");
    while((st1 < 1 || st1 > 304) || (st2 < 1 || st2 > 304)){
        printf("Entrez la station de départ : ");
        scanf("%d", &st1);
        printf("Entrez la station d'arrivée :");
        scanf("%d", &st2);
        putchar('\n');
        if((st1 < 1 || st1 > 304) || (st2 < 1 || st2 > 304))
            printf("Les id entrés sont invalides, veuillez resaisir\n");
    }
    
    
    algorithme_plusCourtChemin(_metro, st1 - 1, st2 - 1);
}

