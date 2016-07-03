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

#ifndef SANDBOXER_TEST_HASHMAP_H_
#define SANDBOXER_TEST_HASHMAP_H_

/**
  * Performs hashmap testing.
  * Suitable for using with initlib.
  *
  * tests performed only with initlib_mode == 1, and errno returned on error.
  * Second argument is ignored.
  *
  * Please note, that this tests general hashmap health,
  * Not details of error handling, specified in hashmap docs.
  */
int test_hashmap(int initlib_mode, void* ignored);

#endif