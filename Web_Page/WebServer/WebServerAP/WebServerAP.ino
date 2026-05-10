#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "NZ_ESP32";
const char* password = "Min_Percolator";

IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

WebServer server (80);

uint8_t LED1pin = 2;
bool LED1status = LOW;

uint8_t LED2pin = 5;
bool LED2status = LOW;

void setup() {
  Serial.begin(115200);
  pinMode(LED1pin, OUTPUT);
  pinMode(LED2pin, OUTPUT);

  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);

  server.on("/",handle_OnConnect);
  server.on("/led1on",handle_led1on);
  server.on("/led1off",handle_led1off);
  server.on("/led2on",handle_led2on);
  server.on("/led2off",handle_led2off);
  server.onNotFound(handle_NotFound); 

  server.begin();
  Serial.println("HTTP server started");
}

void loop(){
  server.handleClient();
  if(LED1status)
  {digitalWrite(LED1pin,HIGH);}
  else
  {digitalWrite(LED1pin,LOW);}
  if(LED2status)
  {digitalWrite(LED2pin,HIGH);}
  else
  {digitalWrite(LED2pin,LOW);}
}

void handle_OnConnect() {
  LED1status = LOW;
  LED2status = LOW;
  Serial.println("GPIO4 Status: OFF | GPIO5 Status: OFF");
  server.send(200, "text/html", SendHTML(LED1status,LED2status)); 
}

void handle_led1on() {
  LED1status = HIGH;
  Serial.println("GPIO4 Status: ON");
  server.send(200, "text/html", SendHTML(true,LED2status)); 
}

void handle_led1off() {
  LED1status = LOW;
  Serial.println("GPIO4 Status: OFF");
  server.send(200, "text/html", SendHTML(false,LED2status)); 
}

void handle_led2on() {
  LED2status = HIGH;
  Serial.println("GPIO5 Status: ON");
  server.send(200, "text/html", SendHTML(LED1status,true)); 
}

void handle_led2off() {
  LED2status = LOW;
  Serial.println("GPIO5 Status: OFF");
  server.send(200, "text/html", SendHTML(LED1status,false)); 
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}
/*
String SendHTML(uint8_t led1stat,uint8_t led2stat){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>LED Control</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr +=".button {display: block;width: 80px;background-color: #3498db;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr +=".button-on {background-color: #3498db;}\n";
  ptr +=".button-on:active {background-color: #2980b9;}\n";
  ptr +=".button-off {background-color: #34495e;}\n";
  ptr +=".button-off:active {background-color: #2c3e50;}\n";
  ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<h1>ESP32 Web Server</h1>\n";
  ptr +="<h3>Using Access Point(AP) Mode</h3>\n";
  
   if(led1stat)
  {ptr +="<p>LED1 Status: ON</p><a class=\"button button-off\" href=\"/led1off\">OFF</a>\n";}
  else
  {ptr +="<p>LED1 Status: OFF</p><a class=\"button button-on\" href=\"/led1on\">ON</a>\n";}

  if(led2stat)
  {ptr +="<p>LED2 Status: ON</p><a class=\"button button-off\" href=\"/led2off\">OFF</a>\n";}
  else
  {ptr +="<p>LED2 Status: OFF</p><a class=\"button button-on\" href=\"/led2on\">ON</a>\n";}

  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}*/

String SendHTML(uint8_t led1stat,uint8_t led2stat){
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

  if (led1stat) {
    pagina += "<tr><td>Parqueo #1</td><td class='ocupado'>OCUPADO &#128683;</td></tr>\n";
    pagina += "<td><button class='btn-tabla' onclick=\"window.location.href='/led1off'\">Ocupar</button></td></tr>\n";
  } else {
    pagina += "<tr><td>Parqueo #1</td><td class='disponible'>DISPONIBLE &#128664;</td></tr>\n";
    pagina += "<td><button class='btn-tabla' onclick=\"window.location.href='/led1on'\">Liberar</button></td></tr>\n";
    
  }

  pagina += "</tbody>\n";
  pagina += "</table>\n";

  pagina += "<script>function intercambiarColores(){document.body.classList.toggle('invertido')}</script>\n";
  pagina += "</body>\n";
  pagina += "</html>\n";

  return pagina;
}