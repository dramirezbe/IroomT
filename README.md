# Configuración KIOSK + PM2 con express (Visualización de un sitio web en pantalla completa de manera local)

Configurar hora, ssh, instalar lo que se necesita etc...
```bash
sudo raspi-config
sudo apt update && sudo apt upgrade
sudo apt install curl cmake chromium-browser git -y

curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.40.1/install.sh | bash

\. "$HOME/.nvm/nvm.sh"

nvm install 22.13.1
```

## Configurar Kiosk:

```bash
chmod +x start-kiosk.sh
```
## Inicializar web:

```bash
git clone https://github.com/dramirezbe/GCPDS-MonRaF2-platform
cd GCPDS-MonRaF2-platform

cd frontend
npm install
npm run build
cd ..
cd backend
npm install
cd ..
npm install
npm install -g pm2
./build-core.sh
pm2 start ecosystem.config.js
pm2 startup
pm2 save

```

Acceder a la aplicación en el navegador:

```javascript
http://localhost:3001
```

