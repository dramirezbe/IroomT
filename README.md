# Configuración KIOSK + Apache (Visualización de un sitio web en pantalla completa de manera local)

Configurar hora, ssh, instalar lo que se necesita etc...
```bash
sudo raspi-config
sudo apt update && sudo apt upgrade
sudo apt install apache2 curl cmake -y

sudo a2enmod proxy
sudo a2enmod proxy_http
sudo a2enmod proxy_wstunnel
sudo systemctl restart apache2

curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.40.1/install.sh | bash
\. "$HOME/.nvm/nvm.sh"
source ~/.bashrc
nvm install 22
```

```bash
git clone https://github.com/dramirezbe/GCPDS-MonRaF2-platform
cd GCPDS-MonRaF2-platform

cd frontend
npm install
npm run build
cd ..

cd backend
npm install
gcc server.c -o server.out -lm
```



Copiar el proyecto compilado a la carpeta de Apache:

```bash
sudo rm -rf /var/www/html/*
sudo cp -r dist/* /var/www/html/
sudo chown -R www-data:www-data /var/www/html

```

Configurar proxy inverso
```bash
sudo nano /etc/apache2/sites-available/kiosk.conf

<VirtualHost *:80>
    ServerName localhost
    DocumentRoot /var/www/html

    # Proxy inverso para WebSocket (socket.io)
    ProxyPass /socket.io ws://localhost:3001/socket.io retry=0
    ProxyPassReverse /socket.io ws://localhost:3001/socket.io

    # Proxy inverso para peticiones HTTP al servidor Node
    ProxyPass /api http://localhost:3001/api
    ProxyPassReverse /api http://localhost:3001/api

    # Si el servidor Node maneja otras rutas, puedes redirigirlas también:
    ProxyPass / http://localhost:3001/
    ProxyPassReverse / http://localhost:3001/
    
    <Directory /var/www/html>
        Options Indexes FollowSymLinks
        AllowOverride All
        Require all granted
    </Directory>

    ErrorLog ${APACHE_LOG_DIR}/kiosk_error.log
    CustomLog ${APACHE_LOG_DIR}/kiosk_access.log combined
</VirtualHost>

sudo a2ensite kiosk.conf
sudo systemctl reload apache2
</VirtualHost>

```


Crear archivo de servicio de systemd para C y Node
```bash
sudo nano /etc/systemd/system/node-server.service
```
```bash
[Unit]
Description=Servidor Node para el proxy inverso del kiosk
After=network.target

[Service]
ExecStart=/usr/bin/node /ruta/a/tu/proyecto/server.js
WorkingDirectory=/ruta/a/tu/proyecto
Restart=always
RestartSec=10
StandardOutput=syslog
StandardError=syslog
SyslogIdentifier=node-server
Environment=NODE_ENV=production

[Install]
WantedBy=multi-user.target
```
```bash
sudo systemctl daemon-reload
sudo systemctl enable node-server.service
sudo systemctl start node-server.service
```

```bash
sudo nano /etc/systemd/system/c-server.service
```
```bash
[Unit]
Description=Servidor C para la comunicación del kiosk
After=network.target

[Service]
ExecStart=/ruta/a/tu/proyecto/server.out
WorkingDirectory=/ruta/a/tu/proyecto
Restart=always
RestartSec=10
StandardOutput=syslog
StandardError=syslog
SyslogIdentifier=c-server

[Install]
WantedBy=multi-user.target
```
```bash
sudo systemctl daemon-reload
sudo systemctl enable c-server.service
sudo systemctl start c-server.service
```



Verificar estado
```bash
sudo systemctl status name_service.service
```
Configurar Firewall
```bash
sudo ufw allow 80/tcp
sudo ufw allow 4883/tcp
sudo ufw allow 5883/tcp
```

Reiniciar apache
```bash
sudo systemctl restart apache2
```

Acceder a la aplicación en el navegador:

```bash
http://localhost
```

Para hacer seguimiento a servicios
```bash
sudo journalctl -f -u name_service.service
```
Ver logs de apache
```bash
sudo tail -f /var/log/apache2/error.log /var/log/apache2/access.log
```



Generar ejecutable de C
```bash
mkdir build
cd build
cmake ..
make
```
