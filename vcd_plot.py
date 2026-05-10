import matplotlib.pyplot as plt
from collections import defaultdict

# --- Paramètres Physiques ---
V_CORE = 1.0
V_IO = 3.3
C_INTERNAL = 2e-12
C_IO = 12e-12
ENERGY_INTERNAL = 0.5 * C_INTERNAL * (V_CORE ** 2)
ENERGY_IO = 0.5 * C_IO * (V_IO ** 2)

def parse_vcd_to_watts(vcd_file, time_bin_size_ns=5.0, timescale_fs=1e-12):
    time_bins_energy = defaultdict(float)
    current_time_fs = 0
    in_dump_vars = False
    io_symbols = set()

    with open(vcd_file, 'r') as f:
        for line in f:
            line = line.strip()
            if line.startswith('$var'):
                parts = line.split()
                if len(parts) >= 5:
                    symbol, name = parts[3], parts[4]
                    if name.isupper() or len(parts) <= 5: io_symbols.add(symbol)
                continue
            if line.startswith('#'):
                current_time_fs = int(line[1:])
                in_dump_vars = True
                continue
            if in_dump_vars:
                if line.startswith(('0', '1')) and len(line) >= 2:
                    symbol = line[1]
                    current_time_ns = current_time_fs * timescale_fs * 1e9
                    bin_index = int(current_time_ns // time_bin_size_ns)
                    time_bins_energy[bin_index] += ENERGY_IO if symbol in io_symbols else ENERGY_INTERNAL

    sorted_bins = sorted(time_bins_energy.items())
    x, y = [], []
    time_window_sec = time_bin_size_ns * 1e-9
    for bin_idx, energy in sorted_bins:
        x.append(bin_idx * time_bin_size_ns)
        y.append((energy / time_window_sec) * 1000)
    return x, y

def plot_individual_phases(x_full, y_full, test_phases):
    for start, end, label, color in test_phases:
        phase_id = label.split(':')[0].strip()
        buffer = 20.0
        start_zoom, end_zoom = max(0, start - buffer), end + buffer
        
        x_phase = [t for t in x_full if start_zoom <= t <= end_zoom]
        y_phase = [p for t, p in zip(x_full, y_full) if start_zoom <= t <= end_zoom]
        
        fig, ax = plt.subplots(figsize=(12, 6))
        ax.fill_between(x_phase, y_phase, color=color, alpha=0.2)
        ax.plot(x_phase, y_phase, drawstyle='steps-post', color=color, linewidth=2)

        # --- Marquage spécifique pour les phases d'alternance (R3, D3, A3) ---
        if phase_id in ["R3", "D3", "A3"]:
            period = 40.0
            t = start
            first_label = True
            while t < end:
                # Zone Bit '0' : [0, 10ns] de la période de 40ns
                ax.axvspan(t, t + 10, color='green', alpha=0.1, label="'0' Active" if first_label else "")
                # Zone Bit '1' : [20ns, 30ns] de la période de 40ns
                ax.axvspan(t + 20, t + 30, color='red', alpha=0.1, label="'1' Active" if first_label else "")
                t += period
                first_label = False
            ax.legend(loc='upper right')

        ax.set_title(f"Détail Consommation - Phase {label}")
        ax.set_xlabel("Temps (ns)")
        ax.set_ylabel("Puissance (mW)")
        ax.set_xlim(start_zoom, end_zoom)
        ax.grid(True, linestyle='--', alpha=0.5)
        
        plt.tight_layout()
        plt.savefig(f"vcd_power_{phase_id}.png", dpi=300)
        plt.close()

if __name__ == "__main__":
    VCD_FILE = "RECEPTEUR_QKD/RECEPTEUR_QKD.sim/sim_1/behav/xsim/test_vcd.vcd"
    
    # Chronologie basée sur ton VHDL (100ns init + 32*20ns par bloc de 100%)
    PHASES_CHRONOLOGIE = [
        (100,  740,  "R1: Base Rect. 100% '0'", 'red'),
        (740,  1380, "R2: Base Rect. 100% '1'", 'red'),
        (1380, 2020, "R3: Base Rect. 50/50",    'orange'), # Alternance
        (2120, 2760, "D1: Base Diag. 100% '0'", 'blue'),
        (2760, 3400, "D2: Base Diag. 100% '1'", 'blue'),
        (3400, 4040, "D3: Base Diag. 50/50",    'purple'), # Alternance
        (4140, 4780, "A1: Alternance 100% '0'", 'green'),
        (4780, 5420, "A2: Alternance 100% '1'", 'green'),
        (5420, 6060, "A3: Alternance 50/50",    'brown')   # Alternance
    ]

    try:
        x, y = parse_vcd_to_watts(VCD_FILE)
        plot_individual_phases(x, y, PHASES_CHRONOLOGIE)
        print("Graphiques générés avec démarquage des zones de bits pour R3, D3 et A3.")
    except FileNotFoundError:
        print("Fichier VCD non trouvé.")