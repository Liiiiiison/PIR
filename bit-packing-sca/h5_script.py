import h5py
import numpy as np
import sys
import re

class H5TraceManager:
    def __init__(self, filename):
        self.filename = filename
        with h5py.File(self.filename, 'a') as f:
            if "Traces" not in f:
                f.create_group("Traces")
            if "Keys" not in f:
                f.create_group("Keys")
    def add_trace(self, key0_str, key1_str, answer_str, leakage_data):
        with h5py.File(self.filename, 'a') as f:
            # Synchronisation par index
            idx = len(f["Traces"].keys())
           
            # --- Groupe TRACES ---
            t_grp = f["Traces"].create_group(f"Trace_{idx}")
            adr_grp = t_grp.create_group("Adresse")
            data_np = np.array(leakage_data, dtype=np.float32)
            adr_grp.create_dataset("data", data=data_np)
            adr_grp.create_dataset("shape", data=data_np.shape)
           
            # --- Groupe KEYS ---
            # On crée un groupe Key_N qui contient Key0 et Key1
            k_grp = f["Keys"].create_group(f"Key_{idx}")
           
            # Fonction pour extraire l'entier (ex: "-1" depuis "-1 (0xfff...)")
            def clean_val(s):
                try: return int(s.split('(')[0].strip(),16)
                except: return 0

            k_grp.create_dataset("Base_Bob", data=np.array([clean_val(key0_str)], dtype=np.int64))
            k_grp.create_dataset("Base_Alice", data=np.array([clean_val(key1_str)], dtype=np.int64))
            k_grp.create_dataset("Clé", data=np.array([clean_val(answer_str)], dtype=np.int64))
           
            print(f" [+] Index {idx} : Keys({clean_val(key0_str)}, {clean_val(key1_str)}, {clean_val(answer_str)} ) | Trace: {len(leakage_data)} pts")


def parse_stdin():
    manager = H5TraceManager("sca_results.h5")
    current_val = None
    current_leakage = []
    in_vector = False
    key1 = 0
    key2 = 0
    answer = 0

    print("Lecture du flux... (Ctrl+C pour arrêter)")

    for line in sys.stdin:
        line = line.strip()
       
        if line.startswith("Bob:"):
            key1 = line.split(":")[1]
        if line.startswith("Alice:"):
            key2 = line.split(":")[1]
        if line.startswith("answer:"):
            answer = line.split(":")[1]

        # 1. Capture de la valeur 'val'
        if line.startswith("val:"):
            current_val = line.split(":")[1].strip()
            current_leakage = [] # Reset pour la nouvelle trace
           
        # 2. Détection du début du vecteur
        elif "Leakage vector: [" in line:
            in_vector = True
           
        # 3. Extraction des données (0xAdresse: Valeur)
        elif in_vector and ":" in line:
            try:
                # On prend la partie après les ':'
                value = float(line.split(":")[1].strip())
                current_leakage.append(value)
            except ValueError:
                pass
               
        # 4. Fin du vecteur et enregistrement
        elif "]" in line and in_vector:
            if current_leakage:
                manager.add_trace(key1,key2, answer, current_leakage)
            in_vector = False

if __name__ == "__main__":
    try:
        parse_stdin()
    except KeyboardInterrupt:
        print("\nFin de l'acquisition.")

