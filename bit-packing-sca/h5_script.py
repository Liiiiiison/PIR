import sys
import re
import h5py
import numpy as np

def parse_and_save():
    # Regex pour capturer l'adresse (hex) et la valeur (entier)
    # Format cible : 0x00000010: 23
    pattern = re.compile(r'(0x[0-9a-fA-F]+):\s*(\d+)')

    addresses = []
    values = []

    print("Lecture du flux Leakage vector... (Ctrl+D pour finaliser)")

    for line in sys.stdin:
        match = pattern.search(line)
        if match:
            # On convertit l'adresse en entier et la valeur en int
            addr_str, val_str = match.groups()
            addresses.append(int(addr_str, 16))
            values.append(int(val_str))

    if not values:
        print("Erreur : Aucune donnée valide trouvée.")
        return

    # Création du fichier HDF5
    filename = "leakage_data.h5"
    with h5py.File(filename, 'w') as f:
        # Création d'un groupe pour l'organisation
        grp = f.create_group("Analyse_Vecteur")
        
        # On stocke les adresses et les valeurs dans deux datasets distincts
        grp.create_dataset("adresses", data=np.array(addresses, dtype=np.uint64))
        grp.create_dataset("valeurs", data=np.array(values, dtype=np.int32))
        
        # Ajout de métadonnées
        f.attrs['format'] = 'Leakage Vector Export'
        f.attrs['date_creation'] = '2026-03-18'

    print(f"\n✅ Terminé ! {len(values)} entrées sauvegardées dans '{filename}'.")

if __name__ == "__main__":
    parse_and_save()