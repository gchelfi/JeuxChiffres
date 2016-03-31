#include <stdio.h>
#include <stdlib.h>
#include <time.h>


static int N = 0;
static int p = 0;
static int Defaults[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 25, 50, 75, 100};
static char operators[] = {'+', '-', '*', '/'};


typedef struct Node{
  int result;
  char operator;
  struct Node * left, * right;
} Tree; // The type of an expression

static Tree * closest = NULL; // Best solution so far

static int * acquire_from_file(char * filename){
	int * numbers;
	FILE * file = fopen(filename, "r");

	if (!file){
		printf("File not found: %s\n", filename);
		exit(1);
	}

	fscanf(file, "%d %d", &N, &p);
	numbers = malloc(p * sizeof(int));

	for(unsigned int i = 0; i < p; ++i){
		fscanf(file, "%d", &numbers[i]);

		if(!numbers[i]){
			numbers[i] = Defaults[random() % 14];
    }
	}

	fclose(file);
	return numbers;
}

static int * acquire_from_command(int l, char ** line){
	int * numbers;
	unsigned int i;

	N = atoi(line[1]);
	p = atoi(line[2]);
	numbers = malloc(p * sizeof(int));

	for(i = 3; (i < l) & (i < p + 3); ++i)
		numbers[i - 3] = atoi(line[i]);

	while(i < p + 3)
	{
		numbers[i - 3] = Defaults[random() % 14];
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

static Tree * new_tree(int result, char operator){
	Tree * root = malloc(sizeof(Tree));
	root->result = result;
	root->operator = operator;
	root->left = NULL;
	root->right = NULL;

	return root;
}

static void write(Tree * from, Tree ** to){
	if(from == *to) return;

	if(from){
		if(!*to){
			*to = new_tree(from->result, from->operator);
		}
		else {
			(*to)->result = from->result;
			(*to)->operator = from->operator;
    }
			write(from->left, &((*to)->left));
			write(from->right, &((*to)->right));
	}
}

static void compare_with_closest(Tree * candidate){
	if(candidate){
		if(abs(candidate->result - N) < abs(closest->result - N) || closest->result < 0){
			write(candidate, &closest);
    }
  }
}

static Tree * fusion(Tree * P1, Tree * P2, char operateur){
	Tree * link = new_tree(eval(P1, P2, operateur), operateur);

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

static Tree ** backtrack(Tree ** arbres, int i, int j, int * taille){
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

// Main part of the algorithm: takes an initial list of candidates, pick any two
// elements, combine them. Process recursively while remembering the best
// combination so far. As soon as a combination has been solved, deconstruct it
// and build the next one.
static int cover(Tree ** arbres, int taille){
	int i, j, k, resultat;
	int p1, p2;
	resultat = -1;
	for(i = 0; i < taille; ++i){
		for(j = 0; j < i; ++j){
			for(k = 0; k < 3; ++k){
				compare_with_closest(arbres[i]);
				compare_with_closest(arbres[j]);
				prepare(arbres, i, j, &taille, operators[k]);
				compare_with_closest(arbres[j]);
				resultat = closest->result;

				if(resultat == N)
					break;
				cover(arbres, taille);
				backtrack(arbres, i, j, &taille);
			}

			if(arbres[i] && arbres[j]){
				p1=arbres[i]->result;
				p2=arbres[j]->result;
			}

			if(resultat != N && p1 && p2 && !(p1 % p2 && p2 % p1)){
				compare_with_closest(arbres[i]);
				compare_with_closest(arbres[j]);
				prepare(arbres, i, j, &taille, operators[3]);
				compare_with_closest(arbres[j]);
				resultat = closest->result;

				if(resultat == N)
					break;
				cover(arbres, taille);
				backtrack(arbres, i, j, &taille);
			}

			compare_with_closest(arbres[i]);
			compare_with_closest(arbres[j]);
			resultat = closest->result;

			if(resultat == N)
				break;

		}
		compare_with_closest(arbres[i]);
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
		Arbres[i] = new_tree(nombres[i], 'o');

	return Arbres;
}

static void pretty_print(Tree * tree){
	if(tree){
		if(tree->operator != 'o'){
			if(tree->operator == '*'){
				pretty_print(tree->left);
				printf(" %c ", tree->operator);
				pretty_print(tree->right);
			}
			else {
				printf("(");

				if(tree->left->result > tree->right->result) {
					pretty_print(tree->left);
					printf(" %c ", tree->operator);
					pretty_print(tree->right);
				}
				else {
					pretty_print(tree->right);
					printf(" %c ", tree->operator);
					pretty_print(tree->left);
				}

				printf(")");
			}
		}
		else {
			printf("%d", tree->result);
		}
	}
}


int main(int argc, char * argv[]){
	static int * numbers;
	unsigned int i;
	Tree ** Arbres;

	closest = new_tree(-1, 'f');
	srand(time(NULL));

	//Usage
	if(argc < 2) {
		printf("Usage: %s filename or %s N p [Int_list]\n", argv[0], argv[0]);
		return 0;
	}

	/* Acquisition */

	//From a file:
	if (argc == 2) {
		numbers = acquire_from_file(argv[1]);
	}

	//From the CL:
	if (argc > 2) {
		numbers = acquire_from_command(argc,argv);
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
	pretty_print(closest);
	printf(" = %d\n", closest->result);

	return 0;
}

