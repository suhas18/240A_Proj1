//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//

#include "stdio.h"
#include "predictor.h"

//
// TODO:Student Information
//
const char *studentName = "Suhas Suraj";
const char *studentID   = "A53089020";
const char *email       = "susuraj@ucsd.edu";

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//

// Handy Global for use in output routines
const char *bpName[5] = { "Static", "Gshare", "Local",
    "Tournament", "Custom" };

int ghistoryBits; // Number of bits used for Global History
int lhistoryBits; // Number of bits used for Local History
int pcIndexBits;  // Number of bits used for PC index
int bpType;       // Branch Prediction Type

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

//gshare
int GHR;
uint8_t *BHT;
int index1;
int mask, local_mask;
void GHR_update(uint32_t pc, uint8_t outcome);
//end gshare

//local

int *PHT;
uint8_t *BHT_local;
int pcMasked;
void LOCAL_update(uint32_t pc, uint8_t outcome);
//end local

//
//TODO: Add your own Branch Predictor data structures here
//


//predictor for gshare
uint8_t gshare(uint32_t pc)
{
    // printf("in gshare ");
    // return TAKEN;
    index1 = (GHR ^ (pc & mask)) & mask;
    uint8_t decision = BHT[index1];
    if(decision == ST || decision == WT)
        return TAKEN;
    else
        return NOTTAKEN;
    
    
    
}

//predictor for local 
uint8_t local(uint32_t pc)
{
    
    pcMasked = (pc & mask);
    uint8_t decision = BHT_local[ ((PHT[pcMasked]) & local_mask) ];
    if(decision == ST || decision == WT)
        return TAKEN;
    else
        return NOTTAKEN;
    
}

/*
//chooser predictor for tournament predictor
unit8_t choose_predictor(uint32_t pc)
{
	//TODO

}

uint8_t tournament(uint32_t pc)
{
	if(chooser == SN || chooser == WN)
		return gshare(pc); //call gshare if chooser predictor is 00 or 01
	else if (chooser == WT || chooser == ST)
		return local(pc); // call local if chooser predictor is 10 or 11
	
}
*/
//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

// Initialize the predictor
//
void
init_predictor()
{
    //
    //TODO: Initialize Branch Predictor Data Structures
    //
    switch (bpType) {
        case STATIC:
        	break;
        case GSHARE:
        {
            //initializing gshare data structures
    		GHR = 0;
  	 		mask = ((1 << ghistoryBits ) - 1);
    		BHT = malloc(sizeof(uint8_t)*(mask+1));
    		int i;
	   		for(i=0; i < (mask+1); i++)
    		{
        		BHT[i]= WN;
    		}
	    	//end
	    }
	    break;
        case LOCAL:
        {
        	// initializing local data structures
        	mask = ((1 << pcIndexBits ) - 1);
        	local_mask = ((1 << lhistoryBits) -1);
        	PHT = malloc(sizeof(int) * (mask+1));
        	BHT_local = malloc(sizeof(uint8_t) * (local_mask+1));
        	int i;
	   		for(i=0; i < (mask+1); i++)
    		{
        		PHT[i]= 0;
    		}
    		for(i=0; i < (local_mask+1); i++)
    		{
    			BHT_local[i]=WN;
    		}      	
        	//end
        }
        break;
        case TOURNAMENT:
        case CUSTOM:
        default:
            break;
    }
    
    
    
}

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t
make_prediction(uint32_t pc)
{
    //
    //TODO: Implement prediction scheme
    //
    
    // Make a prediction based on the bpType
    switch (bpType) {
        case STATIC:
            return TAKEN;
            break;
        case GSHARE:
            return gshare(pc);
            break;
        case LOCAL:
        	return local(pc);
        	break;
        case TOURNAMENT:
//        	return tournament(pc);
        	break;
        case CUSTOM:
        default:
            break;
    }
    
    // If there is not a compatable bpType then return NOTTAKEN
    return NOTTAKEN;
}

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//
void
train_predictor(uint32_t pc, uint8_t outcome)
{
    //
    //TODO: Implement Predictor training
    //
    
    switch (bpType) {
        case STATIC:
        	break;
        case GSHARE:
            GHR_update(pc, outcome);
            break;
        case LOCAL:
        	LOCAL_update(pc, outcome);
        	break;
        case TOURNAMENT:
        	break;
        case CUSTOM:
        	break;
        default:
            break;
    }
    //gshare GHR update
    
    
    
}


//gshare GHR update
 void GHR_update(uint32_t pc, uint8_t outcome)
 {
	if(outcome==1)
	{
 		if(BHT[index1] < ST)
 		{
			 BHT[index1] += 1;
 		}
 
	}
	else 
	{
 		if( BHT[index1]>SN )
 		BHT[index1] -= 1;
	}
	GHR = (GHR << 1);
	GHR |= outcome ;
	GHR = GHR & mask; 
 	
 	//printf("GHR is %d \n",GHR);
    //printf("BHT in %d is %d \n",index1,BHT[index1]);
 
	
 }	
 
 //local predictor update
 void LOCAL_update(uint32_t pc, uint8_t outcome)
 {
 	int pattern = PHT[pcMasked] & local_mask;
 	if(outcome==1)
	{
 		if(BHT_local[ pattern ] < ST)
 		{
			 BHT_local[ pattern ] += 1;
 		}
 
	}
	else 
	{
 		if( BHT_local[pattern] >SN )
 		BHT_local[ pattern ] -= 1;
	}
	//updating each pattern in the PHT
	PHT[pcMasked] = ( PHT[pcMasked] << 1);
	PHT[pcMasked] |= outcome;
	PHT[pcMasked] = PHT[pcMasked] & local_mask;
 
 
 }