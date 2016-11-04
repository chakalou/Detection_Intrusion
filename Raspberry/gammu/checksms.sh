#!/bin/bash

#source /home/pi/.bashrc
PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/local/games:/usr/games
export PATH
PROJET=/home/pi/Projet
export PROJET

DIR_BIN=$PROJET/bin
DIR_ERREUR=$PROJET/erreur
DIR_PARAMETRAGE=$PROJET/parametrage
DIR_SHELL=$PROJET/shell
DIR_FICHIER=$PROJET/fichier
DIR_SOURCE=$PROJET/src
DIR_OBJET=$PROJET/OBJ
DIR_INCLUDE=$PROJET/include
DIR_LOG=$PROJET/log

export DIR_LOG
export DIR_INCLUDE
export DIR_OBJET

export DIR_BIN
export DIR_ERREUR
export DIR_PARAMETRAGE
export DIR_SHELL
export DIR_FICHIER
export DIR_SOURCE


# Mise a jour de PATH
# ------------------------------------------------------------
PATH=$PATH:$DIR_BIN:$DIR_SHELL
export PATH


date > $DIR_LOG/checksms.log 
chmod 770 $DIR_LOG/checksms.log
echo "Debut lancerprog.sh" >> $DIR_LOG/checksms.log 
echo "----------------------" >> $DIR_LOG/checksms.log

# Recherche d'un sms declenchant le demarrage de detection.x
# ----------------------------------------------------------

MESSAGE=$SMS_1_TEXT
export MESSAGE
echo "MESSAGE= $MESSAGE" >> $DIR_LOG/checksms.log
result=`echo "$MESSAGE" | grep -i start`
echo "result= $result" >> $DIR_LOG/checksms.log
#
# Si resultat, on lance le programme
# -------------------------------------------------
if [ "$result" != "" ] 
then
    echo "sms start trouve => on demarre detection " >> $DIR_LOG/checksms.log
	lancerprog.sh "$SMS_1_NUMBER" >> $DIR_LOG/checksms.log
fi
# Recherche d'un sms declenchant l'arret de detection.x
# ----------------------------------------------------------
result=`echo "$MESSAGE" | grep -i stop`
#
# Si resultat, on lance le programme
# -------------------------------------------------
if [ "$result" != "" ] 
then
    echo "sms stop trouve => on arrete detection " >> $DIR_LOG/checksms.log
	arretprog.sh "$SMS_1_NUMBER" >> $DIR_LOG/checksms.log
fi

#----------------------------------------------------------------
# Dans tous les cas, on supprime les sms en attente
#----------------------------------------------------------------
#gammu deleteallsms 1

exit
