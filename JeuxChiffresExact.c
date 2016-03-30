#include <stdio.h>
#include <stdlib.h>
#include <time.h>


static int N = 0;
static int p = 0;
static int Reserve[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 25, 50, 75, 100};
static char operators[] = {'+', '-', '*', '/'};


typedef struct Node{
  int result;
  char operator;
  struct Node * left, * right;
} Tree;

static Tree * closest = NULL;

static int * acquire_file(char * filename){
	unsigned int i;
	int * numbers;
	FILE * file = fopen(filename, "r");

	if (!file){
		printf("fichier inexistant\n");
		exit(1);
	}

	fscanf(file, "%d %d", &N, &p);
	numbers = malloc(p * sizeof(int));

	for(i = 0; i < p; ++i){
		fscanf(file, "%d", &numbers[i]);

		if(!numbers[i]){
			numbers[i] = Reserve[random()%14];
    }
	}

	fclose(file);
	return numbers;
}

static int * acquire_command(int l, char ** Ligne){
	int * numbers;
	unsigned int i;

	N = atoi(Ligne[1]);
	p = atoi(Ligne[2]);
	numbers = malloc(p * sizeof(int));

	for(i = 3; (i < l) & (i < p + 3); ++i)
		numbers[i - 3] = atoi(Ligne[i]);

	while(i < p + 3)
	{
		numbers[i - 3] = Reserve[random() % 14];
		++i;
	}

	return numbers;
}

static int eval(Tree * P1, Tree * P2, char operator){
	int result = 0;
	int p1, p2;

	if (P1 && P2){
		p1 = P1->result;
		p2 = P2->result;
	}
  else {
    return -1;
  }

	switch (operator){
		case '+':
			result = p1 + p2;
			break;
		case '*':
			result = p1 * p2;
			break;
		case '-':
			if (p1 > p2) result = p1 - p2;
			else result = p2 - p1;
			break;
		case '/':
			if (p1 > p2) result = p1 / p2;
			else result = p2 / p1;
			break;
		default :
			break;
	}

	return result;
}

static Tree * new_link(int result, char operateur){
	Tree * root;

	root = malloc(sizeof(Tree));
	root->result = result;
	root->operator = operateur;
	root->left = NULL;
	root->right = NULL;

	return root;
}

static void copy(Tree * Arbre1, Tree ** Arbre2){
	if(Arbre1 == *Arbre2) return;

	if(Arbre1){
		if(!*Arbre2){
			*Arbre2 = new_link(Arbre1->result, Arbre1->operator);
			copy(Arbre1->left, &((*Arbre2)->left));
			copy(Arbre1->right, &((*Arbre2)->right));
		}
		else {
			(*Arbre2)->result = Arbre1->result;
			(*Arbre2)->operator = Arbre1->operator;
			copy(Arbre1->left, &((*Arbre2)->left));
			copy(Arbre1->right, &((*Arbre2)->right));
		}
	}
}

static void compare(Tree * Arbre){
	if(Arbre){
		if(abs(Arbre->result - N) < abs(closest->result - N) || closest->result < 0){
			copy(Arbre, &closest);
    }
  }
}

static Tree * fusion(Tree * P1, Tree * P2, char operateur){
	Tree * link = new_link(eval(P1, P2, operateur), operateur);

	link->left = P1;
	link->right = P2;

	return link;
}

static Tree ** prepare(Tree ** arbres, int i, int j, int * taille, char operateur){
	Tree * tmp;

	tmp = arbres[i];
	arbres[j] = fusion(arbres[i], arbres[j], operateur);
	arbres[i] = arbres[*taille-1];
	arbres[*taille - 1] = tmp;
	--(*taille);

	return arbres;
}

static Tree ** Defaire(Tree ** arbres, int i, int j, int * taille){
	Tree * tmp;

	tmp = arbres[*taille];
	arbres[*taille] = arbres[i];
	arbres[i] = tmp;
	tmp = arbres[j];

	if(tmp){
		arbres[j] = tmp->right;
		++(*taille);
	}

	return arbres;
}

static int cover(Tree ** arbres, int taille){
	int i, j, k, resultat;
	int p1, p2;
	resultat = -1;
	for(i = 0; i < taille; ++i){
		for(j = 0; j < i; ++j){
			for(k = 0; k < 3; ++k){
				compare(arbres[i]);
				compare(arbres[j]);
				prepare(arbres, i, j, &taille, operators[k]);
				compare(arbres[j]);
				resultat = closest->result;

				if(resultat == N)
					break;
				cover(arbres, taille);
				Defaire(arbres, i, j, &taille);
			}

			if(arbres[i] && arbres[j]){
				p1=arbres[i]->result;
				p2=arbres[j]->result;
			}

			if(resultat != N && p1 && p2 && !(p1 % p2 && p2 % p1)){
				compare(arbres[i]);
				compare(arbres[j]);
				prepare(arbres, i, j, &taille, operators[3]);
				compare(arbres[j]);
				resultat=closest->result;

				if(resultat == N)
					break;
				cover(arbres, taille);
				Defaire(arbres, i, j, &taille);
			}

			compare(arbres[i]);
			compare(arbres[j]);
			resultat = closest->result;

			if(resultat == N)
				break;

		}
		compare(arbres[i]);
		resultat = closest->result;

		if(resultat == N)
			break;
	}

	return resultat;
}

static Tree ** transform(int * nombres){
	Tree ** Arbres;
	unsigned int i;

	Arbres = malloc(sizeof(Tree *) * p);
	for(i = 0; i < p; ++i)
		Arbres[i] = new_link(nombres[i], 'o');

	return Arbres;
}

static void print(Tree * Arbre){
	if(Arbre){
		if(Arbre->operator != 'o'){
			if(Arbre->operator == '*'){
				print(Arbre->left);
				printf(" %c ", Arbre->operator);
				print(Arbre->right);
			}
			else {
				printf("(");

				if(Arbre->left->result > Arbre->right->result) {
					print(Arbre->left);
					printf(" %c ", Arbre->operator);
					print(Arbre->right);
				}
				else {
					print(Arbre->right);
					printf(" %c ", Arbre->operator);
					print(Arbre->left);
				}

				printf(")");
			}
		}
		else {
			printf("%d", Arbre->result);
		}
	}
}


int main(int argc, char * argv[]){
	static int * numbers;
	unsigned int i;
	Tree ** Arbres;

	closest = new_link(-1, 'f');
	srand(time(NULL));

	//Usage
	if(argc < 2) {
		printf("Usage: %s filename or %s N p [Int_list]\n", argv[0], argv[0]);
		return 0;
	}

	/* Acquisition */

	//From a file:
	if (argc == 2) {
		numbers = acquire_file(argv[1]);
	}

	//From the CL:
	if (argc > 2) {
		numbers = acquire_command(argc,argv);
	}

	//End of acquisition
	Arbres = transform(numbers);
	cover(Arbres, p);
	printf("Target: %d\n", N);
	printf("Available numbers: ");

	for(i = 0; i < p; ++i){
		printf("%d ", numbers[i]);
  }
	printf("\nResult:\n");
	print(closest);
	printf(" = %d\n", closest->result);

	return 0;
}

