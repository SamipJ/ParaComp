#include "hash.h"
#include <stdlib.h>


int hash(char *str,int m)
{
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return (int)(hash%m);
}

wordDoc newWordDoc(char* key){
	wordDoc temp=(wordDoc) malloc(sizeof(worddoc));
	temp->key=(char*)malloc((1+strlen(key))*sizeof(char));
	strcpy(temp->key,key);
	temp->next=NULL;
	temp->freq=0;
	return temp;
} 

wordNode newWord(char* key){
	wordNode temp=(wordNode)malloc(sizeof(wordnode));
	temp->key=key;
	temp->docLink=NULL;
	temp->next=NULL;
	temp->totalfreq=0;
	temp->size=0;
	return temp;
}


docNode newDoc(char* name,int freq){
	docNode temp=(docNode)malloc(sizeof(docnode));
	temp->name=name;
	// temp->nextDoc=NULL;
	temp->freq=freq;
	return temp;
}

wordDoc addWordDoc(char* key,wordDoc head){
	wordDoc temp=head,temp2=NULL;
	if(temp==NULL){
		temp=newWordDoc(key);
		temp->freq=1;
		head=temp;
		return head;
	}
	else{
		int i=0;
		i=strcmp(temp->key,key);
		if(i==0){
			temp->freq++;
			return head;
		}
		else if(i>0){
			temp2=newWordDoc(key);
			temp2->freq=1;
			temp2->next=temp;
			head=temp2;
			return head;
		}
		else{
			while(temp->next!=NULL){
				i=strcmp(temp->key,key);
				if(i==0){
					temp->freq++;
					return head;
				}
				else if(i>0){
					temp2->next=newWordDoc(key);
					temp2=temp2->next;
					temp2->freq=1;
					temp2->next=temp;
					return head;
				}
				temp2=temp;
				temp=temp->next;
			}

			i = strcmp(temp->key,key);
			if(i==0){
				temp->freq++;
				return head;
			}
			else if(i>0){
				temp2->next=newWordDoc(key);
				temp2=temp2->next;
				temp2->freq=1;
				temp2->next=temp;
				return head;				
			}
			else{
				temp->next = newWordDoc(key);
				temp=temp->next;
				temp->freq =1;
				return head;
			}

		}
	}
}

void printWordDoc(wordDoc* ht,int m){
	int i,count=0,strmax=0,strcount=0;
	wordDoc temp;
	for(i=0;i<m;i++){
		if(ht[i]==NULL) continue;
		count++;		
		temp = ht[i];
		strcount=0;
		while(temp!=NULL){
			printf("%d : %s %d\n",i,temp->key,temp->freq);
			temp=temp->next;
			strcount++;
		}
		strmax=(strmax>strcount)?strmax:strcount;
	}
	printf("Total Load :%f\tstrmax %d\n",((float)count)/m,strmax );
}





wordNode addWordNodes(wordDoc doc,wordNode head,char* docName){
	wordNode temp=head,temp2=NULL;
	wordDoc tempDoc=doc;
	int i;
	if(temp==NULL){
		temp2=newWord(tempDoc->key);
		temp2->docLink=addDoc(docName,tempDoc->freq,temp2->docLink);
		temp2->next=temp;
		temp2->totalfreq=tempDoc->freq;
		temp2->size=1;
		head=temp2;
		temp2=NULL;
		temp=head;
		tempDoc=tempDoc->next;
	}
	else{
		i=strcmp(temp->key,tempDoc->key);
		if(i==0){
			temp->docLink=addDoc(docName,tempDoc->freq,temp->docLink);
			temp->totalfreq+=tempDoc->freq;
			temp->size++;
			tempDoc=tempDoc->next;
			temp2=temp;
			temp=temp->next;
		}
		else if(i>0){
			temp2=newWord(tempDoc->key);
			temp2->docLink=addDoc(docName,tempDoc->freq,temp2->docLink);
			temp2->next=temp;
			temp2->totalfreq=tempDoc->freq;
			temp2->size=1;
			head=temp2;
			tempDoc=tempDoc->next;

		}
		else{
			temp2=temp;
			temp=temp->next;
		}
	}
	while(tempDoc!=NULL){
		if(temp==NULL){
			temp2->next=newWord(tempDoc->key);
			temp2=temp2->next;
			temp2->docLink=addDoc(docName,tempDoc->freq,temp2->docLink);
			temp2->totalfreq=tempDoc->freq;
			temp2->size=1;
		}
		else{
			int flag=0;
			while(temp!=NULL){
				i=strcmp(temp->key,tempDoc->key);
				if(i==0){
					temp->docLink=addDoc(docName,tempDoc->freq,temp->docLink);
					temp->totalfreq+=tempDoc->freq;
					temp->size++;
					temp2=temp;
					temp=temp->next;
					flag=1;
					break;
				}
				else if(i>0){
					temp2->next=newWord(tempDoc->key);
					temp2=temp2->next;
					temp2->docLink=addDoc(docName,tempDoc->freq,temp2->docLink);
					temp2->totalfreq=tempDoc->freq;
					temp2->size=1;
					temp2->next=temp;
					flag=1;
					break;
				}
				else{
					temp2=temp;
					temp=temp->next;
				}
			}
			if(flag==0){
				continue;
			}
		}
		tempDoc=tempDoc->next;
	}	
	return head;		
}

wordNode* wordDoctowordNode(wordDoc* ht,char* name,int m,wordNode* lht){
	int i=0;
	for(i=0;i<m;i++){
		if (ht[i]==NULL){
			continue;
		}
		lht[i]=addWordNodes(ht[i],lht[i],name);
	}
	return lht;
}

docNode addDoc(char* name,int freq,docNode head){
	return head;
}

void printWords(wordNode* lht,int m){
	int i,count=0,strmax=0,strcount=0;
	wordNode temp;
	for(i=0;i<m;i++){
		if(lht[i]==NULL) continue;
		count++;		
		temp = lht[i];
		strcount=0;
		while(temp!=NULL){
			printf("%d : %s %d\n",i,temp->key,temp->totalfreq);
			temp=temp->next;
			strcount++;
		}
		strmax=(strmax>strcount)?strmax:strcount;
	}
	printf("Total Load :%f\tstrmax %d\n",((float)count)/m,strmax );
}

// wordNode addWord(char* key,char* name,wordNode head){
// 	int i=0;
// 	wordNode temp=head;
// 	wordNode temp2;
// 	if(temp==NULL){
// 		temp=newWord(key);
// 		temp->docLink=addDoc(name,NULL);
// 		temp->count=1;
// 		temp->index=1;
// 		head=temp;
// 		return head;
// 	}
// 	else{
// 		temp2=NULL;
// 		if(strcmp(key,temp->key)<0){
// 			temp2=newWord(key);
// 			temp2->docLink=addDoc(name,NULL);
// 			temp2->count=1;
// 			temp2->nextWord=temp;
// 			head=temp2;
// 			return head;
// 		}
// 		while(temp->nextWord!=NULL){
// 			i=strcmp(key,temp->key);
// 			if(i==0){
// 				temp->docLink=addDoc(name,temp->docLink);
// 				temp->count++;
// 				free(key);
// 				return head;
// 			}
// 			else if(i<0){
// 				temp2->nextWord=newWord(key);
// 				temp2=temp2->nextWord;
// 				temp2->docLink=addDoc(name,NULL);
// 				temp2->count=1;
// 				temp2->nextWord=temp;
// 				return head;
// 			}
// 			temp2=temp;	
// 			temp=temp->nextWord;

// 		}
// 		i=strcmp(key,temp->key);
// 		if(i==0){
// 			temp->docLink=addDoc(name,temp->docLink);
// 			temp->count++;
// 			free(key);
// 			return head;
// 		}
// 		else if(i<0){
// 			temp2->nextWord=newWord(key);
// 			temp2=temp2->nextWord;
// 			temp2->nextWord=temp;
// 			temp2->docLink=addDoc(name,NULL);
// 			temp2->count=1;
// 			return head;
// 		}
// 		else{
// 			temp->nextWord=newWord(key);
// 			temp=temp->nextWord;
// 			temp->docLink=addDoc(name,NULL);
// 			temp->count=1;
// 			return head;
// 		}
// 	}

// }


// docNode addDoc(char* name, docNode head){
// 	docNode temp=head;
// 	if(temp==NULL){
// 		temp=newDoc(name);
// 		temp->count=1;
// 		head=temp;
// 		return head;
// 	}
// 	else{
// 		while(temp->nextDoc!=NULL)
// 		{
// 			if(strcmp(name,temp->name)==0){
// 				temp->count++;
// 				free(name);
// 				return head;
// 			}
// 			temp=temp->nextDoc;
// 		}
// 		if(strcmp(temp->name,name)==0){
// 			temp->count++;
// 			free(name);
// 			return head;
// 		}
// 		temp->nextDoc=newDoc(name);
// 		temp=temp->nextDoc;
// 		temp->count=1;
// 		return head;
// 	}
// }




int main(){
	FILE* fp;
	fp=fopen("input","r");
    char * line = NULL,*saveptr1,*str1,*token,*p;
    size_t len = 0;
    ssize_t read;
    int j,k;
    if (fp == NULL)
        exit(EXIT_FAILURE);
    int m= 1049;
    wordDoc* ht = (wordDoc*)malloc(sizeof(wordDoc)*m);
    while ((read = getline(&line, &len, fp)) != -1) {
        // printf("Retrieved line of length %zu :\n", read);
        // printf("%s", line);
    	for (j = 1, str1 = line; ; j++, str1 = NULL) {
	        token = strtok_r(str1," ;:\"'1234567890-(\t)[]{}!$%^*&=_+/.@<>?/,0+\\|\n", &saveptr1);
            if (token == NULL)
            	break;
           	// printf("%d: %s %ld\n", j, token,strlen(token));
           	p=token;
           	for ( ; *p; ++p) *p = tolower(*p);
           	k=(hash(token,m));
           	ht[k]=addWordDoc(token,ht[k]);
	    }
	}
    printWordDoc(ht,m);
    // printf("%s\n",token);
    wordNode* lht=(wordNode*)malloc(m*sizeof(wordNode));
    // lht=wordDoctowordNode(ht,"112",m,lht);
    // printWords(lht,m);

    fclose(fp);
    if (line)
        free(line);
    exit(EXIT_SUCCESS);
	return 0;
}
	