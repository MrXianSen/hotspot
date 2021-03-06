/*
 * Copyright 1999-2007 Sun Microsystems, Inc.  All Rights Reserved.
 * Copyright 2009 Red Hat, Inc.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa Clara,
 * CA 95054 USA or visit www.sun.com if you need additional information or
 * have any questions.
 *
 */

#include "incls/_precompiled.incl"
#include "incls/_sharkMemoryManager.cpp.incl"

using namespace llvm;

void SharkMemoryManager::AllocateGOT() {
  mm()->AllocateGOT();
}

unsigned char* SharkMemoryManager::getGOTBase() const {
  return mm()->getGOTBase();
}

unsigned char* SharkMemoryManager::allocateStub(const GlobalValue* F,
                                                unsigned StubSize,
                                                unsigned Alignment) {
  return mm()->allocateStub(F, StubSize, Alignment);
}

unsigned char* SharkMemoryManager::startFunctionBody(const Function* F,
                                                     uintptr_t& ActualSize) {
  return mm()->startFunctionBody(F, ActualSize);
}

void SharkMemoryManager::endFunctionBody(const Function* F,
                                         unsigned char* FunctionStart,
                                         unsigned char* FunctionEnd) {
  mm()->endFunctionBody(F, FunctionStart, FunctionEnd);

  SharkEntry *entry = get_entry_for_function(F);
  if (entry != NULL)
    entry->set_code_limit(FunctionEnd);
}

unsigned char* SharkMemoryManager::startExceptionTable(const Function* F,
                                                       uintptr_t& ActualSize) {
  return mm()->startExceptionTable(F, ActualSize);
}

void SharkMemoryManager::endExceptionTable(const Function* F,
                                           unsigned char* TableStart,
                                           unsigned char* TableEnd,
                                           unsigned char* FrameRegister) {
  mm()->endExceptionTable(F, TableStart, TableEnd, FrameRegister);
}

void SharkMemoryManager::setMemoryWritable() {
  mm()->setMemoryWritable();
}

void SharkMemoryManager::setMemoryExecutable() {
  mm()->setMemoryExecutable();
}

#if SHARK_LLVM_VERSION >= 27
void SharkMemoryManager::deallocateExceptionTable(void *ptr) {
  mm()->deallocateExceptionTable(ptr);
}

void SharkMemoryManager::deallocateFunctionBody(void *ptr) {
  mm()->deallocateFunctionBody(ptr);
}
#else
void SharkMemoryManager::deallocateMemForFunction(const Function* F) {
  return mm()->deallocateMemForFunction(F);
}
#endif

uint8_t* SharkMemoryManager::allocateGlobal(uintptr_t Size,
                                            unsigned int Alignment) {
  return mm()->allocateGlobal(Size, Alignment);
}

#if SHARK_LLVM_VERSION < 27
void* SharkMemoryManager::getDlsymTable() const {
  return mm()->getDlsymTable();
}

void SharkMemoryManager::SetDlsymTable(void *ptr) {
  mm()->SetDlsymTable(ptr);
}
#endif

void SharkMemoryManager::setPoisonMemory(bool poison) {
  mm()->setPoisonMemory(poison);
}

unsigned char *SharkMemoryManager::allocateSpace(intptr_t Size,
                                                 unsigned int Alignment) {
  return mm()->allocateSpace(Size, Alignment);
}
