#include <stdio.h>
#include <stdlib.h>
#include <time.h>


static int N = 0;
static int p = 0;
static int Reserve[]={1,2,3,4,5,6,7,8,9,10,25,50,75,100};
static char operators[]={'+','-','*','/'};


typedef struct Noeud{
	int result;
	char operator;
	struct Noeud * Fils_Gauche, * Fils_Droit;
} Link;

static Link * Plus_Proche = NULL;

static int *
acquire_file(char * filename){
	unsigned int i;
	int * Nombres;
	FILE * file = fopen(filename,"r");

	if (!file){
		printf("fichier inexistant\n");
		exit(1);
	}

	fscanf(file,"%d %d",&N,&p);
	Nombres = malloc(p*sizeof(int));

	for( i = 0; i < p; ++i)
	{
		fscanf(file,"%d", &Nombres[i]);

		if(!Nombres[i])
			Nombres[i] = Reserve[random()%14];
	}

	fclose(file);
	return Nombres;
}

static int *
acquire_command(int l, char * *Ligne){
	int * Nombres;
	unsigned int i;

	N = atoi(Ligne[1]);
	p = atoi(Ligne[2]);
	Nombres = malloc(p*sizeof(int));

	for(i = 3; (i < l) & (i < p+3); ++i)
		Nombres[i-3] = atoi(Ligne[i]);

	while(i < p+3)
	{
		Nombres[i-3] = Reserve[random()%14];
		++i;
	}

	return Nombres;
}

static int
eval(Link * P1, Link * P2, char operator){
	int result = 0;
	int p1, p2;

	if (P1 && P2){
		p1 = P1->result;
		p2 = P2->result;
	} else return -1;

	switch (operator){
		case '+':
			result = p1+p2;
			break;
		case '*':
			result = p1*p2;
			break;
		case '-':
			if (p1>p2) result = p1-p2;
			else result = p2-p1;
			break;
		case '/':
			if (p1>p2) result = p1/p2;
			else result = p2/p1;
			break;
		default :
			break;
	}

	return result;
}

static Link *
new_link(int result, char operateur){
	Link * root;

	root = malloc(sizeof(Link));
	root->result = result;
	root->operator = operateur;
	root->Fils_Gauche = NULL;
	root->Fils_Droit = NULL;

	return root;
}

static void
copy(Link * Arbre1,Link ** Arbre2){
	if(Arbre1 == *Arbre2) return;

	if(Arbre1){
		if(!*Arbre2){
			*Arbre2 = new_link(Arbre1->result,Arbre1->operator);
			copy(Arbre1->Fils_Gauche,&((*Arbre2)->Fils_Gauche));
			copy(Arbre1->Fils_Droit,&((*Arbre2)->Fils_Droit));
		}
		else {
			(*Arbre2)->result=Arbre1->result;
			(*Arbre2)->operator=Arbre1->operator;
			copy(Arbre1->Fils_Gauche,&((*Arbre2)->Fils_Gauche));
			copy(Arbre1->Fils_Droit,&((*Arbre2)->Fils_Droit));
		}
	}
}

static void
compare(Link * Arbre){
	if(Arbre)
		if(abs(Arbre->result - N) < abs(Plus_Proche->result - N) || Plus_Proche->result < 0)
			copy(Arbre,&Plus_Proche);
}

static Link *
fusion(Link * P1,Link * P2, char operateur){
	Link * link = new_link( eval(P1, P2, operateur), operateur);

	link->Fils_Gauche = P1;
	link->Fils_Droit = P2;

	return link;
}

static Link **
prepare(Link ** arbres,int i,int j, int * taille, char operateur){
	Link * tmp;

	tmp = arbres[i];
	arbres[j] = fusion(arbres[i], arbres[j], operateur);
	arbres[i] = arbres[*taille-1];
	arbres[*taille-1] = tmp;
	--(*taille);

	return arbres;
}

static Link * *
Defaire(Link * *arbres,int i,int j,int *taille){
	Link * tmp;

	tmp = arbres[*taille];
	arbres[*taille] = arbres[i];
	arbres[i] = tmp;
	tmp = arbres[j];

	if(tmp){
		arbres[j] = tmp->Fils_Droit;
		++(*taille);
	}

	return arbres;
}

static int
cover(Link * *arbres, int taille){
	int i,j,k,resultat;
	int p1,p2;
	resultat=-1;
	for( i = 0; i < taille; ++i)
	{
		for( j = 0; j < i; ++j)
		{
			for( k = 0; k < 3; ++k)
			{
				compare(arbres[i]);
				compare(arbres[j]);
				prepare(arbres,i,j,&taille,operators[k]);
				compare(arbres[j]);
				resultat = Plus_Proche->result;

				if(resultat == N)
					break;
				cover(arbres,taille);
				Defaire(arbres, i, j, &taille);
			}

			if(arbres[i] && arbres[j]){
				p1=arbres[i]->result;
				p2=arbres[j]->result;
			}

			if(resultat != N && p1 && p2 && !(p1%p2 && p2%p1)){
				compare(arbres[i]);
				compare(arbres[j]);
				prepare(arbres,i,j,&taille,operators[3]);
				compare(arbres[j]);
				resultat=Plus_Proche->result;

				if(resultat == N)
					break;
				cover(arbres,taille);
				Defaire(arbres, i, j, &taille);
			}

			compare(arbres[i]);
			compare(arbres[j]);
			resultat = Plus_Proche->result;

			if(resultat == N)
				break;

		}
		compare(arbres[i]);
		resultat = Plus_Proche->result;

		if(resultat == N)
			break;
	}

	return resultat;
}

static Link * *
transform(int * nombres){
	Link * *Arbres;
	unsigned int i;

	Arbres = malloc(sizeof(Link *)*p);
	for( i = 0; i < p; ++i)
		Arbres[i] = new_link(nombres[i],'o');

	return Arbres;
}

static void
print(Link * Arbre){
	if(Arbre){
		if(Arbre->operator != 'o'){
			if(Arbre->operator == '*'){
				print(Arbre->Fils_Gauche);
				printf(" %c ",Arbre->operator);
				print(Arbre->Fils_Droit);
			}
			else {
				printf("(");

				if(Arbre->Fils_Gauche->result > Arbre->Fils_Droit->result) {
					print(Arbre->Fils_Gauche);
					printf(" %c ",Arbre->operator);
					print(Arbre->Fils_Droit);
				}
				else {
					print(Arbre->Fils_Droit);
					printf(" %c ",Arbre->operator);
					print(Arbre->Fils_Gauche);
				}

				printf(")");
			}
		}
		else {
			printf("%d",Arbre->result);
		}
	}
}


int main(int argc,char * argv[]){
	static int * Nombres;
	unsigned int i;
	Link * *Arbres;

	Plus_Proche = new_link(-1,'f');
	srand(time(NULL));

	//Usage
	if(argc < 2) {
		printf("Usage: %s filename or %s N p [Int_list]\n",argv[0],argv[0]);
		return 0;
	}

	/* Acquisition */

	//From a file:
	if (argc == 2) {
		Nombres = acquire_file(argv[1]);
	}

	//From the CL:
	if (argc > 2) {
		Nombres = acquire_command(argc,argv);
	}

	//End of acquisition

	Arbres = transform(Nombres);
	cover(Arbres,p);
	printf("Target: %d\n",N);
	printf("Available numbers: ");

	for( i = 0; i < p; ++i)
		printf("%d ", Nombres[i]);

	printf("\nResult:\n");
	print(Plus_Proche);
	printf(" = %d\n",Plus_Proche->result);

	return 0;
}

