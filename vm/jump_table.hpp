static void *jump_table[256] = {&&return_instr, &&next_instr, &&pers_iter, &&testnil, &&opers_iter, &&linear_iter, &&rlinear_iter, &&not_instr, &&send, &&float_instr, &&select, &&return_select, &&olinear_iter, &&not_found, &&reset_linear, &&end_linear, &&rule, &&rule_done, &&orlinear_iter, &&new_node, &&new_axioms, &&send_delay, &&push, &&pop, &&push_regs, &&pop_regs, &&callf, &&calle, &&set_priority, &&make_structr, &&mvintfield, &&mvintreg, &&call, &&mvfieldfield, &&mvfieldreg, &&mvptrreg, &&mvnilreg, &&mvfieldfieldr, &&mvregfield, &&mvregfieldr, &&mvhostfield, &&mvregconst, &&mvconstfield, &&mvconstfieldr, &&mvaddrfield, &&mvfloatfield, &&mvfloatreg, &&mvintconst, &&set_priorityh, &&mvworldfield, &&mvstackpcounter, &&mvpcounterstack, &&mvstackreg, &&mvregstack, &&mvaddrreg, &&mvhostreg, &&addrnotequal, &&addrequal, &&intminus, &&intequal, &&intnotequal, &&intplus, &&intlesser, &&intgreaterequal, &&alloc, &&boolor, &&intlesserequal, &&intgreater, &&intmul, &&intdiv, &&floatplus, &&floatminus, &&floatmul, &&floatdiv, &&floatequal, &&floatnotequal, &&floatlesser, &&floatlesserequal, &&floatgreater, &&floatgreaterequal, &&mvregreg, &&boolequal, &&boolnotequal, &&headrr, &&headfr, &&headff, &&headrf, &&headffr, &&headrfr, &&tailrr, &&tailfr, &&tailff, &&tailrf, &&mvworldreg, &&mvconstreg, &&consrrr, &&if_instr, &&consrff, &&consfrf, &&consffr, &&consrrf, &&consrfr, &&consfrr, &&consfff, &&call0, &&call1, &&call2, &&call3, &&mvintstack, &&pushn, &&make_structf, &&struct_valrr, &&mvnilfield, &&struct_valfr, &&struct_valrf, &&struct_valrfr, &&struct_valff, &&struct_valffr, &&mvfloatstack, &&addlinear, &&addpers, &&runaction, &&enqueue_linear, &&update, &&mvargreg, &&intmod, &&cpu_id, &&node_priority, &&remove, &&if_else, &&jump, &&mvstackfield, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&add_priority, &&add_priorityh, &&stop_program, &&set_defprioh, &&set_defprio, &&set_statich, &&set_static, &&set_movingh, &&set_moving, &&set_affinityh, &&set_affinity, &&cpu_static, &&mvcpusreg, &&set_cpuh, &&is_static, &&is_moving, &&facts_proved, &&rem_priority, &&rem_priorityh, &&facts_consumed, &&tlinear_iter, &&addtpers, &&schedule_next, &&tpers_iter, &&gc, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&return_linear, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&return_derived, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found, &&not_found};
