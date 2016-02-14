#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/user.h>
#include <sys/syscall.h>   /* For SYS_write etc */
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "tracing_utils.h"

const char* get_syscall_name(register_type id) {
    switch (id) {
        case SYS_write:
            return "write";
        case SYS_read:
            return "read";
        case SYS_open:
            return "open";
        case SYS_close:
            return "close";
        case SYS_execve:
            return "execve";
        case SYS_exit:
            return "exit";
        case SYS_mmap:
            return "mmap";
        case SYS_munmap:
            return "munmap";
        case SYS_mprotect:
            return "mprotect";
        case SYS_lseek:
            return "lseek";
        case SYS_access:
            return "access";
        case SYS_lstat:
            return "lstat";
        case SYS_brk:
            return "brk";
        case SYS_fstat:
            return "fstat";
        case SYS_exit_group:
            return "exit_group";
        default:
            return "unknown";
    }
}

void get_syscall_descr(char* buf, size_t sz, register_type id) {
    snprintf(buf, sz, "%s (" REG_FORMAT ")", get_syscall_name(id), id);
}
    
int main(int argc, char** argv) {
    if (argc < 2)
        return -1;

    pid_t child = fork();

    if (child == -1)
        return -1;
    
    if (child == 0) {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execve(argv[1], argv + 1, NULL);
    } else {
        int status;
        int insyscall = 0;
        struct syscall_info inf;
        struct user_regs_struct regs;
        char buf[100];
        
        waitpid(child, &status, __WALL);
        bool firsttime = 1;
        
        while(!WIFEXITED(status)) {
            if (errno) {
                fprintf(stderr, "error, %s\n", strerror(errno));
                errno = 0;
            }
            if (WIFSTOPPED(status) && WSTOPSIG(status) == (SIGTRAP | (firsttime ? 0 : 0x80))) {
                if (insyscall == 0) {
                    // syscall start
                    extract_registers(child, &regs);
                    extract_syscall_params(&regs, &inf);
                    
                    get_syscall_descr(buf, sizeof(buf), inf.id);
                    fprintf(stderr, "Sys call %s, params "
                            REG_FORMAT " " REG_FORMAT " " REG_FORMAT " " REG_FORMAT " " REG_FORMAT " " REG_FORMAT "\n",
                            buf, inf.arg1, inf.arg2, inf.arg3, inf.arg4, inf.arg5, inf.arg6);
                    if (inf.id == SYS_write) {
                        size_t sz = inf.arg3 + 1;
                        size_t sz_al = sz / sizeof(register_type) + 1;
                        register_type buf2[sz_al];
                        memcpy_from_proc((void*)inf.arg2, buf2, sz_al * sizeof(register_type), child);
                        char* str = (char*)buf2;
                        buf2[sz] = '\0';
                        fprintf(stderr, "Write content: %s\n", str);
                    }
                } else {
                    //syscall return
                    extract_registers(child, &regs);
                    extract_syscall_result(child, &regs, &inf);
                    fprintf(stderr, "Sys call %s, res: " REG_FORMAT ", error: " REG_FORMAT "\n", buf, inf.ret, inf.err);
                    
                    if (firsttime) {
                        // first registered syscall (execve),
                        // enable sandboxing.
                        firsttime = false;
                        ptrace(PTRACE_SETOPTIONS, child, NULL, PTRACE_O_EXITKILL | PTRACE_O_TRACESYSGOOD);
                    }
                }
                insyscall = !insyscall;
            } else {
                fprintf(stderr, "Unknown tracing event\n");
                if (insyscall) {
                    fprintf(stderr, "Interrupted syscall?\n");
                    insyscall = 0;
                }
            }
            
            ptrace(PTRACE_SYSCALL, child, NULL, NULL); // continue tracing.
            waitpid(child, &status, __WALL);
        }
        fprintf(stderr, "Program exited with %d\n", WEXITSTATUS(status));
    }
    return 0;
}