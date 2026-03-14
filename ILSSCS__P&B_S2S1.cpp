// *********************************************************************************************** //
//    Program to solve the Integrated Lot-Sizing, Scheduling and Cutting Stock Problem - ILSSCS    //
//  	           with the Column Generation and the Relax-and-Fix Procedures 					   //
//																								   //
//																								   //
//	  Scheduling problem modeled by the ATSP strategy with Setup Carryover    				       //
//																								   //
//    Used in Posdoc: Gislaine Mara Melega (12/2017 - atual)     								   //	
// *********************************************************************************************** //





//Libraries
//#include <stdafx.h>
#include <ilcplex/ilocplex.h>
#include <ilconcert/iloexpression.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include <cstdlib>
#include <vector>





ILOSTLBEGIN																							//macro necessary for portability





// ********************************************************************************************** //
// *********************************** MAIN PROGRAM ********************************************* //
// ********************************************************************************************** //



int main(int argc, char **argv)
{

   
    //Input Data File
    ifstream in(argv[1]);


    //Output Data File 1
    ofstream out(argv[2]);


    //Integrated Problem enviroment: env
    IloEnv env;





    try{  


          int  t,																					//indexes to periods
			   f, q, k,																				//final products
			   p,																					//pieces
			   j, l, g;																				//cutting patterns
		  




		  //Variables name
		  char changfinal[15];																		//changeover of final products
		  char setupfinal[15];																		//setup of final products
		  char prodfinal[15];																		//production of final product
		  char stockfinal[15];																		//stock of final product
		  char mtzfinal[15];																		//MTZ subtour elimination of final product

		  char changepattern[15];																	//changeover of cutting patterns
		  char setuppattern[15];																	//setup of cutting patterns
		  char prodpattern[15];																	    //cutting/production of cutting patterns 
		  char mtzpattern[15];																	    //MTZ subtour elimination of cutting patterns 

		  char varsub[15];																			//subproblem variable (new pattern)










// ******************************* // 
//    Read Data from Imput File    // 
// ******************************* //



		  int    T_aux, F_aux, P_aux,																//number of time periods, final products, pieces

		         capF_aux, capC_aux,																//capacity of final products production, cutting machine
			  
			     W_aux, w_aux[60],																    //object and pieces length
			  
			     r_aux[30][60],																		//number of used pieces in each final product (r_fp)

		         dF_aux[30][30];				                        							//demand of final products
		         
		   

		  double stFF_aux[30][30],																	//changeover time of final products
			  
			     vcF_aux[30], vtF_aux[30],															//production cost and time of final products
			  
			     hcF_aux[30],																		//inventory cost of final products

                 stJJ_aux,																			//changeover time of cutting patterns (independent values)

				 vcJ_aux, vtJ_aux;																	//production cost and time of cutting patterns (independent values)
				 

          


		  if (in) {

				     in  >>  T_aux;
				     in  >>  F_aux;
				     in  >>  P_aux;
				     				 




					 in  >>  capF_aux;
					 in  >>  capC_aux;
					 




				     in  >>  W_aux;


					 for(p=0; p<P_aux; p++)
					    in  >>  w_aux[p];





				     for(f=0; f<F_aux; f++){
			            in  >>  vcF_aux[f]; 
				        in  >>  hcF_aux[f];
					    in  >>  vtF_aux[f]; 
			         }


                     for(t=0; t<T_aux; t++)                                
		 			    for(f=0; f<F_aux; f++)                                     
		 			       in  >>  dF_aux[t][f];    


		 			 for(f=0; f<F_aux; f++)
                        for(q=0; q<F_aux; q++)
                           in  >>  stFF_aux[f][q];





				     in  >>  vcJ_aux;
				     in  >>  stJJ_aux;
					 in  >>  vtJ_aux;





					 for(f=0; f<F_aux; f++)
					    for(p=0; p<P_aux; p++)
						   in  >>  r_aux[f][p];
	         




          }//end if(in)
	      else {
                  cerr << "No such file: " << "dataIP.dat" << endl;
                  throw(1);
			}


// ********************************************************************************************** //










// ************************************************** // 
//    Create the Indexes of the Integrated Problem    // 
// ************************************************** //


		  //Indixes
		  IloInt T, F, P;																			//number of time periods, final products and pieces



		  //Assigning values according 
		  //to the input data file
		  T = T_aux;
		  F = F_aux;
		  P = P_aux;





// ****************************************************** // 
//    Create the Parameterns of the Integrated Problem    // 
// ****************************************************** //


		  //Capacity
		  IloNumArray capF(env, T, 0, IloInfinity, ILOINT);											//capacity of final products production


		  IloNumArray capC(env, T, 0, IloInfinity, ILOINT);										    //capacity of cutting machine





		  //Length
		  IloInt W;																					//object length


		  IloNumArray w(env, P, 0, IloInfinity, ILOINT);											//items length
		  
		  



		  //Final Products
		  IloArray<IloNumArray> dF(env, T);
		  for(t=0; t<T; t++)
		     dF[t] = IloNumArray(env, F, 0, IloInfinity, ILOINT);									//demand of final products


		  IloArray<IloArray<IloNumArray> > scFF(env, T); 
		  for(t=0; t<T; t++){
		     scFF[t] = IloArray<IloNumArray>(env, F);
			 for(f=0; f<F; f++){ 
				scFF[t][f] = IloNumArray(env, F, 0, IloInfinity);									//changeover cost of final products
			 }
	      }


		  IloArray<IloNumArray> vcF(env, T);
		  for(t=0; t<T; t++)
		     vcF[t] = IloNumArray(env, F, 0, IloInfinity);											//production cost of final products


		  IloArray<IloNumArray> hcF(env, T);
		  for(t=0; t<T; t++)
		     hcF[t] = IloNumArray(env, F, 0, IloInfinity);											//inventory cost of final products


		  IloArray<IloArray<IloNumArray> > stFF(env, T); 
		  for(t=0; t<T; t++){
		     stFF[t] = IloArray<IloNumArray>(env, F);
			 for(f=0; f<F; f++){ 
				stFF[t][f] = IloNumArray(env, F, 0, IloInfinity);									//changeover time of final products
			 }
	      }


		  IloArray<IloNumArray> vtF(env, T);
		  for(t=0; t<T; t++)
		     vtF[t] = IloNumArray(env, F, 0, IloInfinity);											//production time of final products 



  

		  //Cutting Patterns
		  IloNumArray scJJ(env, T, 0, IloInfinity);											        //setup cost of cutting patterns


		  IloNumArray vcJ(env, T, 0, IloInfinity);													//production/waste cost of cutting patterns


		  IloNumArray stJJ(env, T, 0, IloInfinity);											        //changeover time of cutting patterns
 

		  IloNumArray vtJ(env, T, 0, IloInfinity);											        //production time of cutting patterns





		  //Pieces
		  IloArray<IloNumArray> r(env, F);
		  for(f=0; f<F; f++)
		     r[f] = IloNumArray(env, P, 0, IloInfinity, ILOINT);									//number of pieces in each final product


// ********************************************************************************************** //










// ************************************************* // 
//    Assigning values according to the data file    // 
//	   Extending the data to the several periods     //
// ************************************************* //


		  for(t=0; t<T; t++){
		     capF[t] = capF_aux;
			 capC[t] = capC_aux;
		  }





  		  W = W_aux;


	      for(p=0; p<P; p++)
		     w[p] = w_aux[p];
		  




		  for(t=0; t<T; t++)
		     for(f=0; f<F; f++){
			    vcF[t][f] = vcF_aux[f];
			    hcF[t][f] = hcF_aux[f];
			    vtF[t][f] = vtF_aux[f];
			 }


		  for(t=0; t<T; t++)
		     for(f=0; f<F; f++)
			    for (q=0; q<F; q++)
				   stFF[t][f][q] = stFF_aux[f][q];
	 
			 
			 for(t=0; t<T; t++)
		     for(f=0; f<F; f++)
		        dF[t][f] = dF_aux[t][f];





          for(t=0; t<T; t++){ 
	         vcJ[t] = vcJ_aux;
		    stJJ[t] = stJJ_aux;
			 vtJ[t] = vtJ_aux;
		  }





		  for(f=0; f<F; f++)
		     for(p=0; p<P; p++)
                r[f][p] = r_aux[f][p];





		  //Changeover cost considered as a penalty term
		  //Value small enough not to override the other terms
		  //in the objective function of the integrated problem

		  //Changeover cost of final products
		  //Value relative to alfa = 1/65 (manually changed)
		  for(t=0; t<T; t++)
		     for(f=0; f<F; f++)
			    for (q=0; q<F; q++)
				   scFF[t][f][q] = 0.015*stFF[t][f][q];


// ********************************************************************************************** //










// ********************************************************************* // 
//    Create the Homogenous Cutting Pattern to the Integrated Problem    // 
// ********************************************************************* //


          //Number of cutting patterns in each period
		  IloNumArray J(env, T, 0, IloInfinity, ILOINT);



		  //Number of homogeneous cutting patterns
		  IloInt JH;


		  //Assigning the number of homogeneous cutting  
		  //patterns equal to the number of different pieces
		  JH = P;





		  //Create the Matrix of cutting patterns
		  IloArray<IloArray<IloNumArray> > a(env, T);
		  for(t=0; t<T; t++){
		     a[t] = IloArray<IloNumArray> (env, P); 
		     for(p=0; p<P; p++){
		        a[t][p] = IloNumArray(env, JH, 0, IloInfinity, ILOINT);								//for each cutting pattern j,
			 }																						//a_{tpj} shows the number of pieces p in the cutting pattern j cut in period t
		  }



		  //Create the matriz of cutting pattern 
		  //initially with just the homogenous cutting patterns
		  for(t=0; t<T; t++){
		     J[t] = 0;  
  	         for(p=0; p<P; p++)
		        for(j=0; j<JH; j++){
		           if (p == j) { 
				                  a[t][p][j] = int(W/w[p]);
								  J[t] += 1;
				   }
				   else a[t][p][j] = 0;
				}
		  }


// ********************************************************************************************** //


			 







// *********************************** // 
//    Create the Integrated Problem    // 
// *********************************** //


		  //Integrated Problem
		  IloModel IPmodel(env);



		  //All the variables are considered linear (relaxed values)

		  //Variables of Final Products
		  //Changeover of final products in each period
		  IloArray<IloArray<IloNumVarArray> > YFF(env, T);
		  for(t=0; t<T; t++){
		     YFF[t] = IloArray<IloNumVarArray> (env, F); 
		     for(f=0; f<F; f++){ 
		        YFF[t][f] = IloNumVarArray(env, F);								
				for(q=0; q<F; q++){ 
				   sprintf(changfinal, "YFF_%d_%d_%d", t, f, q);
                   YFF[t][f][q] = IloNumVar(env, 0, 1, changfinal);
				}
			 }
		  }


		  //Setup State of final product
		  IloArray<IloNumVarArray> YF(env, T);
		  for(t=0; t<T; t++){
		     YF[t] = IloNumVarArray(env, F);
			 for(f=0; f<F; f++){
                sprintf(setupfinal, "YF_%d_%d", t, f);
				YF[t][f] = IloNumVar(env, 0, 1, setupfinal);
			 }
		  }


		  //Production of final product
		  IloArray<IloNumVarArray> XF(env, T);
		  for(t=0; t<T; t++){
		     XF[t] = IloNumVarArray(env, F);
			 for(f=0; f<F; f++){
                sprintf(prodfinal, "XF_%d_%d", t, f);
				XF[t][f] = IloNumVar(env, 0, IloInfinity, prodfinal);
			 }
		  }


		  //Stock of final product
		  IloArray<IloNumVarArray> SF(env, T);
		  for(t=0; t<T; t++){
		     SF[t] = IloNumVarArray(env, F);
			 for(f=0; f<F; f++){
                sprintf(stockfinal, "SF_%d_%d", t, f);
				SF[t][f] = IloNumVar(env, 0, IloInfinity, stockfinal);
			 }
		  }	


		  //MTZ Subtour Elimination to final product
		  IloArray<IloNumVarArray> VF(env, T);
		  for(t=0; t<T; t++){
		     VF[t] = IloNumVarArray(env, F);
			 for(f=0; f<F; f++){
                sprintf(mtzfinal, "VF_%d_%d", t, f);
				VF[t][f] = IloNumVar(env, 0, F, mtzfinal);
			 }
		  }





		  //Variables of Cutting Patterns
		  //Changeover of cutting patterns in each period
		  IloArray<IloArray<IloNumVarArray> > WJJ(env, T);
		  for(t=0; t<T; t++){
		     WJJ[t] = IloArray<IloNumVarArray> (env, J[t]); 
		     for(j=0; j<J[t]; j++){ 
		        WJJ[t][j] = IloNumVarArray(env, J[t]);								
				for(l=0; l<J[t]; l++){ 
				   sprintf(changepattern, "WJJ_%d_%d_%d", t, j, l);
                   WJJ[t][j][l] = IloNumVar(env, 0, 1, changepattern);
				}
			 }
		  }


		  //Setup State of cutting pattern
		  IloArray<IloNumVarArray> WJ(env, T); 
		  for(t=0; t<T; t++){
		     WJ[t] = IloNumVarArray(env, J[t]);
			 for(j=0; j<J[t]; j++){ 
			    sprintf(setuppattern, "WJ_%d_%d", t, j);
			    WJ[t][j] = IloNumVar(env, 0, 1, setuppattern);
			 }
	      } 


		  //Production of cutting pattern
		  //number of objects cut according to a cutting pattern
		  IloArray<IloNumVarArray> ZJ(env, T); 
		  for(t=0; t<T; t++){
		     ZJ[t] = IloNumVarArray(env, J[t]);
			 for(j=0; j<J[t]; j++){ 
			    sprintf(prodpattern, "ZJ_%d_%d", t, j);
				ZJ[t][j] = IloNumVar(env, 0, IloInfinity, prodpattern);
			 }
	      } 


		  //MTZ Subtour Elimination to cutting pattern
		  IloArray<IloNumVarArray> VJ(env, T); 
		  for(t=0; t<T; t++){
		     VJ[t] = IloNumVarArray(env, J[t]);
			 for(j=0; j<J[t]; j++){ 
			    sprintf(mtzpattern, "VJ_%d_%d", t, j);
				VJ[t][j] = IloNumVar(env, 0, P, mtzpattern);
			 }
	      } 


		  // *************************************************************





		  int count1, count2, numpieces;



		  //Objective Function
		  IloExpr objective(env);
		
		  for(t=0; t<T; t++)
		     for(f=0; f<F; f++)
			    objective += (vcF[t][f]*XF[t][f] + hcF[t][f]*SF[t][f]);								//total production and inventory costs of final products



		  for(t=0; t<T; t++)
		     for(f=0; f<F; f++)
			    for(q=0; q<F; q++)
			       objective += scFF[t][f][q]*YFF[t][f][q];											//total changeover costs of final products



		  for(t=0; t<T; t++)
		     for(j=0; j<J[t]; j++)
				 objective += vcJ[t]*W*ZJ[t][j];  													//total production costs of cutting patterns



		  for(t=0; t<T; t++)
		     for(j=0; j<J[t]; j++)
			    for(l=0; l<J[t]; l++){


				   //Count the diference in the number of pieces 
				   //regarding to each two cutting patterns


				   //General Calculation				   
				   count1 = 0;
				   count2 = 0;
				   numpieces = 0;

				   
				   for(p=0; p<P; p++){

				      count1 = a[t][p][j] - a[t][p][l];

					  //Consider count2 as count2 = |count1|
                      if (count1 < -0.5) {
					                        count2 += -count1;
					  }
                      else { 
                              count2 += count1;
					  }
				   }

				   
				   numpieces = count2*stJJ[t]; 


				   //Calculate the changeover costs
				   scJJ[t] = (0.1*numpieces)/(count2 + 1);


				   objective += scJJ[t]*WJJ[t][j][l]; 												//total changeover costs of cutting patterns
				 
				}//end for l





		  //Integrated Problem objective function environment
		  IloObjective IPof = IloMinimize(env, objective);



          //Add the objective function (IPof) to the integrated problem
		  IPmodel.add(IPof);
          objective.end();						 //delet the expression


		  // *************************************************************





		  //Constraints to the Final Products - Level 3 
		  //modeled by a lot-sizing problem and scheduling
		  //problem with ATSP strategy and setup carryover


		  //DemandF balance constraints environment
		  IloArray<IloRangeArray> DemandF(env, T);
		  for(t=0; t<T; t++)
		     DemandF[t] = IloRangeArray(env, F);

		  //DemandF balance constraints
		  for(t=0; t<T; t++)
 		     for(f=0; f<F; f++){
				IloExpr balance(env);
				if (t == 0) {
					           balance += XF[t][f] - SF[t][f];

			                   DemandF[t][f] = (balance == dF[t][f]);
				               balance.end();
				}
				else {
					    balance += SF[t-1][f] + XF[t][f] - SF[t][f];

			            DemandF[t][f] = (balance == dF[t][f]);
				        balance.end();
				  }
			 }

		  //Add the DemandF balance constraint in the integrated problem
		  for(t=0; t<T; t++){
             DemandF[t].setNames("DemandF");   //give name to constraints
	         IPmodel.add(DemandF[t]);
		  }





		  //CapacityF constraint environment
		  IloRangeArray CapacityF(env, T);

		  //CapacityF constraint
		  for(t=0; t<T; t++){
		     IloExpr cap(env);

  		     for(f=0; f<F; f++)
			       cap += vtF[t][f]*XF[t][f];   
  		     
			 for(f=0; f<F; f++)
                   for(q=0; q<F; q++)
			          cap += stFF[t][f][q]*YFF[t][f][q];

			 CapacityF[t] = (cap <= capF[t]);
			 cap.end();
		  }

		  //Add the CapacityF constraint in the integrated problem
          CapacityF.setNames("CapacityF");
	      IPmodel.add(CapacityF);





		  //SetupF1 constraints environment
		  IloArray<IloRangeArray> SetupF1(env, T);
		  for(t=0; t<T; t++)
		     SetupF1[t] = IloRangeArray(env, F);

		  //SetupF1 constraints
		  for(t=0; t<T; t++)
 		     for(f=0; f<F; f++){
				IloExpr setup(env);

				setup += vtF[t][f]*XF[t][f];

				setup += - capF[t]*YF[t][f];

				for(q=0; q<F; q++)
                   setup += - capF[t]*YFF[t][q][f];

			    SetupF1[t][f] = (setup <= 0);
				setup.end();
		     }

		  //Add the SetupF1 constraint in the integrated problem
		  for(t=0; t<T; t++){
             SetupF1[t].setNames("SetupF1");
	         IPmodel.add(SetupF1[t]);
		  }





		  //SetupF2 constraints environment
		  IloRangeArray SetupF2(env, T);

		  //SetupF2 constraints
		  for(t=0; t<T; t++){
             IloExpr setup(env);

 		     for(f=0; f<F; f++)
			    setup += YF[t][f];
			 
			 SetupF2[t] = (setup == 1);
			 setup.end();
		     }

		  //Add the SetupF2 constraint in the integrated problem
          SetupF2.setNames("SetupF2");
	      IPmodel.add(SetupF2);


		  


		  //SetupF3 constraints environment
		  IloArray<IloRangeArray> SetupF3(env, T);
		  for(t=0; t<T; t++)
		     SetupF3[t] = IloRangeArray(env, F);

		  //SetupF3 constraints
		  for(t=0; t<T; t++)
 		     for(f=0; f<F; f++){
				IloExpr setup(env);

				setup += YFF[t][f][f];

			    SetupF3[t][f] = (setup == 0);
				setup.end();
		     }

		  //Add the SetupF3 constraint in the integrated problem
		  for(t=0; t<T; t++){
             SetupF3[t].setNames("SetupF3");
	         IPmodel.add(SetupF3[t]);
		  }


		  // *************************************************************





		  //ATSP Constraints to the Final Products - Level 3 
		  //with consideration of Setup Carryover


		  //ATSPF1 constraints environment
		  IloArray<IloRangeArray> ATSPF1(env, T);
		  for(t=0; t<T; t++)
		     ATSPF1[t] = IloRangeArray(env, F);

		  //ATSPF1 constraints
		  for(t=0; t<T-1; t++)
 		     for(f=0; f<F; f++){
				IloExpr atsp(env);

				for(q=0; q<F; q++)
				   atsp += YFF[t][q][f];

				for(k=0; k<F; k++)
				   atsp += - YFF[t][f][k];

				atsp += - YF[t+1][f];

			    ATSPF1[t][f] = (atsp <= 0);
				atsp.end();
		     }

		  //Add the ATSPF1 constraint in the integrated problem
		  for(t=0; t<T-1; t++){
             ATSPF1[t].setNames("ATSPF1");
	         IPmodel.add(ATSPF1[t]);
		  }


		  


		  //ATSPF2 constraints environment
		  IloArray<IloRangeArray> ATSPF2(env, T);
		  for(t=0; t<T; t++)
		     ATSPF2[t] = IloRangeArray(env, F);

		  //ATSPF2 constraints
		  for(t=0; t<T; t++)
 		     for(f=0; f<F; f++){
				IloExpr atsp(env);

				for(q=0; q<F; q++)
				   atsp += YFF[t][f][q];

				for(k=0; k<F; k++)
				   atsp += - YFF[t][k][f];

				atsp += - YF[t][f];

			    ATSPF2[t][f] = (atsp <= 0);
				atsp.end();
		     }

		  //Add the ATSPF2 constraint in the integrated problem
		  for(t=0; t<T; t++){
             ATSPF2[t].setNames("ATSPF2");
	         IPmodel.add(ATSPF2[t]);
		  }





		  //ATSPF3 constraints environment
		  IloArray<IloRangeArray> ATSPF3(env, T);
		  for(t=0; t<T; t++)
		     ATSPF3[t] = IloRangeArray(env, F);

		  //ATSPF3 constraints
		  for(t=0; t<T; t++)
 		     for(f=0; f<F; f++){
				IloExpr atsp(env);

				for(q=0; q<F; q++)
				   atsp += YFF[t][q][f];
				
				atsp += - 1 + YF[t][f];

			    ATSPF3[t][f] = (atsp <= 0);
				atsp.end();
		     }

		  //Add the ATSPF3 constraint in the integrated problem
		  for(t=0; t<T; t++){
             ATSPF3[t].setNames("ATSPF3");
	         IPmodel.add(ATSPF3[t]);
		  }





		  //ATSPF4 constraints environment
		  IloArray<IloRangeArray> ATSPF4(env, T);
		  for(t=0; t<T-1; t++)
		     ATSPF4[t] = IloRangeArray(env, F);

		  //ATSPF4 constraints
		  for(t=0; t<T-1; t++)
 		     for(f=0; f<F; f++){
				IloExpr atsp(env);

				for(q=0; q<F; q++)
				   atsp += YFF[t][f][q];

  			    atsp += - 1 + YF[t+1][f];

			    ATSPF4[t][f] = (atsp <= 0);
				atsp.end();
		     }

		  //Add the ATSPF4 constraint in the integrated problem
		  for(t=0; t<T-1; t++){
             ATSPF4[t].setNames("ATSPF4");
	         IPmodel.add(ATSPF4[t]);
		  }





		  //ATSPF5 constraints environment
		  //Ccontraint changed, due to periods with no production
		  IloArray<IloRangeArray> ATSPF5(env, T);
		  for(t=0; t<T-1; t++)
		     ATSPF5[t] = IloRangeArray(env, F);

		  //ATSPF5 constraints
		  for(t=0; t<T-1; t++)
 		     for(f=0; f<F; f++){
				IloExpr atsp(env);

			    atsp += YF[t][f];

				for(q=0; q<F; q++)
				   atsp += - YFF[t][f][q];

				atsp += - YF[t+1][f];

			    ATSPF5[t][f] = (atsp <= 0);
				atsp.end();
		     }

		  //Add the ATSPF5 constraint in the integrated problem
		  for(t=0; t<T-1; t++){
             ATSPF5[t].setNames("ATSPF5");
	         IPmodel.add(ATSPF5[t]);
		  }


          // *************************************************************





		  //MTZ Subtour Elimination Constraints of the Final Products - Level 3 
   

		  //MTZ Subtour Elimination constraints environment
		  IloArray <IloArray<IloRangeArray> > MTZF(env, T);
		  for(t=0; t<T; t++){
		     MTZF[t] = IloArray<IloRangeArray>(env, F);
			 for(f=0; f<F; f++){
			    MTZF[t][f] = IloRangeArray(env, F);
			 }
		  }

		  //MTZ Subtour Elimination constraints
		  for(t=0; t<T; t++)
 		     for(f=0; f<F; f++)
			    for(q=0; q<F; q++){
				   if (f != q) {
					              IloExpr mtz(env);
								  mtz += VF[t][q] - VF[t][f] - 1 + F*(1 - YFF[t][f][q]);

								  MTZF[t][f][q] = (mtz >= 0);
								  mtz.end(); 
				   }
				   else{
				   	      IloExpr mtz(env);
						  mtz += VF[t][q];

						  MTZF[t][f][q] = (mtz >= 0);
						  mtz.end();
				   }
				}

		  //Add the MTZ Subtour Elimination constraint in the integrated problem
		  for(t=0; t<T; t++)
 		     for(f=0; f<F; f++)
			    for(q=0; q<F; q++)
				   if(f != q) {
							     MTZF[t][f][q].setName("MTZF");
								 IPmodel.add(MTZF[t][f][q]);
				   }


          // *************************************************************





		  //Constraints to the Pieces and Cutting Patterns - Level 2
		  //modeled by a cutting stock problem and scheduling
		  //problem with ATSP strategy and setup carryover


		  //DemandP Constraints enviroment
		  IloArray<IloRangeArray> DemandP(env, T);
		  for(t=0; t<T; t++)
		     DemandP[t] = IloRangeArray(env, P);

		  //DemandP balance constraint
		  for(t=0; t<T; t++)
 		     for(p=0; p<P; p++){
				IloExpr balance(env);
				
				for(j=0; j<J[t]; j++)
				   balance += a[t][p][j]*ZJ[t][j];

			    for(f=0; f<F; f++)
				   balance -= r[f][p]*XF[t][f];

			    DemandP[t][p] = (balance >= 0);
				balance.end();

			 }

		  //Add the DemandP balance constraint in the integrated problem
		  for(t=0; t<T; t++){
             DemandP[t].setNames("DemandP");
	         IPmodel.add(DemandP[t]);
		  }
		  




		  //CapacityC constraint environment
		  IloRangeArray CapacityC(env, T);

		  //CapacityC constraint
		  for(t=0; t<T; t++){
		     IloExpr cap(env);

			 for(j=0; j<J[t]; j++)
			    cap += vtJ[t]*ZJ[t][j];

			 for(j=0; j<J[t]; j++)
                for(l=0; l<J[t]; l++){


				   //Count the diference of the number of pieces 
				   //regarding to each two cutting patterns

				   //General Calculation				   
				   count1 = 0;
				   count2 = 0;
				   numpieces = 0;
				   

				   for(p=0; p<P; p++){

				      count1 = a[t][p][j] - a[t][p][l];

					  //Consider count2 as count2 = |count1|
                      if (count1 < -0.5) {
					                        count2 += -count1;
					  }
                      else { 
                              count2 += count1;
					  }
				   }


				   numpieces = count2*stJJ[t];

				   cap += numpieces*WJJ[t][j][l];

				}//end for l


			 CapacityC[t] = (cap <= capC[t]);
			 cap.end();

		  }//end for t

		  //Add the CapacityC constraint in the integrated problem
          CapacityC.setNames("CapacityC");
          IPmodel.add(CapacityC);


		  


		  //SetupJ1 constraints environment
    	  IloArray<IloRangeArray> SetupJ1(env, T); 
	      for(t=0; t<T; t++)
		     SetupJ1[t] = IloRangeArray(env, J[t]);
		  

		  //SetupJ1 constraints
		  for(t=0; t<T; t++)
 		     for(j=0; j<J[t]; j++){
				IloExpr setup(env);
			    
				setup += vtJ[t]*ZJ[t][j] - capC[t]*WJ[t][j];

				for(l=0; l<J[t]; l++)
			       setup += - capC[t]*WJJ[t][l][j];
			    
				SetupJ1[t][j] = (setup <= 0);
				setup.end();
			 }

		  //Add the SetupJ1 constraint in the integrated problem
		  for(t=0; t<T; t++){
             SetupJ1[t].setNames("SetupJ1");
	         IPmodel.add(SetupJ1[t]);
		  }





		  //SetupJ2 constraints environment
    	  IloRangeArray SetupJ2(env, T); 

		  //SetupJ2 constraints
		  for(t=0; t<T; t++){
	         IloExpr setup(env);
			 
			 for(j=0; j<J[t]; j++)
			    setup += WJ[t][j];
			 
			 SetupJ2[t] = (setup == 1);
			 setup.end();
		  }

		  //Add the SetupJ2 constraint in the integrated problem
          SetupJ2.setNames("SetupJ2");
	      IPmodel.add(SetupJ2);





		  //SetupJ3 constraints environment
    	  IloArray<IloRangeArray> SetupJ3(env, T); 
	      for(t=0; t<T; t++)
		     SetupJ3[t] = IloRangeArray(env, J[t]); 

		  //SetupJ3 constraints
		  for(t=0; t<T; t++)
		     for(j=0; j<J[t]; j++){
	            IloExpr setup(env);

			    setup += WJJ[t][j][j];
			    
				SetupJ3[t][j] = (setup == 0);
				setup.end();
		  }

		  //Add the SetupJ3 constraint in the integrated problem
		  for(t=0; t<T; t++){
             SetupJ3[t].setNames("SetupJ3");
	         IPmodel.add(SetupJ3[t]);
		  }


		  // *************************************************************





		  //ATSP Constraints of the Cutting Patterns - Level 2 
		  //with consideration of Setup Carryover


		  //ATSPJ1 constraints environment
		  IloArray<IloRangeArray> ATSPJ1(env, T);
		  for(t=0; t<T; t++)
		     ATSPJ1[t] = IloRangeArray(env, J[t]);

		  //ATSPJ1 constraints
		  for(t=0; t<T-1; t++)
 		     for(j=0; j<J[t]; j++){
				IloExpr atsp(env);
				
				for(l=0; l<J[t]; l++)
				   atsp += WJJ[t][l][j];

				for(g=0; g<J[t]; g++)
				   atsp += - WJJ[t][j][g];

				atsp += - WJ[t+1][j];

			    ATSPJ1[t][j] = (atsp <= 0);
				atsp.end();
		     }

		  //Add the ATSPJ1 constraint in the integrated problem
		  for(t=0; t<T-1; t++){
             ATSPJ1[t].setNames("ATSPJ1");
	         IPmodel.add(ATSPJ1[t]);
		  }


		  


		  //ATSPJ2 constraints environment
		  IloArray<IloRangeArray> ATSPJ2(env, T);
		  for(t=0; t<T; t++)
		     ATSPJ2[t] = IloRangeArray(env, J[t]);

		  //ATSPJ2 constraints
		  for(t=0; t<T; t++)
 		     for(j=0; j<J[t]; j++){
				IloExpr atsp(env);

				for(l=0; l<J[t]; l++)
				   atsp += WJJ[t][j][l];

				for(g=0; g<J[t]; g++)
				   atsp += - WJJ[t][g][j];

				atsp += - WJ[t][j];

			    ATSPJ2[t][j] = (atsp <= 0);
				atsp.end();
		     }

		  //Add the ATSPJ2 constraint in the integrated problem
		  for(t=0; t<T; t++){
             ATSPJ2[t].setNames("ATSPJ2");
	         IPmodel.add(ATSPJ2[t]);
		  }


		  


		  //ATSPJ3 constraints environment
		  IloArray<IloRangeArray> ATSPJ3(env, T);
		  for(t=0; t<T; t++)
		     ATSPJ3[t] = IloRangeArray(env, J[t]);

		  //ATSPJ3 constraints
		  for(t=0; t<T; t++)
 		     for(j=0; j<J[t]; j++){
				IloExpr atsp(env);

				for(l=0; l<J[t]; l++)
				   atsp += WJJ[t][l][j];

  			    atsp += -1 + WJ[t][j];

			    ATSPJ3[t][j] = (atsp <= 0);
				atsp.end();
		     }

		  //Add the ATSPJ3 constraint in the integrated problem
		  for(t=0; t<T; t++){
             ATSPJ3[t].setNames("ATSPJ3");
	         IPmodel.add(ATSPJ3[t]);
		  }


		  


		  //ATSPJ4 constraints environment
		  IloArray<IloRangeArray> ATSPJ4(env, T);
		  for(t=0; t<T-1; t++)
		     ATSPJ4[t] = IloRangeArray(env, J[t]);

		  //ATSPJ4 constraints
		  for(t=0; t<T-1; t++)
 		     for(j=0; j<J[t]; j++){
				IloExpr atsp(env);

				for(l=0; l<J[t]; l++)
				   atsp += WJJ[t][j][l];

  			    atsp += -1 + WJ[t+1][j];

			    ATSPJ4[t][j] = (atsp <= 0);
				atsp.end();
		     }

		  //Add the ATSPJ4 constraint in the integrated problem
		  for(t=0; t<T-1; t++){
             ATSPJ4[t].setNames("ATSPJ4");
	         IPmodel.add(ATSPJ4[t]);
		  }





		  //ATSPJ5 constraints environment
		  //Constraint changed, due to periods with no cut
		  IloArray<IloRangeArray> ATSPJ5(env, T);
		  for(t=0; t<T-1; t++)
		     ATSPJ5[t] = IloRangeArray(env, J[t]);

		  //ATSPJ5 constraints
		  for(t=0; t<T-1; t++)
 		     for(j=0; j<J[t]; j++){
				IloExpr atsp(env);

				atsp += WJ[t][j];

				for(l=0; l<J[t]; l++)
				   atsp += - WJJ[t][j][l];	 

				atsp += - WJ[t+1][j];

			    ATSPJ5[t][j] = (atsp <= 0);
				atsp.end();
		     }

		  //Add the ATSPJ5 constraint in the integrated problem
		  for(t=0; t<T-1; t++){
             ATSPJ5[t].setNames("ATSPJ5");
	         IPmodel.add(ATSPJ5[t]);
		  }


		  // *************************************************************





		  //MTZ Subtour Elimination Constraints of the Cutting Process - Level 2


		  //MTZ Subtour Elimination constraints environment
		  IloArray <IloArray<IloRangeArray> > MTZJ(env, T);
		  for(t=0; t<T; t++){
		     MTZJ[t] = IloArray<IloRangeArray>(env, J[t]);
			 for(j=0; j<J[t]; j++){
			    MTZJ[t][j] = IloRangeArray(env, J[t]);
			 }
		  }

		  //MTZ Subtour Elimination constraints
		  for(t=0; t<T; t++)
 		     for(j=0; j<J[t]; j++)
			    for(l=0; l<J[t]; l++){
				   if (j != l) {
					              IloExpr mtz(env);
								  mtz += VJ[t][l] - VJ[t][j] - 1 + P*(1 - WJJ[t][j][l]);

								  MTZJ[t][j][l] = (mtz >= 0);
								  mtz.end();
					              
				   }
				   else{
				          IloExpr mtz(env);
						  mtz += VJ[t][l];

						  MTZJ[t][j][l] = (mtz >= 0);
						  mtz.end();
				   }
				}

		  //MTZ Add the subtour elimination constraint in the integrated problem
		  for(t=0; t<T; t++)
 		     for(j=0; j<J[t]; j++)
			    for(l=0; l<J[t]; l++)
				   if(j != l) {
							     MTZJ[t][j][l].setName("MTZJ");
								 IPmodel.add(MTZJ[t][j][l]);
				   }


		  // *************************************************************


		  //Define CPLEX environment to integrated problem
          IloCplex IPcplex(IPmodel);


// ********************************************************************************************** //










// ************************************************************************************************* // 
//    Solution Method to the Integrated Lot-Sizing, Scheduling and Cutting Stock Problem - ILSSCS    //
// ************************************************************************************************* //



// ************************************************** //
//    Beginning of the Column Generation Procedure    //
// ************************************************** //


          int checkoptCG,																		    //check if the column generation STOPS by optimality
																									//(0-yes; 1-no)			  
			  it_CG,																				//number of iterations in the column generaion procedure
			  
			  numpatterns;																			//number of cutting patterns generated in the column generation procedure


		  double time_CG,																			//time spent in the column generation procedure

		         ZCG;																				//objective function value of the column generation procedure


		  int it_RMP;																				//iterations without improvements in the objective function value
		                                                                                            //of restricted master problem in the column generation procedure

		  double ZRMPit1, ZRMPit2;																	//objective function value of the restricted master problem 
																									//to two consecutive iterations in the column generation procedure
																									//(analyze if there is improvements in the objective function value)

         



          // **************************************************************************************************** 
		  out << "*****  The Integrated Lot-Sizing, Scheduling and Cutting Stock Problem - ILSSCS *****" << endl;
		  out << endl << endl << endl;
		  out <<  "* Scheduling problem modeled by the ATSP strategy with Setup Carryover" << endl;
		  out <<  "* Lower Bound: Column Generation procedure" << endl;
		  out <<  "* Upper Bound: Relax-and-Fix procedure (level-oriented decomposition)" << endl;
		  out << endl << endl << endl << endl << endl;
		  out << "***************  Lower Bound: Beginning of Column Generation Procedure  *************" << endl;
		  // ****************************************************************************************************





		  //Assigning initial values
		  ZCG = IloInfinity;
		  ZRMPit1 = IloInfinity;



          //Start counting the iterations to column generation procedure
		  it_CG = 0;
		  it_RMP = 0;


		 


		  //Column Generation LOOP
		  for(; ;){





				// ***** Solve the Restricted Master Problem ************************************

				//Limite the number of threads in the solution of the Restricted Master Problem
				IPcplex.setParam(IloCplex::Threads, 1);





				//Extract the Restricted Master Problem model
				//IPcplex.extract(IPmodel);
				//Export the Restricted Master Problem model
				//IPcplex.exportModel ("RMPmodel.lp" );





				//SOLVE the Restricted Master Problem (current cutting patterns)
				IPcplex.solve();



				//Recover the objective function value of the restricted master problem
				ZCG = IPcplex.getValue(IPof);





				// ******************************************************************************
				//Print in the output data
		        out << endl << endl << endl;
				out << "#Iteration_" << it_CG+1 << "___________________________________" << endl;  
				out << endl;
				out << "Objective Function Value to RMP = " << ZCG << endl;			
				out << endl << endl;
				// ******************************************************************************





				//Check column generation optimality
				//(0-yes; 1-not)
				checkoptCG = 0;



				//Update the number of iteration in the column generation procedure
				it_CG = it_CG + 1;																		
			
			
			
				//Objective function value to t2
				ZRMPit2 = IPcplex.getValue(IPof);





				//Create the dual variables related to the restricted 
				//master problem - SUBPROBLEM parameters. 
				//These dual variables are auxiliaries due to the
				//fixed periods in the resolution of the subproblems

				IloArray<IloNumArray> pi_aux(env, T);
 				for(t=0; t<T; t++)
				   pi_aux[t] = IloNumArray(env, P);														//dual variables of pieces demand balance constraint


				IloNumArray gama_aux(env, T);															//dual variables of cutting machine capacity constraint

				
				//The dual variables related of setup constraint (SetupJ[t][j]) of cutting 
				//patterns are zero, this fact occurs because in the linear relaxation 
				//these constraints became a equality and redundant





				//Recover the dual variable values from the restricted master problem
				for(t=0; t<T; t++)   
				   for(p=0; p<P; p++){
					  pi_aux[t][p] = IPcplex.getDual(DemandP[t][p]);
				   }

				for(t=0; t<T; t++){
				   gama_aux[t] = IPcplex.getDual(CapacityC[t]);
				}
			









				// **************************************************************************************

				//For each period (and object when there are different types) solve a SUBPROBLEM 
				//to generate a new cutting pattern (column(s)) to the restricted master problem 
				//Different approaches to the choice of the cutting pattern

				for(t=0; t<T; t++){



						  // *********************** //
						  // Create the t-SUBPROBLEM //
						  // *********************** //
                  

						  //SUBPROBLEM environment: envsub
						  IloEnv envsub;



						  //SUBPROBLEM model
						  IloModel SUBmodel(envsub);





						  //SUBPROBLEM parameters
						  IloNumArray pi(envsub, P);

						  IloNum gama;
			      

						  //Assigning values to the parameters
						  //Recover the dual variables for each period, which
						  //have been kept in the auxiliar parameterns (_aux)
						  for(p=0; p<P; p++)
							 pi[p] = pi_aux[t][p];

 						  gama = gama_aux[t];


						  


						  //Create the SUBPROBLEM variables
						  //Cutting patterns to the restricted master problem
						  IloNumVarArray alpha(envsub, P);
						  for(p=0; p<P; p++){
							 sprintf(varsub, "alpha_%d", p);
							 alpha[p] = IloNumVar(envsub, 0, IloInfinity, ILOINT, varsub);
						  }


						  


						  //SUBPROBLEM Objective function
						  IloExpr objective(envsub);

						  objective += vcJ[t]*W;
		          
						  for(p=0; p<P; p++)		  
							 objective += - pi[p]*alpha[p];

						  objective += - vtJ[t]*gama;


						  //SUBPROBLEM objective function enviroment
						  IloObjective SUBof = IloMinimize(envsub, objective);


						  //Add the objective function (SUBof) in the SUBPROBLEM
						  SUBmodel.add(SUBof);
						  objective.end();
						  




						  //SUBPROBLEM constraints - knapsack constraint
						  IloExpr knapsack(envsub);
						  for(p=0; p<P; p++)
							 knapsack += w[p]*alpha[p];
		          
						  //Add the knapsack constraint in the SUBPROBLEM
						  SUBmodel.add(knapsack <= W);
						  knapsack.end();


						  // *****************************************


						  //Define CPLEX enviroment to the SUBPROBLEM
						  IloCplex SUBcplex(SUBmodel);


						  // *****************************************





						  // ***** Solve the t-SUBPROBLEM ***************************************

						  //Omits unused data in the log of the SUBPROBLEM (SUBPROBLEM solution)
						  //SUBcplex.setOut(envsub.getNullStream());
				  

						  //Limite the number of threads in the solution of the SUBPROBLEM
						  SUBcplex.setParam(IloCplex::Threads, 1);





						  //Extract the SUBPROBLEM model
						  //SUBcplex.extract(SUBmodel);
						  //Export the SUBPROBLEM model
						  //SUBcplex.exportModel ("SUBPROBLEM.lp" );





						  //Solve the SUBPROBLEM Model
						  SUBcplex.solve();





						  // **************************************************************
						  //Print in the output data
						  out << "SUBPROBLEM Value = " << SUBcplex.getValue(SUBof) << endl;
						  // **************************************************************

				  
				  







						  //Parameter to recover the generated cutting patter
						  //in the subproblem of the column generation procedure
						  IloNumArray cuttingpattern(envsub, P, 0, IloInfinity, ILOINT);	

				  
				  


						  //Check if the reduced cost found in the subproblem is negative
						  if (SUBcplex.getValue(SUBof) < -0.0001) {
					                                            




					  								//Found a subproblem with negative reduced cost
													//that is, the column generation solution is not optimal
													checkoptCG = 1;		



													//Recover the cutting pattern from SUBPROBLEM solution
													for(p=0; p<P; p++)
													   cuttingpattern[p] = SUBcplex.getValue(alpha[p]);	



													//Update the matrix of cutting patterns for the current period
													for(p=0; p<P; p++)
													   a[t][p].add(cuttingpattern[p]);	



													//Update the number of cutting patterns for the current period
													J[t] += 1;

						    
													


													//Add the new colums (variables) and new constraints to
													//the restricted master problem according to the cutting
													//pattern generated in the column generation procedure	
													

													//Position of the new cutting pattern
													j = J[t]-1;


													//Add to the prodpattern variable ZJ[t][J[t]-1] the coefficients of the
													//new column the restricted master problem to the corresponding period	
													ZJ[t].add(IloNumVar(IPof(vcJ[t]*W) + DemandP[t](cuttingpattern) + CapacityC[t](vtJ[t])));
													sprintf(prodpattern, "ZJ_%d_%d", t, j);
													ZJ[t][j].setName(prodpattern);
												 
													



													//Add to the prodpattern variable WJ[t][J[t]-1] the coefficients of the
													//new column the restricted master problem to the corresponding period
													if ((t >= 1) && (j <= J[t-1]-1)) {							

																					    WJ[t].add(IloNumVar(SetupJ2[t](1) + ATSPJ1[t-1][j](-1) + ATSPJ4[t-1][j](1) + ATSPJ5[t-1][j](-1)));
																					    sprintf(setuppattern, "WJ_%d_%d", t, j);
																					    WJ[t][j].setName(setuppattern);

													}
													else{
													
														   WJ[t].add(IloNumVar(SetupJ2[t](1)));
														   sprintf(setuppattern, "WJ_%d_%d", t, j);
													       WJ[t][j].setName(setuppattern);
													}
													
	


													   
													//Add to the changeover variable WJJ[t][J[t]-1][l], l=0,...,J[t]-1
													//an inner dimension in the matrix of the restricted master problem
													WJJ[t].add(IloNumVarArray(env));

													



													//Add to the changeover variable WJJ[t][j][J[t]-1], j=0,...,J[t]-2
													//the coefficients of the new column in the restricted master problem
													for(j=0; j<J[t]-1; j++){


														   //Position of the new cutting pattern W[t][j][J[t]-1]
														   l = J[t]-1;


														   //Count the diference of the number of pieces 
														   //regarding to each two cutting patterns

														   //General Calculation				   
														   count1 = 0;
														   count2 = 0;
														   numpieces = 0;
				   
														   for(p=0; p<P; p++){

															  count1 = a[t][p][j] - a[t][p][l];

															  if (count1 < -0.5) {
																					count2 += -count1;
															  }
															  else { 
																	  count2 += count1;
															  }
														   }
														   

														   numpieces = count2*stJJ[t];


														   //Calculate the changeover costs
														   scJJ[t] = (0.1*numpieces)/(count2 + 1);



														   if(t <= T-2) {

																		   WJJ[t][j].add(IloNumVar(IPof(scJJ[t]) + CapacityC[t](numpieces) + ATSPJ1[t][j](-1) + ATSPJ2[t][j](1) + ATSPJ4[t][j](1) + ATSPJ5[t][j](-1)));
													   
																		   sprintf(changepattern, "WJJ_%d_%d_%d", t, j, l);
		                     											   WJJ[t][j][l].setName(changepattern);

														   }
														   else{
																  WJJ[t][j].add(IloNumVar(IPof(scJJ[t]) + CapacityC[t](numpieces) + ATSPJ2[t][j](1)));
													   
																  sprintf(changepattern, "WJJ_%d_%d_%d", t, j, l);
		                     									  WJJ[t][j][l].setName(changepattern);
														   }
													   

													}//end for j
												


													//Add to the changeover variable WJJ[t][J[t]-1][l], l=0,...,J[t]-1
													//the coefficients of the new column in the restricted master problem
													for(l=0; l<J[t]; l++){


															//Position of the new cutting pattern W[t][J[t]-1][l]
															j = J[t]-1;

															
														    //Count the diference of the number of pieces 
														    //regarding to each two cutting patterns

														    //General Calculation				   
														    count1 = 0;
														    count2 = 0;
														    numpieces = 0;
				   
														    for(p=0; p<P; p++){

															   count1 = a[t][p][j] - a[t][p][l];

															   if (count1 < -0.5) {
																					 count2 += -count1;
															   }
															   else { 
																	   count2 += count1;
															   }
														    }
															

														    numpieces = count2*stJJ[t];


															//Calculate the changeover costs
															scJJ[t] = (0.1*numpieces)/(count2 + 1);
															


															if (l <= J[t]-2) {
															                
																				if(t <= T-2) {

																								WJJ[t][j].add(IloNumVar(IPof(scJJ[t]) + CapacityC[t](numpieces) + SetupJ1[t][l](-capC[t]) + ATSPJ1[t][l](1) + ATSPJ2[t][l](-1) + ATSPJ3[t][l](1)));
															
																								sprintf(changepattern, "WJJ_%d_%d_%d", t, j, l);
		                     																	WJJ[t][j][l].setName(changepattern);

																				}
																				else{

																					   WJJ[t][j].add(IloNumVar(IPof(scJJ[t]) + CapacityC[t](numpieces) + SetupJ1[t][l](-capC[t]) + ATSPJ2[t][l](-1) + ATSPJ3[t][l](1)));
															
																					   sprintf(changepattern, "WJJ_%d_%d_%d", t, j, l);
		                     														   WJJ[t][j][l].setName(changepattern);

																				}
																			

															}//end if (l <= J[t]-2)
															else{
																   WJJ[t][j].add(IloNumVar(IPof(scJJ[t]) + CapacityC[t](numpieces)));
															
																   sprintf(changepattern, "WJJ_%d_%d_%d", t, j, l);
		                     						 			   WJJ[t][j][l].setName(changepattern);
															}

													}//end for l





													//Add the SetupJ1[t][J[t]-1] constraints in the restricted
													//master problem to the new cutting pattern (new constraints)
													IloExpr setup(env);

													setup += vtJ[t]*ZJ[t][J[t]-1] - capC[t]*WJ[t][J[t]-1];

													for(l=0; l<J[t]; l++)
													   setup += - capC[t]*WJJ[t][l][J[t]-1];

													SetupJ1[t].add(setup <= 0);

													SetupJ1[t].setNames("SetupJ1");
													IPmodel.add(SetupJ1[t]);
													setup.end();
										



													
													//Add the SetupJ3[t][J[t]-1] constraints in the restricted
													//master problem to the new cutting pattern (new constraints)
													SetupJ3[t].add(WJJ[t][J[t]-1][J[t]-1] == 0);

													SetupJ3[t].setNames("SetupJ3");
													IPmodel.add(SetupJ3[t]);




													
													//Add the ATSPJ1[t][J[t]-1] constraints in the restricted
													//master problem to the new cutting pattern (new constraints)
													if(t <= T-2) {
																    IloExpr atsp1(env);

																	for(l=0; l<J[t]; l++)
																	   atsp1 += WJJ[t][l][J[t]-1];

																	for(g=0; g<J[t]; g++)
																	   atsp1 += - WJJ[t][J[t]-1][g];

																	ATSPJ1[t].add(atsp1 <= 0);				         
													
																	ATSPJ1[t].setNames("ATSPJ1");
																	IPmodel.add(ATSPJ1[t]);
																	atsp1.end();
													}



													

													//Add the ATSPJ2[t][J[t]-1] constraints in the restricted
													//master problem to the new cutting pattern (new constraints)
													IloExpr atsp2(env);

											        for(l=0; l<J[t]; l++)
											           atsp2 += WJJ[t][J[t]-1][l];

											        for(g=0; g<J[t]; g++)
													   atsp2 += - WJJ[t][g][J[t]-1];

													atsp2 += - WJ[t][J[t]-1];

			                                        ATSPJ2[t].add(atsp2 <= 0);				         
													
													ATSPJ2[t].setNames("ATSPJ2");
													IPmodel.add(ATSPJ2[t]);
													atsp2.end();




													
													//Add the ATSPJ3[t][J[t]-1] constraints in the restricted
													//master problem to the new cutting pattern (new constraints)
													IloExpr atsp3(env);

											        for(l=0; l<J[t]; l++)
											           atsp3 += WJJ[t][l][J[t]-1];

											        atsp3 += -1 + WJ[t][J[t]-1];;

			                                        ATSPJ3[t].add(atsp3 <= 0);				         
													
													ATSPJ3[t].setNames("ATSPJ3");
													IPmodel.add(ATSPJ3[t]);
													atsp3.end();


													

													
													//Add the ATSPJ4[t][J[t]-1] constraints in the restricted
													//master problem to the new cutting pattern (new constraints)
													if(t <= T-2) {
																	IloExpr atsp4(env);

																	for(l=0; l<J[t]; l++)
																	   atsp4 += WJJ[t][J[t]-1][l];

																	atsp4 += -1;

																	ATSPJ4[t].add(atsp4 <= 0);				         
													
																	ATSPJ4[t].setNames("ATSPJ4");
																	IPmodel.add(ATSPJ4[t]);
																	atsp4.end();
													}



													

													//Add the ATSPJ1[t][J[t]-1] constraints in the restricted
													//master problem to the new cutting pattern (new constraints)
													if(t <= T-2) {
																	IloExpr atsp5(env);

																	atsp5 += WJ[t][J[t]-1];

																	for(l=0; l<J[t]; l++)
																	   atsp5 += - WJJ[t][J[t]-1][l];

																	ATSPJ5[t].add(atsp5 <= 0);				         
													
																	ATSPJ5[t].setNames("ATSPJ5");
																	IPmodel.add(ATSPJ5[t]);
																	atsp5.end();
													}


													

													
													//Position of the new cutting pattern VJ[t][J[t]-1]
                                                    l = J[t]-1;

													//Create the new mtz variables to the new column
													//generated in the column generation procedure
													//This is done in order to add these variables in MTZ constraints
													VJ[t].add(IloNumVar(IPof(0)));
													sprintf(mtzpattern, "VJ_%d_%d", t, l);
													VJ[t][l].setName(mtzpattern);


													


													//Add to the mtz constraints MTZJ[t][J[t]-1][l], l=0,...,J[t]-1 
													//an inner dimension in the matrix of the restricted master problem
													MTZJ[t].add(IloRangeArray(env));



													//Add the mtz constraints MTZJ[t][j][J[t]-1], j=0,...,J[t]-1, in the
													//restricted master problem to the new cutting pattern (new constraints)
													for(j=0; j<J[t]; j++){

														
													   //Position of the new cutting pattern MTZJ[t][j][J[t]-1]
                                                       l = J[t]-1;


													   if(j == l) {
	 															     IloExpr mtz(env);

																     mtz += VJ[t][j];
												   
																     MTZJ[t][j].add(mtz >= 0);

																     //MTZJ[t][j].setNames("MTZJ");
																     //IPmodel.add(MTZJ[t][j]);
																     mtz.end();

													   }
													   else{
															   IloExpr mtz(env);

															   mtz += VJ[t][l] - VJ[t][j] - 1 + P*(1 - WJJ[t][j][l]);
												   
															   MTZJ[t][j].add(mtz >= 0);

															   MTZJ[t][j].setNames("MTZJ");
															   IPmodel.add(MTZJ[t][j]);
															   mtz.end();
													   }


													}//end for j



													//Add the mtz constraints MTZJ[t][J[t]-1][l], l=0,...,J[t]-1, in the
													//restricted master problem to the new cutting pattern (new constraints)
													
													for(l=0; l<J[t]; l++){

														
													   //Position of the new cutting pattern MTZJ[t][j][J[t]-1]
                                                       j = J[t]-1;


													   if(l == j){

														   		  IloExpr mtz(env);

																  mtz += VJ[t][l];
												   
																  MTZJ[t][J[t]-1].add(mtz >= 0);

																  //MTZJ[t][J[t]-1].setNames("MTZJ");
																  //IPmodel.add(MTZJ[t][J[t]-1]);
																  mtz.end();

													   }
													   else{

														      IloExpr mtz(env);

															  mtz += VJ[t][l] - VJ[t][J[t]-1] - 1 + P*(1 - WJJ[t][J[t]-1][l]);
												   
															  MTZJ[t][J[t]-1].add(mtz >= 0);

															  MTZJ[t][J[t]-1].setNames("MTZJ");
															  IPmodel.add(MTZJ[t][J[t]-1]);
															  mtz.end();

													   }


													}//end for j





						  }//end if (check negative reduced cost)


				 
						  //End the parameters, variables and environment to the SUBPROBLEM 
						  pi.end();
						  alpha.end();
						  SUBof.end();
						  SUBmodel.end();
						  SUBcplex.end();
						  cuttingpattern.end();
						  envsub.end();
						  


				}//end for t 





				//Calculate the difference in the objective function value to the RMP
				//of two consecutives interations in the column generation procedure
				//If the diference is smaller than 0.01% then it_RMP is update
				if (ZRMPit1 - ZRMPit2 < 0.0001) it_RMP = it_RMP + 1;


				// *********************************************************************************










				//The Column Generation Procedure STOPS by Optimality or 50 iterations 
				//without improvements of 0.01% in the objective function value of the RMP

				
				//The Column Generation Procedure STOPS by Optimality
				//All the subproblems, for all periods have a positive reduced cost
				//that is, the optimal solution of the restricted master problem has been found
				if (checkoptCG == 0) {
					         
			 

								 //Recover the column generation running time
								 time_CG = IPcplex.getTime();



								 //Recover the objective function value of the column generation procedure
								 ZCG = IPcplex.getValue(IPof);



								 //Calculate the number of cutting patterns
								 //generated in the column generation procedure
								 numpatterns = 0;

								 for(t=0; t<T; t++)
								    numpatterns += J[t];





								 // ****************************************************************************************************
								 //Print in the output data
								 out << endl << endl << endl << endl << endl;
								 out << "****************  Final Solution of the Column Generation Procedure  ****************" << endl;
								 out << endl << endl;
								 out << "Iterations CG = " << it_CG << endl;	
								 out << "Number of Cutting Patterns CG = " << numpatterns << endl;
								 out << "Objective Function Value CG = " << ZCG << endl; 
								 out << "Time CG = " << time_CG << endl; 
								 out << endl << endl;
								 out << "***********************************  Other Values  **********************************" << endl;
								 out << endl << endl;
								 out << "Number of cutting patterns in each time period " << endl;
								 for(t=0; t<T; t++)
									out << "J_" << t+1 << " = "<< J[t] << endl; 
								 out << endl << endl;
								 out << "Matrix of Cutting Patterns:  a_t_j " << endl;
								 out << "(each line corresponds to a cutting pattern)" << endl;
								 out << endl;
								 for(t=0; t<T; t++)
									for(j=0; j<J[t]; j++){
									   for(p=0; p<P; p++)			  
										  out << setw(3) << a[t][p][j] << " " << " " << " ";										
									   out << endl;
									}
								 out << endl << endl;
								 out << "******************  Lower Bound: End of Column Generation Procedure  ****************" << endl;
								 out << endl << endl << endl << endl << endl << endl << endl << endl << endl << endl;
								 // ****************************************************************************************************
					    


								 break;										//STOP the column generation procedure by optimality
					 


				}//end if (checkoptCG)




				





				//The Column Generation Procedure STOPS by Tailling Off
				//There are more than 50 iterations without inprovements in the objective function 
				//value of the restricted master problem in the column generation procedure
				if (it_RMP >= 50) {
 								


								 //Recover the column generation running time
								 time_CG = IPcplex.getTime();



								 //Calculate the number of cutting patterns
								 //generated in the column generation procedure
								 numpatterns = 0;

								 for(t=0; t<T; t++)
								    numpatterns += J[t];





								 // ****************************************************************************************************                 							 
								 //Print in the output data
								 out << endl << endl << endl << endl << endl;
								 out << "****************  Final Solution of the Column Generation Procedure  ****************" << endl;
								 out << endl << endl;
								 out << "Column Generation Procedure STOPS by Tailling Off " << endl;
								 out << "Iterations CG = " << it_CG << endl;	
								 out << "Number of Cutting Patterns CG = " << numpatterns << endl;
								 out << "Time CG = " << time_CG << endl; 
								 out << endl << endl;
								 out << "***********************************  Other Values  **********************************" << endl;
								 out << endl << endl;
								 out << "Number of cutting patterns in each time period " << endl;
								 for(t=0; t<T; t++)
									out << "J_" << t+1 << " = "<< J[t] << endl; 
								 out << endl << endl;
								 out << "Matrix of Cutting Patterns:  a_t_j " << endl;
								 out << "(each line corresponds to a cutting pattern)" << endl;
								 out << endl;
								 for(t=0; t<T; t++)
									for(j=0; j<J[t]; j++){
									   for(p=0; p<P; p++)			  
										  out << setw(3) << a[t][p][j] << " " << " " << " ";										
									   out << endl;
									}
								 out << endl << endl;
								 out << "******************  Lower Bound: End of Column Generation Procedure  ****************" << endl;
								 out << endl << endl << endl << endl << endl << endl << endl << endl << endl << endl;
	   						     // *****************************************************************************************************



								 break;									//STOP the column generation procedure by Tailling Off
			


				}//end if (it_RMP >= 5)


				// *********************************************************************************

				


				





				//Update the objective function value of the RMP 
				//to two consecutive iterations in the column generation procedure
				ZRMPit1 = ZRMPit2;



				//End the parameters of the SUBPROBLEM for the current master problem
				pi_aux.end();
				gama_aux.end();



		  }//end for ;;
		  //Column Generation LOOP


// ******************************************** //
//    End of the Column Generation Procedure    //
// ******************************************** //










// ****************************************** // 
//    Beginning of Relax-and-Fix Procedure    //
//      with Level-Oriented Decomposition      //
// ****************************************** //


		 //Considers all the columns obtained with the column generation procedure and the 
		 //Relax-and-Fix procedure is applied to try to find a feasible solution for the ILSSCS problem.
		 

		 //The Relax-and-Fix procedure considers a level-oriented decomposition.
		 //For each level-window the whole set of periods, final products, pieces is 
		 //considered and the resulting ILSSCS problem is solved by an optimization package (CPLEX)


		 //In the level-orientend decomposition, the first window 
		 //considers the level of final products (L3), which is modeled by
		 //the lot-sizing problem and then the second window considers the 
		 //level of cutting (L2), which is modeld by the cutting stock problem





		 int checkRF_lastWindow;   																	//check in the Relax-and-Fix procedure if it is the last window
																									//(0-no; 1-yes)

		 double TotalTime,
		
			    timeWindow,																			//real time avaialable for each window (total time/number of windows)
			    
				timeRF_Window,																		//time available to the solution of each window 
		        
				timeWindow_begin,																	//beginning time to solve the window in the Relax-and-Fix procedure
			    
				timeWindow_end,																	    //end time to solve the window in the Relax-and-Fix procedure
			    
				timeWindow_used,																	//time spent in the resolution of a window in the Relax-and-Fix procedure
				
				timeWindow_left;																	//time left in the resolution of a window in the Relax-and-Fix procedure


		 double  ZILSSCS, 																			//objective function value of the ILSSCS problem (integer problem)
				 
			     gapILSSCS,																		    //gap of the ILSSCS problem using as lower bound the column generation solution
		         
				 timeILSSCS;																	    //time spent to find a feasible soluiton to the ILSSCS problem




		 





		 // *****************************************************************************************************
		 //Print in the output file
		 out << "**************  Upper Bound: Beginning of the Relax-and-Fix Procedure  **************" << endl;
         out << "**************             with Time-Oriented Decomposition            **************" << endl;
		 out << endl << endl << endl;
         out << "* Level-oriented decomposition: 1) Final Products   (L3 - Lot-Sizing Problem)" << endl;
         out << "                                2) Cutting Patterns (L2 - Cutting Stock Problem)" << endl;
		 out << endl << endl << endl << endl << endl;
		 // ***************************************************************************************************** 










		 //Check if the last window in the 
		 //Relax-and-Fix procedure is reached
		 //(0-no; 1-yes)
		 checkRF_lastWindow = 0;





		 //Calculate the total number of windows in the Relax-and-Fix
		 //procedure, in order to calculate the time allocated in each window
		 //In the level-oriented decomposition, the number of windows is equal to the
		 //number of levels which is 2 and the total time is split equaly between them
		 int nit;
 

		 //Total number of windows
		 nit = 2;


		 //Time available in each level-window
		 TotalTime = 1800;

		 timeWindow = TotalTime/nit;

		 timeRF_Window = TotalTime/nit;










		 //Create the parameters that recover the binary variables
		 //of the final products in the Relax-and-Fix procedure
 	     IloArray<IloArray<IloNumArray> > YFF_fix(env, T);
		 for(t=0; t<T; t++){
		    YFF_fix[t] = IloArray<IloNumArray> (env, F); 
		    for(f=0; f<F; f++){ 
		       YFF_fix[t][f] = IloNumArray(env, F, 0, IloInfinity);								
			}
		 }


		 IloArray<IloNumArray> YF_fix(env, T);
		 for(t=0; t<T; t++)
		    YF_fix[t] = IloNumArray(env, F, 0, 1, ILOINT);
	   




		 //Initialize the parameters == 0
		 for(t=0; t<T; t++)
		    for(f=0; f<F; f++){
			   YF_fix[t][f] = 0;
			   for(q=0; q<F; q++)
			      YFF_fix[t][f][q] = 0;
			}



		 

		 //Create the constraints that fix the binary variables of final
		 //products to their binary values in the Relax-and-Fix procedure
    	 IloArray <IloArray<IloRangeArray> > RestFixYFF(env, T);
		 for(t=0; t<T; t++){
		    RestFixYFF[t] = IloArray<IloRangeArray>(env, F);
		    for(f=0; f<F; f++){
			   RestFixYFF[t][f] = IloRangeArray(env, F);
			}
		 }			

			
		 IloArray<IloRangeArray> RestFixYF(env, T); 
	     for(t=0; t<T; t++)
		    RestFixYF[t] = IloRangeArray(env, F);










		 //Consider the integrality of the binary
		 //variables in the level of final products (L3)

	     //Converte the float variables to binary
		 for(t=0; t<T; t++)
		    for(f=0; f<F; f++)
		       IPmodel.add(IloConversion(env, YFF[t][f], ILOBOOL));	  


		 for(t=0; t<T; t++)
		    IPmodel.add(IloConversion(env, YF[t], ILOBOOL));






		 



		 //LOOP of the Relax-and-Fix Procedure
		 for(; ;){
		    


				  //Reached the last iteration of the Relax-and-Fix procedure
				  if (checkRF_lastWindow == 1) {

					  
												  //Consider the integrality of the binary
												  //variables in the level of cutting patterns (L2)

												  //Converte the float variables to binary
 												  for(t=0; t<T; t++){
												     IPmodel.add(IloConversion(env, WJ[t], ILOBOOL));
													 IPmodel.add(IloConversion(env, ZJ[t], ILOINT));
												  }										    


												  for(t=0; t<T; t++)
													 for(j=0; j<J[t]; j++)
														IPmodel.add(IloConversion(env, WJJ[t][j], ILOBOOL));

				  }


			  







				  // ***** Solve the Resulting ILSSCS Problem **************************************

				  //Add CPLEX Options
				  //Print the output and warnings of cplex to file
				  IPcplex.setOut(out);
				  IPcplex.setWarning(out);


				  //Limite the number of threads in the solution of the problem
				  IPcplex.setParam(IloCplex::Threads, 1);


				  //Mip Display
				  IPcplex.setParam(IloCplex::MIPDisplay, 3);


				  //Mip Interval
	 			  IPcplex.setParam(IloCplex::MIPInterval, 1000);


				  //Time Limit
		 		  IPcplex.setParam(IloCplex::TiLim, timeRF_Window);


				  //Mip Gap
				  IPcplex.setParam(IloCplex::EpGap, 0.001);
 




				  //Extract the resulting ILSSCS problem in the Relax-and-Fix procedure
				  //IPcplex.extract(IPmodel);
				  //Export the resulting ILSSCS problem in the Relax-and-Fix procedure
				  //IPcplex.exportModel ("subILSSCS_RF.lp" );





				  //Recover the time in the beginning of window solution
				  timeWindow_begin = IPcplex.getTime();





				  // *****************************************************************************************************
				  //Print in the output file
				  out << "*******  Solving the Resulting ILSSCS problem in the Relax-and-Fix Procedure  *******" << endl;
				  out << endl << endl;



				  //SOLVE the resulting ILSSCS problem
				  IPcplex.solve();
				  out << endl << endl << endl << endl << endl;
				  // *****************************************************************************************************

				  


				  
				  //Recover the time in the end of window solution
				  timeWindow_end = IPcplex.getTime();


				  // *****************************************************************************************************










				  //Calculate the time used in the window solution
				  timeWindow_used = timeWindow_end - timeWindow_begin;

				  

				  //Calculate the time left in the window solution
				  //after the solution of the resulting problem
				  timeWindow_left = timeRF_Window - timeWindow_used; 

				  

				  //If the time left in the window solution is bigger than 0,
				  //then the time left is added to the next window
				  if (timeWindow_left > 0) {
											  timeRF_Window = timeWindow + timeWindow_left;
				  }










				  // ***** Check the Solution ******************************************************
				  
				  //The Relax-and-Fix procedure stops by infeasiblity
				  //in one of the resulting problems or the Relax-and-Fix
				  //stops by reaching the last iteration of the procedure



				  //The Relax-and-Fix procedure STOPS by infeasibility
				  if (IPcplex.getStatus() == 3) {


									
									// *****************************************************************************************************
								    //Print in the output file
									out << "********************  Upper Bound: End of Relax-and-Fix Procedure  ******************" << endl;
									out << endl << endl << endl << endl << endl << endl << endl << endl << endl << endl;
								    out << "******************   Final Solution to the Integrated Lot-Sizing,  ******************" << endl; 
								    out << "******************  Scheduling and Cutting Stock Problem - ILSSCS  ******************" << endl;
								    out << endl << endl;
									out << "The Relax-and-Fix Based Heuristic is INFEASIBLE" << endl;
									out << "      using a level-oriented decomposition      " << endl;	 
								    out << endl << endl;
								    out << "*************************************************************************************" << endl;
								    // *****************************************************************************************************
									


									break;					//STOP the Relax-and-Fix procedure by infeasibility
				  


				  }//end if infeasibility










				  //The Relax-and-Fix procedure STOPS by reaching the last iteration
				  //Return the final solution of the Relax-and-Fix
				  //procedure to the integrated ILSSCS problem
				  if (checkRF_lastWindow == 1) {



  									//Calcule the total time spent to find
									//a feasible solution to the ILSSCS, which is
									//the time spent in the Relax-and-Fix procedure
									timeILSSCS = IPcplex.getTime() - time_CG;



									//Recover the objective function value of ILSSCS problem 
									//(integer problem), found by the Relax-and-Fix procedure
									ZILSSCS = IPcplex.getValue(IPof);




								    //Calculate the gap to the ILSSC problem (integer problem)
								    //using as lower bound the value from the Column Generation Procedure
								    if (checkoptCG == 0) gapILSSCS = (100*((ZILSSCS - ZCG)/ZCG));
									  else gapILSSCS = IloInfinity;










									// ****************************************************************************************************
 									//Print in the output file  
									out << "********************  Upper Bound: End of Relax-and-Fix Procedure  ******************" << endl;
									out << endl << endl << endl << endl << endl << endl << endl << endl << endl << endl;
								    out << "******************   Final Solution to the Integrated Lot-Sizing,  ******************" << endl; 
								    out << "******************  Scheduling and Cutting Stock Problem - ILSSCS  ******************" << endl;
								    out << endl << endl;
								    out << "Solution Status ILSSCS = " << IPcplex.getStatus()  <<  endl; 
								    out << "Objective Function Value ILSSCS = " << ZILSSCS << endl; 
								    out << "Gap ILSSCS % = " << gapILSSCS << endl; 
								    out << "Time ILSSCS = " << timeILSSCS << endl;	 
								    out << endl << endl;
								    out << "*************************************************************************************" << endl;
								    // ****************************************************************************************************
									 









								    //Calculate the costs in the objective function separately
								    double costYFF, costXF, costSF,
										   costWJJ, costZJ;
		 

								    //Initiate the values 
								    costYFF = 0;
								    costXF = 0;
								    costSF = 0;
								    costWJJ = 0;
								    costZJ = 0;
		


		

								    //Final Products: setup, production and inventory costs
								    for(t=0; t<T; t++)
									   for(f=0; f<F; f++){  
										  costXF += vcF[t][f]*IPcplex.getValue(XF[t][f]);
										  costSF += hcF[t][f]* IPcplex.getValue(SF[t][f]);
				
										  for(q=0; q<F; q++)
										     costYFF += scFF[t][f][q]*IPcplex.getValue(YFF[t][f][q]);				
									   }			 



								    //Cutting Patterns: setup and cutting costs
								    for(t=0; t<T; t++)
									   for(j=0; j<J[t]; j++)
										  costZJ += vcJ[t]*W*IPcplex.getValue(ZJ[t][j]);

								    costWJJ = ZILSSCS - costXF - costSF - costYFF - costZJ;
         

										
								    //Cutting Patterns: percentage of waste
								    double amount_piece, amount_obj, amount_waste, sum_piece, per_waste;


		
								    //Initiate the values 
								    amount_piece = 0;
								    amount_obj = 0;
								    amount_waste = 0;

			


								    //variation of the cutting patterns
								    for(t=0; t<T; t++)
									   for(j=0; j<J[t]; j++){
				
										  //amount (size) of cut objects
										  amount_obj += W*IPcplex.getValue(ZJ[t][j]);

										  sum_piece = 0;
										  for(p=0; p<P; p++)
										     sum_piece += w[p]*a[t][p][j];
				
     									  //amount (size) of cut pieces
										  amount_piece += sum_piece*IPcplex.getValue(ZJ[t][j]);
					
									   }//end for j



								    //amount (size) of waste
								    amount_waste = amount_obj - amount_piece;


								    //percentage of waste
								    per_waste = 100*(amount_waste/amount_obj);
		  



								    // ****************************************************************************************************
								    //Print in the output file
								    out << endl << endl << endl;
								    out << "*****************************  Other Costs and Values  ******************************" << endl;
								    out << endl;
								    out << "Final Products Costs - setup = " << costYFF << endl;
								    out << "Final Products Costs - production = " << costXF << endl;
								    out << "Final Products Costs - inventory = " << costSF << endl;
								    out << "Patterns costs - setup = " << costWJJ << endl;
								    out << "Patterns costs - cutting = " << costZJ << endl;
								    out << endl;
								    out << "Amount (size) of waste = " << amount_waste << endl;
								    out << "Waste % = " << setprecision(4) << per_waste << endl;
								    out << endl;
								    out << "*************************************************************************************" << endl;
								    // ****************************************************************************************************





								    out << endl;
								    out << endl;
								    out << endl;



								    for(t=0; t<T; t++)
									   for(f=0; f<F; f++)
										  if (IPcplex.getValue(SF[t][f]) > 0.01) {
										     out << "S_" << t+1 << "_" << f+1 << "=" << IPcplex.getValue(SF[t][f]) << endl;										//total cost of cut object
										  } 


								    out << endl;


								    for(t=0; t<T; t++)
									   for(f=0; f<F; f++)
										  if (IPcplex.getValue(XF[t][f]) > 0.01) {
										     out << "X_" << t+1 << "_" << f+1 << "=" << IPcplex.getValue(XF[t][f]) << endl;	
										  }


								    out << endl;


								    for(t=0; t<T; t++)
									   for(f=0; f<F; f++)
										  if (IPcplex.getValue(YF[t][f]) > 0.01) {
										     out << "YF_" << t+1  << "_" << f+1 << "=" << IPcplex.getValue(YF[t][f]) << endl;										
										  }


								    out << endl;


								    for(t=0; t<T; t++)
									   for(f=0; f<F; f++)
		 								  for(q=0; q<F; q++)
										     if (IPcplex.getValue(YFF[t][f][q]) > 0.01) {
				   							    out << "YFF_" << t+1  << "_" << f+1 << "_" << q+1 << "=" << IPcplex.getValue(YFF[t][f][q]) << endl;										
										     }
			


								    out << endl;
								    out << endl;
								    out << endl;


		    
								    for(t=0; t<T; t++)
									   for(j=0; j<J[t]; j++)
										  if (IPcplex.getValue(ZJ[t][j]) > 0.01) {
										     out << "ZJ_" << t+1 << "_" << j+1 << "=" << IPcplex.getValue(ZJ[t][j]) << endl;
										  }


								    out << endl;


								    for(t=0; t<T; t++)
									   for(j=0; j<J[t]; j++)
										  if (IPcplex.getValue(WJ[t][j]) > 0.01) {
										     out << "WJ_" << t+1 << "_" << j+1 << "=" << IPcplex.getValue(WJ[t][j]) << endl;
										  }


								    out << endl;


								    for(t=0; t<T; t++)
 									   for(j=0; j<J[t]; j++)
										  for(l=0; l<J[t]; l++)
										     if (IPcplex.getValue(WJJ[t][j][l]) > 0.01) {
											    out << "WJJ_" << t+1 << "_" << j+1 << "_" << l+1 << "=" << IPcplex.getValue(WJJ[t][j][l]) << endl;
										     }
								    // ***************************************************************************************************************



									break;						 //STOP the Relax-and-Fix procedure



				  }//end if (checkRF_lastWindow == 1)


				  // *******************************************************************************
				  









				  //Recover the values of the binary variables to
				  //the final products in the Relax-and-Fix procedure
				  for(t=0; t<T; t++)
					 for(f=0; f<F; f++)
						for(q=0; q<F; q++)
						   YFF_fix[t][f][q] = IPcplex.getValue(YFF[t][f][q]);


				  for(t=0; t<T; t++)
					 for(f=0; f<F; f++)
					    YF_fix[t][f] = IPcplex.getValue(YF[t][f]);










				  //Add the constraints that fix the binary variables to the
				  //final products at their values in the Relax-and-Fix procedure
				  for(t=0; t<T; t++)
					 for(f=0; f<F; f++)
					    for(q=0; q<F; q++){
						   IloExpr fix(env);
						   
						   fix += YFF[t][f][q];
						   
						   RestFixYFF[t][f][q] = (fix == YFF_fix[t][f][q]);
						   RestFixYFF[t][f][q].setName("FixYFF");
						   IPmodel.add(RestFixYFF[t][f][q]);

						   fix.end();

						}



				  for(t=0; t<T; t++)
					 for(f=0; f<F; f++){
						IloExpr fix(env);

						fix += YF[t][f];

						RestFixYF[t][f] = (fix == YF_fix[t][f]);
						RestFixYF[t][f].setName("FixYF");
					    IPmodel.add(RestFixYF[t][f]);

			            fix.end();

				  }










				  //Move the level-window into the other level in
				  //the last iteration of the Relax-and-Fix procedure
				  checkRF_lastWindow = 1;
				  	  




		 }//end for ;;
		 //Relax-and-Fix LOOP





		 //End the parameters of the Relax-and-Fix procedure
		 YFF_fix.end();
		 YF_fix.end();
		 RestFixYFF.end();
		 RestFixYF.end();
		 

// **************************************** //
//    End of the Relax-and-Fix Procedure    //
// **************************************** //





// ********************************************************************************************** //
// *************************************** END MAIN PROGRAM ************************************* //
// ********************************************************************************************** //



   } //end try


      catch (IloException& ex) {
        cerr << "Error Cplex: " << ex << endl;
      } 
	  catch (...) {
        cerr << "Error Cpp" << endl;
      }
     

	  //Finish the enviroment
	  env.end();

	
	return 0;

}
