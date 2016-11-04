<?php
/*
@name Detection
@author Chakalou
@
@licence CC by nc sa
@version 1.0.0
@Plugin permettant de gérer les détecteurs associés au programme détection
*/

//Si vous utiliser la base de donnees a ajouter
include('Detection.class.php');

//Cette fonction va generer un nouveau element dans le menu
function detection_plugin_menu(&$menuItems){
	global $_;
	$menuItems[] = array('sort'=>10,'content'=>'<a href="index.php?module=detection"><i class="icon-th-large"></i> Détection</a>');
}

//Cette fonction va generer une page quand on clique sur Détection dans menu
function detection_plugin_page($_){
	if(isset($_['module']) && $_['module']=='detection'){
	?>
	<div class="span3 bs-docs-sidebar">
	  <ul class="nav nav-tabs nav-stacked">
	    <li  class="active"><a href="#gestiondemarrage"><i class="icon-chevron-right"></i> 1. Gestion démarrage/arrêt prog</a></li>
	    <li><a href="#tableau""><i class="icon-chevron-right"></i> 2. Tableau détection</a></li>
	    <li><a href=""><i class="icon-chevron-right"></i> 3. Menu 3</a></li>
	    <li><a href=""><i class="icon-chevron-right"></i> 4. Menu <span class="badge badge-warning">4</span></a></li>
	  </ul>
	</div>

	<div class="span9">


	<h1>Détection</h1>
	<p>Plugin permettant de gérer les détecteurs associés au Raspberry</p>
	<div><h2 id="gestiondemarrage">Gestion démarrage / arrêt du programme</h2>
	 <button class="btn" onclick="window.location='action.php?action=demarre_detection'">Démarrer détection</button>
	<button class="btn" onclick="window.location='action.php?action=arret_detection'">Arrêt détection</button><br/> </div>
		<h2>Bloc d'onglets</h2>

	    <ul class="nav nav-tabs">
	      <li class="active"><a href="#">Onglet 1</a></li>
	      <li><a href="#">Onglet 2</a></li>
	      <li><a href="#">Onglet 3</a></li>
	    </ul>

	    <h2 id="tableau">Tableau détection</h2>
	    <table class="table table-striped table-bordered table-hover">
	    <thead>
	    <tr>
	    <th>Title</th>
	    <th>Title</th>
	    </tr>
	    </thead>
	    <tr><td>col1</td><td>col2</td></tr>
	    <tr><td>col3</td><td>col4</td></tr>
	    </table>

	    <h2>Barre de progression</h2>
	    <div class="progress progress-striped active">
	    <div class="bar" style="width: 40%;"></div>
	    </div>

	    <h2>Pagination</h2>
	    <div class="pagination">
	    <ul>
	    <li><a href="#">Prev</a></li>
	    <li><a href="#">1</a></li>
	    <li><a href="#">2</a></li>
	    <li><a href="#">3</a></li>
	    <li><a href="#">4</a></li>
	    <li><a href="#">5</a></li>
	    <li><a href="#">Next</a></li>
	    </ul>
	    </div>

	</div>
<?php
	}
}
//fonction permettant de générer un onglet dans le menu configuration
function detection_plugin_setting_menu(){
	global $_;
	echo '<li '.(isset($_['section']) && $_['section']=='detection'?'class="active"':'').'><a href="setting.php?section=detection"><i class="icon-chevron-right"></i> Détecteur</a></li>';
	
}
function detection_action_detection(){
	global $_,$conf;

    
	switch($_['action']){
		case 'demarre_detection':
			exec('. /home/pi/Projet/bin/lancerprog.sh > /home/pi/Projet/log/log.log &');
			header('location:index.php?module=detection');
		break;
		case 'arret_detection':
			exec('. /home/pi/Projet/bin/arretprog.sh > /home/pi/Projet/log/log.log &');
			header('location:index.php?module=detection');
		break;
	}
}


function detection_plugin_setting_page(){}

Plugin::addCss("/css/style.css"); 
//Plugin::addJs("/js/main.js"); 

Plugin::addHook("menubar_pre_home", "detection_plugin_menu");  
Plugin::addHook("action_post_case", "detection_action_detection");  
Plugin::addHook("home", "detection_plugin_page");  
Plugin::addHook("setting_menu", "detection_plugin_setting_menu");  
Plugin::addHook("setting_bloc", "detection_plugin_setting_page"); 
?>