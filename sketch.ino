/*Sistema desenvolvido por Geovana Maria da Silva Cardoso e Mariana Silva do Egito Moreira


  HydroSafe - Sistema de Alerta de Enchentes

  Problema:
  O aumento do nível da água em ruas e áreas residenciais durante períodos de chuva intensa
  pode causar enchentes, colocando em risco a segurança das pessoas e causando danos materiais.

  Objetivo do nosso sistema:
  Monitorar o nível da água utilizando um sensor ultrassônico,
  indicando diferentes níveis de risco através de LEDs e buzzer, exibindo informações em um display LCD.
  Além disso, registra os alertas com data e hora para histórico.

  Funcionamento do Relé:
  O relé é acionado automaticamente quando o nível da água atinge o estado crítico,
  permitindo o acionamento de um mecanismo externo, como bomba de drenagem ou sirene de alerta,
  para mitigar os danos causados pela enchente.

  Assim, o sistema oferece um alerta local eficiente para prevenção e resposta rápida em situações de risco.
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>
#include <EEPROM.h>

// LCD I2C
LiquidCrystal_I2C lcd(0x27, 16, 2);

// RTC
RTC_DS3231 rtc;

// Pinos
const int pinoTrigger = 8;
const int pinoEcho = 9;
const int ledVerde = 2;
const int ledAmarelo = 3;
const int ledVermelho = 4;
const int pinoRele = 7;
const int pinoBuzzer = 6;

// Níveis de alerta (em cm)
float nivelSeguro = 30;
float nivelCritico = 10;

// Ícone personalizado
byte iconeGota[8] = {
  B00100, B00100, B01010, B01010,
  B10001, B10001, B01110, B00000
};

// Registro
struct RegistroAlerta {
  byte dia, mes, hora, minuto;
  float distancia;
};

const int enderecoInicial = 2; // 0 e 1 reservados
int enderecoEEPROM = enderecoInicial;
const int tamanhoRegistro = sizeof(RegistroAlerta);
const int maxEEPROM = EEPROM.length();

unsigned long tempoUltimaLeitura = 0; // Para mostrar registros a cada 1 min

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(pinoTrigger, OUTPUT);
  pinMode(pinoEcho, INPUT);
  pinMode(ledVerde, OUTPUT);
  pinMode(ledAmarelo, OUTPUT);
  pinMode(ledVermelho, OUTPUT);
  pinMode(pinoRele, OUTPUT);
  pinMode(pinoBuzzer, OUTPUT);

  // Inicializa RTC
  if (!rtc.begin()) {
    lcd.clear();
    lcd.print("Erro no RTC!");
    while (1);
  }

  // AJUSTE MANUAL DO HORÁRIO (Horário de Brasília -3)
  // Para usar uma vez e depois comentar
  //rtc.adjust(DateTime(2025, 6, 3, 13, 36, 0)); // 3/jun/2025, 13h36

  lcd.init();
  lcd.backlight();
  lcd.createChar(0, iconeGota);

  EEPROM.get(0, enderecoEEPROM);
  if (enderecoEEPROM < enderecoInicial || enderecoEEPROM + tamanhoRegistro > maxEEPROM) {
    enderecoEEPROM = enderecoInicial;
  }

  animacaoInicial();
  delay(500);
  animacaoCarregando();
}

void loop() {
  if (millis() - tempoUltimaLeitura >= 60000) {
    Serial.println("\nExibindo registros da EEPROM...");
    mostrarRegistrosEEPROM();
    tempoUltimaLeitura = millis();
  }

  float distancia = medirNivelAgua();
  float risco = map(distancia, nivelSeguro, nivelCritico, 0, 100);
  risco = constrain(risco, 0, 100);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Risco: ");
  lcd.print(risco);
  lcd.print("%");

  lcd.setCursor(0, 1);
  lcd.print("Distancia: ");
  lcd.print(distancia, 0);
  lcd.print("cm ");
  lcd.write(0);

  if (distancia <= nivelCritico) {
    digitalWrite(ledVerde, LOW);
    digitalWrite(ledAmarelo, LOW);
    digitalWrite(ledVermelho, HIGH);
    digitalWrite(pinoRele, HIGH);
    tone(pinoBuzzer, 265, 5000);

    DateTime agora = rtc.now();
    RegistroAlerta alerta = {
      agora.day(), agora.month(),
      agora.hour(), agora.minute(),
      distancia
    };

    EEPROM.put(enderecoEEPROM, alerta);
    enderecoEEPROM += tamanhoRegistro;
    if (enderecoEEPROM + tamanhoRegistro > maxEEPROM) {
      enderecoEEPROM = enderecoInicial;
    }
    EEPROM.put(0, enderecoEEPROM);
  }
  else if (distancia <= nivelSeguro) {
    digitalWrite(ledVerde, LOW);
    digitalWrite(ledAmarelo, HIGH);
    digitalWrite(ledVermelho, LOW);
    digitalWrite(pinoRele, LOW);
    tone(pinoBuzzer, 265, 250);
  } else {
    digitalWrite(ledVerde, HIGH);
    digitalWrite(ledAmarelo, LOW);
    digitalWrite(ledVermelho, LOW);
    digitalWrite(pinoRele, LOW);
    digitalWrite(pinoBuzzer, LOW);
  }

  delay(1500);
}

float medirNivelAgua() {
  digitalWrite(pinoTrigger, LOW);
  delayMicroseconds(2);
  digitalWrite(pinoTrigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(pinoTrigger, LOW);

  float duracao = pulseIn(pinoEcho, HIGH, 30000);
  if (duracao == 0) return 999;
  return duracao * 0.034 / 2;
}

void mostrarRegistrosEEPROM() {
  Serial.println("\n--- Registros de ALERTA ---");

  int enderecoLeitura = enderecoInicial;
  while (enderecoLeitura + tamanhoRegistro <= maxEEPROM) {
    RegistroAlerta alerta;
    EEPROM.get(enderecoLeitura, alerta);

    if (alerta.mes >= 1 && alerta.mes <= 12 && alerta.dia >= 1 && alerta.dia <= 31) {
      Serial.print("Data: ");
      Serial.print(alerta.dia); Serial.print("/");
      Serial.print(alerta.mes); Serial.print(" ");
      Serial.print(alerta.hora); Serial.print(":");
      if (alerta.minuto < 10) Serial.print("0");
      Serial.print(alerta.minuto);
      Serial.print(" | Nível: ");
      Serial.print(alerta.distancia);
      Serial.println(" cm");
    }

    enderecoLeitura += tamanhoRegistro;
  }

  Serial.println("----------------------------");
}

void animacaoInicial() {
  lcd.clear();
  lcd.setCursor(0, 0);
  String texto = "HydroSafe ";
  for (int i = 0; i < texto.length(); i++) {
    lcd.print(texto[i]);
    delay(150);
  }
  lcd.write(0);
  lcd.setCursor(0, 1);
  String msg = "Iniciando...";
  for (int i = 0; i < msg.length(); i++) {
    lcd.print(msg[i]);
    delay(100);
  }
  delay(1000);
}

void animacaoCarregando() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Carregando... ");
  lcd.write(0);
}
