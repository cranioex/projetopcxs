/*
 * ============================================================
 *  GIGA DE TESTE - PLACA INCAI KSF8S V01
 *  Hardware: Arduino MEGA 2560
 * ============================================================
 *
 * O Mega tem 54 pinos digitais, então usamos pinos extras para:
 * - Monitorar as 8 saídas dos TIP100 (feedback)
 * - LED de status no Mega
 * - Botão de emergência (desliga tudo)
 *
 * ============================================================
 *  LIGAÇÕES FÍSICAS
 * ============================================================
 *
 *  ARDUINO MEGA          PLACA KSF8S (via conector de borda)
 *  ─────────────         ─────────────────────────────────────
 *  SINAIS DE CONTROLE (saídas do Mega → entradas da placa):
 *    Pin 22 ──────────►  A0   (CD4099 pino 10)
 *    Pin 23 ──────────►  A1   (CD4099 pino 11)
 *    Pin 24 ──────────►  A2   (CD4099 pino 12)
 *    Pin 25 ──────────►  DATA (CD4099 pino 13)
 *    Pin 26 ──────────►  WE   (CD4099 pino 1)
 *    Pin 27 ──────────►  RESET(CD4099 pino 9) [opcional]
 *    GND    ──────────►  GND  (CD4099 pino 8)
 *
 *  FEEDBACK - Monitoramento das saídas dos TIP100:
 *  (Coletor de cada TIP100 → divisor de tensão → pino do Mega)
 *
 *    TIP100 ch0 ──[10K]──┬──► Pin 30 (Mega)
 *                        [4.7K]
 *                         │
 *                        GND
 *
 *    TIP100 ch1 ──[10K]──┬──► Pin 31
 *                        [4.7K]
 *                        GND
 *    ... (mesma coisa para ch2-ch7 nos pinos 32-37)
 *
 *  IMPORTANTE: O divisor de tensão (10K + 4.7K) reduz 24V → ~7.7V
 *              ou 12V → ~3.8V, protegendo o pino do Mega (max 5V).
 *              Para 24V use 10K + 2.2K (dá ~4.3V).
 *              Para 12V use 10K + 4.7K (dá ~3.8V).
 *
 *  EXTRAS:
 *    Pin 13 ──────────►  LED onboard (status)
 *    Pin 2  ◄──────────  Botão EMERGÊNCIA (GND quando pressionado)
 *
 * ============================================================
 *  ALIMENTAÇÃO
 * ============================================================
 *    Fonte 5V  → VCC lógica da placa + Vin Mega (ou USB)
 *    Fonte 12/24V → VCC potência (TIP100) + cargas (LEDs/lâmpadas)
 *    GND COMUM entre Mega, placa, e fontes
 * ============================================================
 */

// ===================== PINOS DO MEGA =====================

// Controle → CD4099BE
#define PIN_A0    22
#define PIN_A1    23
#define PIN_A2    24
#define PIN_DATA  25
#define PIN_WE    26
#define PIN_RESET 27   // Opcional: RESET do CD4099

// Feedback ← Coletores dos TIP100 (via divisor de tensão)
const uint8_t PIN_FB[8] = {30, 31, 32, 33, 34, 35, 36, 37};

// Extras
#define PIN_LED    13  // LED onboard do Mega
#define PIN_EMERG   2  // Botão de emergência (INPUT_PULLUP)

// ===================== VARIÁVEIS =========================

bool saidas[8] = {false};
bool modoAutomatico = false;
unsigned long lastBlink = 0;
bool ledState = false;

// ===================== SETUP =============================

void setup() {
  Serial.begin(115200);

  // Pinos de controle como saída
  pinMode(PIN_A0, OUTPUT);
  pinMode(PIN_A1, OUTPUT);
  pinMode(PIN_A2, OUTPUT);
  pinMode(PIN_DATA, OUTPUT);
  pinMode(PIN_WE, OUTPUT);
  pinMode(PIN_RESET, OUTPUT);

  // WE inativo (HIGH)
  digitalWrite(PIN_WE, HIGH);
  // RESET inativo (HIGH) — CD4099 reseta quando pino 9 vai HIGH
  digitalWrite(PIN_RESET, LOW);

  // Pinos de feedback como entrada
  for (uint8_t i = 0; i < 8; i++) {
    pinMode(PIN_FB[i], INPUT);
  }

  // Extras
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_EMERG, INPUT_PULLUP);

  // Reset geral da placa
  hardReset();

  Serial.println();
  Serial.println(F("╔══════════════════════════════════════╗"));
  Serial.println(F("║   GIGA DE TESTE - INCAI KSF8S V01   ║"));
  Serial.println(F("║   Arduino MEGA 2560                  ║"));
  Serial.println(F("╚══════════════════════════════════════╝"));
  printMenu();
}

// ===================== LOOP ==============================

void loop() {
  // Pisca LED de status (heartbeat)
  if (millis() - lastBlink > 500) {
    lastBlink = millis();
    ledState = !ledState;
    digitalWrite(PIN_LED, ledState);
  }

  // Botão de emergência - desliga tudo instantaneamente
  if (digitalRead(PIN_EMERG) == LOW) {
    hardReset();
    Serial.println(F("\n!! EMERGENCIA - TODAS SAIDAS DESLIGADAS !!"));
    delay(500); // debounce
  }

  // Comandos via Serial Monitor
  if (Serial.available()) {
    char cmd = Serial.read();
    // Limpa caracteres extras (newline etc)
    while (Serial.available() && Serial.peek() == '\n') Serial.read();

    switch (cmd) {
      case '1': testeSequencial();   break;
      case '2': allOn();
                Serial.println(F(">> Todas LIGADAS"));
                printStatus();       break;
      case '3': hardReset();
                Serial.println(F(">> Todas DESLIGADAS"));
                printStatus();       break;
      case '4': testeIndividual();   break;
      case '5': testePulso();        break;
      case '6': testeRampa();        break;
      case '7': testeFeedback();     break;
      case '8': testeEstresse();     break;
      case 'm':
      case 'M': printMenu();         break;
    }
  }
}

// ===================== CONTROLE DO CD4099 ================

void writeOutput(uint8_t channel, bool value) {
  if (channel > 7) return;

  // Endereço
  digitalWrite(PIN_A0, (channel >> 0) & 1);
  digitalWrite(PIN_A1, (channel >> 1) & 1);
  digitalWrite(PIN_A2, (channel >> 2) & 1);

  // Dado
  digitalWrite(PIN_DATA, value ? HIGH : LOW);

  // Pulso WE: HIGH → LOW (escreve) → HIGH (mantém)
  delayMicroseconds(5);
  digitalWrite(PIN_WE, LOW);
  delayMicroseconds(50);
  digitalWrite(PIN_WE, HIGH);
  delayMicroseconds(5);

  saidas[channel] = value;
}

void resetAll() {
  for (uint8_t i = 0; i < 8; i++) {
    writeOutput(i, false);
  }
}

void hardReset() {
  // Usa pino RESET do CD4099 (pino 9) — reseta todos outputs pra LOW
  digitalWrite(PIN_RESET, HIGH);
  delayMicroseconds(100);
  digitalWrite(PIN_RESET, LOW);
  for (uint8_t i = 0; i < 8; i++) saidas[i] = false;
}

void allOn() {
  for (uint8_t i = 0; i < 8; i++) {
    writeOutput(i, true);
  }
}

// ===================== FEEDBACK ==========================

// Lê estado real das saídas dos TIP100 via divisor de tensão
bool readFeedback(uint8_t channel) {
  if (channel > 7) return false;
  // TIP100 conduzindo → coletor LOW → pino lê LOW
  // TIP100 cortado → coletor HIGH (Vcc via carga) → pino lê HIGH
  // Então: feedback LOW = saída ATIVA, HIGH = saída INATIVA
  return (digitalRead(PIN_FB[channel]) == LOW);
}

void printFeedback() {
  Serial.print(F("   Feedback: ["));
  for (uint8_t i = 0; i < 8; i++) {
    bool fb = readFeedback(i);
    if (fb != saidas[i]) {
      Serial.print(F("!"));  // Marca discrepância com !
    }
    Serial.print(fb ? "1" : "0");
    if (i < 7) Serial.print(" ");
  }
  Serial.println(F("]"));
}

// ===================== TESTES ============================

// Teste 1: Sequencial
void testeSequencial() {
  Serial.println(F("\n>> TESTE SEQUENCIAL"));
  Serial.println(F("   Cada saida liga por 1.5s"));
  Serial.println(F("   Osciloscópio: CH1=WE(pin26) CH2=Coletor TIP100"));

  for (uint8_t i = 0; i < 8; i++) {
    resetAll();
    delay(200);
    writeOutput(i, true);

    Serial.print(F("   CH"));
    Serial.print(i);
    Serial.print(F(" = ON  "));

    // Verifica feedback
    delay(50);  // Tempo para TIP100 estabilizar
    bool fb = readFeedback(i);
    if (fb) {
      Serial.println(F("[OK]"));
    } else {
      Serial.println(F("[FALHA - saida nao respondeu!]"));
    }

    delay(1450);
  }

  resetAll();
  Serial.println(F(">> Teste sequencial CONCLUIDO\n"));
  printMenu();
}

// Teste 4: Individual
void testeIndividual() {
  Serial.println(F("\n>> MODO INDIVIDUAL"));
  Serial.println(F("   Formato: <canal><estado>"));
  Serial.println(F("   Ex: 31 = liga CH3 | 30 = desliga CH3"));
  Serial.println(F("   's' = status | 'x' = sair"));

  while (true) {
    if (Serial.available()) {
      char c1 = Serial.read();

      if (c1 == 'x' || c1 == 'X') {
        Serial.println(F(">> Saindo modo individual"));
        printMenu();
        return;
      }
      if (c1 == 's' || c1 == 'S') {
        printStatus();
        printFeedback();
        continue;
      }

      // Espera segundo caractere
      unsigned long t = millis();
      while (!Serial.available() && (millis() - t < 1000));
      if (!Serial.available()) continue;
      char c2 = Serial.read();

      uint8_t ch = c1 - '0';
      bool val = (c2 == '1');

      if (ch <= 7) {
        writeOutput(ch, val);
        Serial.print(F("   CH"));
        Serial.print(ch);
        Serial.println(val ? F(" ON") : F(" OFF"));
        printStatus();
      }
    }
  }
}

// Teste 5: Pulso para osciloscópio
void testePulso() {
  Serial.println(F("\n>> MODO PULSO (osciloscopio)"));
  Serial.println(F("   Gera onda quadrada em cada canal"));
  Serial.println(F("   ~60Hz por canal"));
  Serial.println(F("   Trigger: CH1 no WE (pin 26)"));
  Serial.println(F("   Qualquer tecla para parar\n"));

  while (!Serial.available()) {
    for (uint8_t i = 0; i < 8; i++) {
      writeOutput(i, true);
      delayMicroseconds(1000);
      writeOutput(i, false);
      delayMicroseconds(1000);
    }
  }
  while (Serial.available()) Serial.read();
  resetAll();
  Serial.println(F(">> Pulsos parados"));
  printMenu();
}

// Teste 6: Rampa (liga uma por uma, acumula, depois desliga)
void testeRampa() {
  Serial.println(F("\n>> TESTE RAMPA"));
  Serial.println(F("   Liga canais um a um, depois desliga um a um"));
  Serial.println(F("   Util para ver consumo de corrente incremental"));

  resetAll();

  // Subida
  Serial.println(F("   -- SUBIDA --"));
  for (uint8_t i = 0; i < 8; i++) {
    writeOutput(i, true);
    Serial.print(F("   +CH"));
    Serial.print(i);
    Serial.print(F("  Ativos: "));
    Serial.println(i + 1);
    delay(1500);
  }

  Serial.println(F("   -- TODAS LIGADAS (3s) --"));
  delay(3000);

  // Descida
  Serial.println(F("   -- DESCIDA --"));
  for (int8_t i = 7; i >= 0; i--) {
    writeOutput(i, false);
    Serial.print(F("   -CH"));
    Serial.print(i);
    Serial.print(F("  Ativos: "));
    Serial.println(i);
    delay(1500);
  }

  Serial.println(F(">> Teste rampa CONCLUIDO\n"));
  printMenu();
}

// Teste 7: Verifica feedback de todas as saídas
void testeFeedback() {
  Serial.println(F("\n>> TESTE DE FEEDBACK (verifica TIP100)"));
  Serial.println(F("   Liga cada canal e verifica resposta real\n"));

  uint8_t falhas = 0;

  for (uint8_t i = 0; i < 8; i++) {
    // Desliga tudo
    resetAll();
    delay(100);

    // Liga canal i
    writeOutput(i, true);
    delay(100);  // Tempo de resposta

    bool fb = readFeedback(i);

    Serial.print(F("   CH"));
    Serial.print(i);
    Serial.print(F(": Comando=ON  Feedback="));
    Serial.print(fb ? "ON " : "OFF");

    if (fb) {
      Serial.println(F("  [OK]"));
    } else {
      Serial.println(F("  [FALHA]"));
      falhas++;
    }

    // Verifica se outros canais não ligaram (curto/crosstalk)
    for (uint8_t j = 0; j < 8; j++) {
      if (j == i) continue;
      if (readFeedback(j)) {
        Serial.print(F("   !! CROSSTALK: CH"));
        Serial.print(j);
        Serial.println(F(" ligou sem comando!"));
        falhas++;
      }
    }
  }

  resetAll();

  Serial.println();
  if (falhas == 0) {
    Serial.println(F("   *** RESULTADO: TODAS SAIDAS OK ***"));
  } else {
    Serial.print(F("   *** RESULTADO: "));
    Serial.print(falhas);
    Serial.println(F(" FALHA(S) ENCONTRADA(S) ***"));
  }
  Serial.println();
  printMenu();
}

// Teste 8: Estresse - chaveamento rápido prolongado
void testeEstresse() {
  Serial.println(F("\n>> TESTE DE ESTRESSE"));
  Serial.println(F("   Chaveamento rapido por 30 segundos"));
  Serial.println(F("   Monitore temperatura dos TIP100!"));
  Serial.println(F("   Qualquer tecla para abortar\n"));

  unsigned long inicio = millis();
  unsigned long ciclos = 0;

  while (!Serial.available() && (millis() - inicio < 30000)) {
    for (uint8_t i = 0; i < 8; i++) {
      writeOutput(i, true);
    }
    delay(10);
    resetAll();
    delay(10);
    ciclos++;

    // Status a cada 5 segundos
    if (ciclos % 250 == 0) {
      unsigned long elapsed = (millis() - inicio) / 1000;
      Serial.print(F("   Tempo: "));
      Serial.print(elapsed);
      Serial.print(F("s  Ciclos: "));
      Serial.println(ciclos);
    }
  }

  while (Serial.available()) Serial.read();
  resetAll();

  Serial.print(F("\n>> Estresse concluido. Total ciclos: "));
  Serial.println(ciclos);
  Serial.println(F("   Verifique temperatura dos componentes"));
  printMenu();
}

// ===================== DISPLAY ===========================

void printStatus() {
  Serial.print(F("   Comando: ["));
  for (uint8_t i = 0; i < 8; i++) {
    Serial.print(saidas[i] ? "1" : "0");
    if (i < 7) Serial.print(" ");
  }
  Serial.println(F("]"));
}

void printMenu() {
  Serial.println(F(""));
  Serial.println(F("╔═══════════════════════════════════╗"));
  Serial.println(F("║           MENU DE TESTES          ║"));
  Serial.println(F("╠═══════════════════════════════════╣"));
  Serial.println(F("║ 1 = Sequencial (1 canal por vez)  ║"));
  Serial.println(F("║ 2 = Todas ON                      ║"));
  Serial.println(F("║ 3 = Todas OFF (emergencia)        ║"));
  Serial.println(F("║ 4 = Controle individual            ║"));
  Serial.println(F("║ 5 = Modo pulso (osciloscopio)     ║"));
  Serial.println(F("║ 6 = Rampa (sobe/desce)            ║"));
  Serial.println(F("║ 7 = Teste feedback (diagnostico)  ║"));
  Serial.println(F("║ 8 = Teste estresse (30s)          ║"));
  Serial.println(F("║ M = Mostrar menu                  ║"));
  Serial.println(F("╚═══════════════════════════════════╝"));
  Serial.println(F("  Botao pin 2 = EMERGENCIA (desliga tudo)"));
  Serial.println();
}
