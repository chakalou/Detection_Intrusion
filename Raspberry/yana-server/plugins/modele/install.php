<?php
/* 
	Le code contenu dans cette page ne sera �xecut� qu'� l'activation du plugin 
	Vous pouvez donc l'utiliser pour cr�er des tables SQLite, des dossiers, ou executer une action
	qui ne doit se lancer qu'� l'installation ex :
	
*/
	require_once('Modele.class.php');
	$table = new modele();
	$table->create();
?>