# Calculateur_trajet_metro

Ce programme permet de donner le chemin le plus court (sauf quelques exceptions) reliant 2 stations de métro parisien données, avec une implémentation de l'algorithme de Dijkstra.

Pour compiler :

	gcc -O3 -g -Wall metro.c -o metro

Pour lancer : 

	./metro
	
Les changements de ligne comptent pour 5min et le voyage d'une station à une autre sur la même ligne compte pour 1min.
