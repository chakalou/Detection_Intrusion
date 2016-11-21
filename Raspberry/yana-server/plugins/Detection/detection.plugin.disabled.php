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
	echo '<li '.(isset($_['section']) && $_['section']=='detection'?'class="active"':'').'><a href="setting.php?section=detection"><i class="icon-chevron-right"></i> Détection</a></li>';
	
}
function detection_plugin_setting_page(){
	global $myUser,$_;
	
	if(isset($_['section']) && $_['section']=='detection' ){
		if($myUser!=false){
			if (isset($_['block'])){
				$block = $_['block'];
			}
			else
			{
				$block='detec';
			}
			switch($block){
				case 'detec':
					$detecteurManager = new Detecteur();
					$detecteurs = $detecteurManager->populate();
				//Gestion des modifications des detecteurs
				if (isset($_['id_detec'])){
					$id_mod = $_['id_detec'];
					$selected = $detecteurManager->getById($id_mod);
					$piece = $selected->getPiece();
					
					$button = "Modifier";
				}
				else
				{
					$piece =  "Ajout d'un capteur";
					$button = "Ajouter";
				} 

				?>

				<div class="span9 userBloc">
				<ul class="nav nav-tabs">
						<li  {if="'<?echo $block?>'=='detec'"}class="active"{/if}><a  href="setting.php?section=detection&block=detec"><i class="icon-chevron-right"></i> Détecteurs</a></li>
						<li {if="'<?echo $block?>'=='contact'"}class="active"{/if}><a  href="setting.php?section=detection&block=contact"><i class="icon-chevron-right"></i> Contacts</a></li>
				</ul>

					<h1>Détecteurs</h1>
					<p>Gestion des détecteurs associés au plugin détection</p>  
					<table class="table table-striped table-bordered table-hover">
						<thead>
							<tr>
								<th>Code Détecteur</th>
								<th>Pièce</th>
								<th></th> 
							</tr>
						</thead>

						<?php foreach($detecteurs as $detecteur){ ?>
						<tr>
							<td><?php echo $detecteur->getCodedetec(); ?></td>
							<td><?php echo $detecteur->getPiece(); ?></td>
							<td><a class="btn" href="action.php?action=detection_delete_detecteur&id_detec=<?php echo $detecteur->getId_detecteur(); ?>"><i class="icon-remove"></i></a>
								<a class="btn" href="setting.php?section=detection&block=detec&id_detec=<?php echo $detecteur->getId_detecteur(); ?>"><i class="icon-edit"></i></a></td>
							</tr>
							<?php } ?>
						</table>
					<form action="action.php?action=detection_add_detecteur" method="POST"> 
						<fieldset>
							<legend><? echo $piece ?></legend>

							<div class="left">
								<label for="codeDetec">Code Détection</label>
								<? if(isset($selected)){echo '<input type="hidden" name="id_detec" value="'.$id_mod.'">';} ?>
								<input type="text" value="<? if(isset($selected)){echo $selected->getCodedetec();} ?>" id="codeDetec" name="codeDetec" placeholder="Numéro..."/>
								<label for="nameRoom">Pièces</label>
								<input type="text" value="<? if(isset($selected)){echo $selected->getPiece();} ?>" name="nameRoom" id="nameRoom" placeholder="Cuisine, Salon..." />
							</div>

							<div class="clear"></div>
							<br/><button type="submit" class="btn"><? echo $button; ?></button>
						</fieldset>
						<br/>
					</form>

					
					</div>
					<?php
				break;
				case 'contact':
					$contactManager = new Contact();
					$contacts = $contactManager->populate();
					//Gestion des modifications des detecteurs
				if (isset($_['id_contact'])){
					$id_mod = $_['id_contact'];
					$selected = $contactManager->getById($id_mod);
					$mail = $selected->getMail();
					
					$button = "Modifier";
				}
				else
				{
					$mail =  "Ajout d'un destinataire";
					$button = "Ajouter";
				} 

				?>

				<div class="span9 userBloc">
				<ul class="nav nav-tabs">
						<li  {if="'<?echo $block?>'=='detec'"}class="active"{/if}><a  href="setting.php?section=detection&block=detec"><i class="icon-chevron-right"></i> Détecteurs</a></li>
						<li {if="'<?echo $block?>'=='contact'"}class="active"{/if}><a  href="setting.php?section=detection&block=contact"><i class="icon-chevron-right"></i> Contacts</a></li>
				</ul>

					<h1>Contacts</h1>
					<p>Gestion des destinataires des mails associés au plugin détection en cas de détection</p>  
					<table class="table table-striped table-bordered table-hover">
						<thead>
							<tr>
								<th>Mail</th>
								<th>Notif mail</th>
								<th>Numéro</th>
								<th>Notif sms</th>
								<th></th> 
							</tr>
						</thead>

						<?php foreach($contacts as $contact){ ?>
						<tr>
							<td><?php echo $contact->getMail(); ?></td>
							<td><?php if($contact->getNotifymail()==1){echo 'oui';}else{echo 'non';} ?></td>
							<td><?php echo $contact->getTel(); ?></td>
							<td><?php if($contact->getNotifysms()==1){echo 'oui';}else{echo 'non';} ?></td>
							<td><a class="btn" href="action.php?action=detection_delete_contact&id_contact=<?php echo $contact->getId(); ?>"><i class="icon-remove"></i></a>
								<a class="btn" href="setting.php?section=detection&block=contact&id_contact=<?php echo $contact->getId(); ?>"><i class="icon-edit"></i></a></td>
							</tr>
							<?php } ?>
						</table>
					
					<form action="action.php?action=detection_add_contact" method="POST"> 
						<fieldset>
							<legend><? echo $mail; ?></legend>

							<div class="left">
								<h2><label for="mail">Mail:</label></h2>
								<? if(isset($selected)){echo '<input type="hidden" name="id_contact" value="'.$id_mod.'">';} ?>
								<input type="text" value="<? if(isset($selected)){echo $selected->getMail();} ?>" id="mail" name="mail" placeholder="Email..."/>
								<table class="table table-striped table-bordered table-hover">
								<thead>
								<tr>
								<td style="text-align: center; vertical-align: middle;"><label for="notifymail">Activer notification mail</label></td>
								 <td ><input type="checkbox" name="notifymail" id="notifymail" value="1" <?if(isset($selected)){if($selected->getNotifymail()==1){echo 'checked';}}?>/></td>
								</tr>
								</table>
								
								<h2><label for="tel">Téléphone:</label></h2>
								<input type="text" value="<? if(isset($selected)){echo $selected->getTel();} ?>" name="tel" id="tel" placeholder="Numéro de téléphone..." />
								<table class="table table-striped table-bordered table-hover">
								<thead>
								<tr>
								<td style="text-align: center; vertical-align: middle;"><label for="notifysms">Activer notification sms</label></td>
								<td ><input type="checkbox" name="notifysms" id="notifysms" value="1" <?if(isset($selected)){if($selected->getNotifysms()==1){echo 'checked';}}?>/></td>
								</tr>
								</table>
							</div>

							<div class="clear"></div>
							<br/><button type="submit" class="btn"><? echo $button; ?></button>
						</fieldset>
						<br/>
					</form>
					</div>

				<?php
				break;
				}?>

			<?php 
			}
			else{ ?>

				<div id="main" class="wrapper clearfix">
					<article>
						<h3>Vous devez être connecté</h3>
					</article>
				</div>
				<?php
			}
		}

	}

function detection_action_detection(){
	//global $_,$conf;
	global $_,$myUser;
						
		//Erreur dans les droits sinon!
		$myUser->loadRight();
    
	switch($_['action']){
		case 'demarre_detection':
			exec('. /home/pi/Projet/bin/lancerprog.sh > /home/pi/Projet/log/log.log &');
			header('location:index.php?module=detection');
		break;
		case 'arret_detection':
			exec('. /home/pi/Projet/bin/arretprog.sh > /home/pi/Projet/log/log.log &');
			header('location:index.php?module=detection');
		break;
		case 'detection_add_detecteur':
		$right_toverify = isset($_['id_detec']) ? 'u' : 'c';
			if($myUser->can('detection',$right_toverify))
			{
				$detecteur = new Detecteur();
				if ($right_toverify == "u"){$detecteur = $detecteur->load(array("id"=>$_['id_detec']));}
				$detecteur->setPiece($_['nameRoom']);
				$detecteur->setCodedetec($_['codeDetec']);
				$detecteur->save();
				header('location:setting.php?section=detection&block=detec');
			}
			else
			{
				header('location:setting.php?section=detection&error=Vous n\'avez pas le droit de faire ça!');
			}
		break;
		case 'detection_delete_detecteur':
		$right_toverify = isset($_['id_detec']) ? 'u' : 'c';
			if($myUser->can('detection',$right_toverify))
			{
				$detecteur = new Detecteur();
				if ($right_toverify == "u"){$detecteur = $detecteur->load(array("id"=>$_['id_detec']));}
				$detecteurManager = new Detecteur();
				$detecteurManager->delete(array('id'=>$_['id_detec']));
				header('location:setting.php?section=detection');
			}
			else
			{
				header('location:setting.php?section=detection&error=Vous n\'avez pas le droit de faire ça!');
			}
		break;
		case 'detection_add_contact':
		$right_toverify = isset($_['id_contact']) ? 'u' : 'c';
			if($myUser->can('detection',$right_toverify))
			{
				$contact = new Contact();
				if ($right_toverify == "u"){$contact = $contact->load(array("id"=>$_['id_contact']));}
				$contact->setMail($_['mail']);
				$contact->setTel($_['tel']);
				if(isset($_['notifysms']))
				{
					$contact->setNotifysms($_['notifysms']);
				}
				else
				{
					$contact->setNotifysms(0);
				}
				if(isset($_['notifymail']))
				{
					$contact->setNotifymail($_['notifymail']);
				}
				else
				{
					$contact->setNotifymail(0);
				}
				//$message='mail='.$contact->getMail().'tel='.$contact->getTel().'sms='.$contact->getNotifysms().'mail2='.$contact->getNotifymail();
				//echo '<script type="text/javascript">window.alert("'.$message.'");</script>';
				$contact->save();
				
				header('location:setting.php?section=detection&block=contact');
			}
			else
			{
				header('location:setting.php?section=detection&error=Vous n\'avez pas le droit de faire ça!');
			}
		break;
		case 'detection_delete_contact':
		$right_toverify = isset($_['id_contact']) ? 'u' : 'c';
			if($myUser->can('detection',$right_toverify))
			{
				$contact = new Contact();
				if ($right_toverify == "u"){$contact = $contact->load(array("id"=>$_['id_contact']));}
				$contactManager = new Contact();
				$contactManager->delete(array('id'=>$_['id_contact']));
				header('location:setting.php?section=detection&block=contact');
			}
			else
			{
				header('location:setting.php?section=detection&error=Vous n\'avez pas le droit de faire ça!');
			}
		break;
	}
}


Plugin::addCss("/css/style.css"); 
//Plugin::addJs("/js/main.js"); 

Plugin::addHook("menubar_pre_home", "detection_plugin_menu");  
Plugin::addHook("action_post_case", "detection_action_detection");  
Plugin::addHook("home", "detection_plugin_page");  
Plugin::addHook("setting_menu", "detection_plugin_setting_menu");  
Plugin::addHook("setting_bloc", "detection_plugin_setting_page"); 
?>