<?php
/* 
	Le code contenu dans cette page ne sera éxecuté qu'à la désactivation du plugin 
	Vous pouvez donc l'utiliser pour supprimer des tables SQLite, des dossiers, ou executer une action
	qui ne doit se lancer qu'à la désinstallation ex :
*/

$table = new detecteur();
$table->drop();
$table= new detection();
$table->drop();
$table= new viewDetection();
$table->drop();
$table= new contact();
$table->drop();

$table_section = new Section();
$id_section = $table_section->load(array("label"=>"detection"))->getId();
$table_section->delete(array('label'=>'detection'));

$table_right = new Right();
$table_right->delete(array('section'=>$id_section));
?>