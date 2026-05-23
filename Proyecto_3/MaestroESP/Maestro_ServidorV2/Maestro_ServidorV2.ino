

#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>

// ── CONFIGURACIÓN ACCESS POINT ───────────────────────────────
const char* ssid     = "ESP32";   // Nombre de la red WiFi
const char* password = "12345678";      // Contraseña (mín. 8 caracteres)

IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway  (192, 168, 1, 1);
IPAddress subnet   (255, 255, 255, 0);

// ── CONFIGURACIÓN I2C ────────────────────────────────────────
#define I2C_SDA        21          // Pin SDA del ESP32
#define I2C_SCL        22          // Pin SCL del ESP32
#define ADDR_STM32_A   0x18        // Dirección I2C del STM32 Grupo A
#define ADDR_STM32_B   0x20        // Dirección I2C del STM32 Grupo B
#define I2C_MSG_LEN    4           // Longitud del mensaje

// ── ABRIR PUERTO ─────────────────────────────────────────────
WebServer server(80);

// ====== Variables utiles ==============
int flag_parqueo = 0;


// ═══════════════════════════════════════════════════════════════
//  ESTADO DE LOS 8 PARQUEOS
//  parkState[0..3] = Grupo A (STM32 #1), posiciones 0-3
//  parkState[4..7] = Grupo B (STM32 #2), posiciones 0-3
//  false = libre | true = ocupado
// ═══════════════════════════════════════════════════════════════
bool parkState[8] = { false, false, false, false,   // Grupo A: A-1, A-2, A-3, A-4
                      false, false, false, false };  // Grupo B: B-1, B-2, B-3, B-4

// ═══════════════════════════════════════════════════════════════
//  SETUP
// ═══════════════════════════════════════════════════════════════
void setup() {
  Serial.begin(115200);
  Serial.println("\n== ParkControl arrancando ==");

  // Iniciar I2C como Maestro 
  Wire.begin(I2C_SDA, I2C_SCL);
  Serial.println("I2C maestro iniciado");

  // Verifcar conexion en I2C
  i2cScanner();

  // ── Iniciar Access Point ──
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
  Serial.print("Access Point activo: ");
  Serial.println(ssid);
  Serial.print("IP: ");
  Serial.println(WiFi.softAPIP());

  // ── Rutas del servidor web ──
  server.on("/", handle_root);
  server.onNotFound(handle_notFound);
  server.on("/estado", handle_estado);
  server.begin();
  Serial.println("Servidor HTTP iniciado en puerto 80");

  

}

// ═══════════════════════════════════════════════════════════════
//  LOOP PRINCIPAL
// ═══════════════════════════════════════════════════════════════
void loop() {
  server.handleClient();   // Atender peticiones web
  leerI2C();               // Leer datos de los STM32
  // Debug cada 2 segundos
  escribirEstadoAToB();
  //------------------ solo voy a probar mandarle ocupado al stm para probar pantalla
  /*
  Wire.beginTransmission(ADDR_STM32_B);
  Wire.write('o');
  Wire.write('l');
  Wire.write('o');
  Wire.write('o');
  Wire.endTransmission();
  */
  static unsigned long lastDebug = 0;
  if (millis() - lastDebug >= 2000) {
    lastDebug = millis();
    debugI2C();
  }
  // ------- aca voy a calcular cuantos libres hay y se los mando al stm de mat para desplegar en el display
  uint8_t libres = 0;
  for (int i = 0; i < 8; i++) {
      if (!parkState[i]) libres++;
  }
  // envio cantidad de parqueos disponibles
  Wire.beginTransmission(ADDR_STM32_A);
  Wire.write(libres);   // número del 0 al 8
  Wire.endTransmission();
}

void debugI2C() {
  Serial.println("\n── DEBUG I2C ──────────────────");

  // ── Leer STM32 Grupo A ──
  Serial.print("Grupo A (0x18): ");
  Wire.requestFrom(ADDR_STM32_A, I2C_MSG_LEN);
  if (Wire.available() == I2C_MSG_LEN) {
    char msg[4] = {0};
    for (int i = 0; i < 4; i++) {
      msg[i] = Wire.read();
    }
    Serial.print("Recibido → [");
    for (int i = 0; i < 4; i++) {
      Serial.print("'");
      Serial.print(msg[i]);
      Serial.print("'");
      if (i < 3) Serial.print(", ");
    }
    Serial.println("]");
    Serial.print("  A-1:");Serial.print(msg[0]);
    Serial.print(" A-2:");Serial.print(msg[1]);
    Serial.print(" A-3:");Serial.print(msg[2]);
    Serial.print(" A-4:");Serial.println(msg[3]);
  } else {
    Serial.println("Sin respuesta");
  }

  // ── Leer STM32 Grupo B ──
  Serial.print("Grupo B (0x20): ");
  Wire.requestFrom(ADDR_STM32_B, I2C_MSG_LEN);
  if (Wire.available() == I2C_MSG_LEN) {
    char msg[4] = {0};
    for (int i = 0; i < 4; i++) {
      msg[i] = Wire.read();
    }
    Serial.print("Recibido → [");
    for (int i = 0; i < 4; i++) {
      Serial.print("'");
      Serial.print(msg[i]);
      Serial.print("'");
      if (i < 3) Serial.print(", ");
    }
    Serial.println("]");
    Serial.print("  B-1:");Serial.print(msg[0]);
    Serial.print(" B-2:");Serial.print(msg[1]);
    Serial.print(" B-3:");Serial.print(msg[2]);
    Serial.print(" B-4:");Serial.println(msg[3]);
  } else {
    Serial.println("Sin respuesta");
  }

  Serial.println("───────────────────────────────");
}


// ═══════════════════════════════════════════════════════════════
//  LECTURA I2C
//  Consulta a cada STM32 si tiene un mensaje nuevo.
//  Formato esperado: 3 bytes ASCII, ej: '1', '0', 'O'
// ═══════════════════════════════════════════════════════════════
void i2cScanner(){
  // funcion para buscar los dispositivos que estan conectados
  byte error, address;
  int nDevices = 0;
  Serial.println("Scanning....");
  
  for (address = 1; address < 127; address++){
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0){
      Serial.print("I2C device found at address 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
      nDevices++;
    } else if(error == 4){
      Serial.print("Unknown error at address 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
    }
  }  
  if(nDevices == 0){
    Serial.println("No hay dispositivos");
  } else {
    Serial.println("done");
  }
}

void leerI2C() {

  // ── Leer STM32 Grupo A ──
  Wire.requestFrom(ADDR_STM32_A, I2C_MSG_LEN);
  if (Wire.available() == I2C_MSG_LEN) {
    char msg[4] = {0};
    msg[0] = Wire.read();   // P1
    msg[1] = Wire.read();   // P2
    msg[2] = Wire.read();   // P3
    msg[3] = Wire.read();   // P4
    flag_parqueo = 0;       // Indicamos que es el grupo de parqueos 1
    procesarMensaje(msg);
  }

  // ── Leer STM32 Grupo B ──
  Wire.requestFrom(ADDR_STM32_B, I2C_MSG_LEN);
  if (Wire.available() == I2C_MSG_LEN) {
    char msg[4] = {0};
    msg[0] = Wire.read();   // P1
    msg[1] = Wire.read();   // P2
    msg[2] = Wire.read();   // P3
    msg[3] = Wire.read();   // P4
    flag_parqueo = 1;       // Indicamos que es el grupo de parqueos 2
    procesarMensaje(msg);
  }
}

// ═══════════════════════════════════════════════════════════════
//  PROCESAR MENSAJE I2C
//  Recibe el string de 3 caracteres y actualiza parkState[].
//
//  Ejemplos:
//    "10F" → parkState[0] = false  (A-1 libre)
//    "10O" → parkState[0] = true   (A-1 ocupado)
//    "23O" → parkState[7] = true   (B-4 ocupado)
// ═══════════════════════════════════════════════════════════════
void procesarMensaje(char* msg) {

  if (!flag_parqueo){
    for (int i = 0; i < 4; i++){
      parkState[i] = (msg[i] == 'o');
    }
  }else{
    for (int i = 0; i < 4; i++){
      parkState[i+4] = (msg[i] == 'o');
    }
  }
}

// ═══════════════════════════════════════════════════════════════
//  MANEJADORES WEB
// ═══════════════════════════════════════════════════════════════

// Ruta raíz "/" — sirve la página con el estado actual
void handle_root() {
  server.send(200, "text/html", buildHTML());
}

// Ruta no encontrada
void handle_notFound() {
  server.send(404, "text/plain", "Not found");
}
void escribirEstadoAToB() {
  Wire.beginTransmission(ADDR_STM32_B);   // 0x20

  for (int i = 0; i < 4; i++) {
    Wire.write(parkState[i] ? 'o' : 'l'); // parkState[0..3] = Grupo A
  }

  Wire.endTransmission();
}
void handle_estado() {
  String json = "{\"parkState\":[";
  for (int i = 0; i < 8; i++) {
    json += parkState[i] ? "true" : "false";
    if (i < 7) json += ",";
  }
  json += "]}";
  server.send(200, "application/json", json);
}

// ═══════════════════════════════════════════════════════════════
//  CONSTRUCCIÓN DEL HTML
//  Genera el HTML completo con el estado actual de parkState[].
//  El estado inicial del JS se inyecta desde C++ en el bloque
//  "1. ESTADO", reemplazando occupied: true/false según parkState.
// ═══════════════════════════════════════════════════════════════
String buildHTML() {

  // ── Helper: convierte bool a string JS ──
  // parkState[0..3] = Grupo A, parkState[4..7] = Grupo B
  auto js = [](bool v) -> String { return v ? "true" : "false"; };

  String html = R"rawliteral(<!DOCTYPE html>
<html lang="es">
<head>
  <meta charset="UTF-8"/>
  <meta name="viewport" content="width=device-width, initial-scale=1.0"/>
  <title>ParkControl</title>
  <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.2/dist/css/bootstrap.min.css" rel="stylesheet"/>
  <link href="https://cdn.jsdelivr.net/npm/bootstrap-icons@1.11.3/font/bootstrap-icons.min.css" rel="stylesheet"/>
  <link href="https://fonts.googleapis.com/css2?family=Syne:wght@400;600;700;800&family=DM+Mono:wght@300;400;500&display=swap" rel="stylesheet"/>

  <!-- ═══════════════════════════════════════
       BLOQUE CSS
  ══════════════════════════════════════════ -->
  <style>
    :root{--bg:#0b0f1a;--surface:#111827;--surface2:#1a2235;--border:#1f2d45;--accent-a:#00d4ff;--accent-b:#ff6b35;--free:#22c55e;--occupied:#ef4444;--text:#e2e8f0;--muted:#64748b;--font-head:'Syne',sans-serif;--font-mono:'DM Mono',monospace;}
    *{box-sizing:border-box;margin:0;padding:0;}
    body{background:var(--bg);color:var(--text);font-family:var(--font-mono);min-height:100vh;overflow-x:hidden;}
    body::before{content:'';position:fixed;inset:0;background-image:linear-gradient(rgba(0,212,255,.03) 1px,transparent 1px),linear-gradient(90deg,rgba(0,212,255,.03) 1px,transparent 1px);background-size:40px 40px;pointer-events:none;z-index:0;}
    .navbar-brand{font-family:var(--font-head);font-weight:800;font-size:1.4rem;letter-spacing:-0.5px;}
    .nav-top{background:rgba(11,15,26,.92);backdrop-filter:blur(12px);border-bottom:1px solid var(--border);position:sticky;top:0;z-index:100;padding:.75rem 1.5rem;}
    .live-dot{width:9px;height:9px;border-radius:50%;background:var(--free);display:inline-block;box-shadow:0 0 8px var(--free);animation:pulse 1.8s ease-in-out infinite;}
    @keyframes pulse{0%,100%{opacity:1;transform:scale(1);}50%{opacity:.5;transform:scale(1.3);}}
    .main-wrapper{position:relative;z-index:1;padding:2rem 1.5rem;}
    .section-title{font-family:var(--font-head);font-weight:800;font-size:1.05rem;letter-spacing:.08em;text-transform:uppercase;}
    .badge-group-a{background:var(--accent-a);color:#000;}
    .badge-group-b{background:var(--accent-b);color:#000;}
    .summary-card{background:var(--surface);border:1px solid var(--border);border-radius:14px;padding:1.25rem 1.5rem;transition:transform .2s,box-shadow .2s;}
    .summary-card:hover{transform:translateY(-3px);box-shadow:0 8px 32px rgba(0,0,0,.4);}
    .summary-card .value{font-family:var(--font-head);font-size:2.4rem;font-weight:800;line-height:1;}
    .summary-card .label{font-size:.72rem;color:var(--muted);letter-spacing:.1em;text-transform:uppercase;margin-top:.3rem;}
    .card-accent-a{border-top:3px solid var(--accent-a);}
    .card-accent-b{border-top:3px solid var(--accent-b);}
    .card-accent-free{border-top:3px solid var(--free);}
    .card-accent-occ{border-top:3px solid var(--occupied);}
    .group-panel{background:var(--surface);border:1px solid var(--border);border-radius:16px;padding:1.5rem;height:100%;}
    .group-panel.group-a{border-top:4px solid var(--accent-a);}
    .group-panel.group-b{border-top:4px solid var(--accent-b);}
    .slot-card{background:var(--surface2);border:1px solid var(--border);border-radius:12px;padding:1rem;transition:all .25s ease;position:relative;overflow:hidden;}
    .slot-card::after{content:'';position:absolute;inset:0;border-radius:12px;opacity:0;transition:opacity .25s;}
    .slot-card.free{border-left:4px solid var(--free);}
    .slot-card.occupied{border-left:4px solid var(--occupied);}
    .slot-card.free::after{background:rgba(34,197,94,.06);}
    .slot-card.occupied::after{background:rgba(239,68,68,.06);}
    .slot-icon{font-size:2rem;line-height:1;}
    .slot-name{font-family:var(--font-head);font-weight:700;font-size:1rem;}
    .slot-status{font-size:.68rem;font-weight:500;letter-spacing:.1em;text-transform:uppercase;padding:.2rem .55rem;border-radius:999px;}
    .status-free{background:rgba(34,197,94,.18);color:var(--free);}
    .status-occupied{background:rgba(239,68,68,.18);color:var(--occupied);}
    .slot-time{font-size:.65rem;color:var(--muted);margin-top:.25rem;}
    .occ-bar-wrap{height:6px;background:var(--border);border-radius:999px;overflow:hidden;}
    .occ-bar{height:100%;border-radius:999px;transition:width .6s ease;}
    .occ-bar.a{background:var(--accent-a);}
    .occ-bar.b{background:var(--accent-b);}
    #clock{font-family:var(--font-mono);font-size:.85rem;color:var(--accent-a);letter-spacing:.05em;}
    .fade-up{opacity:0;transform:translateY(20px);animation:fadeUp .5s ease forwards;}
    @keyframes fadeUp{to{opacity:1;transform:none;}}
    .delay-1{animation-delay:.1s;}.delay-2{animation-delay:.2s;}
    @media(max-width:768px){.summary-card .value{font-size:1.8rem;}}
  </style>
</head>
<body>

<!-- ═══════════════════════════════════════════════
     BLOQUE HTML
════════════════════════════════════════════════ -->

  <!-- Navbar -->
  <nav class="nav-top d-flex align-items-center justify-content-between">
    <div class="d-flex align-items-center gap-3">
      <span class="live-dot"></span>
      <span class="navbar-brand text-white mb-0">
        <span style="color:var(--accent-a)">Park</span>Control
      </span>
      <span class="text-muted d-none d-md-inline" style="font-size:.75rem;letter-spacing:.05em;">MONITOR DE PARQUEOS</span>
    </div>
    <span id="clock">--:--:--</span>
  </nav>

  <!-- Contenido principal -->
  <div class="main-wrapper">

    <!-- Resumen global -->
    <div class="row g-3 mb-4 fade-up delay-1">
      <div class="col-6 col-md-3">
        <div class="summary-card card-accent-a">
          <div class="value text-white" id="total-spots">8</div>
          <div class="label">Espacios totales</div>
        </div>
      </div>
      <div class="col-6 col-md-3">
        <div class="summary-card card-accent-free">
          <div class="value" style="color:var(--free)" id="total-free">0</div>
          <div class="label">Libres ahora</div>
        </div>
      </div>
      <div class="col-6 col-md-3">
        <div class="summary-card card-accent-occ">
          <div class="value" style="color:var(--occupied)" id="total-occ">0</div>
          <div class="label">Ocupados ahora</div>
        </div>
      </div>
      <div class="col-6 col-md-3">
        <div class="summary-card" style="border-top:3px solid var(--muted)">
          <div class="value text-white" id="total-pct">0%</div>
          <div class="label">Ocupación global</div>
        </div>
      </div>
    </div>

    <!-- Grupos A y B -->
    <div class="row g-4 mb-4 fade-up delay-2">

      <!-- Grupo A -->
      <div class="col-12 col-lg-6">
        <div class="group-panel group-a">
          <div class="d-flex align-items-center justify-content-between mb-3">
            <div>
              <span class="section-title me-2">Grupo A</span>
              <span class="badge badge-group-a" style="font-size:.7rem;border-radius:6px;padding:.3rem .6rem;">4 espacios</span>
            </div>
            <div class="text-end">
              <div style="font-size:.72rem;color:var(--muted);">Ocupación</div>
              <div style="font-family:var(--font-head);font-size:1.1rem;color:var(--accent-a);" id="pct-a">0%</div>
            </div>
          </div>
          <div class="occ-bar-wrap mb-3"><div class="occ-bar a" id="bar-a" style="width:0%"></div></div>
          <div class="row g-2" id="slots-a"></div>
        </div>
      </div>

      <!-- Grupo B -->
      <div class="col-12 col-lg-6">
        <div class="group-panel group-b">
          <div class="d-flex align-items-center justify-content-between mb-3">
            <div>
              <span class="section-title me-2">Grupo B</span>
              <span class="badge badge-group-b" style="font-size:.7rem;border-radius:6px;padding:.3rem .6rem;">4 espacios</span>
            </div>
            <div class="text-end">
              <div style="font-size:.72rem;color:var(--muted);">Ocupación</div>
              <div style="font-family:var(--font-head);font-size:1.1rem;color:var(--accent-b);" id="pct-b">0%</div>
            </div>
          </div>
          <div class="occ-bar-wrap mb-3"><div class="occ-bar b" id="bar-b" style="width:0%"></div></div>
          <div class="row g-2" id="slots-b"></div>
        </div>
      </div>

    </div>
  </div>

<script src="https://cdn.jsdelivr.net/npm/bootstrap@5.3.2/dist/js/bootstrap.bundle.min.js"></script>

<!-- ═══════════════════════════════════════════════
     BLOQUE JAVASCRIPT
════════════════════════════════════════════════ -->
<script>

  /* ── 1. ESTADO
     Inyectado desde C++ con los valores reales de parkState[].
     El ESP32 reemplaza true/false según el arreglo al momento
     de servir la página.
  ─────────────────────────────────────────────── */
  const state = {
    groupA: [
)rawliteral";

  // ── Inyectar estado Grupo A (parkState[0..3]) ──
  for (int i = 0; i < 4; i++) {
    html += "      { id:'A" + String(i+1) + "', name:'A-" + String(i+1) +
            "', occupied:" + js(parkState[i]) +
            ", since:" + (parkState[i] ? "new Date()" : "null") + " }";
    html += (i < 3) ? ",\n" : "\n";
  }

  html += R"rawliteral(    ],
    groupB: [
)rawliteral";

  // ── Inyectar estado Grupo B (parkState[4..7]) ──
  for (int i = 0; i < 4; i++) {
    html += "      { id:'B" + String(i+1) + "', name:'B-" + String(i+1) +
            "', occupied:" + js(parkState[i+4]) +
            ", since:" + (parkState[i+4] ? "new Date()" : "null") + " }";
    html += (i < 3) ? ",\n" : "\n";
  }

  html += R"rawliteral(    ],
  };

  /* ── 2. RELOJ ── */
  function updateClock(){
    document.getElementById('clock').textContent =
      new Date().toLocaleTimeString('es-GT',{hour12:false});
  }
  setInterval(updateClock,1000);
  updateClock();

  /* ── 3. FORMATO DE HORA ── */
  function formatTime(date){
    if(!date) return '--:--';
    return date.toLocaleTimeString('es-GT',{hour:'2-digit',minute:'2-digit',hour12:false});
  }

  /* ── 4. RENDER SLOTS
     Construye visualmente las tarjetas de parqueo.
  ─────────────────────────────────────────────── */
  function renderSlots(){
    ['A','B'].forEach(g=>{
      const slots     = state['group'+g];
      const container = document.getElementById('slots-'+g.toLowerCase());
      container.innerHTML='';
      slots.forEach(slot=>{
        const col=document.createElement('div');
        col.className='col-6';
        col.innerHTML=
          '<div class="slot-card '+(slot.occupied?'occupied':'free')+'">' +
            '<div class="d-flex align-items-start justify-content-between">' +
              '<div>' +
                '<div class="slot-icon mb-1">'+(slot.occupied?'&#x1F697;':'&#x1F17F;&#xFE0F;')+'</div>' +
                '<div class="slot-name">'+slot.name+'</div>' +
              '</div>' +
              '<span class="slot-status '+(slot.occupied?'status-occupied':'status-free')+'">' +
                (slot.occupied?'Ocupado':'Libre') +
              '</span>' +
            '</div>' +
            '<div class="slot-time mt-2">' +
              (slot.occupied?'Desde '+formatTime(slot.since):'Disponible') +
            '</div>' +
          '</div>';
        container.appendChild(col);
      });
    });
    updateStats();
  }

  /* ── 5. ESTADÍSTICAS ── */
  function updateStats(){
    const all    = state.groupA.concat(state.groupB);
    const occ    = all.filter(s=>s.occupied).length;
    const free   = 8-occ;
    const pctG   = Math.round(occ/8*100);
    document.getElementById('total-free').textContent = free;
    document.getElementById('total-occ').textContent  = occ;
    document.getElementById('total-pct').textContent  = pctG+'%';
    ['A','B'].forEach(g=>{
      const slots = state['group'+g];
      const o     = slots.filter(s=>s.occupied).length;
      const p     = Math.round(o/4*100);
      document.getElementById('pct-'+g.toLowerCase()).textContent = p+'%';
      document.getElementById('bar-'+g.toLowerCase()).style.width = p+'%';
    });
  }

  setInterval(function() {
  fetch('/estado')
    .then(r => r.json())
    .then(data => {
      data.parkState.forEach((ocupado, i) => {
        if (i < 4) {
          state.groupA[i].occupied = ocupado;
          state.groupA[i].since = ocupado ? new Date() : null;
        } else {
          state.groupB[i-4].occupied = ocupado;
          state.groupB[i-4].since = ocupado ? new Date() : null;
        }
      });
      renderSlots();
    });
}, 1000);

  /* ── 6. INICIO ── */
  renderSlots();

</script>
</body>
</html>)rawliteral";

  return html;
}
