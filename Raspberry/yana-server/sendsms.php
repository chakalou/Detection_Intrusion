<?php
$numero = $_GET['numero'];

$texte = $_GET['texte'];
echo `echo "$texte" | gammu sendsms TEXT "$numero" > toto.log 2>&1 &`
?>
