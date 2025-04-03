#!/usr/bin/env bash
set -e

./build.sh

echo "Avvio dell'eseguibile engine_app..."
./build/engine_app
