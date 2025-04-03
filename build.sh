#!/usr/bin/env bash
set -e

# Imposta il compilatore e le opzioni di compilazione
CXX=g++
CXXFLAGS="-std=c++17 -O2 -DVK_USE_PLATFORM_XLIB_KHR \
-Iengine/window \
-Iengine/core \
-Iengine/graphics \
-Iengine/input \
-Iengine/app"
LDFLAGS="-lvulkan -lX11"

# Se la variabile d'ambiente VULKAN_SDK è settata, 
# allora usiamo i suoi path personalizzati (include, lib).
if [ -n "$VULKAN_SDK" ]; then
    CXXFLAGS="$CXXFLAGS -I$VULKAN_SDK/include"
    LDFLAGS="$LDFLAGS -L$VULKAN_SDK/lib -ldl -lpthread"
fi

# Cartella build per gli oggetti .o e l'eseguibile
mkdir -p build

# 1) COMPILAZIONE DEGLI SHADER
# Assicurati di avere installato glslc (pacchetto: glslang-tools o vulkan-sdk)
SHADER_DIR="engine/shaders"
echo "Compilazione degli shader nella cartella '$SHADER_DIR'..."

# Lista dei file sorgenti shader – personalizza se hai nomi diversi
SHADERS=(
    "torus.vert"
    "torus.frag"
)

for SHADER_FILE in "${SHADERS[@]}"; do
    SRC_PATH="$SHADER_DIR/$SHADER_FILE"
    # Estrazione estensione (.vert, .frag)
    EXT="${SHADER_FILE##*.}"
    # Creiamo il file di output .spv
    OUT_PATH="$SHADER_DIR/$SHADER_FILE.spv"

    echo " > Compilo $SRC_PATH -> $OUT_PATH"
    glslc "$SRC_PATH" -o "$OUT_PATH"
done

# 2) COMPILAZIONE C++
echo "Compilazione del codice C++..."

# Trova tutti i file .cpp nelle cartelle engine/ e main.cpp
SRCS=$(find engine -type f -name "*.cpp")
SRCS+=" main.cpp"

OBJFILES=()
for SRC in $SRCS; do
    OBJ="build/$(basename "$SRC" .cpp).o"
    echo " > Compilando $SRC -> $OBJ"
    $CXX $CXXFLAGS -c "$SRC" -o "$OBJ"
    OBJFILES+=("$OBJ")
done

# 3) Link
echo "Link dell'eseguibile..."
$CXX "${OBJFILES[@]}" $LDFLAGS -o build/engine_app

echo "Build completata con successo in build/engine_app"
