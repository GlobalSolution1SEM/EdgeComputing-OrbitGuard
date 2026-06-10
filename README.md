# OrbitGuard - Protótipo Arduino/C++ (Wokwi)

Console de apoio à decisão para operadores de satélite, implementado como protótipo físico simulado no Wokwi. O sistema detecta a aproximação de um objeto, calcula um risco explicável e recomenda uma ação operacional antes que a janela de decisão se feche.

## Índice

1. [Descrição do projeto](#descrição-do-projeto)
2. [Objetivo da solução](#objetivo-da-solução)
3. [Componentes utilizados](#componentes-utilizados)
4. [Explicação do funcionamento](#explicação-do-funcionamento)
5. [Estrutura do circuito](#estrutura-do-circuito)
6. [Instruções de execução](#instruções-de-execução)
7. [Integrantes do grupo](#integrantes-do-grupo)
8. [Arquivos do projeto](#arquivos-do-projeto)

## Descrição do projeto

O OrbitGuard é uma solução de apoio à decisão para situações de risco orbital. Satélites ativos sustentam serviços essenciais como comunicação, navegação, previsão do tempo e monitoramento ambiental, e operam num ambiente cada vez mais congestionado por detritos espaciais. Quando um objeto se aproxima, o operador tem pouco tempo para decidir entre preservar combustível, manter a missão ou agir para evitar uma colisão.

Este protótipo representa fisicamente esse cenário. O sensor ultrassônico simula um detrito se aproximando do satélite. À medida que a distância diminui, o sistema calcula um score de risco, classifica a gravidade, emite alertas sonoros e visuais e mostra no display uma recomendação clara de ação. Os dados são simulados de propósito, já que o foco está na lógica de decisão, não na física orbital real.

## Objetivo da solução

Transformar minutos de incerteza em uma decisão clara, justificável e rápida. Em vez de apenas avisar que existe um risco, o sistema responde o que importa para o operador: qual o nível de risco, por que ele é alto, quanto tempo resta para agir e qual a ação recomendada.

Os objetivos específicos do protótipo são:

- Calcular um score de risco com critérios visíveis, não um número arbitrário.
- Classificar o risco em faixas e traduzir cada faixa numa recomendação operacional.
- Sinalizar a urgência por mais de um canal: visual (LEDs e display) e sonoro (buzzer).
- Registrar cada leitura pela porta serial, permitindo auditar a decisão depois do evento.

## Componentes utilizados

| Componente | Quantidade | Função no projeto |
|---|---|---|
| Arduino Uno | 1 | Controlador principal |
| Protoboard | 1 | Distribuição de energia |
| Sensor ultrassônico HC-SR04 | 1 | Mede a distância do objeto que se aproxima |
| Potenciômetro | 1 | Simula a velocidade relativa do objeto |
| Display LCD 16x2 com módulo I2C | 1 | Mostra o resumo de risco e a recomendação |
| Buzzer | 1 | Alerta sonoro proporcional ao risco |
| LED verde | 1 | Indica risco baixo |
| LED amarelo | 1 | Indica risco médio |
| LED vermelho | 1 | Indica risco alto ou crítico |
| Resistor de 220 ohm | 3 | Limita a corrente de cada LED |

Bibliotecas de software: `Wire.h` e `LiquidCrystal_I2C.h`.

## Explicação do funcionamento

### Leitura dos dados

A cada ciclo o Arduino lê duas entradas. A distância vem do HC-SR04 e representa a distância mínima estimada até o objeto. A velocidade relativa vem do potenciômetro, num intervalo de 0 a 100, e permite variar o cenário durante a demonstração.

### Cálculo do score de risco

O score vai de 0 a 100 e combina dois fatores ponderados, ambos visíveis no código:

| Fator | Peso máximo | Regra |
|---|---|---|
| Distância | 55 pontos | Quanto mais perto, maior o risco |
| Velocidade relativa | 45 pontos | Quanto mais rápido, maior o risco |

O motivo exibido no display é sempre o fator que mais contribuiu naquele instante, o que mantém a decisão explicável.

### Classificação e resposta

O score define a faixa de risco. Cada faixa aciona um LED, um padrão de buzzer e uma recomendação no display:

| Score | Nível | LED | Recomendação | Buzzer |
|---|---|---|---|---|
| 0 a 39 | Baixo | Verde | Monitorar | Mudo |
| 40 a 69 | Médio | Amarelo | Preparar | Beep lento |
| 70 a 84 | Alto | Vermelho | Manobrar | Beep rápido |
| 85 a 100 | Crítico | Vermelho piscando | Escalar | Alarme contínuo |

### Saída para o operador

O display LCD alterna entre duas telas a cada 1,5 segundo. A primeira mostra o score, o nível e o motivo principal. A segunda mostra a ação recomendada e uma janela de tempo simulada em segundos. Em paralelo, a porta serial imprime distância, velocidade, score, nível e ação a cada ciclo, funcionando como registro do evento.

## Estrutura do circuito

A alimentação é distribuída pela protoboard. Um fio leva o 5V do Arduino ao trilho positivo e outro leva o GND ao trilho negativo. Os componentes puxam energia desses trilhos. Os sinais vão direto aos pinos do Arduino.

### Sinais

| Pino do Arduino | Ligado a |
|---|---|
| 9 | TRIG do HC-SR04 |
| 10 | ECHO do HC-SR04 |
| A0 | Sinal do potenciômetro |
| A4 | SDA do LCD |
| A5 | SCL do LCD |
| 8 | Buzzer (terminal positivo) |
| 5 | LED verde, através de resistor de 220 ohm |
| 6 | LED amarelo, através de resistor de 220 ohm |
| 7 | LED vermelho, através de resistor de 220 ohm |

### Alimentação

| Trilho da protoboard | Recebe |
|---|---|
| Positivo (5V) | VCC do sensor, do LCD e do potenciômetro |
| Negativo (GND) | GND do sensor, do LCD e do potenciômetro, terminal negativo do buzzer e cátodos dos três LEDs |

O arquivo `diagram.json` contém a montagem completa e pode ser carregado direto no Wokwi.

## Integrantes do grupo

- Nome completo: João Pedro Ferreira Pinheiro - RM: 570569
- Nome completo: Vitor de Castro Buzato - RM: 569720
- Nome completo: João Pedro Gomes de Matos - RM: 569934
- Nome completo: Davi Pereira - RM: 572337
- Nome completo: Gabriel Palmieri - RM: 570508

## Arquivos do projeto

| Arquivo | Conteúdo |
|---|---|
| `sketch.ino` | Código fonte em Arduino/C++ com toda a lógica de decisão |
| `README.md` | Este documento |
