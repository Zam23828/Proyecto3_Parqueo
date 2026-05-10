#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "Digital2";
const char* password = "digital2";

WebServer server(80);

uint8_t LED1pin = 2;

bool LED1status = LOW;
bool p1status = LOW;

void setup() {
  Serial.begin(115200);
  Serial.println("Try connecting to");
  Serial.println(ssid);

  pinMode(23,INPUT_PULLUP);
  pinMode(LED1pin,OUTPUT);

  WiFi.begin(ssid,password);

  while (WiFi.status() != WL_CONNECTED){
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected successfully");
  Serial.println("Direccion IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", handle_led1off);
  server.on("/actualizar",handle_actualizar);
  server.on("/led1on",handle_led1on);
  server.on("/led1off",handle_led1off);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");
  delay(100);
}

void loop() {
  server.handleClient();

  p1status = digitalRead(23);
  if (LED1status){
    digitalWrite(LED1pin, HIGH);
  }
  else{
    digitalWrite(LED1pin, LOW);
  }
}

// Handler Inicio de página
void handle_actualizar(){
  Serial.println("Actualizar pagina");
  Serial.println(p1status);
  server.send(200, "text/html", SendHTML2(p1status));
}

// Handler de Inicio de página

void handle_OnConnect(){
  LED1status = LOW;
  Serial.println("GPIO2 Status: OFF");
  server.send(200, "text/html",SendHTML2(LED1status));
}

// Handler de led1on
void handle_led1on(){
  LED1status = HIGH;
  Serial.println("GPIO2 Status: ON");
  server.send(200, "text/html",SendHTML2(LED1status));
}

// Handler de led1off
void handle_led1off(){
  LED1status = LOW;
  Serial.println("GPIO2 Status: OFF");
  server.send(200, "text/html",SendHTML2(LED1status));
}

void handle_NotFound(){
  server.send(404, "text/plain", "Error 404: Pagina no encontrada");
}

//Procesador de HTML

String SendHTML2(uint8_t p1stat){
  String pagina = "<!DOCTYPE html>\n";
  pagina += "<html>\n";
  pagina += "<head>\n";
  // Importamos fuentes y estilos
  pagina += "<link rel=preconnect href=https://fonts.googleapis.com>\n";
  pagina += "<link rel=preconnect href=https://fonts.gstatic.com crossorigin>\n";
  pagina += "<link href=\"https://fonts.googleapis.com/css2?family=Cinzel&family=Montserrat:wght@300;700&family=Playfair+Display:ital@0;1&display=swap\" rel=stylesheet>\n";
  
  pagina += "<style>\n";
  pagina += "body{background-color:#000034; transition:all .4s ease; display:flex; flex-direction:column; align-items:center; justify-content:center; min-height:100vh; margin:0; font-family:'Montserrat',sans-serif; color:white}\n";
  pagina += "h1{text-align:center; font-family:'Playfair Display',serif; font-weight:bold; margin-bottom:20px}\n";
  pagina += "button{padding:15px 30px; font-family:'Montserrat',sans-serif; font-weight:700; cursor:pointer; background-color:#fff; color:#000034; border:0; border-radius:8px; box-shadow:0 5px 0 #b1b1b1; transition:all .1s ease; position:relative; outline:0; margin-bottom:30px}\n";
  pagina += "button:active{box-shadow:0 2px 0 #b1b1b1; top:3px}\n";
  pagina += "table{width:80%; max-width:600px; border-collapse:collapse; background-color:rgba(255,255,255,0.05);}\n";
  pagina += "th, td{padding:15px; text-align:center; border-bottom:1px solid rgba(255,255,255,0.1)}\n";
  
  
  pagina += ".disponible{color: #2ecc71; font-weight: bold;}\n"; 
  pagina += ".ocupado{color: #e74c3c; font-weight: bold;}\n";
  
  pagina += "body.invertido{background-color:white; color:#000034}\n";
  pagina += "body.invertido h1{color:#000034}\n";
  pagina += "body.invertido button{background-color:#000034; color:white; box-shadow:0 5px 0 #000014}\n";
  pagina += "</style>\n";
  pagina += "</head>\n";

  pagina += "<body>\n";
  pagina += "<h1>Sistema de Parqueo</h1>\n";
  pagina += "<button onclick=intercambiarColores()>Cambiar Tema</button>\n";
  
  pagina += "<table>\n";
  pagina += "<thead><tr><th>Espacio</th><th>Estado</th></tr></thead>\n";
  pagina += "<tbody>\n";

  if (p1stat) {
    pagina += "<tr><td>Parqueo #1</td><td class='disponible'>DISPONIBLE &#128664;</td></tr>\n";
  } else {
    pagina += "<tr><td>Parqueo #1</td><td class='ocupado'>OCUPADO &#128683;</td></tr>\n";
  }

  pagina += "</tbody>\n";
  pagina += "</table>\n";

  pagina += "<script>function intercambiarColores(){document.body.classList.toggle('invertido')}</script>\n";
  pagina += "</body>\n";
  pagina += "</html>\n";

  return pagina;
}