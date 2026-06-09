#ifndef DASHBOARD_PAGE_H
#define DASHBOARD_PAGE_H

// ============================================================================
//  DashboardPage.h - HTML/CSS/JS del dashboard embebido.
//
//  Se sirve una sola vez por navegador y despues los datos llegan por
//  Server-Sent Events a /stream. El JS solo renderiza, no consume CPU del ESP.
// ============================================================================

const char DASHBOARD_HTML[] PROGMEM = R"HTML(<!doctype html>
<html lang="es">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Chaleco Haptico - Dashboard</title>
<style>
:root{
  --bg:#0a0e14;--panel:#131820;--panel2:#1a212c;--line:#2a3340;
  --text:#d8e0ea;--dim:#7a8896;--accent:#4cc9f0;--accent2:#f72585;
  --ok:#4ade80;--warn:#facc15;--bad:#ef4444;
  --mono:ui-monospace,"SF Mono",Menlo,Consolas,monospace;
}
*{box-sizing:border-box}
body{margin:0;background:var(--bg);color:var(--text);font-family:system-ui,-apple-system,sans-serif;font-size:14px}
header{display:flex;flex-wrap:wrap;align-items:center;gap:12px;padding:12px 20px;background:var(--panel);border-bottom:1px solid var(--line)}
header h1{font-size:15px;margin:0;font-weight:600;letter-spacing:.5px}
.pill{padding:4px 10px;border-radius:999px;font-size:12px;background:var(--panel2);display:inline-flex;align-items:center;gap:6px;font-family:var(--mono)}
.dot{width:8px;height:8px;border-radius:50%;background:#3a4250;transition:background .15s,box-shadow .15s}
.dot.on{background:var(--ok);box-shadow:0 0 8px var(--ok)}
.dot.warn{background:var(--warn);box-shadow:0 0 8px var(--warn)}
.dot.bad{background:var(--bad)}
main{display:grid;gap:14px;padding:14px;grid-template-columns:repeat(12,1fr)}
.panel{background:var(--panel);border:1px solid var(--line);border-radius:10px;padding:14px;min-width:0}
.panel h2{font-size:11px;letter-spacing:1.2px;text-transform:uppercase;color:var(--dim);margin:0 0 12px 0;font-weight:600;display:flex;align-items:center;gap:8px}
.panel h2 .tag{font-size:10px;background:var(--panel2);padding:2px 6px;border-radius:4px;letter-spacing:.5px;color:var(--text)}
.col-3{grid-column:span 3}.col-4{grid-column:span 4}.col-6{grid-column:span 6}.col-8{grid-column:span 8}.col-12{grid-column:span 12}
@media (max-width:1100px){.col-3,.col-4{grid-column:span 6}.col-6,.col-8{grid-column:span 12}}
@media (max-width:640px){.col-3,.col-4,.col-6,.col-8{grid-column:span 12}}

.btn-grid{display:grid;grid-template-columns:repeat(2,1fr);gap:10px}
.btn{background:var(--panel2);border-radius:8px;padding:10px;display:flex;align-items:center;gap:10px;border:1px solid var(--line);transition:border-color .15s,box-shadow .15s,background .15s}
.btn.on{border-color:var(--accent);background:#16222c;box-shadow:0 0 0 1px var(--accent),0 0 16px rgba(76,201,240,.35)}
.btn .label{font-family:var(--mono);font-size:12px}
.btn .sub{color:var(--dim);font-size:11px}

.joy-pair{display:grid;grid-template-columns:1fr 1fr;gap:14px}
.joy{position:relative;aspect-ratio:1;background:radial-gradient(circle,#161d27 0%,#0e131b 100%);border-radius:50%;border:1px solid var(--line);overflow:hidden}
.joy::after{content:"";position:absolute;inset:50% 0 auto 0;height:1px;background:var(--line)}
.joy::before{content:"";position:absolute;left:50%;top:0;bottom:0;width:1px;background:var(--line)}
.joy .dot-xy{position:absolute;width:14px;height:14px;border-radius:50%;background:var(--accent);transform:translate(-50%,-50%);box-shadow:0 0 12px var(--accent);transition:left .05s linear,top .05s linear;left:50%;top:50%}
.joy-label{text-align:center;font-family:var(--mono);font-size:11px;color:var(--dim);margin-top:6px}

.bar{position:relative;height:14px;background:var(--panel2);border:1px solid var(--line);border-radius:6px;overflow:hidden}
.bar>.fill{position:absolute;inset:0 auto 0 0;background:linear-gradient(90deg,var(--accent),var(--accent2));width:0%;transition:width .05s linear}
.bar>.mark{position:absolute;top:-2px;bottom:-2px;width:2px;background:var(--warn);box-shadow:0 0 4px var(--warn)}

.kv{display:grid;grid-template-columns:1fr auto;gap:6px 12px;font-family:var(--mono);font-size:12px}
.kv .k{color:var(--dim)}.kv .v{text-align:right}

.chest{display:grid;grid-template-columns:1fr 1fr;gap:10px}
.pad{background:var(--panel2);border:1px solid var(--line);border-radius:10px;padding:10px;transition:border-color .1s,box-shadow .1s}
.pad.on{border-color:var(--accent2);box-shadow:0 0 12px rgba(247,37,133,.35)}
.pad .num{font-family:var(--mono);font-size:11px;color:var(--dim);display:flex;justify-content:space-between}
.pad .level{height:8px;background:var(--bg);border-radius:4px;overflow:hidden;margin-top:8px}
.pad .level>div{height:100%;width:0%;background:linear-gradient(90deg,var(--accent),var(--accent2));transition:width .05s linear}

.neo{display:grid;grid-template-columns:repeat(8,1fr);gap:6px;margin-top:6px}
.neo div{aspect-ratio:1;border-radius:50%;background:#0e131b;border:1px solid var(--line);transition:background .1s,box-shadow .1s,border-color .1s}

.gauges{display:grid;grid-template-columns:1fr 1fr;gap:14px}
.gauge{text-align:center}
.ring{width:104px;height:104px;border-radius:50%;margin:0 auto;background:conic-gradient(from -90deg,var(--accent) calc(var(--p) * 1%),#0e131b 0);display:grid;place-items:center;transition:background .1s}
.ring>div{width:82px;height:82px;border-radius:50%;background:var(--panel);display:grid;place-items:center;font-family:var(--mono);font-size:17px;font-weight:600}

canvas{display:block;width:100%;height:130px;background:#0e131b;border-radius:8px}
.legend{font-size:11px;color:var(--dim);margin-top:6px;display:flex;gap:14px;flex-wrap:wrap}
.legend .sw{display:inline-block;width:10px;height:10px;border-radius:2px;margin-right:4px;vertical-align:middle}

.hp-row{display:flex;align-items:center;gap:12px;margin-bottom:8px}
.hp-row .hp-num{font-family:var(--mono);font-size:24px;font-weight:600}
.hp-row .hp-lbl{color:var(--dim);font-size:11px;letter-spacing:1px}

.dmg-pulse{position:absolute;inset:0;border-radius:10px;background:var(--bad);opacity:0;pointer-events:none}
.dmg-pulse.flash{animation:flash .6s ease-out}
@keyframes flash{0%{opacity:.5}100%{opacity:0}}
.panel{position:relative}

/* Control manual */
.ctrl{display:grid;gap:12px}
.slider-row{display:grid;grid-template-columns:90px 1fr 104px;align-items:center;gap:10px}
.slider-row .lab{font-family:var(--mono);font-size:12px;color:var(--dim)}
.slider-row .val{font-family:var(--mono);font-size:13px;text-align:right}
input[type=range]{-webkit-appearance:none;appearance:none;height:6px;border-radius:3px;background:var(--panel2);border:1px solid var(--line);outline:none}
input[type=range]::-webkit-slider-thumb{-webkit-appearance:none;width:18px;height:18px;border-radius:50%;background:var(--accent);box-shadow:0 0 8px var(--accent);cursor:pointer}
input[type=range]::-moz-range-thumb{width:18px;height:18px;border:none;border-radius:50%;background:var(--accent);box-shadow:0 0 8px var(--accent);cursor:pointer}
.cmd-btns{display:flex;flex-wrap:wrap;gap:8px}
.cbtn{font-family:var(--mono);font-size:12px;padding:8px 12px;border-radius:8px;background:var(--panel2);border:1px solid var(--line);color:var(--text);cursor:pointer;transition:border-color .1s,background .1s}
.cbtn:hover{border-color:var(--accent)}
.cbtn:active{background:#16222c;border-color:var(--accent)}
.cbtn.bad{border-color:#5a2030}.cbtn.bad:hover{border-color:var(--bad)}
.raw-row{display:flex;gap:8px}
.raw-row input[type=text]{flex:1;font-family:var(--mono);font-size:13px;padding:8px 10px;border-radius:8px;background:var(--panel2);border:1px solid var(--line);color:var(--text);outline:none}
.raw-row input[type=text]:focus{border-color:var(--accent)}
.cmd-echo{font-family:var(--mono);font-size:11px;color:var(--dim);min-height:14px}
</style>
</head>
<body>
<header>
  <h1>CHALECO HAPTICO · TSM 2026-II</h1>
  <span class="pill"><span class="dot" id="sse-dot"></span>Dashboard</span>
  <span class="pill"><span class="dot" id="unity-dot"></span>Unity</span>
  <span class="pill" id="uptime-pill">00:00:00</span>
  <span class="pill" id="rate-pill">0 Hz</span>
  <span class="pill" id="clients-pill">0 STA</span>
</header>
<main>
  <section class="panel col-3">
    <h2>Entradas digitales <span class="tag">PB1-PB4</span></h2>
    <div class="btn-grid">
      <div class="btn" id="btn-jump"><div class="dot"></div><div><div class="label">PB1 · SALTO</div><div class="sub">jump</div></div></div>
      <div class="btn" id="btn-weapon"><div class="dot"></div><div><div class="label">PB2 · reservado</div><div class="sub">pb2</div></div></div>
      <div class="btn" id="btn-b3"><div class="dot"></div><div><div class="label">PB3</div><div class="sub">reservado</div></div></div>
      <div class="btn" id="btn-b4"><div class="dot"></div><div><div class="label">PB4</div><div class="sub">reservado</div></div></div>
    </div>
  </section>

  <section class="panel col-3">
    <h2>Joysticks <span class="tag">analog</span></h2>
    <div class="joy-pair">
      <div>
        <div class="joy" id="joy1"><div class="dot-xy"></div></div>
        <div class="joy-label">J1 <span id="j1xy">0.00, 0.00</span></div>
      </div>
      <div>
        <div class="joy" id="joy2"><div class="dot-xy"></div></div>
        <div class="joy-label">J2 <span id="j2xy">0.00, 0.00</span></div>
      </div>
    </div>
  </section>

  <section class="panel col-3">
    <h2>Sensor flex · Gatillo</h2>
    <div class="btn" id="btn-shoot" style="margin-bottom:12px"><div class="dot"></div><div><div class="label">SHOOT</div><div class="sub">flex bajo umbral</div></div></div>
    <div class="bar" id="flex-bar" style="margin-bottom:10px"><div class="fill"></div><div class="mark"></div></div>
    <div class="kv">
      <span class="k">Valor</span><span class="v" id="flex-v">0</span>
      <span class="k">Reposo</span><span class="v" id="flex-r">0</span>
      <span class="k">Umbral</span><span class="v" id="flex-t">0</span>
    </div>
  </section>

  <section class="panel col-3">
    <h2>IMU · Orientacion <span class="tag">BMI160</span></h2>
    <div class="gauges">
      <div class="gauge">
        <div class="ring" id="roll-ring" style="--p:50"><div id="roll-v">+0.0&deg;</div></div>
        <div style="margin-top:6px;color:var(--dim);font-size:11px;letter-spacing:1px">ROLL</div>
      </div>
      <div class="gauge">
        <div class="ring" id="pitch-ring" style="--p:50"><div id="pitch-v">+0.0&deg;</div></div>
        <div style="margin-top:6px;color:var(--dim);font-size:11px;letter-spacing:1px">PITCH</div>
      </div>
    </div>
    <div class="btn" id="btn-arma" style="margin-top:14px"><div class="dot"></div><div><div class="label">ARMA <span id="arma-idx">1</span>/4</div><div class="sub" id="arma-state">gesto listo</div></div></div>
  </section>

  <section class="panel col-8">
    <h2>IMU · Telemetria en tiempo real</h2>
    <canvas id="ch-acc"></canvas>
    <div class="legend">
      <span><span class="sw" style="background:#ef4444"></span>AccX</span>
      <span><span class="sw" style="background:#4ade80"></span>AccY</span>
      <span><span class="sw" style="background:#4cc9f0"></span>AccZ</span>
      <span style="margin-left:auto;color:var(--dim)">g · escala +/-2g</span>
    </div>
    <canvas id="ch-gyr" style="margin-top:10px"></canvas>
    <div class="legend">
      <span><span class="sw" style="background:#ef4444"></span>GyrX</span>
      <span><span class="sw" style="background:#4ade80"></span>GyrY</span>
      <span><span class="sw" style="background:#4cc9f0"></span>GyrZ</span>
      <span style="margin-left:auto;color:var(--dim)">&deg;/s · escala +/-500&deg;/s</span>
    </div>
  </section>

  <section class="panel col-4">
    <h2>Salidas · Motores vibradores <span class="tag">PWM</span></h2>
    <div class="chest">
      <div class="pad" id="mot-1"><div class="num"><span>M1 · Frente Izq</span><span class="pwm">0</span></div><div class="level"><div></div></div></div>
      <div class="pad" id="mot-2"><div class="num"><span>M2 · Frente Der</span><span class="pwm">0</span></div><div class="level"><div></div></div></div>
      <div class="pad" id="mot-3"><div class="num"><span>M3 · Espalda Izq</span><span class="pwm">0</span></div><div class="level"><div></div></div></div>
      <div class="pad" id="mot-4"><div class="num"><span>M4 · Espalda Der</span><span class="pwm">0</span></div><div class="level"><div></div></div></div>
    </div>
    <div class="legend" id="mot-cap" style="margin-top:10px">Recorte PWM por voltaje</div>
    <div class="dmg-pulse" id="dmg-pulse"></div>
  </section>

  <section class="panel col-4">
    <h2>Salidas · Actuadores</h2>
    <div class="btn" id="out-sol" style="margin-bottom:10px"><div class="dot"></div><div><div class="label">SOLENOIDE</div><div class="sub">retroceso del guante</div></div></div>
    <div class="btn" id="out-pel"><div class="dot"></div><div><div class="label">PELTIER</div><div class="sub">golpe termico (pulso)</div></div></div>
    <div class="kv" style="margin-top:14px">
      <span class="k">Zona dano</span><span class="v" id="dmg-zone">-</span>
    </div>
  </section>

  <section class="panel col-4">
    <h2>Vida <span class="tag">NeoPixel x8</span></h2>
    <div class="hp-row">
      <div>
        <div class="hp-num" id="hp-v">100%</div>
        <div class="hp-lbl">HP</div>
      </div>
      <div style="flex:1">
        <div class="bar"><div class="fill" id="hp-bar" style="width:100%"></div></div>
      </div>
    </div>
    <div class="neo" id="neo"></div>
  </section>

  <section class="panel col-12">
    <h2>Control manual <span class="tag">envia comandos al ESP</span></h2>
    <div class="ctrl">
      <div class="slider-row">
        <span class="lab">M1 PWM</span>
        <input type="range" min="0" max="255" value="0" id="sl-m1" data-key="M1">
        <span class="val" id="sv-m1">0</span>
      </div>
      <div class="slider-row">
        <span class="lab">M2 PWM</span>
        <input type="range" min="0" max="255" value="0" id="sl-m2" data-key="M2">
        <span class="val" id="sv-m2">0</span>
      </div>
      <div class="slider-row">
        <span class="lab">M3 PWM</span>
        <input type="range" min="0" max="255" value="0" id="sl-m3" data-key="M3">
        <span class="val" id="sv-m3">0</span>
      </div>
      <div class="slider-row">
        <span class="lab">M4 PWM</span>
        <input type="range" min="0" max="255" value="0" id="sl-m4" data-key="M4">
        <span class="val" id="sv-m4">0</span>
      </div>
      <div class="slider-row">
        <span class="lab">HP vida</span>
        <input type="range" min="0" max="100" value="100" id="sl-hp" data-key="HP">
        <span class="val" id="sv-hp">100</span>
      </div>
      <div class="cmd-btns">
        <button class="cbtn" data-cmd="SOL=1">SOLENOIDE</button>
        <button class="cbtn" data-cmd="DMG=1">DANO M1</button>
        <button class="cbtn" data-cmd="DMG=2">DANO M2</button>
        <button class="cbtn" data-cmd="DMG=3">DANO M3</button>
        <button class="cbtn" data-cmd="DMG=4">DANO M4</button>
        <button class="cbtn" data-cmd="PELT=1">PELTIER</button>
        <button class="cbtn bad" id="btn-stop">APAGAR MOTORES</button>
      </div>
      <div class="raw-row">
        <input type="text" id="raw-cmd" placeholder="Comando crudo, ej: M1=128;HP=80;DMG=2" autocomplete="off" spellcheck="false">
        <button class="cbtn" id="btn-send">ENVIAR</button>
      </div>
      <div class="cmd-echo" id="cmd-echo"></div>
    </div>
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
  ctx.strokeStyle='#2a3340';ctx.lineWidth=1;
  ctx.beginPath();
  for(let i=1;i<4;i++){const y=(h*i)/4;ctx.moveTo(0,y);ctx.lineTo(w,y);}
  ctx.stroke();
  ctx.strokeStyle='#3a4250';ctx.setLineDash([2,3]);
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

function setBtn(el,on){
  if(!el)return;
  el.classList.toggle('on',!!on);
  const dot=el.querySelector('.dot');
  if(dot){dot.classList.toggle('on',!!on);}
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
  const g=hp>50?255:Math.round(255*(hp/50));
  for(let i=0;i<8;i++){
    const el=neoEl.children[i];
    if(i<lit){
      const c=`rgb(${r},${g},0)`;
      el.style.background=c;
      el.style.boxShadow=`0 0 8px ${c}`;
      el.style.borderColor=c;
    }else{
      el.style.background='';el.style.boxShadow='';el.style.borderColor='';
    }
  }
}

function apply(d){
  setBtn($('btn-jump'),d.j);
  setBtn($('btn-weapon'),d.w);
  setBtn($('btn-b3'),d.b3);
  setBtn($('btn-b4'),d.b4);
  setBtn($('btn-shoot'),d.s);

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
  $('arma-idx').textContent=((d.wp|0)+1);
  $('arma-state').textContent=d.wa?'gesto listo':'cambiando...';
  setBtn($('btn-arma'),!d.wa);

  pushSample(d);

  if(d.vm)VMOT=d.vm;
  if(d.vb)VBAT=d.vb;
  $('mot-cap').textContent='Tope '+VMOT.toFixed(2)+' V (100%) · bateria '+VBAT.toFixed(2)+' V';

  for(let i=0;i<4;i++){
    const pad=$('mot-'+(i+1));
    const v=d.m[i]|0;
    pad.querySelector('.level>div').style.width=(v*100/255)+'%';
    pad.querySelector('.pwm').textContent=v+' · '+motVolts(v);
    pad.classList.toggle('on',v>0);
  }

  setBtn($('out-sol'),d.sol);
  setBtn($('out-pel'),d.pel);

  if(d.dz&&d.dz!==lastDmgZone){
    const p=$('dmg-pulse');
    p.classList.remove('flash');void p.offsetWidth;p.classList.add('flash');
  }
  lastDmgZone=d.dz||0;
  $('dmg-zone').textContent=d.dz?('M'+d.dz):'-';

  $('hp-v').textContent=d.hp+'%';
  $('hp-bar').style.width=d.hp+'%';
  setNeo(d.hp);

  const ud=$('unity-dot');
  ud.classList.toggle('on',!!d.u);
  ud.classList.toggle('bad',!d.u);

  $('uptime-pill').textContent=fmtUptime(d.up);
  $('clients-pill').textContent=d.wc+' STA';

  frames++;
}

function render(){
  drawChart($('ch-acc'),[
    {data:buf.ax,color:'#ef4444'},
    {data:buf.ay,color:'#4ade80'},
    {data:buf.az,color:'#4cc9f0'}
  ],-2,2);
  drawChart($('ch-gyr'),[
    {data:buf.gx,color:'#ef4444'},
    {data:buf.gy,color:'#4ade80'},
    {data:buf.gz,color:'#4cc9f0'}
  ],-500,500);
  requestAnimationFrame(render);
}
requestAnimationFrame(render);

setInterval(()=>{
  const now=performance.now();
  const dt=(now-lastRate)/1000;
  $('rate-pill').textContent=Math.round(frames/dt)+' Hz';
  frames=0;lastRate=now;
},1000);

// ---- Control manual: envio de comandos al ESP por /cmd ----
const echo=$('cmd-echo');
function sendCmd(str){
  fetch('/cmd?c='+encodeURIComponent(str))
    .then(r=>{echo.textContent='> '+str+(r.ok?'  [OK]':'  [ERR '+r.status+']');})
    .catch(()=>{echo.textContent='> '+str+'  [sin conexion]';});
}

// Throttle por slider: limita el flujo al arrastrar pero garantiza el valor final.
function bindSlider(id,valId){
  const sl=$(id),vEl=$(valId);
  let last=0,pending=null,timer=null;
  const key=sl.dataset.key;
  const isMotor=key[0]==='M';
  const showVal=()=>{vEl.textContent=isMotor?(sl.value+' · '+motVolts(sl.value)):sl.value;};
  showVal();
  const flush=()=>{timer=null;if(pending!==null){sendCmd(key+'='+pending);pending=null;}};
  sl.addEventListener('input',()=>{
    showVal();
    const now=performance.now();
    if(now-last>70){last=now;sendCmd(key+'='+sl.value);}
    else{pending=sl.value;if(!timer)timer=setTimeout(flush,80);}
  });
  // valor definitivo al soltar
  sl.addEventListener('change',()=>{if(timer){clearTimeout(timer);timer=null;}sendCmd(key+'='+sl.value);});
}
['m1','m2','m3','m4','hp'].forEach(k=>bindSlider('sl-'+k,'sv-'+k));

document.querySelectorAll('.cbtn[data-cmd]').forEach(b=>{
  b.addEventListener('click',()=>sendCmd(b.dataset.cmd));
});

$('btn-stop').addEventListener('click',()=>{
  ['m1','m2','m3','m4'].forEach(k=>{$('sl-'+k).value=0;$('sv-'+k).textContent='0 · '+motVolts(0);});
  sendCmd('M1=0;M2=0;M3=0;M4=0');
});

const rawIn=$('raw-cmd');
function sendRaw(){const c=rawIn.value.trim();if(c)sendCmd(c);}
$('btn-send').addEventListener('click',sendRaw);
rawIn.addEventListener('keydown',e=>{if(e.key==='Enter')sendRaw();});

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
  es.onmessage=(e)=>{try{apply(JSON.parse(e.data));}catch(_){}};
}
connect();
</script>
</body>
</html>)HTML";

#endif
