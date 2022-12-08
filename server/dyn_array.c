#include "dyn_array.h"

int da_init(pDYN_ARRAY p_daArray, int iSize){
    p_daArray->array = NULL;
    p_daArray->size = 0;

    if(NULL == (p_daArray->array =(void **)calloc((size_t) iSize, sizeof(void *)))){
        return 1;
    }
    p_daArray->size = iSize;

    for(int i=0; i<iSize; i++){
        p_daArray->array[i] = NULL;
    }
    return 0;
}

void * da_get(DYN_ARRAY daArray, int iIndex){
    if(iIndex >= daArray.size){
        return NULL;
    }
    return daArray.array[iIndex];

}

int da_insert(pDYN_ARRAY p_daArray, int iIndex, void * p_Value){
    if(iIndex >= p_daArray->size){ //si on a besoin d'augmenter la taille du tableau dynamique

    


        int iTmpSize = 0;
        if(iIndex > p_daArray->size * 2){
            iTmpSize = iIndex+1;
        }
        else{
            iTmpSize = p_daArray->size * 2 + 1;
        }
        
      

        void** pTmpArray = NULL;
        if(NULL == (pTmpArray = (void**)realloc(p_daArray->array, iTmpSize*sizeof(void *)))){
            return 1;
        }

        for(int p=p_daArray->size; p<iTmpSize; p++){
            pTmpArray[p] = NULL;
        }

        p_daArray->size = iTmpSize;
        p_daArray->array = pTmpArray;


    }

    p_daArray->array[iIndex] = p_Value;
    return 0;
}

int da_free(pDYN_ARRAY p_daArray){
    free(p_daArray->array);
    p_daArray->array = NULL;
    p_daArray->size = 0;
    return 0;
}