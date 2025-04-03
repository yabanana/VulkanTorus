# VulkanTorus

# VulkanTorus – Documentazione Tecnica

## Indice

- [1. Introduzione](#1-introduzione)
- [2. Requisiti di Sistema](#2-requisiti-di-sistema)
- [3. Architettura del Progetto](#3-architettura-del-progetto)
- [4. Flusso di Esecuzione](#4-flusso-di-esecuzione)
- [5. Vulkan - Componenti Chiave](#5-vulkan---componenti-chiave)
- [6. TorusApp - Struttura e Funzionalità](#6-torusapp---struttura-e-funzionalità)
- [7. WindowX11](#7-windowx11)
- [8. Diagramma del Flusso](#8-diagramma-del-flusso)
- [9. Setup e Utilizzo](#9-setup-e-utilizzo)
- [10. Cleanup e Best Practices](#10-cleanup-e-best-practices)

---

## 1. Introduzione

**VulkanTorus** è una demo grafica sviluppata in C++ che utilizza l'API Vulkan per renderizzare un toroide in tempo reale in ambiente X11 (Linux). L'obiettivo del progetto è fornire un motore grafico generico e modulare basato su Vulkan.

---

## 2. Requisiti di Sistema

- Sistema operativo: Linux con supporto X11
- Vulkan SDK installato (consigliato LunarG)
- Compilatore C++ (es. `g++` con supporto C++17 o superiore)
- `CMake` per la generazione dei file di build

---

## 3. Architettura del Progetto

```
VulkanTorus/
├── engine/                # Codice dell'engine grafico
├── build/                 # Directory di build (da ignorare)
├── shaders/               # Shader GLSL compilati in SPIR-V
├── main.cpp               # Entry point del programma
├── build.sh               # Script per compilazione
├── run.sh                 # Script per esecuzione
├── setup_project.sh       # Setup iniziale progetto
```

Moduli principali:
- `TorusApp`: applicazione principale che gestisce Vulkan e il rendering.
- `WindowX11`: wrapper minimale per la creazione e gestione della finestra con Xlib.

---

## 4. Flusso di Esecuzione

```cpp
int main() {
    WindowX11 window(1600, 1200, "Generic Vulkan Engine");
    TorusApp app(&window);

    try {
        app.init();                        // Inizializzazione engine
        while (!window.shouldClose()) {
            app.update();                 // Logica di aggiornamento
            app.render();                 // Rendering frame
        }
        app.cleanup();                    // Pulizia risorse
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
```

### Descrizione:
1. Crea la finestra con `WindowX11`
2. Inizializza Vulkan e l'applicazione (`TorusApp::init()`)
3. Loop principale: `update()` e `render()`
4. Cleanup finale con `cleanup()`

---

## 5. Vulkan - Componenti Chiave

### Istanza Vulkan
- Punto d'ingresso di tutte le chiamate Vulkan
- Contiene info su versione, estensioni, e validazione

### Dispositivo Fisico (GPU)
- Selezionato tra quelli disponibili
- Deve supportare le queue per grafica e presentazione

### Dispositivo Logico
- Permette l'interazione con la GPU

### Swapchain
- Serie di immagini/framebuffer per presentare a schermo

### Pipeline Grafica
- Include:
  - Vertex e fragment shader
  - Layout di input/output
  - Configurazione rasterizer, blending, viewport

### Buffer e Memoria
- Vertex buffer contenente geometria del toroide
- Memoria allocata tramite `vkAllocateMemory`

---

## 6. TorusApp - Struttura e Funzionalità

### init()
- Crea istanza Vulkan
- Inizializza dispositivi e swapchain
- Costruisce la pipeline grafica
- Alloca e carica i buffer dei vertici del toroide

### update()
- Gestione logica e trasformazioni (es. rotazione toroide)

### render()
- Acquisizione immagine dal swapchain
- Costruzione comando buffer
- Rendering del toroide con draw call Vulkan

### cleanup()
- Distruzione ordinata di tutte le risorse Vulkan
- Deallocazione memoria
- Distruzione della finestra

---

## 7. WindowX11

### Ruolo
- Abstrae la creazione di una finestra con Xlib
- Gestisce gli eventi di chiusura

### Funzioni principali:
- `shouldClose()` → ritorna true se l'utente ha chiuso la finestra
- `getNativeHandle()` → ritorna il riferimento alla finestra (necessario per Vulkan)

---

## 8. Diagramma del Flusso

```mermaid
graph TD
    A[Inizio Programma] --> B[WindowX11: crea finestra X11]
    B --> C[TorusApp: init() - setup Vulkan]
    C --> D{Loop}
    D -->|true| E[update() - logica e input]
    E --> F[render() - draw frame con Vulkan]
    F --> D
    D -->|false| G[cleanup() - dealloca risorse]
    G --> H[Fine]
```

---

## 9. Setup e Utilizzo

### Clonare il progetto
```bash
git clone https://github.com/yabanana/VulkanTorus.git
cd VulkanTorus
```

### Setup iniziale
```bash
./setup_project.sh
```

### Compilazione
```bash
./build.sh
```

### Esecuzione
```bash
./run.sh
```

---

## 10. Cleanup e Best Practices

- **Distruggere swapchain prima del device**
- **Liberare buffer e memoria associata**
- **Controllare ritorni di errore di tutte le chiamate Vulkan**
- **Usare validation layer in fase di debug**

---

> **Nota finale**: Questo progetto è un ottimo punto di partenza per capire Vulkan. Tuttavia, date le sue complessità, è consigliato avere già esperienza con grafica 3D e un minimo di conoscenza dei driver GPU e della gestione esplicita della memoria.