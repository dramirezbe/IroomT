# Configuración KIOSK con express (Visualización de un sitio web en pantalla completa de manera local)

## Actualizar e Instalar:
```bash
sudo apt update && sudo apt upgrade
sudo apt install --no-install-recommends curl cmake chromium-browser git xserver-xorg x11-xserver-utils xinit openbox -y
```

Instalar node
```bash
curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.40.1/install.sh | bash

\. "$HOME/.nvm/nvm.sh"

nvm install 22.13.1
```

## Inicializar web:
```bash
git clone https://github.com/dramirezbe/GCPDS-MonRaF2-platform.git
cd GCPDS-MonRaF2-platform
npm install
cd frontend
npm run build
cd ..
chmod +x build-core.sh
./build-core.sh
```


## Configurar Kiosk:

Configurar xorg (Kiosk):
```bash
sudo nano /etc/xdg/openbox/autostart
```

```bash
# Disable any form of screen saver / screen blanking / power management
xset s off
xset s noblank
xset -dpms
# Allow quitting the X server with CTRL-ALT-Backspace
setxkbmap -option terminate:ctrl_alt_bksp
# Start Chromium in kiosk mode
sed -i 's/"exited_cleanly":false/"exited_cleanly":true/' ~/.config/chromium/'Local State'
sed -i 's/"exited_cleanly":false/"exited_cleanly":true/; s/"exit_type":"[^"]\+"/"exit_type":"Normal"/' ~/.config/chromium/Default/Preferences
chromium-browser --disable-infobars --kiosk 'http://localhost:3001'
```
Configurar chromium
```bash
sudo nano /home/youruserhere/.bash_profile

```
```bash
[[ -z $DISPLAY && $XDG_VTNR -eq 1 ]] && startx -- -nocursor
```
```bash
sudo reboot now
```

## Configurar .service (TCP server + SDR preprocesado):

```bash
sudo nano /etc/systemd/system/monraf_core.service
```

```bash
[Unit]
Description=Servicio de monraf_core
After=network.target

[Service]
# Ejecuta el binario de tu aplicación
ExecStart=/home/youruserhere/GCPDS-MonRaF2-platform/backend/Core/build/monraf_core
# Reinicia el servicio si se cae
Restart=always
# Tiempo de espera antes de reiniciar (en segundos)
RestartSec=10
# Redirige la salida estándar y de error al journal (útil para ver logs)
StandardOutput=journal
StandardError=journal

[Install]
WantedBy=multi-user.target
```
reiniciar systemctl
```bash
sudo systemctl daemon-reload
```
Habilitar .service en arranque
```bash
sudo systemctl enable monraf_core.service
```
Para ver los logs
```bash
sudo systemctl status monraf_core.service
sudo journalctl -u monraf_core.service -f
```

Acceder a la aplicación en el navegador:
```javascript
http://localhost:3001
```