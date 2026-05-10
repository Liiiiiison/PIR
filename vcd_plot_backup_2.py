import matplotlib.pyplot as plt
from collections import defaultdict

V_CORE = 1.0  # VCCINT: Internal logic 
V_IO = 3.3    # VCCO: External I/O pins 

# approximated node capacitances in Farads for Basys-3
C_INTERNAL = 2e-12  # ~2 pF per internal net toggle
C_IO = 12e-12       # ~12 pF for external PCB trace + package pin

# energy per toggle in Joules ( E = 0.5 * C * V^2 )
ENERGY_INTERNAL = 0.5 * C_INTERNAL * (V_CORE ** 2)
ENERGY_IO = 0.5 * C_IO * (V_IO ** 2)


def parse_vcd_to_watts(vcd_file, time_bin_size_ns=10, timescale_fs=1e-15):
    time_bins_energy = defaultdict(float)
    current_time_fs = 0
    in_dump_vars = False

    io_symbols = set()
    internal_symbols = set()

    with open(vcd_file, 'r') as f:
        for line in f:
            line = line.strip()
            if not line:
                continue

            if line.startswith('$var'):
                parts = line.split()
                if len(parts) >= 5:
                    symbol = parts[3]
                    name = parts[4]
                    if name.isupper() or len(parts) <= 5: 
                        io_symbols.add(symbol)
                    else:
                        internal_symbols.add(symbol)
                continue

            if line.startswith('#'):
                current_time_fs = int(line[1:])
                in_dump_vars = True
                continue

            if in_dump_vars:
                if line.startswith(('0', '1', 'x', 'z', 'X', 'Z')) and len(line) >= 2:
                    symbol = line[1]
                    
                    current_time_ns = current_time_fs * timescale_fs * 1e9
                    bin_index = int(current_time_ns // time_bin_size_ns)

                    if symbol in io_symbols:
                        time_bins_energy[bin_index] += ENERGY_IO
                    else:
                        time_bins_energy[bin_index] += ENERGY_INTERNAL

    sorted_bins = sorted(time_bins_energy.items())
    x_time_ns = []
    y_power_mw = []

    time_window_sec = time_bin_size_ns * 1e-9

    for bin_idx, total_energy_joules in sorted_bins:
        time_point_ns = bin_idx * time_bin_size_ns
        
        power_watts = total_energy_joules / time_window_sec
        power_mw = power_watts * 1000 # Convert to mW

        x_time_ns.append(time_point_ns)
        y_power_mw.append(power_mw)

    return x_time_ns, y_power_mw


def plot_power_profile(x, y, test_phases=None):
    """Génère le graphique global avec toutes les phases."""
    plt.figure(figsize=(14, 7))
    
    plt.fill_between(x, y, color='orange', alpha=0.3)
    plt.plot(x, y, drawstyle='steps-post', color='darkorange', linewidth=1.5)
    
    if test_phases and len(y) > 0:
        y_max = max(y) if max(y) > 0 else 1.0
        
        for start, end, label, color in test_phases:
            plt.axvspan(start, end, facecolor=color, alpha=0.15, edgecolor='gray', linestyle='--')
            plt.text(start + (end - start)/2, y_max * 0.95, label, 
                     horizontalalignment='center', verticalalignment='top', 
                     rotation=90, fontsize=9, fontweight='bold', color='#333333')

    plt.title("Consommation Dynamique Globale (Basys 3)")
    plt.xlabel("Temps de simulation (ns)")
    plt.ylabel("Puissance Dynamique (mW)")
    
    if test_phases:
        plt.xlim(0, test_phases[-1][1] + 200)

    plt.grid(True, linestyle='--', alpha=0.6)
    plt.tight_layout()
    plt.savefig("vcd_power_global.png", dpi=300)
    print("-> Graphique global sauvegardé sous 'vcd_power_global.png'")
    plt.close() # On ferme la figure pour ne pas superposer avec les suivantes


def plot_individual_phases(x_full, y_full, test_phases):
    """Génère un graphique zoomé pour chaque phase spécifique."""
    
    for start, end, label, color in test_phases:
        # Extraire un identifiant court pour le nom du fichier (ex: "R1", "D2")
        phase_id = label.split(':')[0].strip()
        
        # Marge de 20ns pour voir ce qui se passe juste avant et juste après
        buffer = 20.0 
        start_zoom = max(0, start - buffer)
        end_zoom = end + buffer
        
        # Filtrer les données pour cette fenêtre de temps
        x_phase = []
        y_phase = []
        for t, p in zip(x_full, y_full):
            if start_zoom <= t <= end_zoom:
                x_phase.append(t)
                y_phase.append(p)
                
        # Création de la figure individuelle
        plt.figure(figsize=(10, 5))
        
        # On utilise la couleur de la phase pour le tracé pour bien les distinguer
        plt.fill_between(x_phase, y_phase, color=color, alpha=0.2)
        plt.plot(x_phase, y_phase, drawstyle='steps-post', color=color, linewidth=2)
        
        plt.title(f"Détail Consommation - Phase {label}")
        plt.xlabel("Temps de simulation (ns)")
        plt.ylabel("Puissance Dynamique (mW)")
        
        # Fixer les limites de l'axe X à notre zone zoomée
        plt.xlim(start_zoom, end_zoom)
        
        plt.grid(True, linestyle='--', alpha=0.6)
        plt.tight_layout()
        
        # Sauvegarde
        filename = f"vcd_power_{phase_id}.png"
        plt.savefig(filename, dpi=300)
        print(f"-> Graphique détaillé sauvegardé sous '{filename}'")
        plt.close() # Important pour libérer la mémoire à chaque itération


if __name__ == "__main__":
    VCD_FILE = "RECEPTEUR_QKD/RECEPTEUR_QKD.sim/sim_1/behav/xsim/test_vcd.vcd" # À adapter
    
    BIN_SIZE_NS = 5.0 
    TIMESCALE = 1e-12 # 1ps resolution

    PHASES_CHRONOLOGIE = [
        # Groupe 1 : Base Rectiligne (Rouge)
        (100,  740,  "R1: Base Rect. 100% '0'", 'red'),
        (740,  1380, "R2: Base Rect. 100% '1'", 'red'),
        (1380, 2020, "R3: Base Rect. 50/50",    'red'),
        
        # Groupe 2 : Base Diagonale (Bleu)
        (2120, 2760, "D1: Base Diag. 100% '0'", 'blue'),
        (2760, 3400, "D2: Base Diag. 100% '1'", 'blue'),
        (3400, 4040, "D3: Base Diag. 50/50",    'blue'),
        
        # Groupe 3 : Alternance de bases (Vert)
        (4140, 4780, "A1: Alternance 100% '0'", 'green'),
        (4780, 5420, "A2: Alternance 100% '1'", 'green'),
        (5420, 6060, "A3: Alternance 50/50",    'green')
    ]

    print(f"Parsing {VCD_FILE} based on Basys 3 Artix-7 specifications...")
    
    try:
        x, y = parse_vcd_to_watts(VCD_FILE, BIN_SIZE_NS, TIMESCALE)
        if not x:
            print("No data parsed. Verify your timescale and make sure simulation had signal activity.")
        else:
            # 1. Générer et sauvegarder le graphique global
            plot_power_profile(x, y, test_phases=PHASES_CHRONOLOGIE)
            
            # 2. Générer et sauvegarder les graphiques de détail
            print("Génération des vues détaillées...")
            plot_individual_phases(x, y, test_phases=PHASES_CHRONOLOGIE)
            
            print("Terminé ! Toutes les figures ont été générées.")
            
    except FileNotFoundError:
        print(f"Could not open {VCD_FILE}. Check the file path.")