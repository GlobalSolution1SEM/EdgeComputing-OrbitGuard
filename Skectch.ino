#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// ---- Pinos ----
const int trigPin     = 9;
const int echoPin     = 10;
const int buzzerPin   = 8;
const int ledVerde    = 5;
const int ledAmarelo  = 6;
const int ledVermelho = 7;
const int potPin      = A0;   // velocidade relativa simulada

// ---- Estado nao bloqueante ----
unsigned long ultimaTela = 0;
unsigned long ultimoBeep = 0;
bool telaA = true;
bool beepLigado = false;

long medirDistancia() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long dur = pulseIn(echoPin, HIGH, 30000);
  if (dur == 0) return 400;            // sem leitura = objeto distante
  return dur * 0.034 / 2;
}

// Escreve a linha sempre com 16 chars (sobrescreve sobra, sem flicker)
void printLinha(int linha, const char* txt) {
  char buf[17];
  snprintf(buf, sizeof(buf), "%-16s", txt);
  lcd.setCursor(0, linha);
  lcd.print(buf);
}

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledVerde, OUTPUT);
  pinMode(ledAmarelo, OUTPUT);
  pinMode(ledVermelho, OUTPUT);

  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  printLinha(0, "OrbitGuard");
  printLinha(1, "Console ativo");
  delay(1200);
  lcd.clear();
}

void loop() {
  long dist   = medirDistancia();                          // distancia minima estimada (cm)
  int  velRel = map(analogRead(potPin), 0, 1023, 0, 100);  // velocidade relativa simulada (%)

  // ---- Score explicavel (doc OrbitGuard, secao 13): dois fatores ponderados ----
  int fDist = constrain(map(dist, 8, 120, 55, 0), 0, 55);  // mais perto = mais risco
  int fVel  = map(velRel, 0, 100, 0, 45);                  // mais rapido = mais risco
  int score = constrain(fDist + fVel, 0, 100);

  // ---- Banda de risco e recomendacao ----
  int banda; const char* nivel; const char* acao;
  if      (score <= 39) { banda = 0; nivel = "BAIXO"; acao = "MONITORAR"; }
  else if (score <= 69) { banda = 1; nivel = "MEDIO"; acao = "PREPARAR";  }
  else if (score <= 84) { banda = 2; nivel = "ALTO";  acao = "MANOBRAR";  }
  else                  { banda = 3; nivel = "CRIT";  acao = "ESCALAR";   }

  // motivo principal = fator dominante (explicabilidade)
  const char* motivo = (fDist >= fVel) ? "dist min baixa" : "Vrel alta";

  // ---- Action Window simulada (segundos restantes de comando) ----
  int janela = constrain((int)(dist * 2 - velRel / 2), 5, 180);

  unsigned long agora = millis();

  // ---- LEDs por banda (hierarquia visual de risco) ----
  digitalWrite(ledVerde,   banda == 0);
  digitalWrite(ledAmarelo, banda == 1);
  if      (banda == 2) digitalWrite(ledVermelho, HIGH);
  else if (banda == 3) digitalWrite(ledVermelho, (agora / 200) % 2); // pisca
  else                 digitalWrite(ledVermelho, LOW);

  // ---- Buzzer nao bloqueante ----
  if (banda == 0) {
    noTone(buzzerPin);
  } else if (banda == 3) {
    tone(buzzerPin, 2200);                              // alarme continuo
  } else {
    int periodo = (banda == 2) ? 600 : 1500;
    int dur     = (banda == 2) ? 150 : 100;
    int freq    = (banda == 2) ? 1800 : 1200;
    if (!beepLigado && agora - ultimoBeep >= (unsigned long)periodo) {
      tone(buzzerPin, freq); beepLigado = true; ultimoBeep = agora;
    } else if (beepLigado && agora - ultimoBeep >= (unsigned long)dur) {
      noTone(buzzerPin); beepLigado = false; ultimoBeep = agora;
    }
  }

  // ---- LCD: alterna Risk Brief / Recomendacao a cada 1.5s ----
  if (agora - ultimaTela >= 1500) { telaA = !telaA; ultimaTela = agora; }

  char l1[17], l2[17];
  if (telaA) {
    snprintf(l1, sizeof(l1), "RISCO %3d %s", score, nivel);   // Risk Brief
    printLinha(0, l1);
    printLinha(1, motivo);
  } else {
    snprintf(l1, sizeof(l1), "ACAO: %s", acao);               // Decision Recommendation
    snprintf(l2, sizeof(l2), "Janela %ds", janela);           // Action Window
    printLinha(0, l1);
    printLinha(1, l2);
  }

  // ---- Log serial (estilo Post-Event Log) ----
  Serial.print("dist=");   Serial.print(dist);
  Serial.print("cm vrel="); Serial.print(velRel);
  Serial.print(" score=");  Serial.print(score);
  Serial.print(" nivel=");  Serial.print(nivel);
  Serial.print(" acao=");   Serial.println(acao);

  delay(120);
}
