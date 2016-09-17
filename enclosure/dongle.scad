$fn = 100;

pcbd_thick = 1.1;

// the main part of the dongle PCB
pcbw_x = 14.8;
pcbw_y = 27;

// the neck part of the dongle PCB
pcbn_x = 10.5;
pcbn_y = 4.2;

pcb_zoffset = 0.3;

// USB plug
plug_x = 12.15;
plug_y = 14.8;
plug_z = 5;

// USB plug neck
plugn_x = 9.6;
plugn_y = pcbn_y;
plugn_z = plug_z/2 + pcb_zoffset;

module flash_connector(flash_wigglex = 0, flash_wiggley = 0)
{
	// flashing connector
	flash_sizex = 2.4 + flash_wigglex;
	flash_sizey = 14.6 + flash_wiggley;
	flash_sizez = 5.6 - pcbd_thick;
	flash_posx = 0.6 - flash_wigglex/2;
	flash_posy = 22.1 - flash_sizey + flash_wiggley/2;

	translate([-pcbw_x/2 + flash_posx, flash_posy, -pcbd_thick/2 - flash_sizez])
		cube([flash_sizex, flash_sizey, flash_sizez]);
}

module dongle()
{
	// PCB main PCB
	translate([-pcbw_x/2, 0, -pcbd_thick/2])
		cube([pcbw_x, pcbw_y, pcbd_thick]);

	// PCB neck
	translate([-pcbn_x/2, pcbw_y, -pcbd_thick/2])
		cube([pcbn_x, pcbn_y, pcbd_thick]);

	// USB plug
	translate([-plug_x/2, pcbw_y+pcbn_y, -plug_z/2 + pcbd_thick/2 + pcb_zoffset])
		cube([plug_x, plug_y, plug_z]);
	translate([-plugn_x/2, pcbw_y, pcbd_thick/2])
		cube([plugn_x, plugn_y, plugn_z]);

	flash_connector();
}

pcb_wiggle = 0.4;
wall_thick = 2;

inside_x = pcbw_x + pcb_wiggle;
inside_y = pcbw_y + pcb_wiggle;
inside_z = 4.4 + pcb_wiggle;

module pegs(peg_wiggle = 0.0, thick_wiggle = 0.0)
{
	peg_length = 3.5;
	peg_height = inside_z + wall_thick + peg_wiggle;
	peg_z = -inside_z/2 - wall_thick + pcbd_thick/2 + pcb_zoffset;

	peg_height_flash = inside_z + peg_wiggle;
	peg_z_flash = -inside_z/2 + pcbd_thick/2 + pcb_zoffset;

	// pegs across the length
	peg1_y = 2.5;
	peg2_y = 18;
	peg3_y = 28.7;

	translate([inside_x/2 - thick_wiggle/2, peg1_y - peg_wiggle/2, peg_z])
		cube([wall_thick + thick_wiggle, peg_length + peg_wiggle, peg_height]);

	translate([inside_x/2 - thick_wiggle/2, peg2_y - peg_wiggle/2, peg_z])
		cube([wall_thick + thick_wiggle, peg_length + peg_wiggle, peg_height]);

	translate([inside_x/2 - thick_wiggle/2, peg3_y - peg_wiggle/2, peg_z])
		cube([wall_thick + thick_wiggle, peg_length + peg_wiggle, peg_height]);

	translate([-inside_x/2 - wall_thick - thick_wiggle/2, peg1_y - peg_wiggle/2, peg_z])
		cube([wall_thick + thick_wiggle, peg_length + peg_wiggle, peg_height]);

	translate([-inside_x/2 - wall_thick - thick_wiggle/2, peg3_y - peg_wiggle/2, peg_z])
		cube([wall_thick + thick_wiggle, peg_length + peg_wiggle, peg_height]);

	// pegs across the width
	peg4_x = 2.5;

	translate([peg4_x - peg_wiggle/2, -wall_thick - pcb_wiggle/2 - thick_wiggle/2, peg_z])
		cube([peg_length + peg_wiggle, wall_thick + thick_wiggle, peg_height]);
		
	translate([-peg4_x - peg_length - peg_wiggle/2, -wall_thick - pcb_wiggle/2 - thick_wiggle/2, peg_z])
		cube([peg_length + peg_wiggle, wall_thick + thick_wiggle, peg_height]);
}

case_x = inside_x + wall_thick*2;
case_y = inside_y + wall_thick + pcbn_y + 2.5;
case_z = inside_z + wall_thick*2;

module case_top()
{
	// the LED position
	led_x = 3.3;
	led_y = 11;

	difference()
	{
		translate([-case_x/2, -(pcb_wiggle/2 + wall_thick), inside_z/2 + pcbd_thick/2 + pcb_zoffset])
			cube([case_x, case_y, wall_thick]);

		// the LED hole
		translate([-pcbw_x/2 + led_x, led_y, inside_z/2 + pcbd_thick/2 + pcb_zoffset - 0.1])
			cylinder(wall_thick + 0.2, 1, 1);
	}

	// the stand-offs
	stdoff_thick = 1.5;
	stdoff_height = inside_z/2 + pcb_zoffset;
	stdoff_len = 3;
	stdoff_z = inside_z/2 + pcbd_thick/2 + pcb_zoffset - stdoff_height;

	stdoff1_y = 10.3;
	translate([pcbw_x/2 - stdoff_len, stdoff1_y + pcb_wiggle/2 - stdoff_thick/2, stdoff_z])
		cube([stdoff_len, stdoff_thick, stdoff_height]);

	stdoff3_y = 7;
	translate([-pcbw_x/2, stdoff3_y + pcb_wiggle/2 - stdoff_thick/2, stdoff_z])
		cube([stdoff_len, stdoff_thick, stdoff_height]);

	pegs();
}

module case_bottom(pw = 0.0, ptw = 0.0)
{
	difference()
	{
		translate([-case_x/2, -(pcb_wiggle/2 + wall_thick), -case_z/2 + pcbd_thick/2 + pcb_zoffset])
			cube([case_x, case_y, case_z - wall_thick]);

		translate([-inside_x/2, -pcb_wiggle/2, -inside_z/2 + pcbd_thick/2 + pcb_zoffset])
			cube([inside_x, inside_y, inside_z + 0.1]);

		// wiggle room for the flash connector
		flash_connector(flash_wigglex = 0.6, flash_wiggley = 0.5);

		neck_wigglex = 0.3;
		neck_wigglez = 0.2;

		// PCB and USB neck
		translate([-pcbn_x/2 - neck_wigglex/2, pcbw_y, -plug_z/2 + pcbd_thick/2 + pcb_wiggle])
			cube([pcbn_x + neck_wigglex, pcbn_y, plug_z]);

		// cut-offs for the round edge of the pcb neck
		cutoff = 0.8;
		translate([-pcbn_x/2 - neck_wigglex/2, pcbw_y + pcb_wiggle/2, -plug_z/2 + pcbd_thick/2 + pcb_wiggle])
			linear_extrude(height = plug_z)
				polygon([[0, 0], [-cutoff, 0], [0, cutoff]]);

		translate([pcbn_x/2 + neck_wigglex/2, pcbw_y + pcb_wiggle/2, -plug_z/2 + pcbd_thick/2 + pcb_wiggle])
			linear_extrude(height = plug_z)
				polygon([[0, 0], [cutoff, 0], [0, cutoff]]);
				
		// USB plug
		translate([-plug_x/2 - neck_wigglex/2, pcbw_y + pcbn_y, -plug_z/2 + pcbd_thick/2 + pcb_wiggle])
			cube([plug_x + neck_wigglex, plug_y, plug_z]);

		pegs(peg_wiggle = pw, thick_wiggle = ptw);
	}

	// the stand-offs
	stdoff_thick = 1.5;
	stdoff_height = inside_z/2 - pcb_zoffset - pcbd_thick;
	stdoff_len = 3;

	stdoff1_y = 4;
	translate([pcbw_x/2 - stdoff_len, stdoff1_y + pcb_wiggle/2 - stdoff_thick/2, -pcbd_thick/2 - stdoff_height])
		cube([stdoff_len, stdoff_thick, stdoff_height]);

	translate([-pcbw_x/2, stdoff1_y + pcb_wiggle/2 - stdoff_thick/2, -pcbd_thick/2 - stdoff_height])
		cube([stdoff_len, stdoff_thick, stdoff_height]);
}

translate([0, 0, case_z/2 - pcbd_thick/2 - pcb_zoffset])
	case_bottom(pw = 0.6, ptw = 0.3);

rotate([0, 180, 0])
	translate([21, 0, -case_z/2 - pcbd_thick/2 - pcb_zoffset])
	case_top();
