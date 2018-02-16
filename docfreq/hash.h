#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
// #include <mpi.h>


typedef struct docnode
{
	char* name;
	int freq;
	struct docnode* next;
}docnode;

typedef docnode* docNode;

typedef struct stopword
{
	char* key;
	struct stopword* next;
}stopword;

typedef stopword* stopWord;

typedef struct worddoc{
	char* key;
	struct worddoc* next;
	int freq;
}worddoc; 
typedef worddoc* wordDoc;


typedef struct wordnode{
	char* key;
	struct wordnode* next;
	docNode docLink; 
	int totalfreq;
	int size;
}wordnode; 

typedef wordnode* wordNode;

int hash(char *str,int m);

wordDoc newWordDoc(char* key);

wordNode newWord(char* key);

docNode newDoc(char* name,int freq);

wordDoc addWordDoc(char* key,wordDoc head);

void printWordDoc(wordDoc* ht,int m);

wordNode* wordDoctowordNode(wordDoc* ht,char* name,int m,wordNode* lht);

wordNode addWordNodes(wordDoc doc,wordNode head,char* docName);

void printWords(wordNode* lht,int m);

docNode addDoc(char* name,int freq,docNode head);

wordDoc* fill_ht(char* name,int m,wordDoc* ht,stopWord *sw);

wordDoc* free_ht(wordDoc* ht,int m);

stopWord* makeStopWords(char* fname, int m);

int checkStopWord(char* key,int hash, stopWord* sw);