#!/usr/bin/env bash

# Crea le cartelle
mkdir -p engine/window
mkdir -p engine/core
mkdir -p engine/graphics
mkdir -p engine/input
mkdir -p engine/app

# Creazione file in engine/window
touch engine/window/WindowX11.h
touch engine/window/WindowX11.cpp

# Creazione file in engine/core
touch engine/core/VulkanContext.h
touch engine/core/VulkanContext.cpp
touch engine/core/Swapchain.h
touch engine/core/Swapchain.cpp
touch engine/core/RenderPass.h
touch engine/core/RenderPass.cpp
touch engine/core/DepthResources.h
touch engine/core/DepthResources.cpp
touch engine/core/Pipeline.h
touch engine/core/Pipeline.cpp
touch engine/core/BufferHelper.h
touch engine/core/BufferHelper.cpp

# Creazione file in engine/graphics
touch engine/graphics/Mesh.h
touch engine/graphics/Mesh.cpp

# Creazione file in engine/input
touch engine/input/InputHandler.h
touch engine/input/InputHandler.cpp

# Creazione file in engine/app
touch engine/app/Application.h
touch engine/app/TorusApp.h
touch engine/app/TorusApp.cpp

# File principale
touch main.cpp

echo "Struttura cartelle e file vuoti creata con successo!"
