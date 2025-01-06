#define pinMotor 5 // Pin du moteur (Compatible PWM)
#define pinSensor 7 // Pin du capteur de trou
#define pinLed 3 // Pin de la LED
#define pinBtn 8 // Pin du bouton de changement de mode
#define nb_images 12 // Nombre d'images sur le disque
#define nb_trous 60 // Nombre de trous sur le disque
unsigned long start, tpsLed; // Temps de démarrage et temps de la dernière allumage de la LED
int hole = 0; // Nombre de "trous" détectés
int lightHole = 0; // Nombre de trous pour la lumière
int selectedSpeed; // Vitesse sélectionnée pour le moteur (Mode: 1-4)
int mode = 5;
bool EtatLED;
int lastSpeed = HIGH; // Dernière valeur lue du capteur
  
void setup() {
  pinMode(pinSensor, INPUT); // Capteur en entrée (LOGIQUE)
  pinMode(pinLed, OUTPUT); // LED en sortie (LOGIQUE)
  pinMode(pinBtn, INPUT); // Bouton en entrée (LOGIQUE)
  pinMode(pinMotor, OUTPUT); // Moteur en sortie (PWM)
  pinMode(A0, INPUT); // Potentiomètre en entrée (ANALOGIQUE)
  Serial.begin(9600);
}
 
void loop() {
  unsigned long current = millis(); // Récupération du temps actuel
  // Lecture du potentiomètre et contrôle du moteur
  int valpot = analogRead(A0); // Lecture de la valeur du potentiomètre
  if(valpot >= 750) valpot = 750; // Bridage pour éviter que le moteur crash
  int valum = map(valpot, 0, 1023, 0, 255); // Conversion de la valeur du potentiomètre en valeur de PWM (1 octet)
  int btnState = digitalRead(pinBtn); // Lecture de l'état du bouton
  
  
  bool valSpeed = digitalRead(pinSensor); // Lecture de la valeur du capteur

  /**
   * On fait tourner le moteur à une vitesse fixe en fonction du mode
   */
  if(mode == 5){ // Mode potentiomètre
    analogWrite(pinMotor, valum);
  }else{ // Mode de vitesses à valeurs fixes
    analogWrite(pinMotor, selectedSpeed);
  }

  // On verifie que la dernière valeur lu est 1 et qu'on est fasse à un trou (état nul)
  if (valSpeed == 0 && lastSpeed != 0) { 
    hole++; // On incrémente le nombre de trous
    lightHole++; // On incrémente le nombre de trous pour la lumière
  }
  if(lightHole >= nb_trous/nb_images){ // On allume la LED tous les 60/nb_images trous
    lightHole = 0; // On réinitialise le compteur pour la lumière (nouveau cycle)
    tpsLed = current; // On enregistre le temps actuel
    EtatLED= 1;
  }
  if (((current-tpsLed) > 4) & (EtatLED ==1)){ // On éteint la LED après 4ms
    EtatLED = 0;
  }

  if(btnState){ // En cas d'appui sur le bouton, on change de mode
    mode++;
    if(mode > 5) mode = 1; // On revient au premier mode si on dépasse le dernier
    displayMode(mode); // Affichage du mode
    delay(300); // Délai pour éviter les rebonds
  }
  
  
  
  // Mise à jour toutes les secondes
  
  if (current - start >= 1000) {
    float finalSpeed = hole / (float) nb_trous; // Vitesse en tours par seconde
    int imageSpeed = finalSpeed * nb_images; // Vitesse en images par seconde
    // Message de debug
    Serial.println(String(finalSpeed) + " tour/s | " +String(imageSpeed)+ " images/s (hole): " + String(hole) + " | Mode: " + mode);
    hole = 0; // Réinitialisation du compteur de trous
  }
  digitalWrite(pinLed,EtatLED);
  lastSpeed = valSpeed;
}

/**
 * Affiche le mode actuel de capture utilisé
 * En fonction de vitesses prédéfinies ou du potentiomètre
 * @param mode Mode de capture suivant
 */
void displayMode(int mode){
  String text = "";
  switch(mode){
    case 1:
      text = "8 images/s";
      selectedSpeed = 65;
      break;
    case 2:
      text = "12 images/s";
      selectedSpeed = 83;
      break;
    case 3:
      text = "18 images/s";
      selectedSpeed = 112;
      break;
    case 4:
      text = "24 images/s";
      selectedSpeed = 150;
      break;
    case 5:
      text = "Potentiomètre";
      break; 
  }
  Serial.println("Mode de capture:" + String(text));
}
