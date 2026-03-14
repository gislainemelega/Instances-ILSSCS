# G4ILSCS

This repository contains the problem instances and the heuristic implementations used in the paper:

Mathematical model and solution approaches for integrated lot-sizing, scheduling, and cutting stock problems

Repository Contents

- Problem instances used in the computational experiments:\
  SizeF_Classes1-9\
  SizeF_Classes10-15\
  LengthP_Classes16-18\
  Capacity_Classes19-26\
  Capacity_Classes27-36\
  CostsOF_Classes37-45\
  Note: see the file named 'Readme_NameClasses_All' for more details about the class and data variation.

- Implementation of the heuristic solution approaches used in the paper:\
  ILSSCS__P&B_P: Price and Branch with product-oriented decomposition\
  ILSSCS__P&B_T: Price and Branch with time-oriented decomposition\
  ILSSCS__P&B_S1S2: Price and Branch with stage-oriented decomposition (stage 12)\
  ILSSCS__P&B_S2S1: Price and Branch with stage-oriented decomposition (stage 21)\
  ILSSCS__MIP_solver: MIP-solver based heuristic\

Paper Abstract:\
In this paper, we address a two-stage integrated lot-sizing, scheduling and cutting stock problem with sequence-dependent setup times and setup costs. 
In production stage one, a cutting machine is used to cut large objects into smaller pieces, in which cutting patterns are generated and used to cut 
the pieces, and should be sequenced in order to obtain a complete cutting plan for the problem. The cut pieces, from production stage one, are used to 
assemble final products in production stage two, where the final products are scheduled in order to meet the client’s demands. To solve the two-stage 
integrated problem, we present solution methods based on a price-and-branch approach, in which a column generation procedure is proposed to generate 
columns and the integer problem is solved by decomposition solution approaches. A computational study is conducted using randomly generated data and 
an analysis showing the impact of the solution approaches in the two-stage integrated problem is presented. In addition, the performance and benefits 
of the integrated approach are compared to an empirical simulation of the common practice (sequential approach).
