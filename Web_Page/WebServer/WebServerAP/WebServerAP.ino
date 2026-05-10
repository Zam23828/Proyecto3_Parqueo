#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>

// --- CONFIGURACIÓN DEL WEB SERVER ---
const char* ssid = "NZ_ESP32";
const char* password = "Min_Percolator";

IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

WebServer server(80);

// --- CONFIGURACIÓN I2C ---
#define I2CSlaveAddress1 0x18 // Parqueos 1 al 4
#define I2CSlaveAddress2 0x27 // Parqueos 5 al 8

#define I2C_SDA 21
#define I2C_SCL 22

// Arreglo para almacenar el estado de los 8 parqueos (false = disponible, true = ocupado)
bool estadoParqueos[8] = {false, false, false, false, false, false, false, false};

// Temporizador no bloqueante para I2C
unsigned long tiempoAnterior = 0;
const long intervaloI2C = 1000; // Leer datos cada 1 segundo (1000 ms)

// Declaración de funciones
void i2cScanner();
void solicitarDatosI2C();
void handle_OnConnect();
void handle_NotFound();
String SendHTML();

void setup() {
  Serial.begin(115200);
  delay(1000); // Pequeña pausa para que el puerto serial inicie bien
  Serial.println("\n--- Iniciando ESP32 Parqueomatic ---");

  // Inicialización I2C
  Wire.begin(I2C_SDA, I2C_SCL);
  i2cScanner();

  // Inicialización Web Server (AP Mode)
  Serial.println("Configurando Punto de Acceso WiFi...");
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  
  // Rutas del Web Server
  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound); 

  server.begin();
  Serial.println("Servidor HTTP iniciado. Conéctate a la red 'NZ_ESP32' y abre 192.168.1.1");
}

void loop() {
  // 1. Manejar las peticiones de los clientes web continuamente
  server.handleClient();

  // 2. Manejar la comunicación I2C de forma no bloqueante
  unsigned long tiempoActual = millis();
  if (tiempoActual - tiempoAnterior >= intervaloI2C) {
    tiempoAnterior = tiempoActual;
    solicitarDatosI2C();
  }
}

// --- FUNCIONES I2C ---

void solicitarDatosI2C() {
  uint8_t comando = 'S'; 
  uint8_t error;
  
  Serial.println("\n--- Lectura I2C ---");

  // --- LECTURA DEL ESCLAVO 1 (0x18) - Parqueos 1 al 4 ---
  Serial.print("Buscando Esclavo 1 (0x18)... ");
  Wire.beginTransmission(I2CSlaveAddress1);
  Wire.write(comando);
  error = Wire.endTransmission(true);
  
  if (error == 0) {
    Serial.println("OK");
    uint8_t bytesReceived1 = Wire.requestFrom((uint16_t)I2CSlaveAddress1, (uint8_t)4);
    if (bytesReceived1 == 4) {
      for (int i = 0; i < 4; i++) {
        uint8_t valor = Wire.read();
        estadoParqueos[i] = (valor > 0); 
        Serial.print("  Parqueo "); Serial.print(i + 1);
        Serial.print(": "); Serial.println(valor == 0 ? "Libre" : "Ocupado");
      }
    } else {
      Serial.println("  Error: No se recibieron 4 bytes.");
      while(Wire.available()) Wire.read(); // Limpiar buffer
    }
  } else {
    Serial.print("Fallo (Error "); Serial.print(error); Serial.println(")");
  }

  // --- LECTURA DEL ESCLAVO 2 (0x27) - Parqueos 5 al 8 ---
  // (Nota: Si aún no conectas el segundo esclavo, este bloque tirará error, pero no afectará al código)
  Serial.print("Buscando Esclavo 2 (0x27)... ");
  Wire.beginTransmission(I2CSlaveAddress2);
  Wire.write(comando);
  error = Wire.endTransmission(true);
  
  if (error == 0) {
    Serial.println("OK");
    uint8_t bytesReceived2 = Wire.requestFrom((uint16_t)I2CSlaveAddress2, (uint8_t)4);
    if (bytesReceived2 == 4) {
      for (int i = 0; i < 4; i++) {
        uint8_t valor = Wire.read();
        estadoParqueos[i + 4] = (valor > 0); 
        Serial.print("  Parqueo "); Serial.print(i + 5);
        Serial.print(": "); Serial.println(valor == 0 ? "Libre" : "Ocupado");
      }
    } else {
      Serial.println("  Error: No se recibieron 4 bytes.");
      while(Wire.available()) Wire.read(); // Limpiar buffer
    }
  } else {
    Serial.print("Fallo (Error "); Serial.print(error); Serial.println(")");
  }
}

void i2cScanner() {
  byte error, address;
  int nDevices = 0; 
  Serial.println("Escaneando Bus I2C...");
  
  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission(); 
    
    if (error == 0) {
      Serial.print("Dispositivo I2C encontrado en dirección 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
      nDevices++; 
    }
  }
  
  if (nDevices == 0) {
    Serial.println("No se encontraron dispositivos I2C. Revisa cables, GND y resistencias Pull-up.\n"); 
  } else {
    Serial.println("Escaneo I2C completado.\n"); 
  }
}

// --- FUNCIONES DEL WEB SERVER ---

void handle_OnConnect() {
  // Al conectarse, se envía el HTML construido con los datos actuales del arreglo
  server.send(200, "text/html", SendHTML()); 
}

void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}

String SendHTML() {
  String pagina = "<!DOCTYPE html>\n";
  pagina += "<html>\n";
  pagina += "<head>\n";
  pagina += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
  pagina += "<link rel=preconnect href=https://fonts.googleapis.com>\n";
  pagina += "<link rel=preconnect href=https://fonts.gstatic.com crossorigin>\n";
  pagina += "<link href=\"https://fonts.googleapis.com/css2?family=Cinzel&family=Montserrat:wght@300;700&family=Playfair+Display:ital@0;1&display=swap\" rel=stylesheet>\n";
  
  pagina += "<style>\n";
  pagina += "body{background-color:#000034; transition:all .4s ease; display:flex; flex-direction:column; align-items:center; justify-content:center; min-height:100vh; margin:0; font-family:'Montserrat',sans-serif; color:white; padding: 20px 0;}\n";
  pagina += "h1{text-align:center; font-family:'Playfair Display',serif; font-weight:bold; margin-bottom:20px}\n";
  pagina += "button{padding:15px 30px; font-family:'Montserrat',sans-serif; font-weight:700; cursor:pointer; background-color:#fff; color:#000034; border:0; border-radius:8px; box-shadow:0 5px 0 #b1b1b1; transition:all .1s ease; position:relative; outline:0; margin-bottom:30px; margin-top:10px;}\n";
  pagina += "button:active{box-shadow:0 2px 0 #b1b1b1; top:3px}\n";
  pagina += "table{width:90%; max-width:600px; border-collapse:collapse; background-color:rgba(255,255,255,0.05);}\n";
  pagina += "th, td{padding:15px; text-align:center; border-bottom:1px solid rgba(255,255,255,0.1)}\n";
  pagina += ".disponible{color: #2ecc71; font-weight: bold;}\n"; 
  pagina += ".ocupado{color: #e74c3c; font-weight: bold;}\n";
  pagina += "body.invertido{background-color:white; color:#000034}\n";
  pagina += "body.invertido h1{color:#000034}\n";
  pagina += "body.invertido button{background-color:#000034; color:white; box-shadow:0 5px 0 #000014}\n";
  pagina += "</style>\n";
  
  // Script para recargar la página automáticamente cada 2 segundos y mantener el tema
  pagina += "<script>\n";
  pagina += "setInterval(function() {\n";
  pagina += "  location.reload();\n";
  pagina += "}, 2000);\n";
  pagina += "function intercambiarColores(){\n";
  pagina += "  document.body.classList.toggle('invertido');\n";
  pagina += "  localStorage.setItem('tema', document.body.classList.contains('invertido') ? 'claro' : 'oscuro');\n";
  pagina += "}\n";
  pagina += "window.onload = function() {\n";
  pagina += "  if(localStorage.getItem('tema') === 'claro') {\n";
  pagina += "    document.body.classList.add('invertido');\n";
  pagina += "  }\n";
  pagina += "}\n";
  pagina += "</script>\n";
  pagina += "</head>\n";

  pagina += "<body>\n";
  pagina += "<h1>Parqueomatic</h1>\n";
  pagina += "<button onclick=intercambiarColores()>Cambiar Tema</button>\n";
  
  pagina += "<table>\n";
  pagina += "<thead><tr><th>Espacio</th><th>Estado</th></tr></thead>\n";
  pagina += "<tbody>\n";

  // Generación dinámica de las 8 filas de la tabla leyendo el arreglo
  for (int i = 0; i < 8; i++) {
    pagina += "<tr><td>Parqueo #" + String(i + 1) + "</td>";
    if (estadoParqueos[i]) {
      pagina += "<td class='ocupado'>OCUPADO &#128683;</td></tr>\n";
    } else {
      pagina += "<td class='disponible'>DISPONIBLE &#128664;</td></tr>\n";
    }
  }

  pagina += "</tbody>\n";
  pagina += "</table>\n";
  pagina += "</body>\n";
  pagina += "</html>\n";

  return pagina;
}