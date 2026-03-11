# Giga de Teste - Injetora INCAI/Ferbate

Projeto de reparo e validação de **8 tipos de placas PCB** (10 unidades cada = 80 placas) de uma máquina injetora industrial com sistema pneumático. As placas pegaram chuva e foram limpas com ultrassom + desengraxante dielétrico. Este projeto cria gigas de teste para validar cada placa na bancada antes de reinstalar na máquina.

## Placas

| ID | Modelo | Função | CIs Principais | Alimentação | Giga |
|---|---|---|---|---|---|
| 1A | PRO100 2732A | CPU / Processador | EPROMs 2732, 74LS181 | +5V | - |
| 2B | S124-2 Ferbate | Memória / Expansão I/O | 74LS408, HCF4068BE | +5V | - |
| 3C | AVP2 V0.2 | Processamento Analógico | Op-amps, Trimpots | +5V/+12V | - |
| 4D | KSF8S V0.1 | Saídas Potência 8ch | CD4099BE, TIL113, TIP100 | +5V/+24V | PRONTA |
| 5E | F504 V0.0 | Expansão I/O Digital | CD4099BE (múltiplos) | +5V | - |
| 6F | FERBATE-11660-2 | Entradas Isoladas | HCF4572BE, CD4068BE | +5V/+12V | - |
| 7G | F221 V1.0 | DAC Saídas Analógicas | LM324N, HCF4512BE | +5V/+12V | - |
| 8H | F211 03/88 | Entradas Isoladas 8ch | TIL113 (8x), SCL4068BE | +5V | PRONTA |

## Estrutura do Projeto

```
projetopcxs/
├── giga-teste-web/          # Web app - catálogo visual + diagramas interativos
│   ├── index.html           # Página principal (abrir no navegador)
│   ├── style.css            # Tema escuro
│   ├── app.js               # Navegação + tooltips interativos
│   └── img/                 # Fotos frente/verso das 8 placas
├── Frente Verso PCBS/       # Fotos originais alta resolução
├── kicad-gigas/             # Esquemáticos KiCad (referência)
├── teste_ksf8s_mega.ino     # Código Arduino - Giga placa 4D (KSF8S)
├── teste_f211_mega.ino      # Código Arduino - Giga placa 8H (F211)
├── ligacoes_mega.txt        # Diagrama de ligação KSF8S (4D)
├── ligacoes_f211.txt        # Diagrama de ligação F211 (8H)
└── esquema_giga_teste.txt   # Esquema geral do projeto
```

## Web App

Abra `giga-teste-web/index.html` no navegador. Funcionalidades:

- Catálogo visual das 8 placas com fotos frente/verso
- Diagrama de blocos do sistema completo
- **Diagrama interativo SVG** da giga de teste (placa 8H)
- Tooltips com detalhes de cada componente (passe o mouse)
- Tabela de ligações rápida
- Navegação por teclado (setas ← →)
- Zoom nas fotos (clique)

## Hardware Necessário

- **Arduino Mega 2560** — Controlador dos testes
- **Fonte de bancada** — +5V (lógica) e +24V (potência, quando necessário)
- **Multímetro** — Mapear pinos dos edge connectors
- **Osciloscópio Hantek 6022BE** — Debug de sinais (opcional)
- **Protoboard + resistores** — 1K e 4K7 para interface
- **Fios jumper** — Conexão Arduino ↔ placa

## Como Usar

### Placa F211 (8H) - Entradas Isoladas

1. Mapear pinos do edge connector com multímetro (continuidade)
2. Montar resistores 1K e 4K7 na protoboard
3. Conectar conforme `ligacoes_f211.txt` ou diagrama SVG no web app
4. Upload `teste_f211_mega.ino` no Arduino Mega
5. Serial Monitor 115200 baud → pressionar **T** para teste completo

### Placa KSF8S (4D) - Saídas de Potência

1. Mapear pinos do edge connector com multímetro
2. Conectar conforme `ligacoes_mega.txt`
3. Upload `teste_ksf8s_mega.ino` no Arduino Mega
4. Serial Monitor 115200 baud → seguir menu interativo

## Comandos do Serial Monitor

| Tecla | Função |
|---|---|
| T | Teste completo (todos os canais) |
| 0-7 | Testar canal individual |
| N | Testar NAND gate (SCL4068BE) |
| S | Stress test (100 ciclos) |
| L | Ligar todos os canais |
| D | Desligar tudo |
| R | Leitura contínua em tempo real |

## Fotos das Placas

Cada placa tem foto da **frente** (componentes) e **verso** (trilhas):

- `Frente 1A.jpg` / `Back 1A.jpg` — CPU PRO100
- `Frente 2B.jpg` / `Back 2B.jpg` — Memória S124
- `Frente 3C.jpg` / `Back 3C.jpg` — Analógica AVP2
- `Frente 4D.jpg` / `Back 4D.jpg` — Potência KSF8S
- `Frente 5E.jpg` / `Back 5E.jpg` — I/O Digital F504
- `Frente 6F.jpg` / `Back 6F.jpg` — Entradas FERBATE
- `Frente 7G.jpg` / `Back 7G.jpg` — DAC F221
- `Frente 8H.jpg` / `Back 8H.jpg` — Entradas F211

## Ordem Recomendada de Teste

1. **8H - F211** (mais simples, só optoacopladores)
2. **4D - KSF8S** (saídas de potência, já tem código)
3. **6F - FERBATE** (entradas isoladas com lógica)
4. **7G - F221** (DAC analógico, precisa osciloscópio)
5. **3C - AVP2** (analógico, medir tensões)
6. **5E - F504** (I/O digital)
7. **2B - S124** (memória/expansão)
8. **1A - PRO100** (CPU - mais complexa, testar por último)
