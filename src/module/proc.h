//  Sandboxer, kernel module sandboxing stuff
//  Copyright (C) 2016  Sayutin Dmitry, Alferov Vasiliy
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef SANDBOXER_PROC_H_
#define SANDBOXER_PROC_H_

#include <linux/types.h>
#include <linux/seq_file.h>
#include "slot.h"

/* 
 * Initializes /proc/sandboxer directory and files there. Initlib-friendly.
 */
int init_or_shutdown_sandboxer_proc_dir(int initlib_mode, __attribute__((unused))void* ignored);

 
int add_slot_property(const char *name, int (*cb)(struct seq_file *, size_t));

int create_slotid_dir(struct sandbox_slot *s);
void destroy_slotid_dir(struct sandbox_slot *s);

#endif //SANDBOXER_PROC_H_
