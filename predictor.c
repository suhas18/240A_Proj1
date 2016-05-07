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

//tournament
int global_mask;
int chooser;
uint8_t *choose_cntr;
void TOURNAMENT_update(uint32_t pc, uint8_t outcome);
//end tournament
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

//gshare for tournament, separate function to take care of mask variables
uint8_t tournament_gshare(uint32_t GHR)
{
    // printf("in gshare ");
    // return TAKEN;
    // index1 = (GHR ^ (pc & global_mask)) & global_mask;
    uint8_t decision = BHT[GHR & global_mask];
    if(decision == ST || decision == WT)
        return TAKEN;
    else
        return NOTTAKEN;  
    
}

uint8_t tournament(uint32_t pc)
{
	//chooser = choose_predictor(pc);
	chooser = choose_cntr[GHR & global_mask];
	if(chooser <= WN)
		return tournament_gshare(GHR); //call gshare if chooser predictor is 00 or 01
	else 
		return local(pc); // call local if chooser predictor is 10 or 11
	
}

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
        {
        	//local
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
    		//end local
    		//gshare
    		GHR = 0;
  	 		global_mask = ((1 << ghistoryBits ) - 1);
    		BHT = malloc(sizeof(uint8_t)*(global_mask+1));
	   		for(i=0; i < (global_mask+1); i++)
    		{
        		BHT[i]= WN;
    		}
    		//end gshare
    		choose_cntr = malloc(sizeof(uint8_t) * (global_mask+1));
    		for(i=0; i < (global_mask+1); i++)
    		{
        		choose_cntr[i]= WN;
    		}
        }
        break;
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
        	return tournament(pc);
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
        	TOURNAMENT_update(pc, outcome);
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
 
 void TOURNAMENT_update(uint32_t pc, uint8_t outcome)
 {
 	//local part
	pcMasked = (pc & mask);
	LOCAL_update(pc, outcome);
	//end local part
	
	//chooser
	uint8_t gflag, lflag;
	gflag = tournament_gshare(GHR);
	lflag = local(pc);
	
	if(outcome==1)
	{
		if(gflag==1 && lflag == 0 )
		{
			//SN and WN of chooser should direct towards Global predictor
			if(choose_cntr[GHR] > SN)
			{
				choose_cntr[GHR] -= 1;
			}
		}
		else if (gflag==0 && lflag==1)
		{
			//WT and ST of chooser should direct towards Local predictor
			if(choose_cntr[GHR] < ST)
			{
				choose_cntr[GHR] += 1;
			}
		}
	}
	else if(outcome == 0)
	{
		if(gflag==0 && lflag == 1 ) //case when global predicts correctly
		{
			//SN and WN of chooser should direct towards Global predictor
			if(choose_cntr[GHR] > SN)
			{
				choose_cntr[GHR] -= 1;
			}
		}
		else if (gflag==1 && lflag==0) //case when local predicts correctly
		{
			//WT and ST of chooser should direct towards Local predictor
			if(choose_cntr[GHR] < ST)
			{
				choose_cntr[GHR] += 1;
			}
		}
	}
	
	//global part
 	int global_index = GHR & global_mask;
 	if(outcome==1)
	{
 		if(BHT[global_index] < ST)
 		{
			 BHT[global_index] += 1;
 		}
 
	}
	else 
	{
 		if( BHT[global_index]>SN )
 		BHT[global_index] -= 1;
	}
	GHR = (GHR << 1);
	GHR |= outcome ;
	GHR = GHR & global_mask; 
	//end global part
 
 }