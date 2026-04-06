import matplotlib.pyplot as plt
import re
from pprint import pprint

# 1. Dictionnaire de désassemblage (extrait de ton dump)
# On crée un mapping Adresse -> Instruction pour la corrélation

gdb_dump_masked = """
   0x00000014 <+0>:     addi    sp,sp,-16
   0x00000018 <+4>:     mv      a2,t0
   0x0000001c <+8>:     mv      a5,t1
   0x00000020 <+12>:    mv      a0,t2
   0x00000024 <+16>:    mv      a7,s0
   0x00000028 <+20>:    .insn   4, 0x0e00106b
   0x0000002c <+24>:    xnor    a2,a2,a5
   0x00000030 <+28>:    beqz    a2,0xb0 <main+156>
   0x00000034 <+32>:    xor     a0,a0,a7
--Type <RET> for more, q to quit, c to continue without paging--
   0x00000038 <+36>:    li      a1,0
   0x0000003c <+40>:    li      a5,0
   0x00000040 <+44>:    li      a6,0
   0x00000044 <+48>:    ctz     a3,a2
   0x00000048 <+52>:    add     a5,a3,a5
   0x0000004c <+56>:    bext    a4,a0,a5
   0x00000050 <+60>:    zext.b  a4,a4
   0x00000054 <+64>:    bext    t1,a7,a5
   0x00000058 <+68>:    sll     a4,a4,a1
   0x0000005c <+72>:    or      a6,a6,a4
--Type <RET> for more, q to quit, c to continue without paging--
   0x00000060 <+76>:    addi    a5,a5,1
   0x00000064 <+80>:    mv      t0,t0
   0x00000068 <+84>:    addi    a3,a3,1
   0x0000006c <+88>:    srl     a2,a2,a3
   0x00000070 <+92>:    addi    a1,a1,1
   0x00000074 <+96>:    bnez    a2,0x44 <main+48>
   0x00000078 <+100>:   .insn   4, 0x0e00206b
   0x0000007c <+104>:   lui     a5,0x0
   0x00000080 <+108>:   mv      a5,a5
   0x00000084 <+112>:   sw      a5,8(sp)
--Type <RET> for more, q to quit, c to continue without paging--
   0x00000088 <+116>:   lw      a5,8(sp)
   0x0000008c <+120>:   add     a0,a5,zero
   0x00000090 <+124>:   .insn   4, 0x0c00056b
   0x00000094 <+128>:   sw      a6,12(sp)
   0x00000098 <+132>:   lw      a5,12(sp)
   0x0000009c <+136>:   add     a0,a5,zero
   0x000000a0 <+140>:   .insn   4, 0x0a00056b
   0x000000a4 <+144>:   li      a0,0
   0x000000a8 <+148>:   addi    sp,sp,16
   0x000000ac <+152>:   ret
--Type <RET> for more, q to quit, c to continue without paging--
   0x000000b0 <+156>:   li      a6,0
   0x000000b4 <+160>:   j       0x78 <main+100>

"""

gdb_dump_no_mask = """   0x00000014 <+0>:     addi    sp,sp,-16
   0x00000018 <+4>:     mv      a2,t0
   0x0000001c <+8>:     mv      a5,t1
   0x00000020 <+12>:    mv      a0,t2
   0x00000024 <+16>:    .insn   4, 0x0e00106b
   0x00000028 <+20>:    xnor    a2,a2,a5
   0x0000002c <+24>:    beqz    a2,0xa0 <main+140>
   0x00000030 <+28>:    li      a1,0
   0x00000034 <+32>:    li      a4,0
--Type <RET> for more, q to quit, c to continue without paging--
   0x00000038 <+36>:    li      a6,0
   0x0000003c <+40>:    ctz     a3,a2
   0x00000040 <+44>:    add     a4,a3,a4 <-- calc de l'indice d'extraction du bit
   0x00000044 <+48>:    bext    a5,a0,a4 <-- extraction du bit
   0x00000048 <+52>:    zext.b  a5,a5 <-- extension avec des 0 devant le resultat ??
   0x0000004c <+56>:    addi    a3,a3,1 <-- rajoute 1 à a3=absolute_bit_pos
   0x00000050 <+60>:    sll     a5,a5,a1 <-- shift gauche de a1 places
   0x00000054 <+64>:    srl     a2,a2,a3 <-- shift droite
   0x00000058 <+68>:    or      a6,a6,a5 <-- rajoute le bit dans la clé
   0x0000005c <+72>:    addi    a4,a4,1 <-- (packing_pos++) indice du prochain bit dans la clé qu'on va construire
--Type <RET> for more, q to quit, c to continue without paging--
   0x00000060 <+76>:    addi    a1,a1,1 <--  
   0x00000064 <+80>:    bnez    a2,0x3c <main+40> <-- check fin de boucle while
   0x00000068 <+84>:    .insn   4, 0x0e00206b
   0x0000006c <+88>:    lui     a5,0x0
   0x00000070 <+92>:    mv      a5,a5
   0x00000074 <+96>:    sw      a5,8(sp)
   0x00000078 <+100>:   lw      a5,8(sp)
   0x0000007c <+104>:   add     a0,a5,zero
   0x00000080 <+108>:   .insn   4, 0x0c00056b
   0x00000084 <+112>:   sw      a6,12(sp)
--Type <RET> for more, q to quit, c to continue without paging--
   0x00000088 <+116>:   lw      a5,12(sp)
   0x0000008c <+120>:   add     a0,a5,zero
   0x00000090 <+124>:   .insn   4, 0x0a00056b
   0x00000094 <+128>:   li      a0,0
   0x00000098 <+132>:   addi    sp,sp,16
   0x0000009c <+136>:   ret
   0x000000a0 <+140>:   li      a6,0
   0x000000a4 <+144>:   j       0x68 <main+84>
End of assembler dump.
"""




gdb_dump_no_mask_no_annotation = """
   0x00000014 <+0>:     addi    sp,sp,-16
   0x00000018 <+4>:     mv      a2,t0
   0x0000001c <+8>:     mv      a5,t1
   0x00000020 <+12>:    mv      a0,t2
   0x00000024 <+16>:    .insn   4, 0x0e00106b
   0x00000028 <+20>:    xnor    a2,a2,a5
   0x0000002c <+24>:    beqz    a2,0xa0 <main+140>
   0x00000030 <+28>:    li      a1,0
   0x00000034 <+32>:    li      a4,0
--Type <RET> for more, q to quit, c to continue without paging--
   0x00000038 <+36>:    li      a6,0
   0x0000003c <+40>:    ctz     a3,a2
   0x00000040 <+44>:    add     a4,a3,a4
   0x00000044 <+48>:    bext    a5,a0,a4
   0x00000048 <+52>:    zext.b  a5,a5 
   0x0000004c <+56>:    addi    a3,a3,1 
   0x00000050 <+60>:    sll     a5,a5,a1 
   0x00000054 <+64>:    srl     a2,a2,a3 
   0x00000058 <+68>:    or      a6,a6,a5 
   0x0000005c <+72>:    addi    a4,a4,1
--Type <RET> for more, q to quit, c to continue without paging--
   0x00000060 <+76>:    addi    a1,a1,1  
   0x00000064 <+80>:    bnez    a2,0x3c <main+40>
   0x00000068 <+84>:    .insn   4, 0x0e00206b
   0x0000006c <+88>:    lui     a5,0x0
   0x00000070 <+92>:    mv      a5,a5
   0x00000074 <+96>:    sw      a5,8(sp)
   0x00000078 <+100>:   lw      a5,8(sp)
   0x0000007c <+104>:   add     a0,a5,zero
   0x00000080 <+108>:   .insn   4, 0x0c00056b
   0x00000084 <+112>:   sw      a6,12(sp)
--Type <RET> for more, q to quit, c to continue without paging--
   0x00000088 <+116>:   lw      a5,12(sp)
   0x0000008c <+120>:   add     a0,a5,zero
   0x00000090 <+124>:   .insn   4, 0x0a00056b
   0x00000094 <+128>:   li      a0,0
   0x00000098 <+132>:   addi    sp,sp,16
   0x0000009c <+136>:   ret
   0x000000a0 <+140>:   li      a6,0
   0x000000a4 <+144>:   j       0x68 <main+84>
End of assembler dump.
"""




asm_mapping = {
    "14": "addi    sp,sp,-16", 
    "18": "mv      a2,t0", 
    "1c": "mv      a5,t1",
    "20": "mv      a0,t2", 
    "24": "mv      a7,s0", 
    "28": ".insn   4, 0x0e00106b",
    "2c": "xnor    a2,a2,a5", 
    "30": "beqz    a2,0xb0 <main+156>", 
    "34": "xor     a0,a0,a7",
    "38": "li      a1,0", 
    "3c": "li a5,0", "40": "li a6,0",
    "44": "ctz a3,a2", "48": "add a5,a3,a5", "4c": "bext a4,a0,a5",
    "50": "zext.b a4,a4", "54": "bext t1,a7,a5", "58": "sll a4,a4,a1",
    "5c": "or a6,a6,a4", "60": "addi a5,a5,1", "64": "mv t0,t0",
    "68": "addi a3,a3,1", "6c": "srl a2,a2,a3", "70": "addi a1,a1,1",
    "74": "bnez a2,0x44", "78": "custom_insn_2", "7c": "lui a5,0x0",
    "80": "mv a5,a5", "84": "sw a5,8(sp)", "88": "lw a5,8(sp)",
    "8c": "add a0,a5,zero", "90": "custom_insn_3", "94": "sw a6,12(sp)",
    "98": "lw a5,12(sp)", "9c": "add a0,a5,zero", "a0": "custom_insn_4",
    "a4": "li a0,0", "a8": "addi sp,sp,16", "ac": "ret",
    "b0": "li a6,0", "b4": "j 0x78"
}

def parse_gdb_assembly(asm_text):
    # On nettoie d'abord les espaces insécables HTML/Télématique courants
    asm_text = asm_text.replace('\xa0', ' ')
    
    # Regex : cherche l'adresse, ignore le <+offset>, capture tout le reste
    pattern = r"0x([0-9a-fA-F]+)(?:\s+<[+\d]+>)?:\s+(.*)"
    
    mapping = {}
    for line in asm_text.strip().split('\n'):
        match = re.search(pattern, line)
        if match:
            # On normalise l'adresse (ex: 00000058 -> 58)
            addr_hex = match.group(1).lower().lstrip('0')
            if not addr_hex: addr_hex = "0"
            
            # On nettoie l'instruction et on retire les commentaires GDB (ex: <main+40>)
            instr_full = match.group(2).split('<')[0].strip()
            mapping[addr_hex] = instr_full
            
    return mapping

# --- TEST ET ASSERTION ---
def test_parser():
    sample_gdb = gdb_dump
    expected = asm_mapping
    result = parse_gdb_assembly(sample_gdb)
    
    # Vérification par assertion
    for addr, instr in expected.items():
        assert result.get(addr) == instr, f"Erreur de mapping pour l'adresse {addr}"
    
    print("✅ Test d'assertion réussi : Le parser GDB fonctionne correctement.")

def plot_correlated_trace(raw_trace,asm_mapping,key_bits):


    # 2. Extraction des données (Adresse: Puissance)
    pattern = r"0x[0]+([0-9a-f]+):\s+(\d+)"
    matches = re.findall(pattern, raw_trace)
    
    if not matches:
        print("Erreur : Aucune donnée trouvée.")
        return

    # Préparation des listes pour le graphique
    powers = [int(m[1]) for m in matches]
    # On récupère l'instruction correspondante ou l'adresse brute si inconnue
    labels = [asm_mapping.get(m[0], f"unk (0x{m[0]})") for m in matches]
    x_axis = range(len(powers))

    # 3. Création du graphique
    fig, ax = plt.subplots(figsize=(16, 8))
    
    # On utilise un step plot car la conso change "par instruction"
    ax.step(x_axis, powers, where='post', color='#e67e22', linewidth=2, label="Consommation")
    ax.fill_between(x_axis, powers, step="post", alpha=0.2, color='#e67e22')

    # 4. Annotation des instructions (une sur deux ou filtrage pour lisibilité)
    # ne va labeliser que les bext et les or qui rajoutent le bit de clé
    key_bit_index = 0
    for i, instr in enumerate(labels):
        #print(f"instr = {instr}")

        opcode = instr.split()[0] if instr.split() else ""


        if opcode in ["bext"]:
            ax.annotate(instr+f"    b={key_bits[key_bit_index]}", (i, powers[i]), xytext=(0, 10), 
                            textcoords='offset points', rotation=90, 
                            fontsize=12, verticalalignment='bottom', alpha=0.8)
            key_bit_index += 1


    # Mise en évidence de la fin (hors boucle)
    # On détecte quand on dépasse l'adresse de saut 0x74 pour la dernière fois
    last_loop_idx = 0
    for i, m in enumerate(matches):
        if m[0] == "74":
            last_loop_idx = i
    
    if last_loop_idx > 0:
        ax.axvspan(last_loop_idx, len(powers)-1, color='red', alpha=0.1, label="Sortie de boucle / Épilogue")

    ax.set_title("Corrélation Puissance vs Instructions RISC-V", fontsize=15)
    ax.set_xlabel("Cycle d'instruction (temporel)", fontsize=12)
    ax.set_ylabel("Puissance (Leakage)", fontsize=12)
    ax.grid(True, which='both', linestyle=':', alpha=0.5)
    ax.legend()
    
    plt.tight_layout()
    plt.show()


if __name__ == "__main__":


    """
Bob:451a2192
Alice:278f2835
Alice's decoded key bits:7b42ca34
Alice's mask:4c189cdd
answer:111498
[+] generated key :
val: 111498 (0x0001b38a) = 0b00... 0001 1011 0011 1000 1010

size of power leakage vector : 203 
    """

    with open("trace_raw.txt","r") as f:
        trace_raw = f.read()
        #test_parser()
        asm_mapping = parse_gdb_assembly(gdb_dump_no_mask_no_annotation)
        key_bits = list("011011001110001010")
        key_bits.reverse()
        print(key_bits)
        plot_correlated_trace(trace_raw,asm_mapping,key_bits)
