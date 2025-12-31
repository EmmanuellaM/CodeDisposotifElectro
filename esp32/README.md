# Formatage des données
Les données en envoyer representeront un enregistrement (ou ligne) complet d'une table __paramètres__ dont les champs (colonnes) sont les suivants : 

-> id          : (chaine de caractères) identifiant unique du patient (la femme enceinte)
-> poids       : (float) le poids de la femme enceinte 
-> pression    : (float) la pression artérielle de la femme enceinte
-> température : (float) la température corporelle de la femme enceinte
-> saturation  : (float) la saturation de l'oxygène de la femme enceinte

Ces données seront envoyées sous la forme suivante : 
(id : <valeur>, poids : <valeur>, pression : <valeur>, température : <valeur>, saturation : <valeur>).

Il faut donc regrouper ces données dans une variable unique (un objet 'parametres').
Cet objet 'parametres' doit être instancié en donnant l'id, puis il doit y avoir des methodes pour set les autres attributs. Il ne doit pas avoir deux objets 'parametres' différents instanciés  à un moment donné. 

## Description de la construction de l'objet 'parametres'
1. Notre librairie doit exposer une méthode statique 'set_id(id)' qui crée l'objet 'parametres' et set l'id.
2. Notre librairie doit exposer des méthodes statiques 'set_poids(poids)', 'set_pression(pression)', 'set_temperature(temperature)', 'set_saturation(saturation)' pour set les autres attributs. Ces méthodes doivent vérifier que l'objet 'parametres' existe bien et qu'il n'y a pas d'autres objets 'parametres' instanciés, puis set les attributs.
3. Notre librairie doit exposer une méthode statique 'send_parameters()' qui, lorsqu'elle est appelée, formate les données de l'objet parametres (via une methode de formatage) et envoie les données au serveur.
4. La méthode de formatage retourne une chaine de caractères au format suivant : 
(id : <valeur>, poids : <valeur>, pression : <valeur>, température : <valeur>, saturation : <valeur>).

# Envoi des données
Les données seront envoyées via le protocole I2C ou UART.
1. Configuration des broches I2C : SDA et SCL.
2. Configuration des broches UART : TX et RX.
3. Envoi des données  formatées via le protocole I2C ou UART.