/*
 * ============================================================
 *  GIGA DE TESTE - Placa F211 (8H) - Entradas Isoladas 8ch
 *  Arduino Mega 2560
 * ============================================================
 *
 *  Placa: INCAI F211 03/88
 *  Funcao: 8 entradas isoladas via optoacopladores TIL113
 *  CIs: TIL113 (8x) + SCL4068BE (NAND 8 entradas)
 *
 *  O Arduino simula 8 sensores (saidas digitais) e le
 *  as saidas dos fototransistores para validar cada canal.
 *  Tambem verifica a NAND gate SCL4068BE.
 *
 *  LIGACOES:
 *   D22-D29 → Resistor 1K → Anodo LED TIL113 (canais 0-7)
 *   D30-D37 ← Coletor fototransistor TIL113 (com pullup 4K7)
 *   D38     ← Saida NAND SCL4068BE
 *   GND     → GND comum (fonte + placa)
 *
 *  Serial Monitor: 115200 baud
 * ============================================================
 */

// Pinos de SAIDA (simulam sensores → LEDs dos TIL113)
const int PIN_LED[] = {22, 23, 24, 25, 26, 27, 28, 29};

// Pinos de ENTRADA (leitura dos fototransistores TIL113)
const int PIN_FOTO[] = {30, 31, 32, 33, 34, 35, 36, 37};

// Pino da saida NAND do SCL4068BE
const int PIN_NAND = 38;

// Pino do botao de emergencia
const int PIN_EMERG = 2;

const int NUM_CH = 8;
const int DELAY_TESTE = 200;    // ms entre testes
const int DELAY_ESTAB = 50;     // ms para sinal estabilizar

// Resultados
bool resultado[8];
bool nand_ok = false;

void setup() {
  Serial.begin(115200);

  // Configurar pinos de saida (LEDs)
  for (int i = 0; i < NUM_CH; i++) {
    pinMode(PIN_LED[i], OUTPUT);
    digitalWrite(PIN_LED[i], LOW);
  }

  // Configurar pinos de entrada (fototransistores)
  for (int i = 0; i < NUM_CH; i++) {
    pinMode(PIN_FOTO[i], INPUT);
    // Pullup externo 4K7 recomendado, mas ativa interno como backup
    // digitalWrite(PIN_FOTO[i], HIGH); // descomentar se nao tiver pullup externo
  }

  // Pino NAND
  pinMode(PIN_NAND, INPUT);

  // Botao emergencia
  pinMode(PIN_EMERG, INPUT_PULLUP);

  Serial.println(F("============================================"));
  Serial.println(F("  GIGA DE TESTE - Placa F211 (8H)"));
  Serial.println(F("  Entradas Isoladas - 8x TIL113"));
  Serial.println(F("============================================"));
  Serial.println();
  Serial.println(F("Comandos:"));
  Serial.println(F("  T = Teste completo (todos os canais)"));
  Serial.println(F("  0-7 = Testar canal individual"));
  Serial.println(F("  N = Testar NAND (SCL4068BE)"));
  Serial.println(F("  S = Stress test (100 ciclos rapidos)"));
  Serial.println(F("  L = Ligar todos (visual)"));
  Serial.println(F("  D = Desligar todos"));
  Serial.println(F("  R = Leitura continua (ESC para parar)"));
  Serial.println(F("  ? = Mostrar menu"));
  Serial.println();
  Serial.println(F("Aguardando comando..."));
}

void loop() {
  // Verificar emergencia
  if (digitalRead(PIN_EMERG) == LOW) {
    desligarTudo();
    Serial.println(F("!!! EMERGENCIA - Tudo desligado !!!"));
    delay(1000);
    return;
  }

  if (Serial.available()) {
    char cmd = Serial.read();

    switch (cmd) {
      case 'T': case 't':
        testeCompleto();
        break;
      case '0': case '1': case '2': case '3':
      case '4': case '5': case '6': case '7':
        testeCanal(cmd - '0');
        break;
      case 'N': case 'n':
        testeNAND();
        break;
      case 'S': case 's':
        stressTest();
        break;
      case 'L': case 'l':
        ligarTudo();
        break;
      case 'D': case 'd':
        desligarTudo();
        Serial.println(F("Todos os canais desligados."));
        break;
      case 'R': case 'r':
        leituraContinua();
        break;
      case '?':
        setup(); // re-mostra menu
        break;
    }
  }
}

// ==================== TESTE COMPLETO ====================
void testeCompleto() {
  Serial.println();
  Serial.println(F("========== TESTE COMPLETO F211 =========="));
  Serial.println(F("Testando 8 canais individualmente..."));
  Serial.println();

  int falhas = 0;

  // Garantir tudo desligado antes
  desligarTudo();
  delay(DELAY_ESTAB);

  for (int ch = 0; ch < NUM_CH; ch++) {
    resultado[ch] = testarCanalInterno(ch);
    if (!resultado[ch]) falhas++;
    delay(DELAY_TESTE);
  }

  // Teste NAND
  Serial.println();
  Serial.println(F("--- Teste NAND (SCL4068BE) ---"));
  nand_ok = testarNANDInterno();

  // Resumo
  Serial.println();
  Serial.println(F("========== RESULTADO FINAL =========="));
  for (int ch = 0; ch < NUM_CH; ch++) {
    Serial.print(F("  CH"));
    Serial.print(ch);
    Serial.print(F(" (TIL113-"));
    Serial.print(ch);
    Serial.print(F("): "));
    Serial.println(resultado[ch] ? F("PASS") : F("*** FAIL ***"));
  }
  Serial.print(F("  NAND (SCL4068): "));
  Serial.println(nand_ok ? F("PASS") : F("*** FAIL ***"));

  Serial.println();
  if (falhas == 0 && nand_ok) {
    Serial.println(F(">>> PLACA OK! Todos os canais funcionando. <<<"));
  } else {
    Serial.print(F(">>> PLACA COM DEFEITO! "));
    Serial.print(falhas);
    Serial.println(F(" canal(is) falharam. <<<"));
  }
  Serial.println(F("====================================="));
  Serial.println();

  desligarTudo();
}

// ==================== TESTE CANAL INDIVIDUAL ====================
bool testarCanalInterno(int ch) {
  // 1. Desligar tudo
  desligarTudo();
  delay(DELAY_ESTAB);

  // 2. Verificar que saida esta HIGH (pullup, TIL113 desligado)
  int leitura_off = digitalRead(PIN_FOTO[ch]);

  // 3. Ligar LED do TIL113
  digitalWrite(PIN_LED[ch], HIGH);
  delay(DELAY_ESTAB);

  // 4. Ler fototransistor (deve ir para LOW quando LED acende)
  int leitura_on = digitalRead(PIN_FOTO[ch]);

  // 5. Desligar
  digitalWrite(PIN_LED[ch], LOW);

  // Resultado
  bool ok = (leitura_off == HIGH) && (leitura_on == LOW);

  Serial.print(F("CH"));
  Serial.print(ch);
  Serial.print(F(" [D"));
  Serial.print(PIN_LED[ch]);
  Serial.print(F("→TIL113→D"));
  Serial.print(PIN_FOTO[ch]);
  Serial.print(F("] OFF="));
  Serial.print(leitura_off ? "H" : "L");
  Serial.print(F(" ON="));
  Serial.print(leitura_on ? "H" : "L");
  Serial.print(F(" → "));
  Serial.println(ok ? F("PASS") : F("FAIL"));

  return ok;
}

void testeCanal(int ch) {
  Serial.println();
  Serial.print(F("--- Teste individual CH"));
  Serial.print(ch);
  Serial.println(F(" ---"));

  bool ok = testarCanalInterno(ch);

  if (!ok) {
    Serial.println(F("  Possivel causa:"));
    Serial.println(F("  - TIL113 queimado (LED ou fototransistor)"));
    Serial.println(F("  - Resistor de entrada aberto"));
    Serial.println(F("  - Trilha rompida (verificar Back 8H)"));
    Serial.println(F("  - Pullup 4K7 faltando"));
    Serial.println(F("  - Fio mal conectado no edge connector"));
  }
  Serial.println();

  desligarTudo();
}

// ==================== TESTE NAND ====================
bool testarNANDInterno() {
  // SCL4068BE: 8-input NAND
  // Todos HIGH → saida LOW
  // Qualquer LOW → saida HIGH

  bool teste1_ok, teste2_ok;

  // Teste 1: Todos desligados → saidas fototransistor = HIGH → NAND = LOW
  desligarTudo();
  delay(DELAY_ESTAB);
  int nand_todos_off = digitalRead(PIN_NAND);
  // Com TIL113 desligados, coletores = HIGH (pullup)
  // NAND de 8x HIGH = LOW
  teste1_ok = (nand_todos_off == LOW);

  Serial.print(F("  Todos OFF → coletores HIGH → NAND="));
  Serial.print(nand_todos_off ? "H" : "L");
  Serial.print(F(" (esperado L) → "));
  Serial.println(teste1_ok ? F("OK") : F("FAIL"));

  // Teste 2: Ligar 1 canal → 1 coletor LOW → NAND = HIGH
  digitalWrite(PIN_LED[0], HIGH);
  delay(DELAY_ESTAB);
  int nand_um_on = digitalRead(PIN_NAND);
  // 1 entrada LOW + 7 HIGH → NAND = HIGH
  teste2_ok = (nand_um_on == HIGH);

  Serial.print(F("  CH0 ON → 1 LOW + 7 HIGH → NAND="));
  Serial.print(nand_um_on ? "H" : "L");
  Serial.print(F(" (esperado H) → "));
  Serial.println(teste2_ok ? F("OK") : F("FAIL"));

  desligarTudo();

  bool ok = teste1_ok && teste2_ok;
  Serial.print(F("  SCL4068BE: "));
  Serial.println(ok ? F("PASS") : F("FAIL"));

  return ok;
}

void testeNAND() {
  Serial.println();
  Serial.println(F("--- Teste NAND (SCL4068BE) ---"));
  testarNANDInterno();
  Serial.println();
  desligarTudo();
}

// ==================== STRESS TEST ====================
void stressTest() {
  Serial.println();
  Serial.println(F("========== STRESS TEST (100 ciclos) =========="));

  int falhas_total = 0;
  int falhas_canal[8] = {0};

  for (int ciclo = 0; ciclo < 100; ciclo++) {
    // Verificar emergencia
    if (digitalRead(PIN_EMERG) == LOW) {
      desligarTudo();
      Serial.println(F("!!! EMERGENCIA - Stress test abortado !!!"));
      return;
    }

    for (int ch = 0; ch < NUM_CH; ch++) {
      desligarTudo();
      delay(5);

      int off = digitalRead(PIN_FOTO[ch]);
      digitalWrite(PIN_LED[ch], HIGH);
      delay(5);
      int on = digitalRead(PIN_FOTO[ch]);
      digitalWrite(PIN_LED[ch], LOW);

      if (off != HIGH || on != LOW) {
        falhas_canal[ch]++;
        falhas_total++;
      }
    }

    if ((ciclo + 1) % 10 == 0) {
      Serial.print(F("  Ciclo "));
      Serial.print(ciclo + 1);
      Serial.print(F("/100 - Falhas acumuladas: "));
      Serial.println(falhas_total);
    }
  }

  Serial.println();
  Serial.println(F("--- Resultado Stress Test ---"));
  for (int ch = 0; ch < NUM_CH; ch++) {
    Serial.print(F("  CH"));
    Serial.print(ch);
    Serial.print(F(": "));
    Serial.print(falhas_canal[ch]);
    Serial.println(F("/100 falhas"));
  }
  Serial.print(F("  TOTAL: "));
  Serial.print(falhas_total);
  Serial.print(F("/800 ("));
  Serial.print(100.0 - (falhas_total * 100.0 / 800.0), 1);
  Serial.println(F("% sucesso)"));

  if (falhas_total == 0) {
    Serial.println(F("  >>> EXCELENTE! 100% de sucesso <<<"));
  } else if (falhas_total < 5) {
    Serial.println(F("  >>> Aceitavel - possiveis falhas intermitentes <<<"));
  } else {
    Serial.println(F("  >>> REPROVADO - verificar canais com falhas <<<"));
  }
  Serial.println();

  desligarTudo();
}

// ==================== LEITURA CONTINUA ====================
void leituraContinua() {
  Serial.println();
  Serial.println(F("--- Leitura Continua (envie qualquer tecla para parar) ---"));
  Serial.println(F("CH: 0 1 2 3 4 5 6 7 | NAND"));

  while (!Serial.available()) {
    // Verificar emergencia
    if (digitalRead(PIN_EMERG) == LOW) {
      desligarTudo();
      Serial.println(F("!!! EMERGENCIA !!!"));
      return;
    }

    Serial.print(F("    "));
    for (int ch = 0; ch < NUM_CH; ch++) {
      Serial.print(digitalRead(PIN_FOTO[ch]) ? "H " : "L ");
    }
    Serial.print(F("| "));
    Serial.println(digitalRead(PIN_NAND) ? "H" : "L");

    delay(500);
  }

  // Limpar buffer
  while (Serial.available()) Serial.read();
  Serial.println(F("Parado."));
  Serial.println();
}

// ==================== UTILITARIOS ====================
void ligarTudo() {
  Serial.println(F("Ligando todos os 8 canais..."));
  for (int i = 0; i < NUM_CH; i++) {
    digitalWrite(PIN_LED[i], HIGH);
  }
  delay(DELAY_ESTAB);

  Serial.print(F("Leitura: "));
  for (int ch = 0; ch < NUM_CH; ch++) {
    Serial.print(digitalRead(PIN_FOTO[ch]) ? "H " : "L ");
  }
  Serial.print(F("NAND="));
  Serial.println(digitalRead(PIN_NAND) ? "H" : "L");
}

void desligarTudo() {
  for (int i = 0; i < NUM_CH; i++) {
    digitalWrite(PIN_LED[i], LOW);
  }
}
