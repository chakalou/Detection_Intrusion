<?php
/* 
	Le code contenu dans cette page ne sera éxecuté qu'à l'activation du plugin 
	Vous pouvez donc l'utiliser pour créer des tables SQLite, des dossiers, ou executer une action
	qui ne doit se lancer qu'à l'installation ex :
	
*/
	require_once('Detection.class.php');
	$table = new detecteur();
	$table->create();
	
	$table= new detection();
	$table->create();
	
	$table= new viewDetection();
	$table->create();
	
	$table= new contact();
	$table->create();
	
	$s1 = New Section();
	$s1->setLabel('detection');
	$s1->save();
	
	$r1 = New Right();
	$r1->setSection($s1->getId());
	$r1->setRead('1');
	$r1->setDelete('1');
	$r1->setCreate('1');
	$r1->setUpdate('1');
	$r1->setRank('1');
	$r1->save();
?>