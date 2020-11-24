#include <stdio.h>
#include "QBDIPreload.h"


QBDIPRELOAD_INIT;

const long PAGE_SIZE = 4096;
static bool verbose= false;
static FILE* logstream = NULL;

/* This is run after every instruction. It just checks if the stack pointer haw
 * grown, if the instruction touches the stack, and if so reports accordingly.
 */
static VMAction onInstruction(VMInstanceRef vm, GPRState *gprState, FPRState *fprState, void *data) {

    static rword prev_probed_rsp = 0;
    static bool probed = true;

    const InstAnalysis* instAnalysis = qbdi_getInstAnalysis(vm, QBDI_ANALYSIS_INSTRUCTION | QBDI_ANALYSIS_DISASSEMBLY | QBDI_ANALYSIS_SYMBOL | QBDI_ANALYSIS_OPERANDS);

    if(!prev_probed_rsp) {
      prev_probed_rsp = gprState->rsp;
      probed = true;
    }
    else if(((long)prev_probed_rsp - (long)gprState->rsp) > PAGE_SIZE) {
      fprintf(logstream, "[sct][error] stack allocation is too big (%ld)\n", ((long)prev_probed_rsp - (long)gprState->rsp));
      prev_probed_rsp = gprState->rsp;
      probed = true;
    }
    else if(((long)prev_probed_rsp - (long)gprState->rsp) < 0) {
      prev_probed_rsp = gprState->rsp;
    }
    else if(prev_probed_rsp != gprState->rsp) {
      if(verbose)
        fprintf(logstream, "[sct][info] stack allocation is within the page guard (%ld)\n", ((long)prev_probed_rsp - (long)gprState->rsp));
    }

    size_t MemoryAccessCount;
    MemoryAccess * MemoryAccesses = qbdi_getInstMemoryAccess(vm, &MemoryAccessCount);
    for(size_t i = 0; i < MemoryAccessCount; ++i) {
      if((long)MemoryAccesses[i].accessAddress >= (long)gprState->rsp) {
        if(((long)prev_probed_rsp - (long)MemoryAccesses[i].accessAddress) <= PAGE_SIZE) {
          probed = true;
          if(verbose)
            fprintf(logstream, "[sct][info] last allocation is probed (0x%" PRIRWORD ")\n", MemoryAccesses[i].accessAddress);
          prev_probed_rsp = gprState->rsp;
        }
      }
    }
    return QBDI_CONTINUE;
}


int qbdipreload_on_start(void *main) {
    return QBDIPRELOAD_NOT_HANDLED;
}


int qbdipreload_on_premain(void *gprCtx, void *fpuCtx) {
    return QBDIPRELOAD_NOT_HANDLED;
}


int qbdipreload_on_main(int argc, char** argv) {
    qbdi_addLogFilter("*", QBDI_DEBUG);
    return QBDIPRELOAD_NOT_HANDLED;
}


int qbdipreload_on_run(VMInstanceRef vm, rword start, rword stop) {
    char* SCT_VERBOSE = getenv("SCT_VERBOSE");
    if(SCT_VERBOSE)
      verbose = atoi(SCT_VERBOSE);
    char* SCT_OUT = getenv("SCT_OUT");
    if(SCT_OUT)
      logstream = fopen(SCT_OUT, "w");
    else
      logstream = stderr;
    qbdi_recordMemoryAccess(vm, QBDI_MEMORY_READ_WRITE);
    qbdi_addCodeCB(vm, QBDI_POSTINST, onInstruction, NULL);
    qbdi_run(vm, start, stop);
    return QBDIPRELOAD_NO_ERROR;
}


int qbdipreload_on_exit(int status) {
    fclose(logstream);
    return QBDIPRELOAD_NO_ERROR;
}
