#!/bin/bash
# Para verificar a porta.

# ls /dev/ttyACM*

PORTA=$(ls /dev/ttyACM* | head -n 1)

echo "Porta detectada: $PORTA"


avrdude -F -V -c arduino -p m328p -P "$PORTA" -b 115200 -U flash:w:robo.ino.standard.hex:i

if [ $? -eq 0 ]; then
    echo "✅ Upload concluído com sucesso!"
else
    echo "❌ Falha no upload. Verifique as conexões."
fi