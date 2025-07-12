# Orb Rush 🕹️

![Gameplay Screenshot](https://imgur.com/a/rbDARDF)

**Orb Rush** é um jogo de arcade 2D desenvolvido em **C++** com a biblioteca **SFML**. Inspirado no clássico Pac-Man, este projeto traz uma nova roupagem com uma temática baseada no jogo **Valorant**.

O jogador controla a agente Jett, navegando por um labirinto para coletar todos os Orbes enquanto desvia de quatro robôs inimigos com comportamentos distintos.

Este projeto foi desenvolvido como trabalho final para a disciplina de Algoritmos e Estruturas de Dados II do curso de Ciência da Computação.

---

## ✨ Funcionalidades

* **Jogabilidade Clássica:** Uma experiência divertida e desafiadora inspirada no Pac-Man.
* **Temática de Valorant:** Jogue como a agente Jett em um mapa temático.
* **Inimigos Inteligentes:** Quatro robôs inimigos com padrões de movimento únicos (dois perseguidores e dois aleatórios), usando o algoritmo de busca BFS (Busca em Largura) para encontrar o jogador.
* **Movimentação Fluida:** Movimento contínuo e baseado em grade, com um sistema de "intenção de movimento" que torna os controles mais responsivos.
* **Interface Completa:** Inclui menu principal, tela de game over, tela de vitória e créditos.
* **Sons e Músicas:** Efeitos sonoros e músicas temáticas para uma imersão completa.
* **Sprites Animados:** Animações para a personagem principal e para os inimigos.

---

## 🚀 Como Compilar e Executar

Para compilar e jogar, você precisa ter a biblioteca **SFML** instalada em seu sistema.

### **1. Dependências**

* **Compilador C++:** g++ ou outro compilador moderno.
* **Biblioteca SFML (>= 2.5):** Você pode baixá-la no [site oficial da SFML](https://www.sfml-dev.org/download.php).

### **2. Arquivos de Recurso (Assets)**

Certifique-se de que todos os arquivos de assets (`.png`, `.jpg`, `.mp3`, `.ttf`) mencionados no código estejam no mesmo diretório do executável. Os arquivos necessários são:
* **Sprites:** `spritesheet.png`, `spritesheet2.png`, `spritesheet3.png`, `robo*.png`, `heart.png`, `orb.png`, etc.
* **Texturas do Mapa/UI:** `fundo3.png`, `menu.png`, `copas2.png`, `horizontalcheio.png`, etc.
* **Áudio:** `menu.mp3`, `inicio.mp3`, `jogo.mp3`, `flawless.mp3`, `victory.mp3`, `defeat.mp3`.
* **Fonte:** `Valorant_Font.ttf`.

### **3. Compilação**

Abra um terminal na pasta raiz do projeto e execute o seguinte comando para compilar o código:

```bash
g++ gmvpacman.cpp -o OrbRush -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
```

### **4. Executando o Jogo**

Após a compilação bem-sucedida, um arquivo executável chamado `OrbRush` (ou `OrbRush.exe` no Windows) será criado. Execute-o pelo terminal:

```bash
./OrbRush
```
---

## 🎮 Como Jogar

* **Objetivo:** Coletar todos os "orbes" brancos espalhados pelo mapa para vencer.
* **Vidas:** Você começa com 3 vidas. Você perde uma vida se um robô inimigo te tocar.
* **Teletransporte:** Use as cordas nas laterais do mapa para se teletransportar de um lado para o outro.

### **Controles**

* **Setas Direcionais:** Movimentam a Jett pelo labirinto.
* **Tecla Enter:** Seleciona as opções nos menus.

---

## 🧑‍💻 Créditos

Este jogo foi desenvolvido por:

* **Gabriel Costa Reis** 
* **Marcos Vinicius Mariano Dias** 
* **Victor Alexandre S. Ribeiro** 


