/* PANDABEGINCOMMENT
 * 
 * Authors:
 *  Tim Leek               tleek@ll.mit.edu
 *  Ryan Whelan            rwhelan@ll.mit.edu
 *  Joshua Hodosh          josh.hodosh@ll.mit.edu
 *  Michael Zhivich        mzhivich@ll.mit.edu
 *  Brendan Dolan-Gavitt   brendandg@gatech.edu
 * 
 * This work is licensed under the terms of the GNU GPL, version 2. 
 * See the COPYING file in the top-level directory. 
 * 
PANDAENDCOMMENT */
// This needs to be defined before anything is included in order to get
// the PRIx64 macro
#define __STDC_FORMAT_MACROS

#include "panda/plugin.h"
#include <unordered_set>
#include <fstream>
#include <iostream>

// These need to be extern "C" so that the ABI is compatible with
// QEMU/PANDA, which is written in C
extern "C" {
bool init_plugin(void *);
void uninit_plugin(void *);
}

std::unordered_set<target_ulong> basic_block_pcs;

bool before_block_exec(CPUState *cpu, TranslationBlock *tb) {
    basic_block_pcs.insert(tb->pc);
    return true;
}

bool init_plugin(void *self) {
    panda_cb cb;
    panda_disable_tb_chaining();
    cb.before_block_exec_invalidate_opt = before_block_exec;
    panda_register_callback(self, PANDA_CB_BEFORE_BLOCK_EXEC, cb);
    return true;
}

void uninit_plugin(void *self) {
    fprintf(stderr, "Saw a total of %lu unique blocks. Saved list to /tmp/coverage.txt\n", basic_block_pcs.size());

    std::ofstream f;
	f.open ("/tmp/coverage.txt");
    for (auto i : basic_block_pcs) {
        f << "0x" << std::hex << i << "\n";
    }

	f.close();
}
