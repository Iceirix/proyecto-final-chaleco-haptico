# Verificacion de compilacion de scripts C# del proyecto Unity (TSPProyect).
# Equivalente a "arduino-cli compile" pero para Unity: abre el proyecto en
# batch mode, recompila los scripts y reporta errores/warnings desde el log.
#
# REQUISITOS:
#   - El editor de Unity NO debe estar abierto en TSPProyect (bloqueo de archivos).
#   - Licencia personal ya activada (basta con haber abierto el proyecto una vez).
#
# Uso:  powershell -ExecutionPolicy Bypass -File scripts\unity-compile.ps1

$ErrorActionPreference = "Stop"

$unity = "C:\Program Files\Unity\Hub\Editor\6000.3.11f1\Editor\Unity.exe"
# Derivar la ruta del proyecto desde la ubicacion del script evita hardcodear
# el acento de "ChalecoHaptico", que PowerShell 5.1 corrompe al leer el .ps1.
$proj  = (Resolve-Path (Join-Path $PSScriptRoot "..\TSPProyect")).Path
$log   = "$env:TEMP\unity_compile.log"

if (-not (Test-Path $unity)) { throw "No encuentro Unity.exe en $unity" }
if (-not (Test-Path $proj))  { throw "No encuentro el proyecto en $proj" }
if (Test-Path $log) { Remove-Item $log -Force }

Write-Host "Compilando $proj ..." -ForegroundColor Cyan
# Unity.exe es una app de subsistema GUI: PowerShell NO espera a apps GUI con
# el operador "&" (sigue de inmediato y $LASTEXITCODE queda vacio). Start-Process
# -Wait fuerza la espera y entrega el ExitCode real.
# Las rutas llevan comillas explicitas: la ruta del repo tiene un espacio
# ("Proyecto Final") y Start-Process -ArgumentList no entrecomilla solo.
$argString = '-batchmode -quit -projectPath "{0}" -logFile "{1}"' -f $proj, $log
$proc = Start-Process -FilePath $unity -ArgumentList $argString -Wait -PassThru
$code = $proc.ExitCode

if (-not (Test-Path $log)) {
    throw "Unity no genero el log ($log). Editor abierto en el proyecto o fallo al lanzar."
}

$errors   = Select-String -Path $log -Pattern "error CS|Compilation failed" -ErrorAction SilentlyContinue
$warnings = Select-String -Path $log -Pattern "warning CS" -ErrorAction SilentlyContinue

Write-Host ""
if ($errors) {
    Write-Host "ERRORES de compilacion:" -ForegroundColor Red
    $errors | ForEach-Object { Write-Host "  $($_.Line)" }
} else {
    Write-Host "Sin errores de compilacion." -ForegroundColor Green
}
if ($warnings) {
    Write-Host "Warnings ($($warnings.Count)):" -ForegroundColor Yellow
    $warnings | ForEach-Object { Write-Host "  $($_.Line)" }
}

Write-Host ""
Write-Host "Exit code Unity: $code  (0 = OK)" -ForegroundColor Cyan
Write-Host "Log completo: $log"
exit $code
