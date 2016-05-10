# 240A_Proj1

This is a simulator of these branch predictors: Gshare, Local, Tournament (Alpha 21264 Predictor) and Piece Wise Linear. 
The main function handles input stream reads, option handling and output prints while predictor.c implments the different predictors.
The application reads in trace files with branch addresses and ouput of the branch. Intention is to predict branch behavior so that pipeline flushes in the processor are minimised. Objective is to minimise mis-predict rate.

Traces are piped into the application with a command like so:

bunzip2 -kc <trace_file.bz2> | ./predictor --<option>

<option> can be gshare:N OR local:M:N OR tournament:M:N:P OR custom; where M, N, P are positive integers of reasonable size (for something that can be implemented in hardware).
