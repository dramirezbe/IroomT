# Configuración KIOSK + Apache (Visualización de un sitio web en pantalla completa de manera local)

Después de bajar el repositorio, se debe compilar el proyecto de React:

```bash
npm install
npm run build
```
En la carpeta dist se genera el proyecto compilado.

Instalar Apache:

```bash
sudo apt update
sudo apt install apache2
```

Verificar funcionamiento de Apache:

```bash
sudo systemctl status apache2
```
Si no está corriendo, iniciar Apache:

```bash
sudo systemctl start apache2
```

Copiar el proyecto compilado a la carpeta de Apache:

```bash
sudo rm -rf /var/www/html/*
sudo cp -r dist/* /var/www/html/
```

Modificar permisos de la carpeta de Apache:

```bash
sudo chown -R www-data:www-data /var/www/html/
sudo chmod -R 755 /var/www/html/
```

Acceder a la aplicación en el navegador:

```bash
http://localhost
```
