Diretorias do Projeto:
CircuitRouter-SimpleShell corresponde à shell realizada na primeira entrega de projeto
CircuitRouter-SeqSolver corresponde à versão sequencial.
CircuitRouter-ParSolverCoarse corresponde à versão já paralelizada, com um trinco global para a grid.
CircuitRouter-ParSolverFine corresponde a outra versão paralelizada, com um trinco para cada posição da matriz.

O script doTest.sh faz o que foi pedido no enunciado, e o testAll.sh corre o doTest.sh para todos os ficheiros de input que acabem em .txt (tendo alterado o sufixo random-x512-y512-z7-n512 de .txt para .notest para evitar testar).
