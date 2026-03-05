
MEMORY
{
    rom (rx) : ORIGIN = 0x00000000, LENGTH = 0x10000000
    ram (rw) : ORIGIN = 0x10000000, LENGTH = 0x20000000
}

ENTRY (main)

SECTIONS
{
	.text : { 
		*(.text*)
		*(.text.*)
	} > rom
	
	.rodata : {

		*(.rdata*)
		*(.rodata*)
		*(.rodata.*)    
	} > rom
	
	.data : {
		*(.data*)
		*(.data.*)
	} > rom
	
	.sbss : {
		*(.sbss*)
		*(.sbss.*)
	}
  
	.bss : { 
		*(.bss*) 
		*(.bss.*)
	} > ram
}

