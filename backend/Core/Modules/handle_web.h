#ifndef HANDLE_WEB_H
#define HANDLE_WEB_H

// Inicia el entorno web: cambia de directorio subiendo tres niveles relativos al
// directorio donde se encuentra el ejecutable e inicia "npm start" en un proceso hijo.
// Retorna 0 en caso de éxito o un valor distinto de cero en caso de error.
int start_web(void);

// Detiene el proceso que ejecuta "npm start" iniciado por start_web().
// Retorna 0 en caso de éxito o un valor distinto de cero en caso de error.
int stop_web(void);

#endif // HANDLE_WEB_H
