<?php

/*
 @nom: Detection
 @auteur: Chakalou 
 @description:  Plugin permettant de gérer les détecteurs associé au programme détection
 */

//Ce fichier permet de gerer vos donnees en provenance de la base de donnees

//Il faut changer le nom de la classe ici (je sens que vous allez oublier)
class Detection extends SQLiteEntityDectection{

	
	protected $id_detecteur,$piece,$codedetec; //Pour rajouter des champs il faut ajouter les variables ici...
	protected $TABLE_NAME = 'plugin_detection'; 	//Penser a mettre le nom du plugin et de la classe ici
	protected $CLASS_NAME = 'detection';
	protected $object_fields = 
	array( //...Puis dans l'array ici mettre nom du champ => type
		'id_detecteur'=>'key',
		'piece'=>'string',
		'codedetec'=>'int'
	);

	function __construct(){
		parent::__construct();
	}
//Methodes pour recuperer et modifier les champs (set/get)
	function setId_detecteur($id_detecteur){
		$this->id_detecteur = $id_detecteur;
	}
	
	function getId_detecteur(){
		return $this->id_detecteur;
	}

	function getPiece(){
		return $this->piece;
	}

	function setPiece($piece){
		$this->piece = $piece;
	}

	function getCodedetec(){
		return $this->codedetec;
	}

	function setCodedetec($codedetec){
		$this->codedetec = $codedetec;
	}

}

?>