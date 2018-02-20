#include "hash.h"

char* isalphabet(char* temp){
	char* p;
	for ( p=temp; *p; ++p) if(!isalpha(*p)) *p=' ';
	return temp;
}

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
	temp->next=NULL;
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
			if(tempDoc->key==NULL){
				printf("hi\n");
			}
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
	docNode temp=head,temp2=NULL;
	if(temp==NULL){
		temp=newDoc(name,freq);
		head=temp;
		return head;
	}
	else{
		if(temp->freq<=freq){
			temp2=newDoc(name,freq);
			temp2->next=temp;
			head=temp2;
			return head;
		}
		else{
			temp2=temp;
			temp=temp->next;
		}
		while(temp!=NULL){
			if(temp->freq<=freq){
				temp2->next=newDoc(name,freq);
				temp2=temp2->next;
				temp2->next=temp;
				return head;
			}
			temp2=temp;
			temp=temp->next;
		}
		temp2->next=newDoc(name,freq);
		return head;
	}
	return head;
}

void printWords(wordNode* lht,int m){
	int i,count=0,strmax=0,strcount=0;
	wordNode temp;
	docNode tempDoc;
	for(i=0;i<m;i++){
		if(lht[i]==NULL) continue;
		count++;		
		temp = lht[i];
		strcount=0;
		while(temp!=NULL){
			printf("%d : %s %d\nSplit:\n",i,temp->key,temp->totalfreq);
			tempDoc=temp->docLink;
			while(tempDoc!=NULL){
				printf("\t%s-->%d\n",tempDoc->name,tempDoc->freq);
				tempDoc=tempDoc->next;
			}
			temp=temp->next;
			strcount++;
		}
		strmax=(strmax>strcount)?strmax:strcount;
	}
	printf("Total Load :%f\tstrmax %d\n",((float)count)/m,strmax );
}

wordDoc* fill_ht(char* name,int m,wordDoc* ht,stopWord* sw){
	char * line = NULL,*saveptr1,*str1,*token,*p;
	size_t len = 0;
	ssize_t read;
	int j,k;
	FILE* entry_file = fopen(name, "r");
	if (entry_file == NULL)
	{
		fprintf(stderr, "Error : Failed to open entry file\n");
		return NULL;
		// return 1;
	}
	for (j=0;j<m;j++){
		ht[j]=NULL;
	}
	while ((read = getline(&line, &len, entry_file)) != -1) {
		line = isalphabet(line);
		for (j = 1, str1 = line; ; j++, str1 = NULL) {
			token = strtok_r(str1," ;:\"'1234567890-(\t)”[]{}!$%^*&=_+/.@<>?/,0+\\|\n", &saveptr1);
			if (token == NULL)
				break;
			for ( p=token; *p; ++p) *p = tolower(*p);
			k=(hash(token,m));
			if(checkStopWord(token,k,sw)==0){
				ht[k]=addWordDoc(token,ht[k]);
			}
			
		}
	}
	fclose(entry_file);
	return ht;
}

wordNode* fill_lht(int m,wordNode* lht,char* dirname,char* procid){
    wordDoc* ht = (wordDoc*)malloc(sizeof(wordDoc)*m);	
	int i;
	stopWord* sw=makeStopWords("stop",m);
	for(i=0;i<m;i++) ht[i]=NULL;
    DIR* FD;
    struct dirent* in_file;
    FILE    *output_file;
    FILE    *entry_file;
    char* name=(char*)malloc(256*sizeof(char));
	char* filename;
    char* file = (char*)malloc((strlen(dirname)+1)*sizeof(char));
	char* dirslash =(char*)malloc(sizeof(char)*(strlen(dirname+2)));
	strcpy(file,dirname);
	strcpy(dirslash,dirname);
	strcat(dirslash,"/");
    if (NULL == (FD = opendir (file))) 
    {	
        fprintf(stderr, "Error : Failed to open input directory\n");
        return NULL;
    }
    while ((in_file = readdir(FD))) 
    {

		strcpy(file,dirslash);
        if (!strcmp (in_file->d_name, "."))
            continue;
        if (!strcmp (in_file->d_name, ".."))    
            continue;
    	name=strcat(file,in_file->d_name);
		filename=(char*)malloc(sizeof(char)*(strlen(in_file->d_name)+5));
		strcpy(filename,in_file->d_name);
		strcat(filename,procid);
    	// printf("%s\n",file );
  		// printf("%s\n",in_file->d_name);
        ht = fill_ht(name,m,ht,sw);
		lht=wordDoctowordNode(ht,filename,m,lht);
		ht = free_ht(ht,m);
    }
	free(ht);
	return lht;
}

wordDoc* free_ht(wordDoc* ht,int m){
	int i;
	wordDoc temp,temp2;
	for (i=0;i<m;i++){
		if(ht[i]!=NULL){
			temp=ht[i];
			while(temp!=NULL){
				temp2=temp;
				temp=temp->next;
				free(temp2);
			}
			ht[i]=NULL;
		}
	}
	return ht;
}

void free_lht(wordNode* lht,int m){
	int i;
	wordNode temp,temp2;
	docNode doct,doct2;
	for (i=0;i<m;i++){
		if(lht[i]!=NULL){
			while(temp!=NULL){
				temp2=temp;
				temp=temp->next;
				doct=temp2->docLink;
				while(doct!=NULL){
					doct2=doct;
					doct=doct->next;
					free(doct2);
				}
				free(temp2);
			}
		}
	}
}

stopWord* makeStopWords(char* fname, int m){
	stopWord* ht = (stopWord*)malloc(m*sizeof(stopWord));
	int i;
	for(i=0;i<m;i++){
		ht[i]=NULL;
	} 
	FILE* fp = fopen(fname,"r");
	if (fp == NULL)
	{
		fprintf(stderr, "Error : Failed to open entry file\n");
		return NULL;
		// return 1;
	}
	char * line = NULL,*saveptr1,*str1,*token,*p;
	size_t len = 0;
	ssize_t read;
	stopWord temp;
	char* key;
	int j,k;
	while ((read = getline(&line, &len, fp)) != -1) {
		line = isalphabet(line);
		for (j = 1, str1 = line; ; j++, str1 = NULL) {
			token = strtok_r(str1," ;:\"'1234567890-(\t)[]{}”!$%^*&=_+/.@<>?/,0+\\|\n", &saveptr1);
			if (token == NULL)
				break;
			for ( p=token; *p; ++p) *p = tolower(*p);
			k=hash(token,m);
			key=malloc(sizeof(char)*(strlen(token)+1));
			strcpy(key,token);
			temp = (stopWord)malloc(sizeof(stopword));
			temp->key=key;
			temp->next=ht[k];
			ht[k]=temp;
			// ht[k]=(token,ht[k]);
			
		}
	}
	fclose(fp);
	return ht;
}

int checkStopWord(char* key,int hash, stopWord* sw){
	if(sw[hash]==NULL){
		return 0;
	}
	else{
		stopWord temp;
		temp=sw[hash];
		while(temp!=NULL){
			if(strcmp(temp->key,key)==0){
				return 1;
			}
			temp=temp->next;
		}
		return 0;
	}
}

// int main(){
    
//     int m= 5000;
//     // wordDoc* ht;
//     wordNode* lht=(wordNode*)malloc(m*sizeof(wordNode));
// 	char* dirname =(char*)malloc(sizeof(char)*10);
// 	char* procid =(char*)malloc(sizeof(char)*5);
// 	char* pc=(char*)malloc(sizeof(char)*3);
// 	int p=1;
// 	sprintf(pc,"%d",p);
// 	char* sqo =(char*)malloc(sizeof(char)*2);
// 	strcpy(procid,strcat(strcat(strcpy(sqo,"["),pc),"]"));
// 	strcpy(dirname,"files");
// 	lht = fill_lht(m,lht,dirname,procid);
//     // printWords(lht,m);
// 	// free_lht(lht,m);
// 	// free(lht);			
// 	return 0;
// }
	