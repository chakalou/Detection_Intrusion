<?php

/*
 @nom: Detection
 @auteur: Chakalou 
 @description:  Plugin permettant de gérer les détecteurs associé au programme détection
 */

//Ce fichier permet de gerer vos donnees en provenance de la base de donnees

//Il faut changer le nom de la classe ici (je sens que vous allez oublier)
class Detecteur extends SQLiteEntityDectection{

	
	protected $id,$piece,$codedetec; //Pour rajouter des champs il faut ajouter les variables ici...
	protected $TABLE_NAME = 'detecteur'; 	//Penser a mettre le nom du plugin et de la classe ici
	protected $CLASS_NAME = 'detecteur';
	protected $object_fields = 
	array( //...Puis dans l'array ici mettre nom du champ => type
		'id'=>'key',
		'piece'=>'string',
		'codedetec'=>'int'
	);

	function __construct(){
		parent::__construct();
	}
//Methodes pour recuperer et modifier les champs (set/get)
	function setId_detecteur($id){
		$this->id = $id;
	}
	
	function getId_detecteur(){
		return $this->id;
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

class Detection extends SQLiteEntityDectection{

	
	protected $id,$id_detecteur,$image,$date; //Pour rajouter des champs il faut ajouter les variables ici...
	protected $TABLE_NAME = 'detection'; 	//Penser a mettre le nom du plugin et de la classe ici
	protected $CLASS_NAME = 'detection';
	protected $object_fields = 
	array( //...Puis dans l'array ici mettre nom du champ => type
		'id'=>'key',
		'id_detecteur'=>'int',
		'image'=>'string',
		'date'=>'date'
	);

	function __construct(){
		parent::__construct();
	}
//Methodes pour recuperer et modifier les champs (set/get)
	function setId($id){
		$this->id = $id;
	}
	
	function getId(){
		return $this->id;
	}
	function setId_Deteteur($id_detecteur){
		$this->id_detecteur = $id_detecteur;
	}
	
	function getId_Deteteur(){
		return $this->id_detecteur;
	}
	
	function getImage(){
		return $this->image;
	}

	function setImage($image){
		$this->image = $image;
	}
	
	function getDate(){
		return $this->date;
	}

	function setDate($date){
		$this->date = $date;
	}

}

class Contact extends SQLiteEntityDectection{

	
	protected $id,$mail,$tel; //Pour rajouter des champs il faut ajouter les variables ici...
	protected $TABLE_NAME = 'contact'; 	//Penser a mettre le nom du plugin et de la classe ici
	protected $CLASS_NAME = 'contact';
	protected $object_fields = 
	array( //...Puis dans l'array ici mettre nom du champ => type
		'id'=>'key',
		'mail'=>'string',
		'tel'=>'string',
		'notifymail'=>'int',
		'notifysms'=>'int'
	);

	function __construct(){
		parent::__construct();
	}
//Methodes pour recuperer et modifier les champs (set/get)
	function setId($id){
		$this->id = $id;
	}
	
	function getId(){
		return $this->id;
	}
	function setMail($mail){
		$this->mail = $mail;
	}
	
	function getMail(){
		return $this->mail;
	}
	
	function getTel(){
		return $this->tel;
	}

	function setTel($tel){
		$this->tel = $tel;
	}
	function getNotifysms(){
		return $this->notifysms;
	}

	function setNotifysms($notifysms){
		$this->notifysms = $notifysms;
	}
	function getNotifymail(){
		return $this->notifymail;
	}

	function setNotifymail($notifymail){
		$this->notifymail = $notifymail;
	}

}

class ViewDetection extends SQLite3
{
	protected $id,$codedetec, $piece, $image, $date ; //Pour rajouter des champs il faut ajouter les variables ici...
	protected $VIEW_NAME = 'v_detection'; 	//Penser a mettre le nom du plugin et de la classe ici
	protected $CLASS_NAME = 'viewdetection';
	protected $object_fields = 
	array( //...Puis dans l'array ici mettre nom du champ => type
		'id'=>'key',
		'codedetec'=>'int',
		'piece'=>'string',
		'image'=>'string',
		'date'=>'date'
	);
	private $debug = false;
	
	function __construct(){
		$this->open(DB_NAME_DETECTION);
	}
	function __destruct(){
		 $this->close();
	}

	function sgbdType($type){
		$return = false;
		switch($type){
			case 'string':
			case 'timestamp':
			case 'date':
				$return = 'VARCHAR(255)';
			break;
			case 'longstring':
				$return = 'longtext';
			break;
			case 'key':
				$return = 'INTEGER NOT NULL PRIMARY KEY';
			break;
			case 'object':
			case 'integer':
				$return = 'bigint(20)';
			break;
			case 'boolean':
				$return = 'INT(1)';
			break;
			default;
				$return = 'TEXT';
			break;
		}
		return $return ;
	}
	

	public function closeDatabase(){
		$this->close();
	}


	// GESTION SQL



	/**
	* Methode de creation de l'entité
	* @author Valentin CARRUESCO
	* @category manipulation SQL
	* @param <String> $debug='false' active le debug mode (0 ou 1)
	* @return Aucun retour
	*/
	public function create($debug='false'){
		$query = 'CREATE VIEW IF NOT EXISTS `'.MYSQL_PREFIX.$this->VIEW_NAME.'` as select yana_detection.id,codedetec, piece, image, date from yana_detection, yana_detecteur where yana_detection.id_detecteur=yana_detecteur.id;';
		if($this->debug)echo '<hr>'.$this->CLASS_NAME.' ('.__METHOD__ .') : Requete --> '.$query;
		if(!$this->exec($query)) echo $this->lastErrorMsg();
	}

	public function drop($debug='false'){
		$query = 'DROP VIEW `'.MYSQL_PREFIX.$this->VIEW_NAME.'`;';
		if($this->debug)echo '<hr>'.$this->CLASS_NAME.' ('.__METHOD__ .') : Requete --> '.$query;
		if(!$this->exec($query)) echo $this->lastErrorMsg();
	}

	/**
	* Méthode de selection de tous les elements de l'entité
	* @author Valentin CARRUESCO
	* @category manipulation SQL
	* @param <String> $ordre=null
	* @param <String> $limite=null
	* @param <String> $debug='false' active le debug mode (0 ou 1)
	* @return <Array<Entity>> $Entity
	*/
	public function populate($order='null',$limit='null',$debug='false'){
		eval('$results = '.$this->CLASS_NAME.'::loadAll(array(),\''.$order.'\','.$limit.',\'=\','.$debug.');');
		return $results;
	}


	/**
	* Méthode de selection multiple d'elements de l'entité
	* @author Valentin CARRUESCO
	* @category manipulation SQL
	* @param <Array> $colonnes (WHERE)
	* @param <Array> $valeurs (WHERE)
	* @param <String> $ordre=null
	* @param <String> $limite=null
	* @param <String> $operation="=" definis le type d'operateur pour la requete select
	* @param <String> $debug='false' active le debug mode (0 ou 1)
	* @return <Array<Entity>> $Entity
	*/
	public function loadAll($columns,$order=null,$limit=null,$operation="=",$debug='false',$selColumn='*'){
		$objects = array();
		$whereClause = '';
	
			if($columns!=null && sizeof($columns)!=0){
			$whereClause .= ' WHERE ';
				$i = false;
				foreach($columns as $column=>$value){

					if($i){$whereClause .=' AND ';}else{$i=true;}
					$whereClause .= '`'.$column.'`'.$operation.'"'.$value.'"';
				}
			}
			$query = 'SELECT '.$selColumn.' FROM `'.MYSQL_PREFIX.$this->VIEW_NAME.'` '.$whereClause.' ';
			if($order!=null) $query .='ORDER BY '.$order.' ';
			if($limit!=null) $query .='LIMIT '.$limit.' ';
			$query .=';';
			  
			//echo '<hr>'.__METHOD__.' : Requete --> '.$query.'<br>';
			$execQuery = $this->query($query);

			if(!$execQuery) 
				echo $this->lastErrorMsg();
			while($queryReturn = $execQuery->fetchArray() ){
				$object = eval(' return new '.$this->CLASS_NAME.'();');
				foreach($this->object_fields as $field=>$type){
					if(isset($queryReturn[$field])) eval('$object->'.$field .'= html_entity_decode(\''. addslashes($queryReturn[$field]).'\');');
				}
				$objects[] = $object;
				unset($object);
			}
			return $objects;
	}

	public function loadAllOnlyColumn($selColumn,$columns,$order=null,$limit=null,$operation="=",$debug='false'){
		eval('$objects = $this->loadAll($columns,\''.$order.'\',\''.$limit.'\',\''.$operation.'\',\''.$debug.'\',\''.$selColumn.'\');');
		if(count($objects)==0)$objects = array();
		return $objects;
	}

	/**
	* Méthode de selection unique d'élements de l'entité
	* @author Valentin CARRUESCO
	* @category manipulation SQL
	* @param <Array> $colonnes (WHERE)
	* @param <Array> $valeurs (WHERE)
	* @param <String> $operation="=" definis le type d'operateur pour la requete select
	* @param <String> $debug='false' active le debug mode (0 ou 1)
	* @return <Entity> $Entity ou false si aucun objet n'est trouvé en base
	*/
	public function load($columns,$operation='=',$debug='false'){
		eval('$objects = $this->loadAll($columns,null,\'1\',\''.$operation.'\',\''.$debug.'\');');
		if(!isset($objects[0]))$objects[0] = false;
		return $objects[0];
	}

	/**
	* Méthode de selection unique d'élements de l'entité
	* @author Valentin CARRUESCO
	* @category manipulation SQL
	* @param <Array> $colonnes (WHERE)
	* @param <Array> $valeurs (WHERE)
	* @param <String> $operation="=" definis le type d'operateur pour la requete select
	* @param <String> $debug='false' active le debug mode (0 ou 1)
	* @return <Entity> $Entity ou false si aucun objet n'est trouvé en base
	*/
	public function getById($id,$operation='=',$debug='false'){
		return $this->load(array('id'=>$id),$operation,$debug);
	}

	/**
	* Methode de comptage des éléments de l'entité
	* @author Valentin CARRUESCO
	* @category manipulation SQL
	* @param <String> $debug='false' active le debug mode (0 ou 1)
	* @return<Integer> nombre de ligne dans l'entité'
	*/
	public function rowCount($columns=null)
	{
		$whereClause ='';
		if($columns!=null){
			$whereClause = ' WHERE ';
			$i=false;
			foreach($columns as $column=>$value){
					if($i){$whereClause .=' AND ';}else{$i=true;}
					$whereClause .= '`'.$column.'`="'.$value.'"';
			}
		}
		$query = 'SELECT COUNT(id) FROM '.MYSQL_PREFIX.$this->VIEW_NAME.$whereClause;
		//echo '<hr>'.$this->CLASS_NAME.' ('.__METHOD__ .') : Requete --> '.$query.'<br>';
		$execQuery = $this->querySingle($query);
		//echo $this->lastErrorMsg();
		return (!$execQuery?0:$execQuery);
	}	
	
		
	public function customExecute($request){
		$this->exec($request);
	}
	public function customQuery($request){
		return $this->query($request);
	}

	// ACCESSEURS
		/**
	* Méthode de récuperation de l'attribut debug de l'entité
	* @author Valentin CARRUESCO
	* @category Accesseur
	* @param Aucun
	* @return <Attribute> debug
	*/
	
	public function getDebug(){
		return $this->debug;
	}
	public function getId(){
		return $this->id;
	}
	public function getCodedetec(){
		return $this->codedetec;
	}
	public function getPiece(){
		return $this->piece;
	}
	public function getImage(){
		return $this->image;
	}
	public function getDate(){
		return $this->date;
	}
	
	/**
	* Méthode de définition de l'attribut debug de l'entité
	* @author Valentin CARRUESCO
	* @category Accesseur
	* @param <boolean> $debug 
	*/

	public function setDebug($debug){
		$this->debug = $debug;
	}

	public function getObject_fields(){
		return $this->object_fields;
	}
}

?>