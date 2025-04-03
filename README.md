# VulkanTorus

WindowX11 window(...)
👉 Crea una finestra usando X11 (il sistema grafico di Linux). È come accendere un monitor vuoto pronto per mostrare qualcosa.

TorusApp app(&window);
👉 Crea l’applicazione vera e propria, dandole la finestra dove dovrà disegnare. TorusApp è il motore del progetto.

app.init();
👉 Inizializza tutto: Vulkan, swapchain (buffer per i frame), pipeline grafica, mesh del toroide… tutta la baracca.

while (!window.shouldClose())
👉 Loop principale: il programma gira finché l’utente non chiude la finestra.

app.update();
👉 Calcola eventuali cambiamenti (movimento, rotazione, input…).

app.render();
👉 Chiama Vulkan per disegnare un nuovo frame del toroide sullo schermo.

app.cleanup();
👉 Quando chiudi la finestra, libera tutta la memoria occupata (distrugge pipeline, buffer, ecc.).

🧪 Sotto il cofano: cosa fa Vulkan?
Vulkan è una API a basso livello. Tradotto: ti dà il controllo totale su ogni parte del rendering, ma ti devi fare tutto tu.

📦 Durante app.init():
Creazione dell’istanza Vulkan

Scelta del dispositivo GPU

Creazione dello swapchain → un array di immagini da disegnare sullo schermo.

Creazione della pipeline grafica → tutto ciò che serve per disegnare (shaders, configurazioni, layout...).

Buffer dei vertici → definisce la forma geometrica del toroide.

Allocazione memoria → tutta la gestione della VRAM è manuale in Vulkan.

🔁 Durante app.render():
Prende un’immagine dallo swapchain

Costruisce i comandi da eseguire sulla GPU

Manda i comandi alla GPU

Dice alla GPU: “Fai partire il rendering!”

Mostra il frame renderizzato sullo schermo

🧹 Durante app.cleanup():
Distrugge tutti gli oggetti creati (pipeline, swapchain, device, shader module, semafori, ecc.)

Libera la memoria Vulkan

Chiude la finestra