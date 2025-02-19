# Configuración KIOSK + Apache (Visualización de un sitio web en pantalla completa de manera local)

Después de bajar el repositorio, se debe compilar el proyecto de React:

```bash
cd frontend
npm install
npm run build
cd ..
cd backend
npm install
gcc -o server server.c
```

Instalar Apache:

```bash
sudo apt update
sudo apt install apache2
```

Verificar funcionamiento de Apache:

```bash
sudo systemctl status apache2
sudo systemctl start apache2

```

Copiar el proyecto compilado a la carpeta de Apache:

```bash
sudo rm -rf /var/www/html/*
sudo cp -r dist/* /var/www/html/
```

Configurar proxy inverso
```bash
sudo nano /etc/apache2/sites-available/000-default.conf
```

```html
<VirtualHost *:80>
    ServerAdmin webmaster@localhost
    DocumentRoot /var/www/html

    ProxyPreserveHost On
    ProxyPass / http://localhost:3000/
    ProxyPassReverse / http://localhost:3000/

    ErrorLog ${APACHE_LOG_DIR}/error.log
    CustomLog ${APACHE_LOG_DIR}/access.log combined
</VirtualHost>
```


Modificar permisos de la carpeta de Apache:

```bash
sudo chown -R www-data:www-data /var/www/html/
sudo chmod -R 755 /var/www/html/
```

Habilitar módulos apache:

```bash
sudo a2enmod proxy proxy_http proxy_wstunnel
sudo systemctl restart apache2
```

Crear archivo de servicio para systemd

```bash
sudo nano /etc/systemd/system/server_c.service
```
Agregar contenido
```bash
[Unit]
Description=Server en C
After=network.target

[Service]
ExecStart=/home/pi/GCPDS-MonRaF-Platform/backend/server
Restart=always
User=pi
Environment=NODE_ENV=production

[Install]
WantedBy=multi-user.target
```
Habilita y arranca el servicio
```bash
sudo systemctl enable server_c.service
sudo systemctl start server_c.service
```

```bash
sudo nano /etc/systemd/system/server_node.service
```
Agregar contenido
```bash
[Unit]
Description=Server en Node
After=network.target

[Service]
ExecStart=/usr/bin/node /home/pi/GCPDS-MonRaF2-Platform/backend/server.js
Restart=always
User=pi
Environment=NODE_ENV=production

[Install]
WantedBy=multi-user.target
```
Habilita y arranca el servicio
```bash
sudo systemctl enable server_node.service
sudo systemctl start server_node.service
```

Configurar Firewall
```bash
sudo ufw allow 80/tcp
sudo ufw allow 4883/tcp
sudo ufw allow 5883/tcp
```

Comandos útiles PM2:
```bash
pm2 restart mqtt-backend
pm2 logs mqtt-backend
pm2 monit
```

Acceder a la aplicación en el navegador:

```bash
http://localhost
```
Compilar programa en c
```bash
gcc program_name.c -o executable_name -lm
```

Usar mqtt para conexión back-front
```bash
sudo apt install mosquitto
```

