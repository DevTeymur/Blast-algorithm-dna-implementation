#include<stdio.h>
#include<string.h>
#include<stdlib.h>

// Returns the number of the letters in chain
// file - pointer to the file
// sStringToFind - string which located at the beginning of the chain
// returns - an integer which is the size of the chain
int fReturnSizeOfChain(FILE *file, char *sStringToFind);

// Function to implement Blast algorithm
// s1 - pattern to compare
// s2 - pattern taken from chain
// returns - pointer to the integer that consists the local position and the best score
int* fReturnBlastResults(char *s1, char *s2);

// Compares given pattern with pattern that taken from chain
// c1 - pattern to compare
// c2 - pattern taken from chain
// returns - pointer to the char ex: "0101011101101"
char* fReturnCorrArray(char *c1, char *c2);

// Adds new results to the beginning of the linked list
// globalPos - position of element in sequence
// localPos - position of local minimum inside pattern
// score - score of the best match
void fAddFirst(int globalPos, int localPos, int score);

// Prints first best match case from dna chain
// score - best score gotten from the algorithm
void fPrintFirstBestMatch(int score);

// Prints all elements of the linked list with best socore
// score - best score gotten from the algorithm
void fPrintList(int score);

struct linkedList{
    int nGlobalPos;
    int nLocalPos;
    int nBestScore;
    struct linkedList *next;
};

struct linkedList *head = NULL;


void fAddFirst(int globalPos, int localPos, int score){
    struct linkedList *temp = (struct linkedList*) malloc(sizeof(struct linkedList));
    temp->nGlobalPos=globalPos;
    temp->nLocalPos=localPos;
    temp->nBestScore=score;
    temp->next = head;
    head = temp;
}

void fPrintList(int score){
    struct linkedList *plist = head;
    printf("\n-------- All best scores -------\n");
    while(plist != NULL) {
        if(plist->nBestScore == score){
            printf("Position in sequence=%d\nPosition in word=%d\nBest Score=%d\n", plist->nGlobalPos, plist->nLocalPos, plist->nBestScore);
            printf("__________________________\n");
            plist = plist->next;
        }
        else{
            break;
        }
    }
}


void fPrintFirstBestMatch(int score){
    struct linkedList *plist = head;
    while(plist != NULL){
        if(plist->nBestScore == score && plist->next->nBestScore != score){
            printf("Position in sequence=%d\nPosition in word=%d\nBest Score=%d\n", plist->nGlobalPos, plist->nLocalPos, plist->nBestScore);
            break;
        }
        plist = plist->next;
    }
}


int fReturnSizeOfChain(FILE *file, char *sStringToFind){
    int nStringFound = 0;
    int nSize = 0;
    char pLine[75];

    while (fgets(pLine, sizeof(pLine), file)){
        if(nStringFound==0){
            if(strstr(pLine, sStringToFind) != NULL) nStringFound=1;
        }else{
            int i=0;
            while(pLine[i] != '\0'){
                if(pLine[i] == 'a' || pLine[i] == 'c' || pLine[i] == 'g' || pLine[i] == 't'){ 
                    nSize++; 
                }
                i++;
            }
        }
    }

    if(nStringFound==0){
        fprintf(stderr, "The required string is not found inside file, so nothing wrote from file\n");
        return -1;
    }

    fseek(file, 0, SEEK_SET); // setting cursor to the beginning of the file as we'll loop the file again
    return nSize;
}


char *fReturnCorrArray(char *c1, char *c2){
    char *pArrOfZeroOnes = (char*)malloc(sizeof(char)*strlen(c2));
    if(pArrOfZeroOnes == NULL){ 
        fprintf(stderr, "fReturnCorrArray: An error occured while creating array\n"); 
        exit(6);
    }
    for (int i = 0; i < strlen(c2); i++){
        if (c1[i] == c2[i]) pArrOfZeroOnes[i] = '1';
        else pArrOfZeroOnes[i] = '0';
        }
    return pArrOfZeroOnes;
}


int* fReturnBlastResults(char *s1, char *s2){
    int nLocalPos, nTempPos, nCumSum, nTempScore, nLocalMax, nLocalMin, nCurr;
    nLocalPos = 0;
    nCurr = 0;
    nTempPos = 0;
    nCumSum = 0;
    nTempScore = 0;
    nLocalMax = 0;
    nLocalMin = 0;
    int i = 0;
    char *pCorr = fReturnCorrArray(s1, s2);
    int *pArr2 = (int *)malloc(sizeof(int) * 2);
    if(pArr2 == NULL){ 
        fprintf(stderr, "fReturnBlastResults: An error occured while creating array\n"); 
        exit(7);
    }
    while (i < strlen(s2)){
        if (pCorr[i] == '0') nCurr--;
        else nCurr++;
        if (nCurr < nLocalMin){
            nLocalMin = nCurr;
            nTempPos = i+1;
        }
        else if (nCurr > nLocalMin){
            nLocalMax = nCurr;
            nTempScore = nLocalMax - nLocalMin;
        }
        if (nCumSum < nTempScore){
            nCumSum = nTempScore;
            nLocalPos = nTempPos;
        }
        i++;
    }
    pArr2[0] = nCumSum;
    pArr2[1] = nLocalPos;
    return pArr2;
}


int main(int argc, char *argv[]){
    char *sPattern;
    char *sFileName;
    if(argc == 2 && strcmp(argv[1], "-help")==0){
        printf("Please provide either filename and pattern or just filename itself.\nExmp 1: ./a.out filename pattern\n");
        printf("Exmp 2: ./a.out filename\n\nFor second case you are going to input pattern.\n");
        return 0;
    }
    if(argc == 3){
        sFileName = argv[1];
        sPattern = argv[2];
    }
    if(argc==2 && strcmp(argv[1], "-help")!=0){
        sFileName = argv[1];
        printf("Please enter a string to find in the file: ");
        sPattern = (char *)calloc(256, sizeof(char));
        scanf("%[^\n]%*c", sPattern);
    }

    FILE *file = fopen(sFileName, "r");
    if(file == NULL){ 
        fprintf(stderr, "Error while opening the file\n");
        exit(1);
    }

    char sStringToFind[10] = "ORIGIN";
    int nStringFound = 0;
    char pLine[75];

    int nSizeOfDna = fReturnSizeOfChain(file, sStringToFind);
    if(nSizeOfDna==-1){
        exit(5);
    }

    char *dnaArr = malloc(nSizeOfDna);
    if(dnaArr == NULL){ 
        fprintf(stderr, "An error occured while creating array\n"); 
        exit(2);
    }
    int arrCursor = 0;

    // storing chain to dnaArr
    nStringFound=0;
    while (fgets(pLine, sizeof(pLine), file)){
        if(nStringFound==0){
            if(strstr(pLine, sStringToFind) != NULL) nStringFound=1; // now we are at the beginning of the chain
        }else{
            int i=0;
            while(pLine[i] != '\0'){
                if(pLine[i] == 'a' || pLine[i] == 'c' || pLine[i] == 'g' || pLine[i] == 't'){ 
                    dnaArr[arrCursor] = pLine[i];
                    arrCursor++; 
                }
                i++;
            }
        }
    }

    if(nStringFound==0){
        fprintf(stderr, "The required string is not found inside file, so nothing wrote from file\n");
        exit(3);
    }

    fclose(file);

    int nPatternSize = strlen(sPattern)+1;

    int nBestScore, nLocalPos, nGlobalPos;
    nBestScore = 0;
    char pTempChain[nPatternSize];
    
    // Comparing pattern with every subchain
    for(int i = 0; i<nSizeOfDna-nPatternSize+1; i++){
        for(int j=0; j<nPatternSize; j++){
            pTempChain[j] = dnaArr[j+i];
        }

        int *res = fReturnBlastResults(pTempChain, sPattern);
        if (res[0] >= nBestScore){
            nGlobalPos = i;
            nBestScore = res[0];
            nLocalPos = res[1];
        }
        if(res[0] == nBestScore){  
            fAddFirst(nGlobalPos, res[1], res[0]); // adding all current best scores to linked list
        }
    }

    fPrintFirstBestMatch(nBestScore);
    
    fPrintList(nBestScore);

    free(dnaArr);

    return 0;
}