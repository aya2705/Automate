#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
int etat[50];
char etiquete[20];
int etat_suivant[50];
} Transition;

typedef struct {
char **etatsInitiaux;
char **etatsFinaux;
Transition *transitions;
int nombreTransition;
int nb_etats_initiaux;
int nb_etats_finaux;
} Automate;

FILE *fp;
char currentTransition[50];
char filename[30];
Automate a, result;
Automate ouvrir_et_stocker(char *filename);
void generateDot(Automate a);
void AfficherAlphabets(const char *filename);
int estAtteignable(const char *etat, char **etats_atteignables, int nb_etats_atteignables);
int motengendre(Automate a, char *mot);
void fichierMotsEngendre(Automate a);
Automate unionAutomate();
Automate etoileAutomate();
Automate produitAutomates(Automate a1, Automate a2);
void automateChoice(Automate a);
Automate suppressionEpsilon(Automate a);
Automate determinisation(Automate a);
Automate renversement(Automate a);
Automate minimisation(Automate a);

int main() {
int action;
Automate a, result;
int continuer = 1;
int num;
char filename[30];


do {
printf("************Menu principal************\n");
printf("1. Ouvrir le fichier:\n");
printf("2. Generer le fichier .dot et le fichier .png associe:\n");
printf("3. Tester un mot \n");
printf("4. Tester les mots stockes dans un fichier:\n");
printf("5. Union de deux automate:\n");
printf("6. Etoile automate:\n");
printf("7. Produit de deux automates:\n");
printf("8. Supprimmer les epsilons:\n");
printf("9. Determinisation:\n");
printf("10. Meminisation\n");
printf("11. Quitter\n");
printf("Votre choix : ");
scanf("%d", &action);
printf("\n");

if (action == 1) {
printf("Entrez le nom du fichier contenant votre automate: ");
scanf("%s", filename);
FILE *fp = fopen(filename, "r");
if (fp == NULL)
exit(1);

int lettre = 0;
while ((lettre = fgetc(fp)) != EOF) {
printf("%c", lettre);
}
printf("\n");
printf("l'alphabet existant dans le fichier qui contient votre automate:\n");
AfficherAlphabets(filename);
fclose(fp);

} else if (action == 2) {
printf("Entrez le nom du fichier contenant votre automate: ");
scanf("%s", filename);
fp = fopen(filename, "r");
a = ouvrir_et_stocker(filename);
generateDot(a);

} else if (action == 3) {
printf("Entrez le nom du fichier contenant votre automate: ");
scanf("%s", filename);
fp = fopen(filename, "r");
a = ouvrir_et_stocker(filename);
char mot[30];
printf("\n Veuillez entrer le mot a tester : ");
scanf("%s", mot);
if (motengendre(a, mot)) {
printf("\n\t Le mot est engendre par cet automate.\n\n");
} else {
printf("\n\t Le mot n'est pas engendre par cet automate.\n\n");
}

}else if (action == 4) {
printf("Entrez le nom du fichier contenant votre automate: ");
scanf("%s", filename);
fp = fopen(filename, "r");
a = ouvrir_et_stocker(filename);
fichierMotsEngendre(a);

} else if (action == 5) {
result = unionAutomate();
automateChoice(result);

}else if(action ==6){
result = etoileAutomate();
automateChoice(result);

}else if(action ==7){
char Auto1[30];
char Auto2[30];
printf("Le nom du fichier qui contient l'automate 1: ");
scanf("%s", Auto1);
sprintf(filename, "%s", Auto1);
Automate a1 = ouvrir_et_stocker(filename);

printf("le nom du fichier qui contient l'automate 2: ");
scanf("%s", Auto2);
sprintf(filename, "%s", Auto2);
Automate a2 = ouvrir_et_stocker(filename);
// Calcul du produit des automates A1 et A2
Automate produit = produitAutomates(a1, a2);
printf("Le produit des deux automates a ete calcule avec succes.\n");
generateDot(produit);

} else if (action ==8){
printf("Entrez le nom du fichier contenant l'automate : ");
scanf("%s", filename);
a = ouvrir_et_stocker(filename);
result = suppressionEpsilon(a);
automateChoice(result);

}else if(action==9){
char file[30];
printf("Entrez le nom du fichier contenant votre automate: ");
scanf("%s",file);
sprintf(filename, "%s", file);
a =ouvrir_et_stocker(filename);
result = determinisation(a);
automateChoice(result);

}else if(action==10){
char file[30];
printf("Entrez le nom du fichier contenant votre automate: ");
scanf("%s",file);
sprintf(filename, "%s", file);
a = ouvrir_et_stocker(filename);
result = renversement(a);
result = determinisation(result);
result = renversement(result);
result = determinisation(result);
automateChoice(result);
}
} while (action != 11);
for (int i = 0; i < a.nb_etats_initiaux; i++) {
free(a.etatsInitiaux[i]);
}
free(a.etatsInitiaux);

for (int i = 0; i < a.nb_etats_finaux; i++) {
free(a.etatsFinaux[i]);
}
free(a.etatsFinaux);

free(a.transitions);

return 0;
}

Automate ouvrir_et_stocker(char *filename) {
Automate a;
char ligne[150];
int i = 0;
fp = fopen(filename, "r");
if (fp == NULL) {
system("@cls||clear");
printf("Erreur : impossible d'ouvrir le fichier %s\n\n");
exit(EXIT_FAILURE);
}

a.nombreTransition = 0;
while (fgets(ligne, 150, fp) != NULL) {
a.nombreTransition++;
}

a.transitions = (Transition*) malloc(a.nombreTransition * sizeof(Transition));
rewind(fp);

char depart[50], arrivee[50], etiquette[50];
while (fscanf(fp, "%s %s %s\n", &depart, &arrivee, &etiquette) != EOF) {

strcpy(a.transitions[i].etat, depart);
strcpy(a.transitions[i].etat_suivant, arrivee);
strcpy(a.transitions[i].etiquete, etiquette);

if(i == a.nombreTransition - 3) {
break;
}
i++;
}
fgets(ligne, 150, fp);

char* token = strtok(ligne, " ");

a.etatsInitiaux = (char**) malloc(50 * sizeof(char));

a.nb_etats_initiaux = 0;

while (token != NULL) {
// Initialiser le longeur de chaine de caractere
a.etatsInitiaux[a.nb_etats_initiaux] = (char*) malloc(sizeof(char) * 50);

// Reinitialiser le longeur de tableau des etats initiaux
a.etatsInitiaux = (char**) realloc(a.etatsInitiaux, sizeof(char) * (a.nb_etats_initiaux + 1) * 50);

if(token[strlen(token) - 1] == '\n') {
token[strlen(token) - 1] = '\0';
}

// Stocker Etat dans la tableu des etats inital
strcpy(a.etatsInitiaux[a.nb_etats_initiaux], token);

// Recevoir le prochaine etat
token= strtok(NULL, " ");

a.nb_etats_initiaux += 1;
}

// Refaire le meme chose comme les etats initiaux
fgets(ligne, 150, fp);
token = strtok(ligne, " ");
a.etatsFinaux = (char**) malloc(50 * sizeof(char));

a.nb_etats_finaux= 0;

while (token != NULL) {
a.etatsFinaux[a.nb_etats_finaux] = (char*) malloc(sizeof(char) * 50);
a.etatsFinaux = (char**) realloc(a.etatsFinaux, sizeof(char) * (a.nb_etats_finaux + 1) * 50);
if(token[strlen(token) - 1] == '\n') {
token[strlen(token) - 1] = '\0';
}
strcpy(a.etatsFinaux[a.nb_etats_finaux], token);
token= strtok(NULL, " ");
a.nb_etats_finaux += 1;
}

a.nombreTransition -= 2;

return a;
}

void AfficherAlphabets(const char *filename) {
char alphabet;
bool encountered[26] = {false};

fp = fopen(filename, "r");
if (fp == NULL) {
fprintf(stderr, "Impossible d'ouvrir le fichier '%s'\n", filename);
return;
}
while (fscanf(fp, " %c", &alphabet) != EOF) {
if ((alphabet >= 'a') && (alphabet <= 'z') && !encountered[alphabet - 'a']) {
printf("%c\t", alphabet);
encountered[alphabet - 'a'] = true;
}
else if ((alphabet >= 'A' && alphabet <= 'Z') && !encountered[alphabet - 'A' + 26]) {
printf("%c\t", alphabet);
encountered[alphabet - 'A' + 26] = true;
}
}
fclose(fp);
}

int estAtteignable(const char *etat, char **etats_atteignables, int nb_etats_atteignables) {
for (int i = 0; i < nb_etats_atteignables; i++) {
if (strcmp(etat, etats_atteignables[i]) == 0) {
return 1;
}
}
return 0;
}

void generateDot(Automate a) {
char outputDotFilename[40];
char outputPngFilename[40];
char command[100];

printf("Entrez le nom du fichier DOT de sortie : ");
scanf("%s", outputDotFilename);

printf("Entrez le nom du fichier PNG de sortie : ");
scanf("%s", outputPngFilename);

if (a.nombreTransition < 1) {
system("@cls||clear");
printf("Automate n'est pas saisi dans le fichier txt\n\n");
return;
}
// les etats atteignables
char *etats_atteignables[100];
for (int i = 0; i < a.nb_etats_initiaux; i++) {
etats_atteignables[i] = a.etatsInitiaux[i];
}
int nb_etats_atteignables = a.nb_etats_initiaux;

int i = 0;
while (i < nb_etats_atteignables) {
for (int j = 0; j < a.nombreTransition; j++) {
if (strcmp(etats_atteignables[i], a.transitions[j].etat) == 0) {
// Verifier si l' etat suivant n'est pas deja dans la liste
int existe = 0;
for (int k = 0; k < nb_etats_atteignables; k++) {
if (strcmp(etats_atteignables[k], a.transitions[j].etat_suivant) == 0) {
existe = 1;
break;
}
}
if (!existe) {
etats_atteignables[nb_etats_atteignables] = a.transitions[j].etat_suivant;
nb_etats_atteignables++;
}
}
}
i++;
}
// Afficher les tats atteignables
printf("Etats Atteignables : ");
for (int i = 0; i < nb_etats_atteignables; i++) {
printf("%s ", etats_atteignables[i]);
}
printf("\n");

// Afficher les tats inatteignables
printf("Etats Inatteignables : ");
for (int i = 0; i < a.nombreTransition; i++) {
if (!estAtteignable(a.transitions[i].etat, etats_atteignables, nb_etats_atteignables)) {
printf("%s ", a.transitions[i].etat);
}
}
printf("\n");

fp = fopen(outputDotFilename, "w+");

// Initialiser par le syntaxe de l'automate
fprintf(fp, "digraph Thl {\n");
fprintf(fp, "\trankdir=LR;\n");
fprintf(fp, "\tnode [shape = circle];\n");

// ecrire les transitions dans le fichier
for (int i = 0; i < a.nombreTransition; i++) {
// Verifier si l' etat et l' etat suivant sont inatteignables
if (!estAtteignable(a.transitions[i].etat, etats_atteignables, nb_etats_atteignables) ||
!estAtteignable(a.transitions[i].etat_suivant, etats_atteignables, nb_etats_atteignables)) {
fprintf(fp, "\t\"%s\" [color=gray, style=filled];\n", a.transitions[i].etat);
fprintf(fp, "\t\"%s\" -> \"%s\" [label=\"%s\" color=gray];\n", a.transitions[i].etat,a.transitions[i].etat_suivant,a.transitions[i].etiquete);
} else {
fprintf(fp, "\t\"%s\" -> \"%s\" [label=\"%s\"];\n", a.transitions[i].etat,a.transitions[i].etat_suivant,a.transitions[i].etiquete);
}
}

for (int i = 0; i < a.nb_etats_initiaux; i++) {
fprintf(fp, "\t\"start%d\" -> \"%s\" [label = \"\", color = green];\n",i, a.etatsInitiaux[i]);
fprintf(fp, "\t\"start%d\" [label = \"\", shape = none, height= 0, width = 0];\n",i);
fprintf(fp, "\t\"%s\" [color = green,style=filled];\n", a.etatsInitiaux[i]);
}

for (int i = 0; i < a.nb_etats_finaux; i++) {
fprintf(fp, "\t\"%s\" -> \"end%d\" [label = \"\", color = blue];\n", a.etatsFinaux[i], i);
fprintf(fp, "\t\"end%d\" [label = \"\", shape = none, height =0, width = 0];\n", i);
fprintf(fp, "\t\"%s\" [color = blue, style=filled];\n", a.etatsFinaux[i]);
}
fprintf(fp, "}");
fclose(fp);

// Generer le fichier PNG partir du fichier DOT
sprintf(command, "dot -Tpng %s -o %s", outputDotFilename, outputPngFilename);
system(command);
printf("\nle fichier est genere avec succes\n\n");
system(outputDotFilename);
}

int motengendre(Automate a, char *mot) {
for (int j = 0; j < a.nb_etats_initiaux; j++) {
strcpy(currentTransition, a.etatsInitiaux[j]);

int i = 0;
while (i < strlen(mot)) {
int transitionTrouvee = 0;
for (int k = 0; k < a.nombreTransition; k++) {
char toCompare[strlen(a.transitions[k].etiquete)];
strncpy(toCompare, &mot[i], strlen(a.transitions[k].etiquete));
toCompare[strlen(a.transitions[k].etiquete)] = '\0';

if (strcmp(currentTransition, a.transitions[k].etat) == 0 &&
strcmp(a.transitions[k].etiquete, toCompare) == 0) {
// La transition est trouv e, met jour l' tat actuel
strcpy(currentTransition, a.transitions[k].etat_suivant);
i += strlen(a.transitions[k].etiquete); // Passer a la lettre suivant du mot

transitionTrouvee = 1;
break;
}
}

if (!transitionTrouvee) {
// Le mot n'est pas accept puisque aucune transition n'est trouv e.
return 0;
}
}
}

// Verifie si l'etat final est atteint apres avoir parcouru tout le mot
for (int i = 0; i < a.nb_etats_finaux; i++) {
if (strcmp(a.etatsFinaux[i], currentTransition) == 0) {
return 1;
}
}

return 0;
}

void fichierMotsEngendre(Automate a) {
printf("\n");
char mot[30];
char nomFichierMots[100];
printf("Veuillez entrer le nom du fichier des mots : ");
scanf("%s", nomFichierMots);
FILE *fp = fopen(nomFichierMots, "r");

while (fscanf(fp, "%s", mot) != -1) {
printf("\t le mot est %s: ", mot);
if (motengendre(a, mot)) {
printf("Le mot est engendre\n\n");
} else {
printf("Le mot n'est pas engendre\n\n");
}
}
printf("\n");
fclose(fp);
}

Automate unionAutomate() {
system("@cls||clear");
char Auto1[30];
char Auto2[30];
char filename[30];

printf("Le nom du fichier qui contient l'automate 1: ");
scanf("%s", Auto1);
sprintf(filename, "%s", Auto1);
Automate a1 = ouvrir_et_stocker(filename);

printf("le Nom du fichier qui contient l'automate 2: ");
scanf("%s", Auto2);
sprintf(filename, "%s", Auto2);
Automate a2 = ouvrir_et_stocker(filename);

Automate a3;

// Initialisation
a3.nb_etats_finaux = 1; // Un seul état final pour a3
a3.nb_etats_initiaux = 1;
a3.nombreTransition = a1.nombreTransition + a2.nombreTransition + a1.nb_etats_initiaux + a2.nb_etats_initiaux + 2; // L'ajout de 2 pour les transitions epsilon

a3.etatsInitiaux = (char*)malloc(a3.nb_etats_initiaux * sizeof(char));
a3.etatsFinaux = (char*)malloc(a3.nb_etats_finaux * sizeof(char));
a3.transitions = (Transition*)malloc(a3.nombreTransition * sizeof(Transition));

a3.etatsInitiaux[0] = (char*)malloc(sizeof(char) * 50);
strcpy(a3.etatsInitiaux[0], "0");

// Copier les etats initiaux
int i, j;
for(i = 0, j = 0; i < a1.nb_etats_initiaux; i++, j++) {
strcpy(a3.transitions[j].etat, "0");
strcpy(a3.transitions[j].etat_suivant, "1_");
strcat(a3.transitions[j].etat_suivant, a1.etatsInitiaux[j]);
strcpy(a3.transitions[j].etiquete, "&epsilon;");
}

for(i = 0; i < a2.nb_etats_initiaux; i++, j++) {
strcpy(a3.transitions[j].etat, "0");
strcpy(a3.transitions[j].etat_suivant, "2_");
strcat(a3.transitions[j].etat_suivant, a2.etatsInitiaux[i]);
strcpy(a3.transitions[j].etiquete, "&epsilon;");
}

// Copier les Transitions
for(i = 0, j = a1.nb_etats_initiaux + a2.nb_etats_initiaux; i < a1.nombreTransition; i++, j++) {
strcpy(a3.transitions[j].etat, "1_");
strcat(a3.transitions[j].etat, a1.transitions[i].etat);
strcpy(a3.transitions[j].etat_suivant, "1_");
strcat(a3.transitions[j].etat_suivant, a1.transitions[i].etat_suivant);
strcpy(a3.transitions[j].etiquete, a1.transitions[i].etiquete);
}
for(int i = 0; i < a2.nombreTransition; i++, j++) {
strcpy(a3.transitions[j].etat, "2_");
strcat(a3.transitions[j].etat, a2.transitions[i].etat);
strcpy(a3.transitions[j].etat_suivant, "2_");
strcat(a3.transitions[j].etat_suivant, a2.transitions[i].etat_suivant);
strcpy(a3.transitions[j].etiquete, a2.transitions[i].etiquete);
}

// Ajout des transitions epsilon pour les états finaux de a1
strcpy(a3.transitions[j].etat, "1_");
strcat(a3.transitions[j].etat, a1.etatsFinaux[0]); // Ajoutez les transitions seulement pour le premier état final de a1
strcpy(a3.transitions[j].etat_suivant, "3");
strcpy(a3.transitions[j].etiquete, "&epsilon;");
j++;

// Ajout des transitions epsilon pour les états finaux de a2
strcpy(a3.transitions[j].etat, "2_");
strcat(a3.transitions[j].etat, a2.etatsFinaux[0]); // Ajoutez les transitions seulement pour le premier état final de a2
strcpy(a3.transitions[j].etat_suivant, "3");
strcpy(a3.transitions[j].etiquete, "&epsilon;");
j++;

// Retirer les états finaux de a1 et a2 de la liste des états finaux de a3
a3.etatsFinaux[0] = (char*)malloc(sizeof(char) * 50);
strcpy(a3.etatsFinaux[0], "3");

return a3;
}
Automate etoileAutomate() {
system("@cls||clear");
char filename[30];
char etoileauto[30];

printf("Le nom du fichier qui contient l'automate : ");
scanf("%s",filename);
// Supposons que la fonction ouvrir_et_stocker() existe et initialisons correctement la structure Automate a.
Automate a = ouvrir_et_stocker(filename);
int nbr_etat_finaux = a.nb_etats_finaux;
int nbr_etat_initiaux = a.nb_etats_initiaux;
int transition_nbr = a.nombreTransition;

// Redimensionnement des tableaux
a.nb_etats_finaux = 1;
a.nb_etats_initiaux = 1;
a.nombreTransition += nbr_etat_initiaux + nbr_etat_finaux + 2;

// Allocation mémoire pour les transitions
a.transitions = (Transition*)realloc(a.transitions, a.nombreTransition * sizeof(Transition));
if (a.transitions == NULL) {
fprintf(stderr, "Erreur lors de l'allocation mémoire pour les transitions\n");
exit(EXIT_FAILURE);
}

// Ajout des nouvelles transitions
for (int i = 0; i < nbr_etat_finaux; i++) {
strcpy(a.transitions[transition_nbr].etat, a.etatsFinaux[i]);
strcpy(a.transitions[transition_nbr].etat_suivant, "end");
strcpy(a.transitions[transition_nbr].etiquete, "&epsilon;");
transition_nbr++;
}

for (int i = 0; i < nbr_etat_initiaux; i++) {
strcpy(a.transitions[transition_nbr].etat, "start");
strcpy(a.transitions[transition_nbr].etat_suivant, a.etatsInitiaux[i]);
strcpy(a.transitions[transition_nbr].etiquete, "&epsilon;");
transition_nbr++;
}
for (int i = 0; i < nbr_etat_initiaux; i++) {
strcpy(a.transitions[transition_nbr].etat, a.etatsFinaux[i]);
strcpy(a.transitions[transition_nbr].etat_suivant, a.etatsInitiaux[i]);
strcpy(a.transitions[transition_nbr].etiquete, "&epsilon;");
transition_nbr++;
}

// Ajout des transitions pour la boucle de l'étoile
strcpy(a.transitions[transition_nbr].etat, "start");
strcpy(a.transitions[transition_nbr].etat_suivant, "end");
strcpy(a.transitions[transition_nbr].etiquete, "&epsilon;");
transition_nbr++;

// Allocation de mémoire pour les nouveaux états initiaux et finaux
a.etatsInitiaux[0] = (char*)malloc(sizeof(char) * 50);
if (a.etatsInitiaux[0] == NULL) {
fprintf(stderr, "Erreur lors de l'allocation mémoire pour l'état initial\n");
exit(EXIT_FAILURE);
}
strcpy(a.etatsInitiaux[0], "start");

a.etatsFinaux[0] = (char*)malloc(sizeof(char) * 50);
if (a.etatsFinaux[0] == NULL) {
fprintf(stderr, "Erreur lors de l'allocation mémoire pour l'état final\n");
exit(EXIT_FAILURE);
}
strcpy(a.etatsFinaux[0], "end");

return a;
}
Automate produitAutomates(Automate a1, Automate a2) {
Automate produit;
char etat_final[50];
int index = 0;

// Initialisation des états initiaux du produit
produit.nb_etats_initiaux = a1.nb_etats_initiaux * a2.nb_etats_initiaux;
produit.etatsInitiaux = (char**)malloc(produit.nb_etats_initiaux * sizeof(char*));
for (int i = 0; i < produit.nb_etats_initiaux; i++) {
produit.etatsInitiaux[i] = (char*)malloc(50 * sizeof(char));
}
for (int i = 0; i < a1.nb_etats_initiaux; i++) {
for (int j = 0; j < a2.nb_etats_initiaux; j++) {
sprintf(produit.etatsInitiaux[index], "%s_%s", a1.etatsInitiaux[i], a2.etatsInitiaux[j]);
index++;
}
}

// Initialisation des états finaux du produit
produit.nb_etats_finaux = 0;
produit.etatsFinaux = NULL;
for (int i = 0; i < a1.nb_etats_finaux; i++) {
for (int j = 0; j < a2.nb_etats_finaux; j++) {
sprintf(etat_final, "%s_%s", a1.etatsFinaux[i], a2.etatsFinaux[j]);
produit.nb_etats_finaux++;
produit.etatsFinaux = (char**)realloc(produit.etatsFinaux, produit.nb_etats_finaux * sizeof(char*));
produit.etatsFinaux[produit.nb_etats_finaux - 1] = strdup(etat_final);
}
}

// Construction des transitions du produit
produit.nombreTransition = 0;
produit.transitions = NULL;
for (int i = 0; i < a1.nombreTransition; i++) {
for (int j = 0; j < a2.nombreTransition; j++) {
char etat_depart[50], etat_arrivee[50], etiquette[20];
sprintf(etat_depart, "%s_%s", a1.transitions[i].etat, a2.transitions[j].etat);
sprintf(etat_arrivee, "%s_%s", a1.transitions[i].etat_suivant, a2.transitions[j].etat_suivant);
strcpy(etiquette, a1.transitions[i].etiquete);

// Vérifier si les deux transitions ont la même étiquette
if (strcmp(a2.transitions[j].etiquete, etiquette) == 0) {
// Ajouter la transition au produit
produit.nombreTransition++;
produit.transitions = (Transition*)realloc(produit.transitions, produit.nombreTransition * sizeof(Transition));
strcpy(produit.transitions[produit.nombreTransition - 1].etat, etat_depart);
strcpy(produit.transitions[produit.nombreTransition - 1].etat_suivant, etat_arrivee);
strcpy(produit.transitions[produit.nombreTransition - 1].etiquete, etiquette);
}
}
}

return produit;
}
Automate suppressionEpsilon(Automate a) {
Automate result;
result.nb_etats_initiaux = 0;
result.nb_etats_finaux =0;
result.transitions = (Transition*) malloc(sizeof(Transition));
result.etatsInitiaux = (char**) malloc(sizeof(char));
result.etatsFinaux = (char**) malloc(sizeof(char));

int i, j, k,l, tr = 0, found;
for(k = 0; k < a.nb_etats_finaux; k++) {
for(i = 0; i < a.nb_etats_finaux; i++) {
for(j = 0; j < a.nombreTransition; j++) {
if(strcmp(a.transitions[j].etiquete ,"&epsilon;") == 0 && strcmp(a.transitions[j].etat_suivant, a.etatsFinaux[i]) == 0) {

found = 0;
for(l = 0; l < a.nb_etats_finaux; l++) {
if(strcmp(a.etatsFinaux[l], a.transitions[j].etat) == 0) {
found = 1;
}
}
if(!found) {
a.etatsFinaux = (char*) realloc(a.etatsFinaux, (a.nb_etats_finaux + 1) * sizeof(char));
a.etatsFinaux[a.nb_etats_finaux] = (char*) malloc(sizeof(char) * 50);
strcpy(a.etatsFinaux[a.nb_etats_finaux], a.transitions[j].etat);
a.nb_etats_finaux++;
}
}
}
}
}
for(k = 0; k < a.nombreTransition; k++) {
tr = 0;
for(i = 0; i < a.nombreTransition; i++) {
if(strcmp(a.transitions[i].etiquete, "&epsilon;") == 0) {
for(j = 0; j < a.nombreTransition; j++) {
if(j == i) continue;
if(strcmp(a.transitions[i].etat_suivant, a.transitions[j].etat) == 0) {
result.transitions = (Transition*) realloc(result.transitions, (tr + 1) * sizeof(Transition));
strcpy(result.transitions[tr].etat, a.transitions[i].etat);
strcpy(result.transitions[tr].etat_suivant, a.transitions[j].etat_suivant);
strcpy(result.transitions[tr].etiquete, a.transitions[j].etiquete);
tr++;
}
}
} else {
result.transitions = (Transition*) realloc(result.transitions, (tr + 1) * sizeof(Transition));
strcpy(result.transitions[tr].etat, a.transitions[i].etat);
strcpy(result.transitions[tr].etat_suivant, a.transitions[i].etat_suivant);
strcpy(result.transitions[tr].etiquete, a.transitions[i].etiquete);
tr++;
}
}
a.transitions = (Transition*) realloc(a.transitions, tr * sizeof(Transition));
for(i = 0; i < tr; i++) {
strcpy(a.transitions[i].etat, result.transitions[i].etat);
strcpy(a.transitions[i].etat_suivant, result.transitions[i].etat_suivant);
strcpy(a.transitions[i].etiquete, result.transitions[i].etiquete);
}
a.nombreTransition = tr;
}
result.nombreTransition = tr;

// Etat initiaux depart
for(i = 0; i < a.nb_etats_initiaux; i++) {
for(j = 0; j < result.nombreTransition; j++) {
if(strcmp(result.transitions[i].etat , a.etatsInitiaux[i]) == 0) {
result.etatsInitiaux = (char**) realloc(result.etatsInitiaux, (result.nb_etats_initiaux + 1) * sizeof(char));
result.etatsInitiaux[result.nb_etats_initiaux] = (char*) malloc(sizeof(char) * 50);
strcpy(result.etatsInitiaux[result.nb_etats_initiaux], result.transitions[j].etat);
result.nb_etats_initiaux++;
break;
} else if(strcmp(result.transitions[i].etat_suivant , a.etatsInitiaux[i]) == 0) {
result.etatsInitiaux = (char**) realloc(result.etatsInitiaux, (result.nb_etats_initiaux + 1) * sizeof(char));
result.etatsInitiaux[result.nb_etats_initiaux] = (char*) malloc(sizeof(char) * 50);
strcpy(result.etatsInitiaux[result.nb_etats_initiaux], result.transitions[j].etat_suivant);
result.nb_etats_initiaux++;
break;
}
}
}
// Etat Finaux depart
for(i = 0; i < a.nb_etats_finaux; i++) {
for(j = 0; j < result.nombreTransition; j++) {
if(strcmp(result.transitions[j].etat , a.etatsFinaux[i]) == 0) {
result.etatsFinaux = (char**) realloc(result.etatsFinaux, (result.nb_etats_finaux + 1) * sizeof(char));
result.etatsFinaux[result.nb_etats_finaux] = (char*) malloc(sizeof(char) * 50);
strcpy(result.etatsFinaux[result.nb_etats_finaux], result.transitions[j].etat);
result.nb_etats_finaux++;
break;
} else if(strcmp(result.transitions[j].etat_suivant , a.etatsFinaux[i]) == 0) {
result.etatsFinaux = (char**) realloc(result.etatsFinaux, (result.nb_etats_finaux + 1) * sizeof(char));
result.etatsFinaux[result.nb_etats_finaux] = (char*) malloc(sizeof(char) * 50);
strcpy(result.etatsFinaux[result.nb_etats_finaux], result.transitions[j].etat_suivant);
result.nb_etats_finaux++;
break;
}
}
}
return result;
}
Automate determinisation(Automate a) {
Automate resultat;
resultat.nb_etats_finaux = 0;
resultat.nb_etats_initiaux = 0;
resultat.nombreTransition = 0;
resultat.transitions = (Transition*) malloc(resultat.nombreTransition * sizeof(Transition));
resultat.etatsFinaux = (char**) malloc(resultat.nb_etats_finaux * sizeof(char));
resultat.etatsInitiaux = (char**) malloc(resultat.nb_etats_initiaux * sizeof(char));

int i,j ,k, found;
char etiquete[50][50], nb_etiquete = 0;

for (i = 0; i < a.nombreTransition; i++) {
found = 0;
for(k = 0; k < nb_etiquete; k++) {
if(strcmp(etiquete[k], a.transitions[i].etiquete) == 0) {
found = 1;
}
}
if(!found) {
strcpy(etiquete[nb_etiquete], a.transitions[i].etiquete);
nb_etiquete++;
}
}

char etats[50][50][50], nb_etat = 0, nb_current_etat = 0;
char tmp[50][50];
int nbr_etat[50], tmp_var = 0;
char etat_courant[1000] = "";

for(i = 0; i < a.nb_etats_initiaux; i++) {
strcpy(etats[nb_etat][i], a.etatsInitiaux[i]);
strcat(etat_courant, a.etatsInitiaux[i]);
strcat(etat_courant, ",");
}

etat_courant[strlen(etat_courant) - 1] = '\0';

resultat.etatsInitiaux[resultat.nb_etats_initiaux] = (char*) malloc(sizeof(char) * 50);
strcpy(resultat.etatsInitiaux[resultat.nb_etats_initiaux], etat_courant);
resultat.nb_etats_initiaux++;

nbr_etat[nb_etat] = a.nb_etats_initiaux;
nb_etat++;

while(nb_current_etat < nb_etat) {
int final = 0;
for(j = 0; j < nbr_etat[nb_current_etat]; j++) {
strcpy(etats[nb_etat][j], etats[nb_current_etat][j]);
for(k = 0; k < a.nb_etats_finaux; k++) {
if(strcmp(etats[nb_current_etat][j], a.etatsFinaux[k]) == 0) {
final = 1;
break;
}
}
}

if(final) {
char etat_courant[1000] = "";

for(j = 0; j < nbr_etat[nb_current_etat]; j++) {
strcat(etat_courant, etats[nb_current_etat][j]);
strcat(etat_courant, ",");
}

etat_courant[strlen(etat_courant) - 1] = '\0';

resultat.etatsFinaux = (char**) realloc(resultat.etatsFinaux, (resultat.nb_etats_finaux + 1) * sizeof(char));
resultat.etatsFinaux[resultat.nb_etats_finaux] = (char*) malloc(sizeof(char) * 50);
strcpy(resultat.etatsFinaux[resultat.nb_etats_finaux], etat_courant);
resultat.nb_etats_finaux++;
}

for(i = 0; i < nb_etiquete; i++) {
tmp_var = 0;
for(j = 0; j < nbr_etat[nb_current_etat]; j++) {
for(k = 0; k < a.nombreTransition; k++) {
if(strcmp(a.transitions[k].etat, etats[nb_current_etat][j]) == 0 && strcmp(a.transitions[k].etiquete, etiquete[i]) == 0) {
strcpy(tmp[tmp_var] ,a.transitions[k].etat_suivant);
tmp_var++;
}
}
}

int size = tmp_var;
char* uniqueArr[size];
int uniqueSize = 0;
int isDuplicate = 0;

// Parcourez le tableau d'origine et ajoutez chaque élément au tableau unique uniquement s'il n'est pas déjà présent
for (k = 0; k < size; k++) {
isDuplicate = 0;
for (j = 0; j < uniqueSize; j++) {
if (strcmp(tmp[k], uniqueArr[j]) == 0) {
isDuplicate = 1;
break;
}
}
if (!isDuplicate) {
uniqueArr[uniqueSize++] = tmp[k];
}
}

// tri le tableau en utilisant le tri a bulles
for (k = 0; k < uniqueSize - 1; k++) {
for (j = 0; j < uniqueSize - k - 1; j++) {
if (strcmp(uniqueArr[j], uniqueArr[j+1]) > 0) {
char* temp = uniqueArr[j];
uniqueArr[j] = uniqueArr[j+1];
uniqueArr[j+1] = temp;
}
}
}

int found = 0;

for(j = 0; j < nb_etat; j++) {
for(k = 0; k < uniqueSize; k++) {
if(strcmp(uniqueArr[k], etats[j][k]) != 0 || nbr_etat[j] != uniqueSize) {
break;
} else if(k == (uniqueSize - 1)) {
found = 1;
}
}
}

if(uniqueSize != 0) {
char etat_courant[1000] = "", etat_suivant[1000] = "";
resultat.transitions = (Transition*) realloc(resultat.transitions, (resultat.nombreTransition + 1) * sizeof(Transition));

for(j = 0; j < uniqueSize; j++) {
strcat(etat_suivant, uniqueArr[j]);
strcat(etat_suivant, ",");
}
etat_suivant[strlen(etat_suivant) - 1] = '\0';

for(j = 0; j < nbr_etat[nb_current_etat]; j++) {
strcat(etat_courant, etats[nb_current_etat][j]);
strcat(etat_courant, ",");
}

etat_courant[strlen(etat_courant) - 1] = '\0';

strcpy(resultat.transitions[resultat.nombreTransition].etat, etat_courant);
strcpy(resultat.transitions[resultat.nombreTransition].etat_suivant, etat_suivant);
strcpy(resultat.transitions[resultat.nombreTransition].etiquete, etiquete[i]);


resultat.nombreTransition++;

if(!found) {
for(j = 0; j < uniqueSize; j++) {
strcpy(etats[nb_etat][j], uniqueArr[j]);
}

nbr_etat[nb_etat] = uniqueSize;
nb_etat++;
}
}

}
nb_current_etat++;
}

//renommer
Transition saved_tr[resultat.nombreTransition];
char etatIn[resultat.nb_etats_initiaux][50], etatFi[resultat.nb_etats_finaux][50];

// copier le source tableau vers le tableau d'arrivee
for (i = 0; i < resultat.nombreTransition; i++) {
strcpy(saved_tr[i].etat, resultat.transitions[i].etat);
strcpy(saved_tr[i].etiquete, resultat.transitions[i].etiquete);
strcpy(saved_tr[i].etat_suivant, resultat.transitions[i].etat_suivant);
}

for(i = 0; i < resultat.nb_etats_initiaux; i++) {
strcpy(etatIn[i], resultat.etatsInitiaux[i]);
}

for(i = 0; i < resultat.nb_etats_finaux; i++) {
strcpy(etatFi[i], resultat.etatsFinaux[i]);
}

for(i = 0; i < nb_etat; i++) {
etat_courant[0] = '\0';
for(k = 0; k < nbr_etat[i]; k++) {
strcat(etat_courant, etats[i][k]);
strcat(etat_courant, ",");
}

etat_courant[strlen(etat_courant) - 1] = '\0';
char strindex[50];
sprintf(strindex, "%d", i);

for(j = 0; j < resultat.nombreTransition; j++) {
if(strcmp(saved_tr[j].etat, etat_courant) == 0) {
strcpy(resultat.transitions[j].etat, strindex);
}

if(strcmp(saved_tr[j].etat_suivant, etat_courant) == 0) {
strcpy(resultat.transitions[j].etat_suivant, strindex);
}
}

for(j = 0; j < resultat.nb_etats_initiaux; j++) {
if(strcmp(etatIn[j], etat_courant) == 0) {
strcpy(resultat.etatsInitiaux[j], strindex);
}
}

for(j = 0; j < resultat.nb_etats_finaux; j++) {
if(strcmp(etatFi[j], etat_courant) == 0) {
strcpy(resultat.etatsFinaux[j], strindex);
}
}
}

return resultat;
}

Automate renversement(Automate a) {
int i;
char tmp[50];
for(i = 0; i < a.nombreTransition; i++) {
strcpy(tmp, a.transitions[i].etat);
strcpy(a.transitions[i].etat, a.transitions[i].etat_suivant);
strcpy(a.transitions[i].etat_suivant, tmp);
}

char **temp = a.etatsInitiaux;
int tmpInt = a.nb_etats_initiaux;
a.etatsInitiaux = a.etatsFinaux;
a.nb_etats_initiaux = a.nb_etats_finaux;
a.etatsFinaux = temp;
a.nb_etats_finaux = tmpInt;

return a;
}

void automateChoice(Automate a) {
int action, continuer = 1;
do {
system("@cls||clear");
printf("\n\t1. Generate Dot\n");
printf("\t2. Return to menu principal\n");
printf("\nEntrer Votre Choix : ");
scanf("%d", &action);
switch(action) {
case 1:
generateDot(a);
break;
case 2:
continuer = 0;
break;
}
}while(continuer);
}
