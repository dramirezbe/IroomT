module.exports = {
    apps: [
      {
        name: "gcpds-monraf2-platform",
        script: "npm",
        args: "start",
        // Usa la ruta actual o especifica la absoluta según tu entorno
        cwd: process.cwd(),
        env: {
          NODE_ENV: "production",
          PORT: 3001  // Asegúrate de que tu backend use este puerto
        }
      },
      {
        name: "kiosk",
        // Ajusta la ruta al ejecutable de Chromium si es necesario (en Ubuntu suele ser chromium-browser)
        script: "chromium-browser",
        args: "--kiosk http://localhost:3001",
        env: {}
      }
    ]
  };
  