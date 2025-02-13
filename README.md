# Configuración KIOSK + Apache (Visualización de un sitio web en pantalla completa de manera local)

Después de bajar el repositorio, se debe compilar el proyecto de React:

```bash
cd frontend
npm install
npm run build
cd ..
cd backend
npm install
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
sudo mv /ruta/de/tu/dist /var/www/html/tu-app
```

Configurar tu-app.conf
```bash
sudo nano /etc/apache2/sites-available/tu-app.conf
```

```html
<VirtualHost *:80>
    ServerName http://localhost
    ServerAdmin webmaster@localhost
    DocumentRoot /var/www/html/tu-app

    <Directory /var/www/html/tu-app>
        Options -Indexes +FollowSymLinks
        AllowOverride All
        Require all granted
    </Directory>

    # Proxy para API HTTP
    ProxyPreserveHost On
    ProxyPass /api http://localhost:5883/
    ProxyPassReverse /api http://localhost:5883/

    # Proxy para WebSockets MQTT
    ProxyPass /mqtt ws://localhost:4883/mqtt
    ProxyPassReverse /mqtt ws://localhost:4883/mqtt

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

Configurar PM2 para el backend

```bash
cd /ruta/al/backend
npm install pm2 -g
pm2 start server.js --name "mqtt-backend" --env .env
pm2 save
pm2 startup
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

