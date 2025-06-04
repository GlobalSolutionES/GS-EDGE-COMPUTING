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

// Registro de informações que serão armazenadas quando estiver em estado de Alerta
struct RegistroAlerta {
  byte dia, mes, hora, minuto;
  float distancia;
};

const int enderecoInicial = 2; // Endereço inicial na EEPROM para armazenar registros (0 e 1 reservados)
int enderecoEEPROM = enderecoInicial; // Variável para controle do endereço atual de gravação
const int tamanhoRegistro = sizeof(RegistroAlerta); // Tamanho do registro de alerta em bytes
const int maxEEPROM = EEPROM.length(); // Tamanho total da EEPROM disponível no Arduino

unsigned long tempoUltimaLeitura = 0; // Armazena o tempo da última exibição dos registros para fazer isso a cada 1 minuto

void setup() {
  Serial.begin(9600); // Inicializa comunicação serial para monitoramento no PC

  // Configuração dos pinos de controle e sensores
  pinMode(pinoTrigger, OUTPUT);
  pinMode(pinoEcho, INPUT);
  pinMode(ledVerde, OUTPUT);
  pinMode(ledAmarelo, OUTPUT);
  pinMode(ledVermelho, OUTPUT);
  pinMode(pinoRele, OUTPUT);
  pinMode(pinoBuzzer, OUTPUT);

  // Inicializa RTC (Relógio de Tempo Real)
  if (!rtc.begin()) {
    lcd.clear();
    lcd.print("Erro no RTC!");
    while (1);  // Para o código caso o RTC não funcione
  }

  // AJUSTE MANUAL DO HORÁRIO (Horário de Brasília -3)
  // Para usar uma vez e depois comentar
  //rtc.adjust(DateTime(2025, 6, 3, 13, 36, 0)); // 3/jun/2025, 13h36

// Inicializa o display LCD I2C
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, iconeGota); // Cria um caractere personalizado (ícone gota) no LCD posição 0

  // Lê o último endereço usado na EEPROM para continuar a gravação sem sobrescrever dados
  EEPROM.get(0, enderecoEEPROM);

  // Validação para garantir que o endereço está dentro dos limites válidos
  if (enderecoEEPROM < enderecoInicial || enderecoEEPROM + tamanhoRegistro > maxEEPROM) {
    enderecoEEPROM = enderecoInicial;
  }

  // Animações iniciais no display para feedback visual ao ligar o sistema
  animacaoInicial();
  delay(500);
  animacaoCarregando();
}

void loop() {
  // A cada 1 minuto, exibe os registros armazenados na EEPROM no Serial Monitor
  if (millis() - tempoUltimaLeitura >= 60000) {
    Serial.println("\nExibindo registros da EEPROM...");
    mostrarRegistrosEEPROM();
    tempoUltimaLeitura = millis();
  }

  // Mede o nível da água utilizando sensor ultrassônico
  float distancia = medirNivelAgua();
  
  // Calcula o percentual de risco a partir da distância medida, mapeando entre níveis seguro e crítico
  float risco = map(distancia, nivelSeguro, nivelCritico, 0, 100);
  risco = constrain(risco, 0, 100);

// Atualiza o LCD com os dados atuais de risco e distância
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Risco: ");
  lcd.print(risco);
  lcd.print("%");

  lcd.setCursor(0, 1);
  lcd.print("Distancia: ");
  lcd.print(distancia, 0);
  lcd.print("cm ");
  lcd.write(0); // Exibe o ícone personalizado de gota


  // Lógica de alerta e acionamento conforme o nível da água detectado
  if (distancia <= nivelCritico) {
    // Estado crítico: LED vermelho aceso, relé ativado e buzzer com alerta longo
    digitalWrite(ledVerde, LOW);
    digitalWrite(ledAmarelo, LOW);
    digitalWrite(ledVermelho, HIGH);
    digitalWrite(pinoRele, HIGH);
    tone(pinoBuzzer, 265, 5000); // Emite tom por 5 segundos

  // Registra alerta crítico na EEPROM com data, hora e nível medido
    DateTime agora = rtc.now();
    RegistroAlerta alerta = {
      agora.day(), agora.month(),
      agora.hour(), agora.minute(),
      distancia
    };

    EEPROM.put(enderecoEEPROM, alerta); // Grava o registro na EEPROM
    enderecoEEPROM += tamanhoRegistro;  // Atualiza o endereço para próximo registro

    // Verifica se o endereço excedeu a capacidade da EEPROM e reinicia (memória circular)
    if (enderecoEEPROM + tamanhoRegistro > maxEEPROM) {
      enderecoEEPROM = enderecoInicial;
    }
    EEPROM.put(0, enderecoEEPROM); // Salva o endereço atual para próxima inicialização
  }
  else if (distancia <= nivelSeguro) {
    // Estado de atenção: LED amarelo e buzzer com alerta curto
    digitalWrite(ledVerde, LOW);
    digitalWrite(ledAmarelo, HIGH);
    digitalWrite(ledVermelho, LOW);
    digitalWrite(pinoRele, LOW);
    tone(pinoBuzzer, 265, 250); // Emite tom por 0,25 segundos
  } else {
    // Estado seguro: LED verde aceso, buzzer desligado e relé desligado
    digitalWrite(ledVerde, HIGH);
    digitalWrite(ledAmarelo, LOW);
    digitalWrite(ledVermelho, LOW);
    digitalWrite(pinoRele, LOW);
    digitalWrite(pinoBuzzer, LOW);
  }

  delay(1500); // Aguarda 1,5 segundos antes da próxima leitura
}

// Função que mede o nível da água com sensor ultrassônico HC-SR04
float medirNivelAgua() {
  digitalWrite(pinoTrigger, LOW);
  delayMicroseconds(2);
  digitalWrite(pinoTrigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(pinoTrigger, LOW);

  float duracao = pulseIn(pinoEcho, HIGH, 30000);

  // Caso nenhuma leitura válida, retorna valor alto
  if (duracao == 0) return 999;

  // Calcula a distância em cm (velocidade do som 0.034 cm/us, divide por 2 por ida e volta)
  return duracao * 0.034 / 2;
}

// Função que exibe os registros de alerta armazenados na EEPROM no Serial Monitor
void mostrarRegistrosEEPROM() {
  Serial.println("\n--- Registros de ALERTA ---");

  int enderecoLeitura = enderecoInicial;

  // Percorre a EEPROM lendo registros até o final da memória
  while (enderecoLeitura + tamanhoRegistro <= maxEEPROM) {
    RegistroAlerta alerta;
    EEPROM.get(enderecoLeitura, alerta);

  // Validação simples para verificar se o registro é válido (data plausível)
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

// Animação inicial para dar feedback visual no LCD ao ligar o sistema
void animacaoInicial() {
  lcd.clear();
  lcd.setCursor(0, 0);
  String texto = "HydroSafe ";
  for (int i = 0; i < texto.length(); i++) {
    lcd.print(texto[i]);
    delay(150);
  }

  lcd.write(0); // Mostra o ícone gota criado

  lcd.setCursor(0, 1);
  String msg = "Iniciando...";
  for (int i = 0; i < msg.length(); i++) {
    lcd.print(msg[i]);
    delay(100);
  }
  delay(1000);
}

// Animação de carregamento para a inicialização do sistema
void animacaoCarregando() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Carregando... ");
  lcd.write(0);
}
