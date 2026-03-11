/*
 * Giga de Teste - Placa INCAI KSF8S V01
 * Emula sinais do controlador CNC para testar as 8 saídas
 *
 * Conexões Arduino -> Conector de borda da placa:
 *   Pin 2 -> A0 (Endereço bit 0 - CD4099 pino 10)
 *   Pin 3 -> A1 (Endereço bit 1 - CD4099 pino 11)
 *   Pin 4 -> A2 (Endereço bit 2 - CD4099 pino 12)
 *   Pin 5 -> DATA (Dado - CD4099 pino 13)
 *   Pin 6 -> WE (Write Enable - CD4099 pino 1)
 *   GND   -> GND da placa
 *
 * IMPORTANTE: O Arduino trabalha em 5V, compatível com CD4099BE (CMOS 3-18V)
 *
 * Modos de teste:
 *   1 - Sequencial: liga cada saída uma por vez (visual)
 *   2 - Todas ON: liga todas as 8 saídas
 *   3 - Todas OFF: desliga todas as 8 saídas
 *   4 - Individual: liga/desliga saída específica via Serial Monitor
 */

// Pinos do Arduino
#define PIN_A0   2   // Endereço bit 0
#define PIN_A1   3   // Endereço bit 1
#define PIN_A2   4   // Endereço bit 2
#define PIN_DATA 5   // Dado (1=liga, 0=desliga)
#define PIN_WE   6   // Write Enable (pulso LOW para escrever)

// Estado das 8 saídas
bool saidas[8] = {false, false, false, false, false, false, false, false};

void setup() {
  Serial.begin(9600);

  pinMode(PIN_A0, OUTPUT);
  pinMode(PIN_A1, OUTPUT);
  pinMode(PIN_A2, OUTPUT);
  pinMode(PIN_DATA, OUTPUT);
  pinMode(PIN_WE, OUTPUT);

  // WE inativo (HIGH) - CD4099 só escreve quando WE vai LOW
  digitalWrite(PIN_WE, HIGH);

  // Desliga todas as saídas no início
  resetAll();

  Serial.println("=================================");
  Serial.println("  Giga de Teste INCAI KSF8S V01");
  Serial.println("=================================");
  printMenu();
}

void loop() {
  if (Serial.available()) {
    char cmd = Serial.read();

    switch (cmd) {
      case '1':
        testeSequencial();
        break;
      case '2':
        allOn();
        Serial.println(">> Todas as saidas LIGADAS");
        break;
      case '3':
        resetAll();
        Serial.println(">> Todas as saidas DESLIGADAS");
        break;
      case '4':
        testeIndividual();
        break;
      case '5':
        testePulso();
        break;
      case 'm':
      case 'M':
        printMenu();
        break;
    }
  }
}

// Escreve um valor (0 ou 1) na saída especificada (0-7)
void writeOutput(uint8_t channel, bool value) {
  if (channel > 7) return;

  // Coloca endereço nos pinos A0-A2
  digitalWrite(PIN_A0, (channel >> 0) & 1);
  digitalWrite(PIN_A1, (channel >> 1) & 1);
  digitalWrite(PIN_A2, (channel >> 2) & 1);

  // Coloca o dado
  digitalWrite(PIN_DATA, value ? HIGH : LOW);

  // Pulso de escrita: WE LOW -> espera -> WE HIGH (dado é latched)
  delayMicroseconds(10);
  digitalWrite(PIN_WE, LOW);
  delayMicroseconds(50);  // Tempo mínimo de escrita
  digitalWrite(PIN_WE, HIGH);
  delayMicroseconds(10);

  saidas[channel] = value;
}

// Desliga todas as saídas
void resetAll() {
  for (uint8_t i = 0; i < 8; i++) {
    writeOutput(i, false);
  }
}

// Liga todas as saídas
void allOn() {
  for (uint8_t i = 0; i < 8; i++) {
    writeOutput(i, true);
  }
}

// Teste sequencial - liga uma por vez
void testeSequencial() {
  Serial.println(">> Teste sequencial (1 segundo cada)");
  Serial.println("   Use osciloscopio no coletor dos TIP100");

  for (uint8_t i = 0; i < 8; i++) {
    resetAll();
    writeOutput(i, true);
    Serial.print("   Saida ");
    Serial.print(i);
    Serial.println(" LIGADA");
    delay(1000);
  }

  resetAll();
  Serial.println(">> Teste sequencial concluido");
  printMenu();
}

// Teste individual via Serial
void testeIndividual() {
  Serial.println(">> Modo individual");
  Serial.println("   Digite canal (0-7) seguido de estado (0 ou 1)");
  Serial.println("   Exemplo: 31 = liga saida 3");
  Serial.println("   Exemplo: 30 = desliga saida 3");
  Serial.println("   Digite 'x' para sair");

  while (true) {
    if (Serial.available() >= 2) {
      char c1 = Serial.read();

      if (c1 == 'x' || c1 == 'X') {
        Serial.println(">> Saindo do modo individual");
        printMenu();
        return;
      }

      char c2 = Serial.read();

      uint8_t channel = c1 - '0';
      bool value = (c2 == '1');

      if (channel <= 7) {
        writeOutput(channel, value);
        Serial.print("   Saida ");
        Serial.print(channel);
        Serial.println(value ? " LIGADA" : " DESLIGADA");
        printStatus();
      }
    }
  }
}

// Teste com pulso para medir com osciloscópio
void testePulso() {
  Serial.println(">> Modo pulso (para osciloscopio)");
  Serial.println("   Gerando pulsos em todas as saidas");
  Serial.println("   Frequencia: ~100Hz em cada canal");
  Serial.println("   Coloque trigger no pino WE (pin 6)");
  Serial.println("   Digite qualquer tecla para parar");

  while (!Serial.available()) {
    for (uint8_t i = 0; i < 8; i++) {
      writeOutput(i, true);
      delayMicroseconds(500);
      writeOutput(i, false);
      delayMicroseconds(500);
    }
  }
  Serial.read(); // limpa buffer
  resetAll();
  Serial.println(">> Pulsos parados");
  printMenu();
}

void printStatus() {
  Serial.print("   Status: [");
  for (uint8_t i = 0; i < 8; i++) {
    Serial.print(saidas[i] ? "1" : "0");
    if (i < 7) Serial.print(" ");
  }
  Serial.println("]");
}

void printMenu() {
  Serial.println("");
  Serial.println("--- MENU ---");
  Serial.println("1 = Teste sequencial (1s cada saida)");
  Serial.println("2 = Todas ON");
  Serial.println("3 = Todas OFF");
  Serial.println("4 = Controle individual");
  Serial.println("5 = Modo pulso (p/ osciloscopio)");
  Serial.println("M = Mostrar menu");
  Serial.println("------------");
}
