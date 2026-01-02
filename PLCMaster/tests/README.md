# Guide de compilation minimale

Ce dépôt ne fournit pas encore d'outils de build dédiés pour les tests. Pour
compiler rapidement l'exécutable de runtime et vérifier que les fichiers
s'assemblent, un GCC récent suffit :

```sh
mkdir -p build
gcc -std=c11 \
    -I./src/core/runtime \
    -I./src/system/config \
    -I./src/system/builder \
    -o build/plc_runtime_stub \
    src/apps/plc_runtime/main.c \
    src/core/runtime/runtime.c \
    src/system/config/config_static.c \
    src/system/builder/system_builder.c
./build/plc_runtime_stub
```

Le binaire affiche `OK` ou `FAIL rc=...` selon la valeur renvoyée par
`system_build`. À ce stade, l'implémentation de `system_build` retourne `-1`,
ce qui produit `FAIL rc=-1`. Ce comportement est attendu tant que la logique
n'est pas implémentée.

# TODO tests

- Choisir un framework de tests C léger (par exemple Unity, CMocka ou un
  harness maison minimal) et l'intégrer sans imposer d'outil externe.
- Ajouter des tests unitaires pour `runtime_init`/`runtime_deinit`, y compris
  le cas NULL.
- Vérifier que `config_static` expose bien les constantes attendues (backends,
  devices, variables process et tags HMI) et couvrir les évolutions à venir.
- Tester `system_build` en simulant des backends/devices valides et des cas
  d'erreur (retours non nuls attendus).
- Préparer des tests d'intégration légers qui exécutent le binaire compilé et
  contrôlent les messages de sortie standards.
