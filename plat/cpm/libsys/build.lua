acklibrary {
    name = "internal",
    hdrs = { "./*.h" }
}

local bdos_calls = {
    [ 0] = "cpm_exit",
    [ 1] = "cpm_conin",
    [ 2] = "cpm_conout",
    [ 3] = "cpm_auxin",
    [ 4] = "cpm_auxout",
    [ 5] = "cpm_lstout",
    [ 6] = "cpm_conio",
    [ 7] = "cpm_get_iobyte",
    [ 8] = "cpm_set_iobyte",
    [ 9] = "cpm_printstring",
    [10] = "cpm_readline",
    [11] = "cpm_const",
    [12] = "cpm_get_version",
    [13] = "cpm_reset_disk_system",
    [14] = "cpm_select_disk",
    [15] = "cpm_open_file",
    [16] = "cpm_close_file",
    [17] = "cpm_findfirst",
    [18] = "cpm_findnext",
    [19] = "cpm_delete_file",
    [20] = "cpm_read_sequential",
    [21] = "cpm_write_sequential",
    [22] = "cpm_make_file",
    [23] = "cpm_rename_file",
    [24] = "cpm_get_login_vector",
    [25] = "cpm_get_current_drive",
    [26] = "cpm_set_dma",
    [27] = "cpm_get_allocation_vector",
    [28] = "cpm_write_protect_drive",
    [29] = "cpm_get_readonly_vector",
    [30] = "cpm_set_file_attributes",
    [31] = "cpm_get_dpb",
    [32] = "cpm_get_set_user",
    [33] = "cpm_read_random",
    [34] = "cpm_write_random",
    [35] = "cpm_seek_to_end",
    [36] = "cpm_seek_to_seq_pos",
    [37] = "cpm_reset_drives",
    [40] = "cpm_write_random_filled",
}

local trap_calls = {
    "EARRAY",
    "ERANGE",
    "ESET",
    "EIOVFL",
    "EFOVFL",
    "EFUNFL",
    "EIDIVZ",
    "EFDIVZ",
    "EIUND",
    "EFUND",
    "ECONV",
    "ESTACK",
    "EHEAP",
    "EILLINS",
    "EODDZ",
    "ECASE",
    "EMEMFLT",
    "EBADPTR",
    "EBADPC",
    "EBADLAE",
    "EBADMON",
    "EBADLIN",
    "EBADGTO",
    "EUNIMPL",
}

local generated = {}
for n, name in pairs(bdos_calls) do
    generated[#generated+1] = normalrule {
        name = name,
        ins = { "./make_bdos_call.sh" },
        outleaves = { name..".s" },
        commands = {
            "%{ins[1]} "..n.." "..name.." > %{outs}"
        }
    }
end
for _, name in pairs(trap_calls) do
    generated[#generated+1] = normalrule {
        name = name,
        ins = { "./make_trap.sh" },
        outleaves = { name..".s" },
        commands = {
            "%{ins[1]} "..name:lower().." "..name.." > %{outs}"
        }
    }
end

acklibrary {
    name = "lib",
    srcs = {
        "./*.c",
        "./*.s",
        generated
    },
	deps = {
		"lang/cem/libcc.ansi/headers+headers",
        "plat/cpm/include+headers",
        "+internal",
	},
    vars = {
        plat = "cpm"
    }
}

