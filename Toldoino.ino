#include <Servo.h>
#include <MeetAndroid.h>

Servo motorToldo;
MeetAndroid meetAndroid;
int d;

bool ultimoToldo = LOW; // LOW = aberto HIGH = fechado
bool atualToldo = LOW;

bool ultimaChuva = LOW; // LOW = seco HIGH = molhado
bool atualChuva = LOW;

bool ultimoSol = LOW; //LOW = pouco sol HIGH = muito sol
bool atualSol = LOW;

bool ultimoBotao = LOW; // LOW = repouso HIGH = apertado
bool atualBotao = LOW;

int rain;             //Variaveis de leitura 
int sun;
bool button;
bool automatico = LOW; // Default: Modo manual

String strRain;
String strSun;
String strToldo;

void setup() {
  Serial.begin(38400); //Seta a velocidade de comunicação com o módulo BT
//  meetAndroid.registerFunction(lerSensores, 'A');
  meetAndroid.registerFunction(androidToldo, 'T');
   meetAndroid.registerFunction(setupSystem, 'S');
   meetAndroid.registerFunction(updateState, 'U');
  motorToldo.attach(9); // Conecta o controle do servo ao pino 9
  motorToldo.write(0); //Posiciona o toldo como aberto.
  pinMode (2, INPUT); // Pushbutton

}

void loop() {
  
  meetAndroid.receive(); // Recebe dados do Android
  rain = analogRead(A0); //Lê o sensor de chuva
  
  sun = analogRead(A1); //Lê o LDR
  button = digitalRead(2); //Lê o valor do pushbutton


  if (automatico == LOW) {

  if (button != ultimoBotao) { //compara o estado lido do botao com o estado anterior
      delay(10);  // aguarda 10ms
      button = digitalRead(2); // faz uma nova leitura do botão
      if (button != ultimoBotao) { // compara o valor da nova leitura com o valor atual
        button = ultimoBotao; // valor lido passa a ser o valor atual
         ultimoBotao = atualBotao; // o ultimo valor passa a ser o atual
         if (ultimoToldo == HIGH && ultimaChuva == LOW) { // verifica o estado do toldo e abre ou fecha conforme estado
          abreToldo();
          updateState('U','1');
          ultimoToldo=atualToldo;
         }
         else {
          fechaToldo();
          updateState('U','1');
          ultimoToldo=atualToldo;
         }
         
      }
  }
  } else { // Modo totalmente automático

     
  if (rain < 512 && ultimoToldo == LOW && ultimoSol == LOW) { 
    atualChuva = HIGH;
    fechaToldo();
    updateState('U','1');
    ultimoToldo=atualToldo;
    ultimaChuva=atualChuva;
  }

  if (rain > 900 && ultimoToldo == HIGH && ultimoSol == LOW) {
    atualChuva = LOW;
    abreToldo();
    updateState('U','1');
    ultimoToldo=atualToldo;
    ultimaChuva=atualChuva;
  }

  if (sun < 300 && ultimoToldo == LOW && ultimaChuva == LOW) {
    atualSol = HIGH;
    fechaToldo();
    updateState('U','1');
    ultimoSol=atualSol;
    ultimoToldo=atualToldo;
  }

  if (sun > 400 && ultimoToldo == HIGH && ultimaChuva == LOW) {
    atualSol = LOW;
    abreToldo();
    updateState('U','1');
    ultimoSol=atualSol;
    ultimoToldo=atualToldo;
  }

  }

}

void fechaToldo() {
  for (d = 0 ; d <= 50 ; d++) {
  motorToldo.write(d);
  delay (10);
  }
  atualToldo=HIGH;
}

void abreToldo() {
  for (d = 50 ; d >= 0 ; d--) {
  motorToldo.write(d);
  delay (10);
  }
  atualToldo=LOW;
}

void androidToldo(byte flag, byte numOfValues) {
 if (ultimoToldo == LOW && ultimaChuva == LOW) { // verifica o estado do toldo e abre ou fecha conforme estado
          fechaToldo(); 
          updateState('U','1');
          ultimoToldo=atualToldo;
         } else {
          abreToldo();
          updateState('U','1');
          ultimoToldo=atualToldo;
         }
  
}

void setupSystem(byte flag, byte numOfValues) {

   automatico=meetAndroid.getInt();
}

void updateState(byte flag, byte numOfValues) {
  String strResposta;
  if (ultimoToldo != atualToldo || ultimaChuva != atualChuva || ultimoSol != atualSol) {
  if (atualToldo == HIGH){
    strToldo="true";
  } else {
    strToldo="false";
  }
    if (atualChuva == HIGH){
    strRain="true";
  } else {
    strRain="false";
  }
    if (atualSol == HIGH){
    strSun="true";
  } else {
    strSun="false";
  }
  strResposta=strToldo+";"+strRain+";"+strSun;
  char Resposta[18];
  strResposta.toCharArray(Resposta,18);
  meetAndroid.send(Resposta);
}
} 




