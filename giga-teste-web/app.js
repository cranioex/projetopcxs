// ==================== NAVIGATION ====================
document.querySelectorAll('.nav-btn').forEach(btn => {
  btn.addEventListener('click', () => {
    document.querySelectorAll('.nav-btn').forEach(b => b.classList.remove('active'));
    document.querySelectorAll('.board-section').forEach(s => s.classList.remove('active'));
    btn.classList.add('active');
    const boardId = btn.dataset.board;
    const section = document.getElementById(`board-${boardId}`);
    if (section) {
      section.classList.add('active');
      section.scrollIntoView({ behavior: 'smooth', block: 'start' });
    }
  });
});

// ==================== IMAGE ZOOM ====================
document.querySelectorAll('.photo-container img').forEach(img => {
  img.addEventListener('click', () => {
    img.classList.toggle('zoomed');
  });
});

document.addEventListener('keydown', (e) => {
  if (e.key === 'Escape') {
    document.querySelectorAll('.photo-container img.zoomed').forEach(img => {
      img.classList.remove('zoomed');
    });
  }
});

// ==================== KEYBOARD SHORTCUTS ====================
document.addEventListener('keydown', (e) => {
  const btns = document.querySelectorAll('.nav-btn');
  const activeIdx = Array.from(btns).findIndex(b => b.classList.contains('active'));
  if (e.key === 'ArrowRight' && activeIdx < btns.length - 1) {
    btns[activeIdx + 1].click();
  } else if (e.key === 'ArrowLeft' && activeIdx > 0) {
    btns[activeIdx - 1].click();
  }
});

// ==================== DIAGRAM TOOLTIPS ====================
const tooltipData = {
  arduino: {
    title: 'Arduino Mega 2560',
    lines: [
      'D22-D29: Saidas digitais (HIGH/LOW) simulam sensores',
      'D30-D37: Entradas digitais leem fototransistores TIL113',
      'D38: Entrada para saida NAND do SCL4068BE',
      'GND: Conectar no GND comum da fonte e placa',
      'Alimentacao: Via USB do PC (5V separado da fonte)'
    ]
  },
  resistores: {
    title: 'Resistores Externos 1K (8x)',
    lines: [
      'Valor: 1K ohm (marrom-preto-vermelho)',
      'Funcao: Limitar corrente nos LEDs dos TIL113',
      'Corrente: ~5mA por canal (5V / 1K)',
      'Montar na protoboard entre Arduino e placa',
      'Se a placa JA tem resistores, nao precisa duplicar!'
    ]
  },
  f211: {
    title: 'Placa F211 (Frente 8H / Back 8H)',
    lines: [
      'TIL113 x8: Optoacopladores Darlington',
      '  - Pino 1: Anodo LED (entrada)',
      '  - Pino 2: Catodo LED (GND)',
      '  - Pino 4: Emissor fototransistor (GND)',
      '  - Pino 5: Coletor (saida - open collector)',
      'SCL4068BE: NAND 8 entradas',
      '  - Todos HIGH = saida LOW (placa OK)',
      'IMPORTANTE: Mapear pinos do edge connector!'
    ]
  },
  fonte: {
    title: 'Fonte de Bancada',
    lines: [
      'Canal 1: +5V (logica da placa F211)',
      'Limite corrente: 200mA (placa gasta ~50mA)',
      'GND: Comum com Arduino Mega',
      'NAO precisa de 12V/24V para esta placa!',
      'O Arduino gera 5V pelo USB - suficiente pros LEDs'
    ]
  },
  hantek: {
    title: 'Osciloscopio Hantek 6022BE',
    lines: [
      'OPCIONAL - Usar para debug se algum canal falhar',
      'CH1: Medir coletor do TIL113 (deve pulsar 0-5V)',
      'CH2: Medir saida NAND do SCL4068BE',
      'GND: Clip jacare no GND comum',
      'Software: OpenHantek no PC'
    ]
  }
};

// Create tooltip element
const tooltip = document.createElement('div');
tooltip.className = 'diagram-tooltip';
tooltip.innerHTML = '<h5></h5><div class="tooltip-lines"></div>';
document.body.appendChild(tooltip);

document.querySelectorAll('.clickable-comp').forEach(comp => {
  comp.addEventListener('mouseenter', (e) => {
    const info = comp.dataset.info;
    const data = tooltipData[info];
    if (!data) return;
    tooltip.querySelector('h5').textContent = data.title;
    tooltip.querySelector('.tooltip-lines').innerHTML = data.lines.map(l => `<p>${l}</p>`).join('');
    tooltip.classList.add('show');
  });

  comp.addEventListener('mousemove', (e) => {
    tooltip.style.left = (e.clientX + 15) + 'px';
    tooltip.style.top = (e.clientY + 15) + 'px';
  });

  comp.addEventListener('mouseleave', () => {
    tooltip.classList.remove('show');
  });
});

console.log('Giga de Teste Web - Carregado');
console.log('Use setas <- -> para navegar entre placas');
console.log('Clique nas fotos para zoom');
console.log('Passe o mouse nos componentes do diagrama para ver detalhes');
