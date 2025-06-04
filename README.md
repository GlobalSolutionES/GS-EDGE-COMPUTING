
# 💧 HydroSafe – Sistema de Alerta de Enchentes com Arduino

## 📋 Descrição  
- Projeto para monitorar riscos de enchentes em áreas urbanas sujeitas a alagamentos por chuvas intensas.  
- Mede o nível da água em tempo real usando sensor ultrassônico HC-SR04.  
- Indica níveis de risco com LEDs (verde, amarelo e vermelho), buzzer e display LCD I2C.  
- Registra alertas críticos na EEPROM com data e hora via módulo RTC.  
- Aciona relé automaticamente no estado crítico para ativar bombas ou sirenes.

## 🎯 Objetivo  
- Criar uma solução acessível e automatizada para prevenção de enchentes.  
- Fornecer alertas visuais e sonoros, registros confiáveis e acionamento rápido de mecanismos de resposta.

## 🛠️ Componentes Utilizados  
- Arduino Uno R3  
- Sensor ultrassônico HC-SR04  
- Display LCD 16x2 com módulo I2C  
- Módulo RTC DS3231  
- Relé  
- LEDs (verde, amarelo e vermelho)  
- Buzzer  
- EEPROM interna do Arduino

## ⚙️ Funcionamento  
- Mede o nível da água a cada poucos segundos com o sensor ultrassônico.  
- Exibe no LCD a distância medida e percentual de risco.  
- LEDs e buzzer indicam o nível do risco:  
  - Verde: nível seguro  
  - Amarelo: atenção  
  - Vermelho: alerta crítico  
- No nível crítico, o relé é acionado para disparar dispositivos externos.  
- Alertas críticos são registrados na EEPROM com data e hora do RTC.  
- Registros podem ser acessados via Serial Monitor a cada minuto.  
- EEPROM opera em modo circular para evitar perda de dados.

## 🧪 Parâmetros de Alerta (distância do nível da água)  
- Nível seguro: acima de 30 cm  
- Atenção: entre 10 e 30 cm  
- Crítico: abaixo de 10 cm

## 📦 Bibliotecas Utilizadas  
- LiquidCrystal_I2C.h  
- RTClib.h  
- EEPROM.h  
- Wire.h

## 📷 Imagens  
- ![sistemaSeguro](https://github.com/user-attachments/assets/6f413143-4f28-4fb3-9918-5a378dc3295d)
- ![sistemaAtencao](https://github.com/user-attachments/assets/b722155c-c1ed-46db-8cbd-f087a2c888d9)
- ![sistemaAlerta](https://github.com/user-attachments/assets/e94d899b-3369-4d74-90de-bcdfab907c03)


## 🧠 Aprendizados  
- Uso do sensor ultrassônico para medição de distância  
- Manipulação do display LCD via I2C com caracteres personalizados  
- Registro de dados na EEPROM com sistema de memória circular  
- Integração do módulo RTC para data e hora em tempo real  
- Controle de LEDs, buzzer e relé para alertas visuais e sonoros  
- Desenvolvimento de lógica para monitoramento ambiental com múltiplos níveis de risco

## 👥 Equipe HydroSafe  
- Geovana Maria da Silva Cardoso  
- Mariana Silva do Egito Moreira

Turma: 1ESPF  
Professor: Fábio Henrique Cabrini

## 💼 Aplicação Real  
- Sistema eficaz e acessível para alertar e agir preventivamente contra enchentes.  
- Pode ser adaptado para diversas regiões sujeitas a riscos de alagamento.

## 🔌 Simulação  
- Simulação no Wokwi: https://wokwi.com/projects/432441431948100609 

## 📽️ Demonstração  
- (link do vídeo para demonstração/explicação do projeto)

Este projeto foi desenvolvido como entrega da GLOBAL SOLUTION da disciplina de EDGE COMPUTING AND COMPUTER SYSTEMS do curso de Engenharia de Software - FIAP.
