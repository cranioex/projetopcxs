"""
Gerador de Esquemático KiCad - Giga de Teste Placa 8H (F211)
Gera arquivo .kicad_sch com componentes posicionados e fios conectados
"""
import uuid

def uid():
    return str(uuid.uuid4())

# ============================================================
# CONFIGURAÇÃO DO LAYOUT (tudo em mm)
# ============================================================

# Colunas X
X_TITULO = 30
X_MEGA_OUT = 55       # Arduino Mega saidas (estimulo)
X_RESISTOR = 90       # Resistores R1-R8
X_F211_IN = 125       # F211 entrada (edge esquerdo)
X_F211_OUT = 195      # F211 saida (edge direito)
X_MEGA_IN = 255       # Arduino Mega entradas (feedback)
X_SCOPE = 310         # Osciloscopio

# Linha Y base dos pinos
Y_BASE = 70
PIN_STEP = 7.62       # Espaçamento entre canais (3x o padrão 2.54)

# Posição dos textos
Y_TITLE = 25
Y_NOTES = 200

NUM_CHANNELS = 8

def header():
    return f"""(kicad_sch
  (version 20231120)
  (generator "eeschema")
  (generator_version "9.0")
  (uuid "{uid()}")
  (paper "A3")
  (title_block
    (title "GIGA DE TESTE - Placa 8H (F211) - Frente 8H / Back 8H")
    (date "2026-03-11")
    (rev "1.0")
    (company "Projeto PCXs - Injetora INCAI/Ferbate")
    (comment 1 "Placa de entrada isolada - 8x TIL113 + SCL4068BE")
    (comment 2 "Teste com Arduino Mega 2560 + Hantek 6022BE")
    (comment 3 "Fotos: Frente 8H.jpg / Back 8H.jpg")
  )
"""

def lib_symbols():
    """Define símbolos usados no esquemático"""
    return """
  (lib_symbols
    (symbol "Connector_Generic:Conn_01x02"
      (pin_names (offset 1.016) hide)
      (exclude_from_sim no)
      (in_bom yes)
      (on_board yes)
      (property "Reference" "J" (at 0 2.54 0) (effects (font (size 1.27 1.27))))
      (property "Value" "Conn_01x02" (at 0 -5.08 0) (effects (font (size 1.27 1.27))))
      (symbol "Conn_01x02_1_1"
        (polyline (pts (xy -1.27 0) (xy -0.508 0)) (stroke (width 0.1524) (type default)) (fill (type none)))
        (polyline (pts (xy -1.27 -2.54) (xy -0.508 -2.54)) (stroke (width 0.1524) (type default)) (fill (type none)))
        (rectangle (start -0.508 0.508) (end 0.508 -3.048) (stroke (width 0.254) (type default)) (fill (type background)))
        (pin passive line (at -3.81 0 0) (length 2.54) (name "Pin_1" (effects (font (size 1.27 1.27)))) (number "1" (effects (font (size 1.27 1.27)))))
        (pin passive line (at -3.81 -2.54 0) (length 2.54) (name "Pin_2" (effects (font (size 1.27 1.27)))) (number "2" (effects (font (size 1.27 1.27)))))
      )
    )
    (symbol "Connector_Generic:Conn_01x03"
      (pin_names (offset 1.016) hide)
      (exclude_from_sim no)
      (in_bom yes)
      (on_board yes)
      (property "Reference" "J" (at 0 2.54 0) (effects (font (size 1.27 1.27))))
      (property "Value" "Conn_01x03" (at 0 -7.62 0) (effects (font (size 1.27 1.27))))
      (symbol "Conn_01x03_1_1"
        (polyline (pts (xy -1.27 0) (xy -0.508 0)) (stroke (width 0.1524) (type default)) (fill (type none)))
        (polyline (pts (xy -1.27 -2.54) (xy -0.508 -2.54)) (stroke (width 0.1524) (type default)) (fill (type none)))
        (polyline (pts (xy -1.27 -5.08) (xy -0.508 -5.08)) (stroke (width 0.1524) (type default)) (fill (type none)))
        (rectangle (start -0.508 0.508) (end 0.508 -5.588) (stroke (width 0.254) (type default)) (fill (type background)))
        (pin passive line (at -3.81 0 0) (length 2.54) (name "Pin_1" (effects (font (size 1.27 1.27)))) (number "1" (effects (font (size 1.27 1.27)))))
        (pin passive line (at -3.81 -2.54 0) (length 2.54) (name "Pin_2" (effects (font (size 1.27 1.27)))) (number "2" (effects (font (size 1.27 1.27)))))
        (pin passive line (at -3.81 -5.08 0) (length 2.54) (name "Pin_3" (effects (font (size 1.27 1.27)))) (number "3" (effects (font (size 1.27 1.27)))))
      )
    )
    (symbol "Device:R"
      (pin_numbers hide)
      (pin_names (offset 0))
      (exclude_from_sim no)
      (in_bom yes)
      (on_board yes)
      (property "Reference" "R" (at 2.032 0 90) (effects (font (size 1.27 1.27))))
      (property "Value" "R" (at 0 0 90) (effects (font (size 1.27 1.27))))
      (symbol "R_0_1"
        (rectangle (start -1.016 -2.54) (end 1.016 2.54) (stroke (width 0.254) (type default)) (fill (type none)))
      )
      (symbol "R_1_1"
        (pin passive line (at 0 3.81 270) (length 1.27) (name "~" (effects (font (size 1.27 1.27)))) (number "1" (effects (font (size 1.27 1.27)))))
        (pin passive line (at 0 -3.81 90) (length 1.27) (name "~" (effects (font (size 1.27 1.27)))) (number "2" (effects (font (size 1.27 1.27)))))
      )
    )
  )
"""

def wire(x1, y1, x2, y2):
    """Gera um fio entre dois pontos"""
    return f'  (wire (pts (xy {x1:.2f} {y1:.2f}) (xy {x2:.2f} {y2:.2f})) (stroke (width 0) (type default)) (uuid "{uid()}"))\n'

def label(name, x, y, angle=0):
    """Gera um net label"""
    return f'  (label "{name}" (at {x:.2f} {y:.2f} {angle}) (effects (font (size 1.27 1.27))) (uuid "{uid()}"))\n'

def text_block(txt, x, y, size=2.0, bold=False):
    bold_str = " bold" if bold else ""
    return f'  (text "{txt}" (at {x:.2f} {y:.2f} 0) (effects (font (size {size} {size}){bold_str}) (justify left)))\n'

def connector(ref, value, x, y, num_pins, angle=0):
    """Gera um conector genérico"""
    lib = f"Connector_Generic:Conn_01x{num_pins:02d}"

    # Gerar lib_symbol inline se necessário
    pins_str = ""
    for i in range(num_pins):
        py = -i * 2.54
        pins_str += f'        (pin passive line (at -3.81 {py:.2f} 0) (length 2.54) (name "Pin_{i+1}" (effects (font (size 1.27 1.27)))) (number "{i+1}" (effects (font (size 1.27 1.27)))))\n'

    return f"""  (symbol
    (lib_id "{lib}")
    (at {x:.2f} {y:.2f} {angle})
    (unit 1)
    (exclude_from_sim no)
    (in_bom yes)
    (on_board yes)
    (dnp no)
    (uuid "{uid()}")
    (property "Reference" "{ref}" (at {x:.2f} {y - 5:.2f} 0) (effects (font (size 1.27 1.27))))
    (property "Value" "{value}" (at {x:.2f} {y - 3:.2f} 0) (effects (font (size 1.27 1.27))))
  )
"""

def resistor(ref, x, y, value="1K", angle=90):
    """Gera um resistor"""
    return f"""  (symbol
    (lib_id "Device:R")
    (at {x:.2f} {y:.2f} {angle})
    (unit 1)
    (exclude_from_sim no)
    (in_bom yes)
    (on_board yes)
    (dnp no)
    (uuid "{uid()}")
    (property "Reference" "{ref}" (at {x:.2f} {y - 3:.2f} 0) (effects (font (size 1.0 1.0))))
    (property "Value" "{value}" (at {x:.2f} {y + 3:.2f} 0) (effects (font (size 1.0 1.0))))
  )
"""

def conn_01x_lib(num_pins):
    """Gera a definição de biblioteca para um conector de N pinos"""
    lib_name = f"Connector_Generic:Conn_01x{num_pins:02d}"
    h = (num_pins - 1) * 2.54 + 1.016

    pins = ""
    lines = ""
    for i in range(num_pins):
        py = -i * 2.54
        pins += f'        (pin passive line (at -3.81 {py:.2f} 0) (length 2.54) (name "Pin_{i+1}" (effects (font (size 1.27 1.27)))) (number "{i+1}" (effects (font (size 1.27 1.27)))))\n'
        lines += f'        (polyline (pts (xy -1.27 {py:.2f}) (xy -0.508 {py:.2f})) (stroke (width 0.1524) (type default)) (fill (type none)))\n'

    return f"""    (symbol "{lib_name}"
      (pin_names (offset 1.016) hide)
      (exclude_from_sim no)
      (in_bom yes)
      (on_board yes)
      (property "Reference" "J" (at 0 2.54 0) (effects (font (size 1.27 1.27))))
      (property "Value" "Conn_01x{num_pins:02d}" (at 0 {-(num_pins)*2.54:.2f} 0) (effects (font (size 1.27 1.27))))
      (symbol "Conn_01x{num_pins:02d}_1_1"
{lines}        (rectangle (start -0.508 0.508) (end 0.508 {-(num_pins-1)*2.54 - 0.508:.2f}) (stroke (width 0.254) (type default)) (fill (type background)))
{pins}      )
    )
"""

def pin_pos(cx, cy, pin_num, angle=0):
    """Calcula posição do endpoint de um pino de conector.
    Pin 1 = (cx - 3.81, cy) para angle=0
    Pin 2 = (cx - 3.81, cy + 2.54)
    ...
    Para angle=180: pinos saem pela direita
    """
    # Offset do pino relativo ao símbolo (symbol space, Y-up)
    sx = -3.81
    sy = -(pin_num - 1) * 2.54

    if angle == 0:
        return (cx + sx, cy - sy)
    elif angle == 180:
        return (cx - sx, cy + sy)
    else:
        return (cx + sx, cy - sy)  # fallback

def resistor_pins(rx, ry, angle=90):
    """Retorna (pin1_pos, pin2_pos) do resistor.
    Angle 90 = horizontal: pin1 à esquerda, pin2 à direita
    """
    if angle == 90:
        return ((rx - 3.81, ry), (rx + 3.81, ry))
    elif angle == 0:
        return ((rx, ry - 3.81), (rx, ry + 3.81))
    return ((rx - 3.81, ry), (rx + 3.81, ry))


def main():
    out = header()

    # ============================================================
    # LIB SYMBOLS
    # ============================================================
    out += "\n  (lib_symbols\n"
    out += conn_01x_lib(2)
    out += conn_01x_lib(3)
    out += conn_01x_lib(10)
    out += """    (symbol "Device:R"
      (pin_numbers hide)
      (pin_names (offset 0))
      (exclude_from_sim no)
      (in_bom yes)
      (on_board yes)
      (property "Reference" "R" (at 2.032 0 90) (effects (font (size 1.27 1.27))))
      (property "Value" "R" (at 0 0 90) (effects (font (size 1.27 1.27))))
      (symbol "R_0_1"
        (rectangle (start -1.016 -2.54) (end 1.016 2.54) (stroke (width 0.254) (type default)) (fill (type none)))
      )
      (symbol "R_1_1"
        (pin passive line (at 0 3.81 270) (length 1.27) (name "~" (effects (font (size 1.27 1.27)))) (number "1" (effects (font (size 1.27 1.27)))))
        (pin passive line (at 0 -3.81 90) (length 1.27) (name "~" (effects (font (size 1.27 1.27)))) (number "2" (effects (font (size 1.27 1.27)))))
      )
    )
"""
    out += "  )\n"

    # ============================================================
    # TÍTULOS DAS SEÇÕES
    # ============================================================
    out += text_block("ARDUINO MEGA", X_MEGA_OUT - 5, Y_TITLE, 2.5, bold=True)
    out += text_block("Saidas Pin 22-29", X_MEGA_OUT - 5, Y_TITLE + 5, 1.5)

    out += text_block("R1-R8  1K", X_RESISTOR - 5, Y_TITLE, 2.0, bold=True)
    out += text_block("Protecao", X_RESISTOR - 5, Y_TITLE + 5, 1.5)

    out += text_block("PLACA 8H - F211", X_F211_IN - 5, Y_TITLE, 2.5, bold=True)
    out += text_block("Frente 8H / Back 8H", X_F211_IN - 5, Y_TITLE + 5, 1.5)
    out += text_block("Entrada (Edge Esq)", X_F211_IN - 5, Y_TITLE + 9, 1.3)

    out += text_block("PLACA 8H - F211", X_F211_OUT - 5, Y_TITLE, 2.5, bold=True)
    out += text_block("Saida (Edge Dir)", X_F211_OUT - 5, Y_TITLE + 5, 1.5)

    out += text_block("ARDUINO MEGA", X_MEGA_IN - 5, Y_TITLE, 2.5, bold=True)
    out += text_block("Entradas Pin 30-37", X_MEGA_IN - 5, Y_TITLE + 5, 1.5)

    out += text_block("HANTEK 6022BE", X_SCOPE - 5, Y_TITLE, 2.0, bold=True)

    # ============================================================
    # COMPONENTES
    # ============================================================

    # Arduino Mega SAÍDAS (J1) - pinos saem pela DIREITA (angle 180)
    j1_x, j1_y = X_MEGA_OUT, Y_BASE
    out += connector("J1", "MEGA_OUT", j1_x, j1_y, 10, angle=180)

    # F211 ENTRADA (J2) - pinos saem pela ESQUERDA (angle 0)
    j2_x, j2_y = X_F211_IN, Y_BASE
    out += connector("J2", "8H_ENTRADA", j2_x, j2_y, 10, angle=0)

    # F211 SAÍDA (J3) - pinos saem pela DIREITA (angle 180)
    j3_x, j3_y = X_F211_OUT, Y_BASE
    out += connector("J3", "8H_SAIDA", j3_x, j3_y, 10, angle=180)

    # Arduino Mega ENTRADAS (J4) - pinos saem pela ESQUERDA (angle 0)
    j4_x, j4_y = X_MEGA_IN, Y_BASE
    out += connector("J4", "MEGA_IN", j4_x, j4_y, 10, angle=0)

    # Osciloscópio (J5) - 3 pinos
    j5_x, j5_y = X_SCOPE, Y_BASE
    out += connector("J5", "SCOPE", j5_x, j5_y, 3, angle=0)

    # Resistores R1-R8 (horizontal, angle=90)
    for i in range(NUM_CHANNELS):
        ry = Y_BASE + i * PIN_STEP
        out += resistor(f"R{i+1}", X_RESISTOR, ry, "1K", angle=90)

    # ============================================================
    # FIOS (WIRES)
    # ============================================================

    # J1 (Mega Out, angle 180) → R1-R8 → J2 (F211 entrada, angle 0)
    for i in range(NUM_CHANNELS):
        # Posição do pino do J1 (Mega Out) - angle 180, pinos saem DIREITA
        j1_pin = pin_pos(j1_x, j1_y, i + 1, angle=180)

        # Posição dos pinos do resistor (horizontal)
        ry = Y_BASE + i * PIN_STEP
        r_pin1, r_pin2 = resistor_pins(X_RESISTOR, ry, angle=90)

        # Posição do pino do J2 (F211 entrada) - angle 0, pinos saem ESQUERDA
        j2_pin = pin_pos(j2_x, j2_y, i + 1, angle=0)

        # Fio: J1 pino → ponto intermediário → R pin1
        # Primeiro desce/sobe até a altura do resistor, depois vai horizontal
        out += wire(j1_pin[0], j1_pin[1], j1_pin[0] + 5, j1_pin[1])  # sai do J1
        out += wire(j1_pin[0] + 5, j1_pin[1], j1_pin[0] + 5, ry)    # vertical
        out += wire(j1_pin[0] + 5, ry, r_pin1[0], r_pin1[1])         # horizontal ao R

        # Fio: R pin2 → J2 pino
        out += wire(r_pin2[0], r_pin2[1], j2_pin[0], j2_pin[1])
        # Se pinos não estão alinhados verticalmente, precisa de fio extra
        if abs(r_pin2[1] - j2_pin[1]) > 0.1:
            mid_x = r_pin2[0] + (j2_pin[0] - r_pin2[0]) / 2
            out += wire(r_pin2[0], r_pin2[1], mid_x, r_pin2[1])
            out += wire(mid_x, r_pin2[1], mid_x, j2_pin[1])
            out += wire(mid_x, j2_pin[1], j2_pin[0], j2_pin[1])

    # J3 (F211 saída, angle 180) → J4 (Mega In, angle 0)
    for i in range(NUM_CHANNELS):
        j3_pin = pin_pos(j3_x, j3_y, i + 1, angle=180)
        j4_pin = pin_pos(j4_x, j4_y, i + 1, angle=0)

        # Fio direto horizontal (mesma altura se PIN_STEP coincide)
        if abs(j3_pin[1] - j4_pin[1]) < 0.1:
            out += wire(j3_pin[0], j3_pin[1], j4_pin[0], j4_pin[1])
        else:
            mid_x = (j3_pin[0] + j4_pin[0]) / 2
            out += wire(j3_pin[0], j3_pin[1], mid_x, j3_pin[1])
            out += wire(mid_x, j3_pin[1], mid_x, j4_pin[1])
            out += wire(mid_x, j4_pin[1], j4_pin[0], j4_pin[1])

    # ============================================================
    # LABELS nos pinos (identificação clara)
    # ============================================================
    for i in range(NUM_CHANNELS):
        j1_pin = pin_pos(j1_x, j1_y, i + 1, angle=180)
        out += label(f"Pin{22+i}", j1_pin[0] + 2, j1_pin[1])

        j2_pin = pin_pos(j2_x, j2_y, i + 1, angle=0)
        out += label(f"CH{i}", j2_pin[0] - 2, j2_pin[1], 180)

        j3_pin = pin_pos(j3_x, j3_y, i + 1, angle=180)
        out += label(f"OUT{i}", j3_pin[0] + 2, j3_pin[1])

        j4_pin = pin_pos(j4_x, j4_y, i + 1, angle=0)
        out += label(f"Pin{30+i}", j4_pin[0] - 2, j4_pin[1], 180)

    # Labels VCC e GND nos pinos 9 e 10
    j2_vcc = pin_pos(j2_x, j2_y, 9, angle=0)
    j2_gnd = pin_pos(j2_x, j2_y, 10, angle=0)
    out += label("+5V", j2_vcc[0] - 2, j2_vcc[1], 180)
    out += label("GND", j2_gnd[0] - 2, j2_gnd[1], 180)

    j3_vcc = pin_pos(j3_x, j3_y, 9, angle=180)
    j3_gnd = pin_pos(j3_x, j3_y, 10, angle=180)
    out += label("+5V", j3_vcc[0] + 2, j3_vcc[1])
    out += label("GND", j3_gnd[0] + 2, j3_gnd[1])

    # Scope labels
    j5_ch1 = pin_pos(j5_x, j5_y, 1, angle=0)
    j5_ch2 = pin_pos(j5_x, j5_y, 2, angle=0)
    j5_gnd = pin_pos(j5_x, j5_y, 3, angle=0)
    out += label("SCOPE_CH1", j5_ch1[0] - 2, j5_ch1[1], 180)
    out += label("SCOPE_CH2", j5_ch2[0] - 2, j5_ch2[1], 180)
    out += label("GND", j5_gnd[0] - 2, j5_gnd[1], 180)

    # ============================================================
    # NOTAS (embaixo, espaçadas)
    # ============================================================
    out += text_block(
        "COMO TESTAR A PLACA 8H (F211) - Fotos: Frente 8H.jpg / Back 8H.jpg",
        X_TITULO, Y_NOTES, 2.5, bold=True
    )

    out += text_block(
        "1. Alimentar +5V no pino VCC (J2.9) e GND (J2.10) - usar fonte de bancada limitada a 200mA",
        X_TITULO, Y_NOTES + 8, 1.5
    )
    out += text_block(
        "2. Arduino Mega Pin 22-29 (J1) -> Resistores 1K (R1-R8) -> Entrada dos TIL113 (J2.1-J2.8)",
        X_TITULO, Y_NOTES + 14, 1.5
    )
    out += text_block(
        "3. Saida digital dos TIL113 (J3.1-J3.8) -> Arduino Mega Pin 30-37 (J4) para leitura",
        X_TITULO, Y_NOTES + 20, 1.5
    )
    out += text_block(
        "4. Osciloscopio: CH1 na entrada de um TIL113, CH2 na saida. Trigger CH1 borda subida",
        X_TITULO, Y_NOTES + 26, 1.5
    )
    out += text_block(
        "5. Os TIL113 precisam ~10-20mA no LED. Com 5V/1K=5mA (pode ser baixo). Se nao funcionar, trocar R para 470R",
        X_TITULO, Y_NOTES + 32, 1.5
    )
    out += text_block(
        "6. NUNCA conectar 24V direto no Arduino! Se usar 24V, manter os resistores R1-R8",
        X_TITULO, Y_NOTES + 38, 1.5
    )
    out += text_block(
        "7. Codigo Arduino: teste_f211_8h_mega.ino",
        X_TITULO, Y_NOTES + 44, 1.5
    )

    out += "\n)\n"
    return out


if __name__ == "__main__":
    sch = main()
    output_path = r"C:\projetopcxs\kicad-gigas\giga-f211-8h.kicad_sch"
    with open(output_path, "w", encoding="utf-8") as f:
        f.write(sch)
    print(f"Esquematico gerado: {output_path}")
    print(f"Tamanho: {len(sch)} bytes")
