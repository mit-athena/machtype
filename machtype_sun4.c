/*
 *  Machtype: determine machine type & display type
 *
 * RCS Info
 *    $Id: machtype_sun4.c,v 1.4 1993-07-19 15:30:04 vrt Exp $
 *    $Locker:  $
 */

#include <stdio.h>
#include <string.h>
#include <kvm.h>
#include <nlist.h>
#include <fcntl.h>
#undef NBPP
#define NBPP 4
#include <sys/types.h>
#include <sys/file.h>
#include <sys/cpu.h>

int verbose =0;

struct nlist nl[] = {
#define X_cpu 0
      { "cputype" },
#define X_maxmem 1
      { "maxmem" },
#define X_physmem 2
      { "physmem" },
      { "" }
};

main(argc, argv)
int   argc;
char  **argv;
{
kvm_t *kv;
    int i;
    int cpuflg = 0, dpyflg = 0, raflg = 0, memflg = 0;
    int doathenaV = 0;
    int dosyspV = 0;
    int dolocalV = 0;
    int dobosN = 0;
    int dobosV = 0;
    char *kernel = "/dev/ksyms",  *memory = "/dev/mem";
    FILE *f;

    for (i = 1; i < argc; i++) {
      if (argv[i][0] != '-')
        usage(argv[0]);

      switch (argv[i][1]) {
      case 'c':
          cpuflg++;
          break;
      case 'd':
          dpyflg++;
          break;
      case 'r':
          raflg++;
          break;
      case 'M':
          memflg++;
        break;
      case 'k':
          kernel = argv[i+1];
          i++;
          break;
      case 'm':
          memory = argv[i+1];
          i++;
          break;
        case 'A':
          doathenaV = 1;
          break;
        case 'L':
          dolocalV = 1;
          break;
        case 'P':
          dosyspV = 1;
          break;
      case 'N':
          dobosN = 1;
          break;
      case 'E':
          dobosV = 1;
          break;
      case 'v':
          verbose++;
          break;
      default:
          usage(argv[0]);
      }
    }

    if ((argc == 1) || ((argc == 2) && verbose)) {
      puts("sun4");
      exit(0);
    }

     /* Print out version of Athena machtype compiled against */
    if (doathenaV) {
      if (verbose)
      printf("Machtype version: %s.%s\n",ATHMAJV,ATHMINV);
      else
      printf("%s.%s\n",ATHMAJV,ATHMINV);
    }

   /* Print out version of attached packs */
    if (dosyspV) {
      char buf[256],rvd_version[256], *p;
      if ((f = fopen("/srvd/.rvdinfo","r")) == NULL) {
      printf("Syspack information unavailable\n");
      } else {
      fgets(buf,256,f);
      fclose(f);
     /* If it is verbose, give the whole line, else just the vers # */
      if (verbose) {
        printf(buf);
      } else {
        p = index(buf,' '); /* skip "Athena" */
        p = index(p+1,' '); /* skip "RVD" */
        p = index(p+1,' '); /* Skip "RSAIX" */
        p = index(p+1,' '); /* skip "version" */
        strncpy(rvd_version,p+1,256);
        p = index(rvd_version,' ');
        *p = '\0';
        printf("%s\n",rvd_version);
      }
      }
    }

    /* Print out local version from /etc/athena/version */
    if (dolocalV) {
      char buf[256],loc_version[256], *p;
      if ((f = fopen("/etc/athena/version","r")) == NULL) {
      printf("Local version information unavailable\n");
      } else {
      fseek(f,-100,2);
      while (fgets(buf,256,f) != NULL)
        ;
      fclose(f);

      if (verbose) {
        printf(buf);
      } else {
        p = index(buf,' '); /* skip "Athena" */
        p = index(p+1,' '); /* skip "Workstation/Server" */
        p = index(p+1,' '); /* Skip "RSAIX" */
        p = index(p+1,' '); /* skip "version" */
        strncpy(loc_version,p+1,256);
        p = index(loc_version,' ');
        *p = '\0';
        printf("%s\n",loc_version);
      }
      }
    }

    /* Print out vendor OS name */
    if (dobosN) {
      if (verbose) {
     printf("SunOS 5.1\n");
      } else {
        printf("SunOS\n");
      }
    }

    /* Print out vendor OS version */
    if (dobosV) {
        printf("5.1\n");
    }

    if (cpuflg || dpyflg || raflg || memflg)
      {
        int memfd;
      kv = kvm_open(NULL,NULL,NULL,O_RDONLY,NULL);
      if (!kv) {
        fprintf(stderr,"%s: unable to examine the kernel\n", argv[0]);
        exit(2);
      }
      if (kvm_nlist(kv, &nl) < 0) {
        fprintf(stderr,"%s: can't get namelist\n", argv[0]);
        exit(2);
      }
     if (cpuflg)
        do_cpu(kv, memfd);
      if (dpyflg)
        do_dpy(kernel, memfd);
      if (raflg)
        do_disk(kernel, memfd);
      if (memflg)
        do_memory(kv, memfd);
      }
      if (cpuflg || dpyflg || raflg || memflg)
	kvm_close(kv);
    exit(0);
}

usage(name)
char *name;
{
    fprintf(stderr, "usage: %s [-v] [-c] [-d] [-r] [-M]\n",name);
    fprintf(stderr, "             [-k kernel] [-m memory] [-A] [-L] [-P]\n");
    exit(1);
}

do_cpu(kernel, mf)
kvm_t *kernel;
int mf;
{
     short cpu;
short cpu_type;

    cpu_type = kvm_read(kernel,nl[X_cpu].n_value,&cpu, sizeof(cpu));
{
        switch(cpu) {
          case CPU_SUN4C_60:
            puts(verbose ? "SPARCstation 1": "SPARC/1");
            break;
          case CPU_SUN4C_40:
            puts(verbose ? "SPARCstation IPC" : "SPARC/IPC");
            break;
          case CPU_SUN4C_65:
            puts(verbose ? "SPARCstation 1+" : "SPARC/1+");
            break;
          case CPU_SUN4C_20:
            puts(verbose ? "SPARCstation SLC" : "SPARC/SLC");
            break;
          case CPU_SUN4C_75:
            puts(verbose ? "SPARCstation 2" : "SPARC/2");
            break;
          case CPU_SUN4C_25:
            puts(verbose ? "SPARCstation ELC" : "SPARC/ELC");
            break;
          case CPU_SUN4C_50:
            puts(verbose ? "SPARCstation IPX" : "SPARC/IPX");
            break;
	case 128:
	puts(verbose ? "SPARCstation Classic" : "SPARC/Classic");
		break;

         default:
           if(verbose)
                printf("Unknown SUN type %d\n", cpu);
           else
              puts("SUN???");
         }
       }
    return;
}

do_dpy(kernel, mf)
char *kernel;
int mf;
{
   puts(verbose? "cgthree" : "cgthree frame buffer");
    return;
}

do_disk(kernel, mf)
char *kernel;
int mf;
{
    fprintf (stderr, "Don't know how to read disk types for this machine.\n");
    return;
}

#define MEG (1024*1024)

do_memory (kernel, mf)
kvm_t *kernel;
int mf;
{
  int pos, mem;

   kvm_read(kernel,nl[X_maxmem].n_value,&mem, sizeof(mem));
   if(verbose)
      printf("%d user, ",mem * NBPP);
   kvm_read(kernel,nl[X_physmem].n_value,&mem, sizeof(mem));
   if(verbose)
      printf("%d (%d M) total\n",mem * NBPP ,(mem * NBPP + 916)/1024 );
    else
      printf("%d\n", mem * NBPP + 916);
   return;
}
