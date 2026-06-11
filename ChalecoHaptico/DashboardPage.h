#ifndef DASHBOARD_PAGE_H
#define DASHBOARD_PAGE_H

// ============================================================================
//  DashboardPage.h - HTML/CSS/JS del dashboard embebido.
//
//  Se sirve una sola vez por navegador y despues los datos llegan por
//  Server-Sent Events a /stream. El JS solo renderiza, no consume CPU del ESP.
//
//  Estetica "consola de equipamiento tactico": paleta ambar sobre charcoal,
//  paneles numerados con marcas de esquina. Cada zona de motor es control y
//  feedback a la vez: un input range NATIVO invisible sobre la barra manda
//  M#=valor (el navegador maneja el touch, igual que los sliders del
//  dashboard original) y el relleno muestra el PWM real que reporta el ESP
//  (la muesca blanca es el setpoint pedido). El header muestra la version
//  (V3) para distinguir de un vistazo que build cargo el navegador.
// ============================================================================

const char DASHBOARD_HTML[] PROGMEM = R"HTML(<!doctype html>
<html lang="es">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>CHALECO // CONSOLA</title>
<style>
:root{
  --bg:#131110;--panel:#1b1714;--inset:#100e0c;--line:#332a20;--line2:#241e18;
  --text:#e8dfcd;--dim:#8d8170;--amber:#ffb454;--orange:#ff7a2f;
  --ok:#a8e063;--bad:#ff4d3a;--cold:#6fc7e8;
  --cond:"Bahnschrift","Avenir Next Condensed","Roboto Condensed","Arial Narrow",sans-serif;
  --mono:ui-monospace,"Cascadia Mono",Consolas,Menlo,monospace;
}
*{box-sizing:border-box;-webkit-tap-highlight-color:transparent}
html,body{overscroll-behavior:none} /* sin pull-to-refresh: recargar mataba el drag y dejaba streams zombi */
body{
  margin:0;color:var(--text);font-family:var(--cond);font-size:14px;
  background:var(--bg);
  background-image:
    radial-gradient(ellipse at 50% -10%,rgba(255,122,47,.06),transparent 60%),
    repeating-linear-gradient(0deg,transparent 0 27px,rgba(255,180,84,.03) 27px 28px),
    repeating-linear-gradient(90deg,transparent 0 27px,rgba(255,180,84,.03) 27px 28px);
}
header{
  position:sticky;top:0;z-index:9;display:flex;flex-wrap:wrap;align-items:center;
  gap:8px 14px;padding:10px 16px;background:#181411f2;border-bottom:1px solid var(--line);
}
header h1{font-size:16px;margin:0 auto 0 0;font-weight:700;letter-spacing:3px;color:var(--amber)}
header h1 small{color:var(--dim);font-weight:400;letter-spacing:2px;font-size:10px;display:block}
.pill{display:inline-flex;align-items:center;gap:6px;font-family:var(--mono);font-size:11px;color:var(--dim)}
.pill b{color:var(--text);font-weight:400}
.led{width:7px;height:7px;background:#3d362c;transform:rotate(45deg);transition:background .15s,box-shadow .15s}
.led.on{background:var(--ok);box-shadow:0 0 7px var(--ok)}
.led.bad{background:var(--bad);box-shadow:0 0 7px var(--bad)}

main{display:grid;gap:12px;padding:12px;grid-template-columns:repeat(12,1fr);counter-reset:sec;max-width:1500px;margin:0 auto}
.panel{
  position:relative;background:var(--panel);border:1px solid var(--line);
  padding:12px 14px;min-width:0;animation:in .4s ease-out backwards;
}
.panel:nth-child(2){animation-delay:.05s}.panel:nth-child(3){animation-delay:.1s}
.panel:nth-child(4){animation-delay:.15s}.panel:nth-child(5){animation-delay:.2s}
.panel:nth-child(6){animation-delay:.25s}.panel:nth-child(7){animation-delay:.3s}
.panel:nth-child(8){animation-delay:.35s}
@keyframes in{from{opacity:0;transform:translateY(8px)}}
.panel::before,.panel::after{content:"";position:absolute;width:10px;height:10px;pointer-events:none}
.panel::before{top:-1px;left:-1px;border-top:2px solid var(--amber);border-left:2px solid var(--amber)}
.panel::after{bottom:-1px;right:-1px;border-bottom:2px solid var(--amber);border-right:2px solid var(--amber)}
.panel h2{
  counter-increment:sec;font-size:12px;letter-spacing:2.5px;text-transform:uppercase;
  color:var(--text);margin:0 0 12px;font-weight:700;display:flex;align-items:baseline;gap:8px;
  border-bottom:1px solid var(--line2);padding-bottom:8px;
}
.panel h2::before{content:counter(sec,decimal-leading-zero);color:var(--orange);font-family:var(--mono);font-size:11px}
.panel h2 .tag{margin-left:auto;font-size:10px;color:var(--dim);letter-spacing:1px;font-weight:400}
.col-3{grid-column:span 3}.col-4{grid-column:span 4}.col-6{grid-column:span 6}.col-12{grid-column:span 12}
@media (max-width:1100px){.col-3,.col-4{grid-column:span 6}.col-6{grid-column:span 12}}
@media (max-width:640px){.col-3,.col-4,.col-6{grid-column:span 12}}

/* --- 01 Chaleco: zonas de motor control+feedback --- */
.vest{display:grid;grid-template-columns:1fr 1fr;gap:10px}
.vest-lbl{grid-column:span 2;font-size:10px;letter-spacing:3px;color:var(--dim);display:flex;align-items:center;gap:8px}
.vest-lbl::after{content:"";flex:1;height:1px;background:var(--line2)}
.mz{background:var(--inset);border:1px solid var(--line2);padding:8px;transition:border-color .12s}
.mz.hit{animation:dmg .6s ease-out}
@keyframes dmg{0%{border-color:var(--bad);box-shadow:0 0 14px rgba(255,77,58,.5),inset 0 0 20px rgba(255,77,58,.25)}100%{}}
.mz-top{display:flex;justify-content:space-between;font-size:11px;letter-spacing:1px;margin-bottom:6px}
.mz-top .v{font-family:var(--mono);color:var(--amber)}
.mz-bar{
  position:relative;height:34px;background:#0a0908;border:1px solid var(--line2);
  overflow:hidden;
}
/* El control real es un input range NATIVO invisible encima de la barra: el
   navegador maneja el touch igual que en los sliders del dashboard original
   (sin pull-to-refresh ni gestos cancelados). La barra solo dibuja. */
.mz-bar input{position:absolute;inset:0;width:100%;height:100%;margin:0;opacity:0;cursor:pointer}
.mz-bar .fill{
  position:absolute;inset:0 auto 0 0;width:0%;
  background:linear-gradient(90deg,#7a4a1d,var(--orange) 70%,var(--amber));
  transition:width .06s linear;
}
.mz-bar .tick{position:absolute;top:0;bottom:0;width:1px;background:rgba(232,223,205,.12);pointer-events:none}
.mz-bar .set{
  position:absolute;top:-1px;bottom:-1px;left:0;width:2px;background:var(--text);
  box-shadow:0 0 5px rgba(232,223,205,.8);transition:left .05s linear;
}
.mz-sub{display:flex;justify-content:space-between;font-size:10px;color:var(--dim);margin-top:5px;font-family:var(--mono)}
.vest-meta{grid-column:span 2;font-family:var(--mono);font-size:10px;color:var(--dim);text-align:right}

.acts{display:grid;grid-template-columns:repeat(6,minmax(0,1fr));gap:8px;margin-top:12px}
@media (max-width:640px){.acts{grid-template-columns:repeat(3,minmax(0,1fr))}}
.abtn{
  font-family:var(--cond);font-size:11px;letter-spacing:1.5px;font-weight:700;padding:10px 4px;
  background:var(--inset);border:1px solid var(--line);color:var(--text);cursor:pointer;
  text-transform:uppercase;transition:border-color .1s,background .1s,color .1s;
}
.abtn:active{background:#2a2118;border-color:var(--amber);color:var(--amber)}
.abtn.live{border-color:var(--amber);color:var(--amber);box-shadow:inset 0 0 14px rgba(255,180,84,.2)}
.abtn.cold.live{border-color:var(--cold);color:var(--cold);box-shadow:inset 0 0 14px rgba(111,199,232,.25)}
.abtn.dmg{color:var(--dim)}
.abtn.dmg:active{border-color:var(--bad);color:var(--bad)}
.kill{
  grid-column:1/-1;font-size:13px;letter-spacing:4px;padding:12px;
  border:1px solid #5a2520;color:var(--bad);background:#1a0f0d;
}
.kill:active{background:var(--bad);color:#1a0f0d;border-color:var(--bad)}

/* --- Vida --- */
.hp-big{display:flex;align-items:baseline;gap:8px;margin-bottom:10px}
.hp-big .n{font-family:var(--mono);font-size:38px;font-weight:700;color:var(--amber);line-height:1}
.hp-big .u{font-size:11px;color:var(--dim);letter-spacing:2px}
.segs{display:grid;grid-template-columns:repeat(8,1fr);gap:4px;margin-bottom:12px}
.segs div{height:22px;background:#0a0908;border:1px solid var(--line2);transform:skewX(-12deg);transition:background .1s,box-shadow .1s}

/* sliders nativos (HP) */
.srow{display:grid;grid-template-columns:auto 1fr auto;align-items:center;gap:10px;font-family:var(--mono);font-size:11px;color:var(--dim)}
input[type=range]{-webkit-appearance:none;appearance:none;height:4px;background:var(--line);outline:none;min-width:0}
input[type=range]::-webkit-slider-thumb{-webkit-appearance:none;width:20px;height:20px;border-radius:0;background:var(--amber);cursor:pointer;clip-path:polygon(50% 0,100% 50%,50% 100%,0 50%)}
input[type=range]::-moz-range-thumb{width:16px;height:16px;border:none;border-radius:0;background:var(--amber);cursor:pointer}

/* --- Gatillo / flex --- */
.fire{display:flex;align-items:center;gap:12px;background:var(--inset);border:1px solid var(--line2);padding:10px;margin-bottom:12px}
.fire .led{width:12px;height:12px}
.fire .led.on{background:var(--bad);box-shadow:0 0 12px var(--bad)}
.fire b{letter-spacing:3px;font-size:14px}
.fire span{margin-left:auto;font-family:var(--mono);font-size:10px;color:var(--dim)}
.bar{position:relative;height:16px;background:#0a0908;border:1px solid var(--line2);overflow:hidden}
.bar .fill{position:absolute;inset:0 auto 0 0;width:0%;background:linear-gradient(90deg,#6e5a2c,var(--amber));transition:width .06s linear}
.bar .mark{position:absolute;top:-2px;bottom:-2px;width:2px;background:var(--bad);box-shadow:0 0 5px var(--bad)}
.kv{display:grid;grid-template-columns:1fr auto;gap:5px 12px;font-family:var(--mono);font-size:11px;margin-top:10px}
.kv .k{color:var(--dim);letter-spacing:1px}.kv .v{text-align:right}

/* --- Entradas digitales --- */
.btn-grid{display:grid;grid-template-columns:1fr 1fr;gap:8px}
.btn{background:var(--inset);border:1px solid var(--line2);padding:9px 10px;display:flex;align-items:center;gap:9px;transition:border-color .1s}
.btn.on{border-color:var(--ok)}
.btn .label{font-size:12px;letter-spacing:1.5px}
.btn .sub{color:var(--dim);font-size:10px;font-family:var(--mono)}

/* --- Joysticks --- */
.joy-pair{display:grid;grid-template-columns:1fr 1fr;gap:14px}
.joy{position:relative;aspect-ratio:1;background:radial-gradient(circle,#16120e 0%,#0a0908 100%);border-radius:50%;border:1px solid var(--line);overflow:hidden}
.joy::after{content:"";position:absolute;inset:50% 0 auto 0;height:1px;background:var(--line2)}
.joy::before{content:"";position:absolute;left:50%;top:0;bottom:0;width:1px;background:var(--line2)}
.joy .ring{position:absolute;inset:18%;border:1px dashed var(--line);border-radius:50%}
.joy .dot-xy{position:absolute;width:12px;height:12px;background:var(--amber);transform:translate(-50%,-50%) rotate(45deg);box-shadow:0 0 10px var(--orange);transition:left .05s linear,top .05s linear;left:50%;top:50%}
.joy-label{text-align:center;font-family:var(--mono);font-size:10px;color:var(--dim);margin-top:6px}

/* --- IMU / arma --- */
.gauges{display:grid;grid-template-columns:1fr 1fr;gap:14px}
.gauge{text-align:center}
.ring2{width:100px;height:100px;border-radius:50%;margin:0 auto;background:conic-gradient(from -90deg,var(--amber) calc(var(--p)*1%),#0a0908 0);display:grid;place-items:center;border:1px solid var(--line2)}
.ring2>div{width:78px;height:78px;border-radius:50%;background:var(--panel);display:grid;place-items:center;font-family:var(--mono);font-size:16px;font-weight:600}
.glabel{margin-top:6px;color:var(--dim);font-size:10px;letter-spacing:3px}
.wpn{display:grid;grid-template-columns:repeat(4,1fr);gap:6px;margin-top:14px}
.wpn div{
  text-align:center;padding:8px 0;font-family:var(--mono);font-size:13px;
  background:var(--inset);border:1px solid var(--line2);color:var(--dim);transition:all .12s;
}
.wpn div.sel{background:#2a2014;border-color:var(--amber);color:var(--amber);box-shadow:inset 0 0 10px rgba(255,180,84,.15)}
.wpn-state{text-align:center;font-family:var(--mono);font-size:10px;color:var(--dim);margin-top:6px;letter-spacing:1px}
.wpn-state.busy{color:var(--orange)}

/* --- Telemetria --- */
canvas{display:block;width:100%;height:120px;background:#0c0a09;border:1px solid var(--line2)}
.legend{font-size:10px;color:var(--dim);margin:5px 0 10px;display:flex;gap:12px;font-family:var(--mono)}
.legend .sw{display:inline-block;width:9px;height:9px;margin-right:4px;vertical-align:middle}

/* --- Consola --- */
.raw-row{display:flex;gap:8px}
.raw-row input[type=text]{
  flex:1;font-family:var(--mono);font-size:13px;padding:9px 10px;background:#0a0908;
  border:1px solid var(--line2);color:var(--amber);outline:none;min-width:0;
}
.raw-row input[type=text]:focus{border-color:var(--amber)}
.cmd-echo{font-family:var(--mono);font-size:11px;color:var(--dim);min-height:15px;margin-top:8px}
.cmd-echo b{color:var(--ok);font-weight:400}
</style>
</head>
<body>
<header>
  <h1>CHALECO HAPTICO<small>CONSOLA DE EQUIPO &middot; TSM 2026-II &middot; V3</small></h1>
  <span class="pill"><span class="led" id="sse-dot"></span>LINK</span>
  <span class="pill"><span class="led" id="unity-dot"></span>UNITY</span>
  <span class="pill"><b id="rate-pill">0</b>Hz</span>
  <span class="pill"><b id="clients-pill">0</b>STA</span>
  <span class="pill"><b id="uptime-pill">00:00:00</b></span>
</header>
<main>

  <section class="panel col-6">
    <h2>Chaleco &middot; motores <span class="tag">desliza la barra = comando &middot; relleno = PWM real</span></h2>
    <div class="vest">
      <div class="vest-lbl">FRENTE</div>
      <div class="mz" id="mz-1">
        <div class="mz-top"><span>M1 &middot; IZQ</span><span class="v" id="mzv-1">0</span></div>
        <div class="mz-bar" data-m="0"><div class="tick" style="left:25%"></div><div class="tick" style="left:50%"></div><div class="tick" style="left:75%"></div><div class="fill"></div><div class="set"></div><input type="range" class="mzr" min="0" max="255" value="0" data-m="0"></div>
        <div class="mz-sub"><span>REAL <span id="mzp-1">0</span> PWM</span><span id="mzw-1">0.00V</span></div>
      </div>
      <div class="mz" id="mz-2">
        <div class="mz-top"><span>M2 &middot; DER</span><span class="v" id="mzv-2">0</span></div>
        <div class="mz-bar" data-m="1"><div class="tick" style="left:25%"></div><div class="tick" style="left:50%"></div><div class="tick" style="left:75%"></div><div class="fill"></div><div class="set"></div><input type="range" class="mzr" min="0" max="255" value="0" data-m="1"></div>
        <div class="mz-sub"><span>REAL <span id="mzp-2">0</span> PWM</span><span id="mzw-2">0.00V</span></div>
      </div>
      <div class="vest-lbl">ESPALDA</div>
      <div class="mz" id="mz-3">
        <div class="mz-top"><span>M3 &middot; IZQ</span><span class="v" id="mzv-3">0</span></div>
        <div class="mz-bar" data-m="2"><div class="tick" style="left:25%"></div><div class="tick" style="left:50%"></div><div class="tick" style="left:75%"></div><div class="fill"></div><div class="set"></div><input type="range" class="mzr" min="0" max="255" value="0" data-m="2"></div>
        <div class="mz-sub"><span>REAL <span id="mzp-3">0</span> PWM</span><span id="mzw-3">0.00V</span></div>
      </div>
      <div class="mz" id="mz-4">
        <div class="mz-top"><span>M4 &middot; DER</span><span class="v" id="mzv-4">0</span></div>
        <div class="mz-bar" data-m="3"><div class="tick" style="left:25%"></div><div class="tick" style="left:50%"></div><div class="tick" style="left:75%"></div><div class="fill"></div><div class="set"></div><input type="range" class="mzr" min="0" max="255" value="0" data-m="3"></div>
        <div class="mz-sub"><span>REAL <span id="mzp-4">0</span> PWM</span><span id="mzw-4">0.00V</span></div>
      </div>
      <div class="vest-meta" id="mot-cap">TOPE -.-- V &middot; BAT -.-- V</div>
    </div>
    <div class="acts">
      <button class="abtn" id="out-sol" data-cmd="SOL=1">Solenoide</button>
      <button class="abtn cold" id="out-pel" data-cmd="PELT=1">Peltier</button>
      <button class="abtn dmg" data-cmd="DMG=1">Dano 1</button>
      <button class="abtn dmg" data-cmd="DMG=2">Dano 2</button>
      <button class="abtn dmg" data-cmd="DMG=3">Dano 3</button>
      <button class="abtn dmg" data-cmd="DMG=4">Dano 4</button>
      <button class="abtn kill" id="btn-stop">&#9632; Apagar motores</button>
    </div>
  </section>

  <section class="panel col-3">
    <h2>Vida <span class="tag">NeoPixel x8</span></h2>
    <div class="hp-big"><span class="n" id="hp-v">100</span><span class="u">% HP</span></div>
    <div class="segs" id="neo"></div>
    <div class="srow">
      <span>SET</span>
      <input type="range" min="0" max="100" value="100" id="sl-hp">
      <span id="sv-hp">100</span>
    </div>
  </section>

  <section class="panel col-3">
    <h2>Gatillo <span class="tag">flex</span></h2>
    <div class="fire"><span class="led" id="shoot-led"></span><b>DISPARO</b><span>flex sobre umbral</span></div>
    <div class="bar" id="flex-bar"><div class="fill"></div><div class="mark"></div></div>
    <div class="kv">
      <span class="k">VALOR</span><span class="v" id="flex-v">0</span>
      <span class="k">REPOSO</span><span class="v" id="flex-r">0</span>
      <span class="k">UMBRAL</span><span class="v" id="flex-t">0</span>
    </div>
  </section>

  <section class="panel col-3">
    <h2>Botones <span class="tag">PB1-PB4</span></h2>
    <div class="btn-grid">
      <div class="btn" id="btn-jump"><span class="led"></span><div><div class="label">PB1 SALTO</div><div class="sub">jump</div></div></div>
      <div class="btn" id="btn-weapon"><span class="led"></span><div><div class="label">PB2</div><div class="sub">reservado</div></div></div>
      <div class="btn" id="btn-b3"><span class="led"></span><div><div class="label">PB3</div><div class="sub">reservado</div></div></div>
      <div class="btn" id="btn-b4"><span class="led"></span><div><div class="label">PB4</div><div class="sub">reservado</div></div></div>
    </div>
  </section>

  <section class="panel col-3">
    <h2>Joysticks <span class="tag">analog</span></h2>
    <div class="joy-pair">
      <div>
        <div class="joy" id="joy1"><div class="ring"></div><div class="dot-xy"></div></div>
        <div class="joy-label">J1 <span id="j1xy">0.00, 0.00</span></div>
      </div>
      <div>
        <div class="joy" id="joy2"><div class="ring"></div><div class="dot-xy"></div></div>
        <div class="joy-label">J2 <span id="j2xy">0.00, 0.00</span></div>
      </div>
    </div>
  </section>

  <section class="panel col-6">
    <h2>IMU &middot; orientacion y arma <span class="tag">BMI160</span></h2>
    <div class="gauges">
      <div class="gauge">
        <div class="ring2" id="roll-ring" style="--p:50"><div id="roll-v">+0.0&deg;</div></div>
        <div class="glabel">ROLL</div>
      </div>
      <div class="gauge">
        <div class="ring2" id="pitch-ring" style="--p:50"><div id="pitch-v">+0.0&deg;</div></div>
        <div class="glabel">PITCH</div>
      </div>
    </div>
    <div class="wpn" id="wpn"><div>1</div><div>2</div><div>3</div><div>4</div></div>
    <div class="wpn-state" id="arma-state">GESTO LISTO</div>
  </section>

  <section class="panel col-6">
    <h2>Telemetria IMU <span class="tag">tiempo real</span></h2>
    <canvas id="ch-acc"></canvas>
    <div class="legend">
      <span><span class="sw" style="background:#ff5d3a"></span>AccX</span>
      <span><span class="sw" style="background:#a8e063"></span>AccY</span>
      <span><span class="sw" style="background:#ffb454"></span>AccZ</span>
      <span style="margin-left:auto">&plusmn;2g</span>
    </div>
    <canvas id="ch-gyr"></canvas>
    <div class="legend">
      <span><span class="sw" style="background:#ff5d3a"></span>GyrX</span>
      <span><span class="sw" style="background:#a8e063"></span>GyrY</span>
      <span><span class="sw" style="background:#ffb454"></span>GyrZ</span>
      <span style="margin-left:auto">&plusmn;500&deg;/s</span>
    </div>
  </section>

  <section class="panel col-6">
    <h2>Consola <span class="tag">parser Unity/Serial</span></h2>
    <div class="raw-row">
      <input type="text" id="raw-cmd" placeholder="M1=128;HP=80;DMG=2" autocomplete="off" spellcheck="false">
      <button class="abtn" id="btn-send">Enviar</button>
    </div>
    <!-- dmg-zone va en su propio div: si viviera dentro de #cmd-echo, el
         primer comando reescribiria el eco y destruiria el span, y apply()
         tronaria en cada trama (HP congelado, 0 Hz, uptime detenido). -->
    <div class="cmd-echo" id="cmd-echo">&gt; listo</div>
    <div class="cmd-echo">zona de dano: <span id="dmg-zone">-</span></div>
  </section>
</main>

<script>
const $=(id)=>document.getElementById(id);
const N=200;
const buf={ax:new Float32Array(N),ay:new Float32Array(N),az:new Float32Array(N),gx:new Float32Array(N),gy:new Float32Array(N),gz:new Float32Array(N)};
let wIdx=0,frames=0,lastRate=performance.now();
let lastDmgZone=0;
// Voltajes que reporta el ESP: VMOT = tope de los motores (100% del slider),
// VBAT = pack completo. El recorte vive en el firmware; aqui solo se muestra
// el voltaje equivalente a cada nivel de PWM (intensidad/255 * VMOT).
let VMOT=3.0,VBAT=0;
const motVolts=(v)=>((v/255)*VMOT).toFixed(2)+'V';

function pushSample(d){
  buf.ax[wIdx]=d.ax;buf.ay[wIdx]=d.ay;buf.az[wIdx]=d.az;
  buf.gx[wIdx]=d.gx;buf.gy[wIdx]=d.gy;buf.gz[wIdx]=d.gz;
  wIdx=(wIdx+1)%N;
}

function drawChart(canvas,series,minY,maxY){
  const dpr=window.devicePixelRatio||1;
  const w=canvas.clientWidth,h=canvas.clientHeight;
  if(canvas.width!==Math.round(w*dpr)){canvas.width=Math.round(w*dpr);canvas.height=Math.round(h*dpr);}
  const ctx=canvas.getContext('2d');
  ctx.setTransform(dpr,0,0,dpr,0,0);
  ctx.clearRect(0,0,w,h);
  ctx.strokeStyle='#241e18';ctx.lineWidth=1;
  ctx.beginPath();
  for(let i=1;i<4;i++){const y=(h*i)/4;ctx.moveTo(0,y);ctx.lineTo(w,y);}
  ctx.stroke();
  ctx.strokeStyle='#3d342a';ctx.setLineDash([2,3]);
  ctx.beginPath();
  const yz=h-((0-minY)/(maxY-minY))*h;
  ctx.moveTo(0,yz);ctx.lineTo(w,yz);ctx.stroke();
  ctx.setLineDash([]);
  for(const s of series){
    ctx.strokeStyle=s.color;ctx.lineWidth=1.5;
    ctx.beginPath();
    for(let i=0;i<N;i++){
      const idx=(wIdx+i)%N;
      const v=s.data[idx];
      const y=h-((v-minY)/(maxY-minY))*h;
      const x=(i/(N-1))*w;
      if(i===0)ctx.moveTo(x,y);else ctx.lineTo(x,y);
    }
    ctx.stroke();
  }
}

function setLed(el,on,cls){
  if(!el)return;
  el.classList.toggle('on',!!on);
  const led=el.querySelector?el.querySelector('.led'):null;
  if(led)led.classList.toggle('on',!!on);
}

function setJoy(el,x,y){
  const dot=el.querySelector('.dot-xy');
  dot.style.left=((x+1)*50)+'%';
  dot.style.top=((-y+1)*50)+'%';
}

function fmtUptime(s){
  const h=Math.floor(s/3600),m=Math.floor((s%3600)/60),sec=s%60;
  return String(h).padStart(2,'0')+':'+String(m).padStart(2,'0')+':'+String(sec).padStart(2,'0');
}

const neoEl=$('neo');
for(let i=0;i<8;i++)neoEl.appendChild(document.createElement('div'));

function setNeo(hp){
  const lit=Math.ceil(hp*8/100);
  const r=hp<50?255:Math.round(255*(1-(hp-50)/50));
  const g=hp>50?220:Math.round(220*(hp/50));
  for(let i=0;i<8;i++){
    const el=neoEl.children[i];
    if(i<lit){
      const c=`rgb(${r},${g},30)`;
      el.style.background=c;
      el.style.boxShadow=`0 0 7px ${c}`;
      el.style.borderColor=c;
    }else{
      el.style.background='';el.style.boxShadow='';el.style.borderColor='';
    }
  }
}

// ---- Envio de comandos al ESP por /cmd (mismo parser que Unity/Serial) ----
// Un solo fetch en vuelo a la vez: con el keep-alive del firmware todo viaja
// por UNA conexion TCP persistente. Disparar un fetch (conexion nueva) por
// cada movimiento del slider agotaba los sockets del ESP y mataba el SSE.
// Los comandos de una sola clave se coalescen en la cola (gana el ultimo).
const echo=$('cmd-echo');
const cmdQ=[];let cmdInflight=false;
function pumpCmd(){
  if(cmdInflight||!cmdQ.length)return;
  cmdInflight=true;
  const str=cmdQ.shift();
  fetch('/cmd?c='+encodeURIComponent(str))
    .then(r=>{echo.innerHTML='&gt; '+str+(r.ok?' <b>[OK]</b>':' [ERR '+r.status+']');})
    .catch(()=>{echo.textContent='> '+str+'  [sin conexion]';})
    .finally(()=>{cmdInflight=false;pumpCmd();});
}
function sendCmd(str){
  const eq=str.indexOf('=');
  if(eq>0&&str.indexOf(';')<0){
    const pre=str.substring(0,eq+1);
    for(let i=0;i<cmdQ.length;i++){
      if(cmdQ[i].startsWith(pre)&&cmdQ[i].indexOf(';')<0){cmdQ[i]=str;pumpCmd();return;}
    }
  }
  cmdQ.push(str);
  pumpCmd();
}

// ---- Zonas de motor: la barra es feedback y el range nativo el control ----
// El relleno muestra el PWM real que reporta el ESP; la muesca blanca es el
// setpoint local (posicion del range invisible). Mismo mecanismo de eventos
// input/change + throttle que el dashboard original con sliders nativos.
const motorSet=[0,0,0,0];
function paintSet(i){
  const bar=document.querySelector('.mz-bar[data-m="'+i+'"]');
  bar.querySelector('.set').style.left=(motorSet[i]*100/255)+'%';
  $('mzv-'+(i+1)).textContent='SET '+motorSet[i];
}
function bindMotorRange(sl){
  const i=+sl.dataset.m,key='M'+(i+1);
  let last=0,pending=null,timer=null;
  const flush=()=>{timer=null;if(pending!==null){sendCmd(key+'='+pending);pending=null;}};
  sl.addEventListener('input',()=>{
    motorSet[i]=+sl.value;paintSet(i);
    const now=performance.now();
    if(now-last>70){last=now;sendCmd(key+'='+sl.value);}
    else{pending=sl.value;if(!timer)timer=setTimeout(flush,80);}
  });
  // valor definitivo al soltar
  sl.addEventListener('change',()=>{
    if(timer){clearTimeout(timer);timer=null;pending=null;}
    motorSet[i]=+sl.value;paintSet(i);
    sendCmd(key+'='+sl.value);
  });
}
document.querySelectorAll('.mzr').forEach(bindMotorRange);

$('btn-stop').addEventListener('click',()=>{
  document.querySelectorAll('.mzr').forEach(sl=>{sl.value=0;});
  for(let i=0;i<4;i++){motorSet[i]=0;paintSet(i);}
  sendCmd('M1=0;M2=0;M3=0;M4=0');
});

// Botones de accion (SOL, PELT, DMG)
document.querySelectorAll('.abtn[data-cmd]').forEach(b=>{
  b.addEventListener('click',()=>sendCmd(b.dataset.cmd));
});

// Slider de HP con throttle (igual que los motores)
(function(){
  const sl=$('sl-hp'),vEl=$('sv-hp');
  let last=0,pending=null,timer=null;
  const flush=()=>{timer=null;if(pending!==null){sendCmd('HP='+pending);pending=null;}};
  sl.addEventListener('input',()=>{
    vEl.textContent=sl.value;
    const now=performance.now();
    if(now-last>70){last=now;sendCmd('HP='+sl.value);}
    else{pending=sl.value;if(!timer)timer=setTimeout(flush,80);}
  });
  sl.addEventListener('change',()=>{if(timer){clearTimeout(timer);timer=null;}sendCmd('HP='+sl.value);});
})();

const rawIn=$('raw-cmd');
function sendRaw(){const c=rawIn.value.trim();if(c)sendCmd(c);}
$('btn-send').addEventListener('click',sendRaw);
rawIn.addEventListener('keydown',e=>{if(e.key==='Enter')sendRaw();});

function apply(d){
  setLed($('btn-jump'),d.j);
  setLed($('btn-weapon'),d.w);
  setLed($('btn-b3'),d.b3);
  setLed($('btn-b4'),d.b4);
  $('shoot-led').classList.toggle('on',!!d.s);

  setJoy($('joy1'),d.jx1,d.jy1);
  setJoy($('joy2'),d.jx2,d.jy2);
  $('j1xy').textContent='('+d.jx1.toFixed(2)+', '+d.jy1.toFixed(2)+')';
  $('j2xy').textContent='('+d.jx2.toFixed(2)+', '+d.jy2.toFixed(2)+')';

  const bar=$('flex-bar'),max=4095;
  bar.querySelector('.fill').style.width=(d.fx*100/max)+'%';
  bar.querySelector('.mark').style.left=(d.fxt*100/max)+'%';
  $('flex-v').textContent=d.fx;
  $('flex-r').textContent=d.fxr;
  $('flex-t').textContent=d.fxt;

  const rollP=Math.max(0,Math.min(100,(d.r+90)*100/180));
  const pitchP=Math.max(0,Math.min(100,(d.p+90)*100/180));
  $('roll-ring').style.setProperty('--p',rollP);
  $('pitch-ring').style.setProperty('--p',pitchP);
  $('roll-v').innerHTML=(d.r>=0?'+':'')+d.r.toFixed(1)+'&deg;';
  $('pitch-v').innerHTML=(d.p>=0?'+':'')+d.p.toFixed(1)+'&deg;';

  const wp=d.wp|0,slots=$('wpn').children;
  for(let i=0;i<4;i++)slots[i].classList.toggle('sel',i===wp);
  const st=$('arma-state');
  st.textContent=d.wa?'GESTO LISTO':'CAMBIANDO...';
  st.classList.toggle('busy',!d.wa);

  pushSample(d);

  if(d.vm)VMOT=d.vm;
  if(d.vb)VBAT=d.vb;
  $('mot-cap').textContent='TOPE '+VMOT.toFixed(2)+' V (100%) · BAT '+VBAT.toFixed(2)+' V';

  for(let i=0;i<4;i++){
    const v=d.m[i]|0;
    const zone=$('mz-'+(i+1));
    zone.querySelector('.fill').style.width=(v*100/255)+'%';
    $('mzw-'+(i+1)).textContent=motVolts(v);
    $('mzp-'+(i+1)).textContent=v;
  }

  $('out-sol').classList.toggle('live',!!d.sol);
  $('out-pel').classList.toggle('live',!!d.pel);

  if(d.dz&&d.dz!==lastDmgZone){
    const z=$('mz-'+d.dz);
    if(z){z.classList.remove('hit');void z.offsetWidth;z.classList.add('hit');}
  }
  lastDmgZone=d.dz||0;
  $('dmg-zone').textContent=d.dz?('M'+d.dz):'-';

  $('hp-v').textContent=d.hp;
  setNeo(d.hp);

  const ud=$('unity-dot');
  ud.classList.toggle('on',!!d.u);
  ud.classList.toggle('bad',!d.u);

  $('uptime-pill').textContent=fmtUptime(d.up);
  $('clients-pill').textContent=d.wc;

  frames++;
}

function render(){
  drawChart($('ch-acc'),[
    {data:buf.ax,color:'#ff5d3a'},
    {data:buf.ay,color:'#a8e063'},
    {data:buf.az,color:'#ffb454'}
  ],-2,2);
  drawChart($('ch-gyr'),[
    {data:buf.gx,color:'#ff5d3a'},
    {data:buf.gy,color:'#a8e063'},
    {data:buf.gz,color:'#ffb454'}
  ],-500,500);
  requestAnimationFrame(render);
}
requestAnimationFrame(render);

setInterval(()=>{
  const now=performance.now();
  const dt=(now-lastRate)/1000;
  $('rate-pill').textContent=Math.round(frames/dt);
  frames=0;lastRate=now;
},1000);

function connect(){
  const sseDot=$('sse-dot');
  const es=new EventSource('/stream');
  es.onopen=()=>{sseDot.classList.add('on');sseDot.classList.remove('bad');};
  es.onerror=()=>{
    sseDot.classList.remove('on');
    sseDot.classList.add('bad');
    es.close();
    setTimeout(connect,1500);
  };
  // Si una trama rompe el render, decirlo en la consola de la pagina en vez
  // de tragarlo: un catch vacio convierte cualquier excepcion en "0 Hz" mudo.
  es.onmessage=(e)=>{try{apply(JSON.parse(e.data));}catch(err){echo.textContent='! error de render: '+err.message;}};
}
connect();
for(let i=0;i<4;i++)paintSet(i);
</script>
</body>
</html>)HTML";

#endif
