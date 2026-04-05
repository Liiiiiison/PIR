import matplotlib.pyplot as plt
from collections import defaultdict

# --- BASYS 3 ARTIX-7 TAILORED CONSTANTS ---
V_CORE = 1.0  # VCCINT: Internal logic 
V_IO = 3.3    # VCCO: External I/O pins 

# Approximated node capacitances in Farads (typical for Artix-7 families)
C_INTERNAL = 2e-12  # ~2 pF per internal net toggle
C_IO = 12e-12       # ~12 pF for external PCB trace + package pin

# Energy per toggle in Joules ( E = 0.5 * C * V^2 )
ENERGY_INTERNAL = 0.5 * C_INTERNAL * (V_CORE ** 2)
ENERGY_IO = 0.5 * C_IO * (V_IO ** 2)


def parse_vcd_to_watts(vcd_file, time_bin_size_ns=10, timescale_fs=1e-15):
    """
    Parses a VCD and computes Dynamic Power in mW for each time bin.
    """
    time_bins_energy = defaultdict(float)
    current_time_fs = 0
    in_dump_vars = False

    # Track which VCD symbols belong to I/O vs Internal signals
    io_symbols = set()
    internal_symbols = set()

    with open(vcd_file, 'r') as f:
        for line in f:
            line = line.strip()
            if not line:
                continue

            # 1. Look for signal definitions to determine if it is I/O or internal
            # (Heuristic: Top-level hierarchy signals are usually I/O)
            if line.startswith('$var'):
                parts = line.split()
                # A definition looks like: $var wire 1 ! clk $end
                if len(parts) >= 5:
                    symbol = parts[3]
                    name = parts[4]
                    
                    # Heuristic: top-level ports often don't have '/' in their hierarchy or are capitalized.
                    # You can customize this list for your specific design top-level pin names.
                    if name.isupper() or len(parts) <= 5: 
                        io_symbols.add(symbol)
                    else:
                        internal_symbols.add(symbol)
                continue

            # 2. Track time
            if line.startswith('#'):
                current_time_fs = int(line[1:])
                in_dump_vars = True
                continue

            # 3. Calculate energy on toggles
            if in_dump_vars:
                # Single-bit toggle detection (e.g. '1!', '0!')
                if line.startswith(('0', '1', 'x', 'z', 'X', 'Z')) and len(line) >= 2:
                    symbol = line[1]
                    
                    # Convert timescale to Nanoseconds
                    current_time_ns = current_time_fs * timescale_fs * 1e9
                    bin_index = int(current_time_ns // time_bin_size_ns)

                    if symbol in io_symbols:
                        time_bins_energy[bin_index] += ENERGY_IO
                    else:
                        time_bins_energy[bin_index] += ENERGY_INTERNAL

    # 4. Convert Energy per Bin into Power (Power = Energy / Time_window)
    sorted_bins = sorted(time_bins_energy.items())
    x_time_ns = []
    y_power_mw = []

    time_window_sec = time_bin_size_ns * 1e-9

    for bin_idx, total_energy_joules in sorted_bins:
        time_point_ns = bin_idx * time_bin_size_ns
        
        # Power (Watts) = Energy (Joules) / Time (Seconds)
        power_watts = total_energy_joules / time_window_sec
        power_mw = power_watts * 1000 # Convert to mW

        x_time_ns.append(time_point_ns)
        y_power_mw.append(power_mw)

    return x_time_ns, y_power_mw


def plot_power_profile(x, y):
    plt.figure(figsize=(10, 5))
    
    # Fill under curve to visualize power burn
    plt.fill_between(x, y, color='orange', alpha=0.3)
    plt.plot(x, y, drawstyle='steps-post', color='darkorange', linewidth=1.5)
    
    plt.title("Estimated Dynamic Power Consumption vs. Time (Basys 3)")
    plt.xlabel("Simulation Time (ns)")
    plt.ylabel("Dynamic Power Consumption (mW)")
    
    plt.grid(True, linestyle='--', alpha=0.6)
    plt.tight_layout()
    plt.show()


if __name__ == "__main__":
    VCD_FILE = "RECEPTEUR_QKD/RECEPTEUR_QKD.sim/sim_1/behav/xsim/test_vcd_s3.vcd" # Change to your file name
    
    # We slice time into 5ns windows to calculate average power in that window
    BIN_SIZE_NS = 5.0 
    
    # Change if your simulator defaults to picoseconds (1e-12) or nanoseconds (1e-9)
    TIMESCALE = 1e-12 

    print(f"Parsing {VCD_FILE} based on Basys 3 Artix-7 specifications...")
    
    try:
        x, y = parse_vcd_to_watts(VCD_FILE, BIN_SIZE_NS, TIMESCALE)
        if not x:
            print("No data parsed. Verify your timescale and make sure simulation had signal activity.")
        else:
            plot_power_profile(x, y)
    except FileNotFoundError:
        print(f"Could not open {VCD_FILE}. Check the file path.")