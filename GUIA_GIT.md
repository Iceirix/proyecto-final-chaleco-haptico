# Guía rápida de Git — Proyecto Final Chaleco Háptico

Guía para que el equipo trabaje en el repositorio sin pisarse el trabajo.
Repo: https://github.com/Iceirix/proyecto-final-chaleco-haptico

---

## 0. Una sola vez: configuración inicial

Instala [Git](https://git-scm.com/downloads) y configura tu nombre y correo (los que aparecen en tus commits):

```bash
git config --global user.name "Tu Nombre"
git config --global user.email "tucorreo@ejemplo.com"
```

> Necesitas estar agregado como colaborador del repo (es privado). Pídele acceso a Luis si no lo tienes.

---

## 1. Clonar el repositorio (solo la primera vez)

Descarga una copia completa del proyecto a tu computadora:

```bash
git clone https://github.com/Iceirix/proyecto-final-chaleco-haptico.git
cd proyecto-final-chaleco-haptico
```

A partir de aquí trabajas dentro de esa carpeta.

---

## 2. Antes de empezar a trabajar: traer los últimos cambios

**Siempre** actualiza tu copia antes de ponerte a trabajar, para tener lo que subieron los demás:

```bash
git pull
```

---

## 3. Ver en qué estado estás

```bash
git status        # qué archivos cambiaste
git diff          # ver exactamente qué líneas cambiaron
```

---

## 4. Guardar tus cambios (commit) y subirlos

Cuando termines un avance:

```bash
git add .                          # marca todos tus cambios
git commit -m "Describe tu cambio" # guarda un punto en la historia
git pull                           # trae cambios nuevos antes de subir
git push                           # sube tus cambios a GitHub
```

> Escribe mensajes de commit claros: `"Agrega patron de vibracion al recibir dano"` es mejor que `"cambios"`.

---

## 5. Flujo de trabajo recomendado (con ramas)

Para no romper lo que ya funciona, cada quien trabaja en su propia **rama** y luego se integra.

### Crear tu rama para una tarea

```bash
git checkout main        # parate en la rama principal
git pull                 # actualizala
git checkout -b mi-tarea # crea y cambiate a una rama nueva
```

Nombra la rama según lo que harás, por ejemplo: `imu-calibracion`, `dashboard-hp`, `pcb-revision`.

### Trabaja, haz commits en tu rama

```bash
git add .
git commit -m "Avance de mi tarea"
```

### Sube tu rama a GitHub

```bash
git push -u origin mi-tarea   # la primera vez
git push                      # las siguientes
```

### Integrar a `main` con Pull Request (recomendado)

1. Entra al repo en GitHub.
2. Aparecerá un botón **"Compare & pull request"**. Haz clic.
3. Describe tu cambio y crea el Pull Request.
4. Que un compañero lo revise y lo apruebe (**Merge**).

Así `main` siempre se mantiene estable y todos ven qué se cambió.

### Después de integrar, vuelve a main

```bash
git checkout main
git pull
```

---

## 6. Ciclo diario resumido

```bash
git checkout main && git pull        # 1. ponte al día
git checkout -b nombre-tarea         # 2. rama para tu tarea
# ... trabajas ...
git add . && git commit -m "..."     # 3. guarda avances
git push -u origin nombre-tarea      # 4. sube tu rama
# 5. abre Pull Request en GitHub y haz merge
```

---

## 7. Si algo sale mal

| Situación | Comando |
|---|---|
| Quiero descartar cambios de un archivo (¡ojo, se pierden!) | `git checkout -- archivo` |
| Me equivoqué de rama y aún no hice commit | `git stash` (guarda), cambias de rama, `git stash pop` (recupera) |
| Ver historial de commits | `git log --oneline --graph --all` |
| El push falla porque hay cambios nuevos en el remoto | `git pull` y vuelve a `git push` |

### Conflictos al hacer pull/merge

Si Git dice **CONFLICT**, dos personas editaron lo mismo. Git marca el archivo así:

```
<<<<<<< HEAD
tu versión
=======
la versión del otro
>>>>>>> rama
```

Edita el archivo dejando la versión correcta, borra las líneas `<<<`, `===`, `>>>`, y luego:

```bash
git add archivo
git commit
```

---

## Reglas de oro del equipo

1. **`git pull` antes de empezar** y antes de cada `git push`.
2. **Trabaja en ramas**, no directo en `main`.
3. **Commits pequeños y descritos** — es más fácil encontrar errores.
4. **No subas archivos pesados o temporales** (ya hay un `.gitignore` que excluye logs y comprimidos).
5. Si tienes dudas, pregunta antes de forzar nada (`git push --force` puede borrar trabajo de otros).
