/* Mordor Database Utilities */

extern int 	count_obj(), write_obj(), count_inv(), write_crt(), 
		count_mon(), count_ite(), count_ext(), count_ply(),
		write_rom(), read_obj(), read_crt(), read_rom(),
		load_rom(), load_crt(), load_obj(), save_rom(),
		load_rom_from_file(), load_crt_from_file(), 
		load_obj_from_file(), save_rom_to_file(), read_crt_from_mem(),
		write_crt_to_mem(), read_obj_from_mem(), write_obj_to_mem(),
		save_obj_to_file(), save_crt_to_file(), save_ply_to_file();

extern void 	free_obj(), free_crt(), free_rom(), merror(), zero(),
		clearscreen(), posit(), spin();

