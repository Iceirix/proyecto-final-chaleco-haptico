# -*- coding: utf-8 -*-
# Generador parametrico del control ergonomico (estilo nunchuk).
# Produce: control_derecho.stl, control_izquierdo.stl, tapa_trasera.stl
#
# Componentes alojados (medidas investigadas):
# - Joystick KY-023: PCB 34 x 26 mm, 4 barrenos Ø3.2 en reticula 26.7 x 20.3 mm,
#   header de 5 pines en un borde corto. Se atornilla con M3 autorroscante.
# - Push boton 12x12x7.3 mm con capuchon redondo Ø13 (vastago Ø7 sobresale ~4 mm).
# - IMU GY-BMI160: PCB 13 x 18 mm, sin barrenos -> charola con paredes en el piso.
#
# Requiere: pip install trimesh manifold3d numpy scipy numpy-stl

import numpy as np
import trimesh
from trimesh.transformations import rotation_matrix, translation_matrix
from scipy.interpolate import PchipInterpolator

OUT_DIR = r"E:\Documents\ChalecoHáptico\Proyecto Final\Control"

# ---------------- Parametros principales (mm) ----------------
TILT = np.radians(8.0)        # inclinacion de la cara superior hacia la palma
TOP_FACE = np.array([0.0, 5.5, 127.0])  # punto por el que pasa el plano superior

# KY-023
JOY_PCB_X, JOY_PCB_Y = 34.0, 26.0
JOY_HOLE_DX, JOY_HOLE_DY = 26.7, 20.3   # reticula de barrenos
POCKET_X, POCKET_Y = 35.0, 27.0          # bolsillo con holgura 0.5 por lado
POCKET_DEPTH = 9.0
BOSS_H, BOSS_R = 4.5, 3.5
PILOT_R = 1.25      # Ø2.5: el M3x6 milimetrico forma su propia rosca en PLA
                    # (engrane: 6 - 1.6 de PCB = 4.4 mm dentro del poste)

# Boton 12x12 (cuerpo 12x12x3, capuchon Ø13 x 6 de alto, altura total 12)
BTN_Z = (94.0, 76.0)        # alturas: indice y dedo medio (cara frontal, y+)
BTN_BODY = 12.8             # asiento cuadrado de registro para el cuerpo 12x12
BTN_CAP_R = 6.8             # Ø13.6: paso libre del capuchon Ø13 por el panel
BTN_PANEL = 2.0             # espesor del panel frente al boton
# El cuerpo 12x12 no pasa por el barreno Ø13.6 (su diagonal es 17): el panel
# lo retiene por las esquinas y solo el capuchon sobresale.

# Tapa trasera desmontable: toda la espalda (z 8..100, y < -10) se separa como
# concha para poder meter botones, IMU y cables con las manos.
# Montaje SIN presion (ambas piezas son rigidas): dos ganchos arriba que se
# enganchan tras la pared insertando la tapa inclinada, se rota para cerrar
# y se atornilla abajo con 2 M3x6. Sin pestana perimetral.
COVER_Z = (8.0, 100.0)      # rango vertical de la tapa
COVER_Y = -10.0             # plano de corte: lo que queda atras es la tapa
FIT_CLEAR = 0.5             # holgura gancho/guia contra la pared (entra suelto)
TAB_X, TAB_W = 8.0, 9.0     # ganchos superiores: |x| de centro y ancho
GUIDE_Z = (62.0, 80.0)      # guias laterales de alineacion (pegadas al cuerpo)
COVER_BOSS = (8.0, 11.0)    # |x| y z de los postes de tornillo
COVER_PILOT_R = 1.25        # Ø2.5: rosca formada por el M3x6 milimetrico
# La tapa lleva caja escariada Ø6.2 x 1.6 para la cabeza: con la pared de
# ~3.4 mm el M3x6 engrana ~4 mm dentro del poste.

# IMU GY-BMI160 (13 x 18) charola interior 19 x 14
IMU_IN_X, IMU_IN_Y = 19.0, 14.0
IMU_WALL, IMU_H = 1.8, 4.0
IMU_CY = 4.0                # centro Y de la charola

CABLE_HOLE = (0.0, -8.0)    # salida de cable en el piso
CABLE_R = 4.0

N_SEG = 96      # resolucion angular del loft
SUPER_N = 2.5   # exponente de la superelipse (2=elipse, mas alto = mas cuadrado)

# Perfil exterior: (z, semiancho_x, semiprofundidad_y, offset_y)
OUTER = [(0, 21, 17, 0), (12, 23, 19, 0), (45, 24, 20.5, 1), (72, 21.5, 18, 2.5),
         (98, 25, 22, 4.5), (118, 28, 24.5, 5.5), (134, 27.5, 24, 5.5)]
# Cavidad interior (pared ~3 mm, piso 5 mm, techo en z=112 bajo el joystick)
INNER = [(5, 18, 14, 0), (12, 20, 16, 0), (45, 21, 17.5, 1), (72, 18.5, 15, 2.5),
         (98, 22, 19, 4.5), (112, 23, 20, 5)]


def loft(sections, n_seg=N_SEG, n_exp=SUPER_N, step=2.0):
    """Solido lofteado de superelipses con interpolacion suave (PCHIP)."""
    zs0, a0, b0, yc0 = (np.array(c, float) for c in zip(*sections))
    fa, fb, fyc = (PchipInterpolator(zs0, v) for v in (a0, b0, yc0))
    zs = np.arange(zs0[0], zs0[-1] + step / 2, step)
    zs[-1] = zs0[-1]
    t = np.linspace(0, 2 * np.pi, n_seg, endpoint=False)
    ux = np.sign(np.cos(t)) * np.abs(np.cos(t)) ** (2 / n_exp)
    uy = np.sign(np.sin(t)) * np.abs(np.sin(t)) ** (2 / n_exp)
    verts, faces = [], []
    for z in zs:
        verts.append(np.column_stack([fa(z) * ux, fyc(z) + fb(z) * uy,
                                      np.full(n_seg, z)]))
    verts = np.vstack(verts)
    nr = len(zs)
    for i in range(nr - 1):
        for j in range(n_seg):
            j2 = (j + 1) % n_seg
            a_, b_, c_, d_ = (i * n_seg + j, i * n_seg + j2,
                              (i + 1) * n_seg + j, (i + 1) * n_seg + j2)
            faces += [[a_, b_, d_], [a_, d_, c_]]
    # tapas (abanico al centroide)
    cb = len(verts); verts = np.vstack([verts, [0, fyc(zs[0]), zs[0]]])
    ct = len(verts); verts = np.vstack([verts, [0, fyc(zs[-1]), zs[-1]]])
    for j in range(n_seg):
        j2 = (j + 1) % n_seg
        faces.append([cb, j2, j])
        faces.append([ct, (nr - 1) * n_seg + j, (nr - 1) * n_seg + j2])
    m = trimesh.Trimesh(vertices=verts, faces=faces, process=True)
    m.fix_normals()
    assert m.is_watertight and m.volume > 0
    return m


def box(size, center, transform=None):
    m = trimesh.creation.box(extents=size)
    m.apply_translation(center)
    if transform is not None:
        m.apply_transform(transform)
    return m


def cyl(r, h, center, axis='z'):
    m = trimesh.creation.cylinder(radius=r, height=h, sections=48)
    if axis == 'y':
        m.apply_transform(rotation_matrix(np.pi / 2, [1, 0, 0]))
    m.apply_translation(center)
    return m


def diff(a, b):
    return trimesh.boolean.difference([a] + (b if isinstance(b, list) else [b]),
                                      engine='manifold')


def union(parts):
    return trimesh.boolean.union(parts, engine='manifold')


def front_y(z):
    """Y de la superficie frontal exterior en x=0 a la altura z."""
    zs, a, b, yc = (np.array(c, float) for c in zip(*OUTER))
    return PchipInterpolator(zs, yc)(z) + PchipInterpolator(zs, b)(z)


def joy_frame():
    """Transformacion al marco local de la cara superior inclinada."""
    return translation_matrix(TOP_FACE) @ rotation_matrix(TILT, [1, 0, 0])


def build_body():
    outer = loft(OUTER)
    inner = loft(INNER)

    # Corte del plano superior inclinado (la normal se inclina hacia y-)
    T = joy_frame()
    topcut = box([200, 200, 100], [0, 0, 50], T)
    outer = diff(outer, topcut)
    shell = diff(outer, inner)

    # ---- Joystick KY-023 en la cara superior ----
    pocket = box([POCKET_X, POCKET_Y, POCKET_DEPTH + 30],
                 [0, 0, (30 - POCKET_DEPTH) / 2], T)
    # ranura para jumpers dupont en el header de 5 pines (borde -X), abierta
    # hacia arriba; solo llega al piso del bolsillo para no perforar la pared
    # exterior donde el mango se estrecha
    slot = box([6.5, 20, 40], [-(POCKET_X / 2 + 3.25), 0, 10], T)
    # canal de cables a la cavidad, retraido hacia el centro (lejos de la
    # pared exterior y sin tocar los postes del joystick)
    chan = box([5.5, 13, 16], [-18.25, 0, -16], T)
    shell = diff(shell, [pocket, slot, chan])

    # los postes penetran 1 mm en el piso (la tangencia exacta genera aristas
    # no-manifold al redondear el STL a float32)
    bosses = [cyl(BOSS_R, BOSS_H + 1, [sx * JOY_HOLE_DX / 2, sy * JOY_HOLE_DY / 2,
                                       -POCKET_DEPTH + (BOSS_H - 1) / 2])
              for sx in (-1, 1) for sy in (-1, 1)]
    for m in bosses:
        m.apply_transform(T)
    shell = union([shell] + bosses)
    pilots = [cyl(PILOT_R, 9, [sx * JOY_HOLE_DX / 2, sy * JOY_HOLE_DY / 2,
                               -POCKET_DEPTH + BOSS_H - 4])
              for sx in (-1, 1) for sy in (-1, 1)]
    for m in pilots:
        m.apply_transform(T)
    shell = diff(shell, pilots)

    # ---- Botones 12x12 en la cara frontal ----
    pads, cuts = [], []
    for z in BTN_Z:
        ys = float(front_y(z))
        pad = box([22, 16, 22], [0, ys - 8, z])
        pads.append(trimesh.boolean.intersection([pad, loft(OUTER)],
                                                 engine='manifold'))
        # asiento de registro 12.8 justo tras el panel (sujeta el cuerpo 12x12)
        cuts.append(box([BTN_BODY, 7, BTN_BODY], [0, ys - BTN_PANEL - 3.5, z]))
        # camara amplia detras del asiento: espacio para soldar cables a las
        # patas laterales e insertar el boton ya cableado desde la cavidad
        cuts.append(box([19, 13, 19], [0, ys - 11.5, z]))
        # paso libre del capuchon Ø13 a traves del panel
        cuts.append(cyl(BTN_CAP_R, 10, [0, ys + 1, z], axis='y'))
    shell = diff(union([shell] + pads), cuts)

    # ---- Charola para la IMU (GY-BMI160 13x18, acostada, abierta hacia la puerta) ----
    ix, iy = IMU_IN_X / 2 + IMU_WALL / 2, IMU_IN_Y / 2
    tray = [box([IMU_WALL, IMU_IN_Y + 2 * IMU_WALL, IMU_H + 1],
                [s * ix, IMU_CY, 5 + (IMU_H - 1) / 2]) for s in (-1, 1)]
    tray.append(box([IMU_IN_X, IMU_WALL, IMU_H + 1],
                    [0, IMU_CY + iy + IMU_WALL / 2, 5 + (IMU_H - 1) / 2]))
    shell = union([shell] + tray)

    # ---- Salida de cable en el piso ----
    shell = diff(shell, cyl(CABLE_R, 14, [CABLE_HOLE[0], CABLE_HOLE[1], 3]))
    return shell


def offset_sections(sections, d):
    """Secciones con semiejes desplazados d (positivo = hacia afuera)."""
    return [(z, a + d, b + d, yc) for z, a, b, yc in sections]


def split_cover(shell):
    """Separa la espalda como tapa desmontable de gancho-y-tornillo.

    Montaje: tapa inclinada (abajo afuera), los ganchos superiores entran
    detras de la pared sobre z=100, se rota hasta cerrar y se atornilla
    abajo. Nada entra a presion: ganchos y guias llevan FIT_CLEAR de holgura.
    """
    bx, bz = COVER_BOSS
    z0, z1 = COVER_Z
    zc, zh = (z0 + z1) / 2, z1 - z0
    cutbox = box([120, 30, zh], [0, COVER_Y - 15, zc])
    body = diff(shell, cutbox)
    cover = trimesh.boolean.intersection([shell, cutbox], engine='manifold')

    # recorte de los bordes superior e inferior de la tapa (0.4/0.3) para que
    # asiente y rote sin rozar las aristas del cuerpo
    cover = diff(cover, [box([130, 36, 1.4], [0, -16, z1 + 0.3]),
                         box([130, 36, 1.0], [0, -16, z0 - 0.2])])
    # chaflan de 20 grados en la esquina superior exterior: al rotar la tapa
    # para cerrarla, esa esquina sube y sin chaflan pega contra el marco
    wedge = box([130, 24, 16], [0, 0, 8])
    wedge.apply_transform(rotation_matrix(np.radians(20), [1, 0, 0]))
    wedge.apply_translation([0, -14, z1 - 0.4])
    # bolsillo plano en la cara interior, frente a los postes de tornillo: la
    # pared curva tocaria el poste antes de asentar (y deja apoyo plano al M3)
    pockets = [box([10, 9.5, 8.5], [s * COVER_BOSS[0], -10.75, 12.25])
               for s in (-1, 1)]
    cover = diff(cover, [wedge] + pockets)

    # franjas conformales a la cavidad (mismo truco de lofts desplazados):
    # "free" queda FIT_CLEAR adentro de la pared (holgura), "weld" penetra
    # 0.5 mm en la pared de la pieza a la que se une.
    ring_free = diff(loft(offset_sections(INNER, -FIT_CLEAR)),
                     loft(offset_sections(INNER, -FIT_CLEAR - 2.0)))
    ring_weld = diff(loft(offset_sections(INNER, 0.5)),
                     loft(offset_sections(INNER, -FIT_CLEAR - 2.0)))

    # ganchos superiores de la tapa: suben 4 mm sobre el borde (z1) y quedan
    # detras de la pared trasera del cuerpo, que arriba de z1 sigue entera
    tabs = []
    for s in (-1, 1):
        free = trimesh.boolean.intersection(
            [ring_free, box([TAB_W, 8, 5.4], [s * TAB_X, -14, z1 + 1.3])],
            engine='manifold')
        weld = trimesh.boolean.intersection(
            [ring_weld, box([TAB_W, 8, 4.0], [s * TAB_X, -14, z1 - 3.0])],
            engine='manifold')
        tabs += [free, weld]
    cover = union([cover] + tabs)

    # paso Ø3.4 + caja escariada Ø6.2 para la cabeza del M3 (con el bolsillo
    # interior la pared queda ~2.7; ~1.5 bajo la cabeza y ~4.4 mm de rosca)
    holes = [cyl(1.7, 10, [s * bx, -17.5, bz], axis='y') for s in (-1, 1)]
    holes += [cyl(3.1, 8, [s * bx, -21.0, bz], axis='y') for s in (-1, 1)]
    cover = diff(cover, holes)

    # guias laterales del cuerpo: pads conformales que sobresalen del plano
    # de corte hacia adentro de la tapa y la alinean en X cerca de los ganchos
    gz = (GUIDE_Z[0] + GUIDE_Z[1]) / 2
    gh = GUIDE_Z[1] - GUIDE_Z[0]
    guides = []
    for s in (-1, 1):
        free = trimesh.boolean.intersection(
            [ring_free, box([10, 7, gh], [s * 10, COVER_Y - 3, gz])],
            engine='manifold')
        weld = trimesh.boolean.intersection(
            [ring_weld, box([10, 7, gh], [s * 10, COVER_Y + 3.5, gz])],
            engine='manifold')
        guides += [free, weld]
    body = union([body] + guides)

    # postes de tornillo en el cuerpo, anclados al piso con una costilla;
    # la punta (y=-15.4) asienta contra el bolsillo plano de la tapa (-15.5)
    bosses = [cyl(3.6, 6.4, [s * bx, -12.2, bz], axis='y') for s in (-1, 1)]
    ribs = [box([7, 5.5, 7], [s * bx, -11.75, 7.5]) for s in (-1, 1)]
    body = union([body] + bosses + ribs)
    body = diff(body, [cyl(COVER_PILOT_R, 8, [s * bx, -12, bz], axis='y')
                       for s in (-1, 1)])
    return body, cover


def report(m, name):
    s = m.bounds[1] - m.bounds[0]
    print(f"{name}: watertight={m.is_watertight} vol={m.volume/1000:.1f} cm3 "
          f"bbox={s[0]:.1f} x {s[1]:.1f} x {s[2]:.1f} mm tris={len(m.faces)}")


if __name__ == '__main__':
    body, cover = split_cover(build_body())
    report(body, 'control_derecho')
    body.export(OUT_DIR + r"\control_derecho.stl")

    # El cuerpo es simetrico en X salvo la ranura de jumpers; ambos controles
    # son la misma pieza para que los jumpers queden del mismo lado (derecho).
    left = body.copy()
    report(left, 'control_izquierdo')
    left.export(OUT_DIR + r"\control_izquierdo.stl")

    report(cover, 'tapa_trasera')
    cover.export(OUT_DIR + r"\tapa_trasera.stl")
